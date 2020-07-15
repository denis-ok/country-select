open Belt;
open Utils.Infix;

module Types = CountrySelectTypes;

module Text = {
  let loading = "Loading...";
  let selectCountry = "Select Country";
};

module Styles = {
  open Css;

  let root =
    style([
      boxSizing(borderBox),
      width(px(CountrySelectConstants.Style.Size.menuWidthPx)),
    ]);
};

type state = {
  options: option(array(Types.Option.t)),
  selectedCountry: option(Types.Option.t),
  filter: string,
  menuOpened: bool,
  focusedSection: option(Types.Section.t),
  filterRef: option(React.ref(Js.Nullable.t(Dom.element))),
  buttonRef: option(React.ref(Js.Nullable.t(Dom.element))),
};

let initialState = {
  options: None,
  selectedCountry: None,
  filter: "",
  menuOpened: false,
  focusedSection: None,
  filterRef: None,
  buttonRef: None,
};

type action =
  | FetchCountriesSuccess(array(Types.Option.t))
  | FetchCountriesFailure(ReludeFetch.Error.t(string))
  | SetCountry(option(Types.Option.t))
  | ChangeCountry(Types.Option.t, string => unit)
  | SetFilter(string)
  | SetFocusedSection(Types.Section.t)
  | SetFilterRef(React.ref(Js.Nullable.t(Dom.element)))
  | SetButtonRef(React.ref(Js.Nullable.t(Dom.element)))
  | ToggleMenu
  | Blur
  | FocusButton
  | FocusFilter
  | FocusList
  | NextItem
  | PrevItem
  | SelectItem
  | NoOp;

let reducer =
    (state: state, action: action): ReludeReact.Reducer.update(action, state) =>
  switch (action) {
  | FetchCountriesSuccess(countries) =>
    Update({...state, options: Some(countries)})

  | FetchCountriesFailure(error) =>
    SideEffect(_ => Js.log(ReludeFetch.Error.show(e => e, error)))

  | SetCountry(country) => Update({...state, selectedCountry: country})

  | ChangeCountry(country, callback) =>
    UpdateWithSideEffect(
      {...state, selectedCountry: Some(country)},
      _ => callback(country.value),
    )

  | SetFilter(filter) => Update({...state, filter})

  | ToggleMenu => Update({...state, menuOpened: !state.menuOpened})

  | SetFocusedSection(element) =>
    Update({...state, focusedSection: Some(element)})

  | SetFilterRef(ref_) => Update({...state, filterRef: Some(ref_)})

  | SetButtonRef(ref_) => Update({...state, buttonRef: Some(ref_)})

  | Blur => Update({...state, focusedSection: None, menuOpened: false})

  | FocusButton =>
    SideEffect(({state}) => Utils.ReactDom.focusOptRef(state.buttonRef))

  | FocusFilter =>
    SideEffect(({state}) => Utils.ReactDom.focusOptRef(state.filterRef))

  | FocusList
  | NextItem
  | PrevItem
  | SelectItem
  | NoOp => NoUpdate
  };

module Functor = (Request: CountrySelectAPI.Request) => {
  [@react.component]
  let make =
      (
        ~country: option(string),
        ~onChange: string => unit,
        ~optionsUrl: option(string)=?,
        ~className: option(string)=?,
      ) => {
    let (
      {options, selectedCountry, filter, menuOpened, focusedSection}: state,
      send,
    ) =
      ReludeReact.Reducer.useReducer(reducer, initialState);

    ReludeReact.Effect.useIOOnMount(
      Request.getCountriesIO(optionsUrl),
      options => FetchCountriesSuccess(options)->send,
      error => FetchCountriesFailure(error)->send,
    );

    let rootRef: React.ref(Js.Nullable.t(Dom.element)) =
      React.useRef(Js.Nullable.null);

    Utils.ReactDom.useClickOutside(rootRef, () => send(Blur));

    React.useEffect2(
      () => {
        switch (options, country) {
        | (Some(options), Some(country)) =>
          let mbCountry = options->Array.getBy(opt => opt.value == country);
          SetCountry(mbCountry)->send;
        | _ => ()
        };
        None;
      },
      (options, country),
    );

    let toggleMenu = () => ToggleMenu->send;

    let onChangeFilter = str => SetFilter(str)->send;

    let onChangeCountry = (country: Types.Option.t) =>
      ChangeCountry(country, onChange)->send;

    let onFocusButton = () => {
      SetFocusedSection(Types.Section.Button)->send;
    };

    let onFocusFilter = () => {
      SetFocusedSection(Types.Section.Filter)->send;
    };

    let onFocusList = () => {
      switch (focusedSection) {
      | Some(Options) => ()
      | _ => SetFocusedSection(Types.Section.Options)->send
      };
    };

    let onKeyDown = (event: ReactEvent.Keyboard.t) => {
      let action: action =
        Types.Section.(
          switch (focusedSection) {
          | None => NoOp
          | Some(element) =>
            let key = Utils.ReactDom.keyFromEvent(event);
            switch (element, key) {
            | (_, Unsupported) => NoOp
            | (_, Escape) => Blur
            | (Button, ArrowUp) => ToggleMenu
            | (Button, ArrowDown) when !menuOpened => ToggleMenu
            | (Button, ArrowDown) when menuOpened => FocusFilter
            | (Button, Tab) when !menuOpened => ToggleMenu
            | (Filter, ArrowUp) => FocusButton
            | (Filter, ArrowDown) => FocusList
            | (Filter, Tab) => FocusList
            | (Options, ArrowUp) => PrevItem
            | (Options, ArrowDown) => NextItem
            | (Options, Tab) => NoOp
            | (Options, Space) => SelectItem
            | (Options, Enter) => SelectItem
            | _ => NoOp
            };
          }
        );

      send(action);
    };

    let className = Styles.root ++? className;

    <div ref={ReactDOMRe.Ref.domRef(rootRef)} className onKeyDown>
      {switch (options) {
       | None =>
         <CountrySelectDropdownButton
           text=Text.loading
           onClick=ignore
           opened=false
           onFocus=onFocusButton
           setRef={ref_ => send(SetButtonRef(ref_))}
         />
       | Some(options) =>
         let filteredOptions = Utils.filterOptions(options, filter);

         <>
           <CountrySelectDropdownButton
             text=Option.(
               map(selectedCountry, c => c.label)
               ->getWithDefault(Text.selectCountry)
             )
             onClick=toggleMenu
             opened=menuOpened
             onFocus=onFocusButton
             setRef={ref_ => send(SetButtonRef(ref_))}
           />
           {menuOpened
            &&& <CountrySelectMenu.Wrapper>
                  <CountrySelectSearchFilter
                    value=filter
                    onChange=onChangeFilter
                    onFocus=onFocusFilter
                    setRef={ref_ => send(SetFilterRef(ref_))}
                  />
                  <CountrySelectMenu.CountryList
                    options=filteredOptions
                    selectedCountry
                    onChangeCountry
                    onFocus=onFocusList
                  />
                </CountrySelectMenu.Wrapper>}
         </>;
       }}
    </div>;
  };
};

include Functor(CountrySelectAPI.Request);
