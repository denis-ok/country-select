open Belt;
open Utils.Infix;

module Types = CountrySelectTypes;
module Styles = CountrySelectStyles;

module Text = {
  let loading = "Loading...";
  let selectCountry = "Select Country";
};

type state = {
  options: option(array(Types.Option.t)),
  selectedCountry: option(Types.Option.t),
  filter: string,
  menuOpened: bool,
  focusedSection: option(Types.Section.t),
  rootRef: option(React.ref(Js.Nullable.t(Dom.element))),
  filterRef: option(React.ref(Js.Nullable.t(Dom.element))),
  buttonRef: option(React.ref(Js.Nullable.t(Dom.element))),
};

let initialState = {
  options: None,
  selectedCountry: None,
  filter: "",
  menuOpened: false,
  focusedSection: None,
  rootRef: None,
  filterRef: None,
  buttonRef: None,
};

type action =
  | FetchCountriesSuccess(array(Types.Option.t))
  | FetchCountriesFailure(ReludeFetch.Error.t(string))
  | SetCountry(option(Types.Option.t))
  | SelectCountry(Types.Option.t, string => unit)
  | SetFilter(string)
  | SetFocusedSection(Types.Section.t)
  | SetRootRef(React.ref(Js.Nullable.t(Dom.element)))
  | SetFilterRef(React.ref(Js.Nullable.t(Dom.element)))
  | SetButtonRef(React.ref(Js.Nullable.t(Dom.element)))
  | ToggleMenu
  | Blur
  | FocusButton
  | FocusFilter
  | FocusOptions(int)
  | NoOp;

let reducer =
    (state: state, action: action): ReludeReact.Reducer.update(action, state) =>
  switch (action) {
  | FetchCountriesSuccess(countries) =>
    Update({...state, options: Some(countries)})

  | FetchCountriesFailure(error) =>
    SideEffect(_ => Js.log(ReludeFetch.Error.show(e => e, error)))

  | SetCountry(country) => Update({...state, selectedCountry: country})

  | SelectCountry(country, callback) =>
    UpdateWithSideEffect(
      {
        ...state,
        selectedCountry: Some(country),
        menuOpened: false,
        focusedSection: Some(Button),
      },
      ({send}) => {
        send(FocusButton);
        callback(country.value);
      },
    )

  | SetFilter(filter) => Update({...state, filter})

  | ToggleMenu => Update({...state, menuOpened: !state.menuOpened})

  | SetFocusedSection(element) =>
    Update({...state, focusedSection: Some(element)})

  | SetRootRef(ref_) => Update({...state, rootRef: Some(ref_)})

  | SetFilterRef(ref_) => Update({...state, filterRef: Some(ref_)})

  | SetButtonRef(ref_) => Update({...state, buttonRef: Some(ref_)})

  | Blur =>
    UpdateWithSideEffect(
      {...state, focusedSection: None, menuOpened: false, filter: ""},
      ({state}) => Utils.ReactDom.blurOptRef(state.buttonRef),
    )

  | FocusButton =>
    SideEffect(({state}) => Utils.ReactDom.focusOptRef(state.buttonRef))

  | FocusFilter =>
    SideEffect(({state}) => Utils.ReactDom.focusOptRef(state.filterRef))

  | FocusOptions(focusedIndex) =>
    Update({
      ...state,
      focusedSection: Some(Types.Section.Options(focusedIndex)),
    })

  | NoOp => NoUpdate
  };

module FunctorComponent = (Request: CountrySelectAPI.Request) => {
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

    let options = options->Option.map(Utils.filterOptions(_, filter));

    let focusedIndex =
      switch (focusedSection) {
      | Some(Options(index)) => Some(index)
      | Some(Button) => None
      | Some(Filter) => None
      | None => None
      };

    ReludeReact.Effect.useIOOnMount(
      Request.getCountriesIO(optionsUrl),
      options => FetchCountriesSuccess(options)->send,
      error => FetchCountriesFailure(error)->send,
    );

    let rootRef: React.ref(Js.Nullable.t(Dom.element)) =
      React.useRef(Js.Nullable.null);

    ReludeReact.Effect.useOnMount(() => send(SetRootRef(rootRef)));

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
      SelectCountry(country, onChange)->send;

    let onFocusButton = () => SetFocusedSection(Button)->send;

    let onFocusFilter = () => SetFocusedSection(Filter)->send;

    let onDropdownKeyDown = (event: ReactEvent.Keyboard.t) => {
      let key = Utils.ReactDom.keyFromEvent(event);

      let action =
        switch (key) {
        | ArrowUp => ToggleMenu
        | ArrowDown => menuOpened ? FocusFilter : ToggleMenu
        | Space
        | Enter => ToggleMenu
        | Tab
        | Escape => Blur
        | PageUp
        | PageDown
        | Unsupported => NoOp
        };

      send(action);
    };

    let onFilterKeyDown = (event: ReactEvent.Keyboard.t) => {
      let key = Utils.ReactDom.keyFromEvent(event);

      let action =
        switch (key) {
        | ArrowUp => FocusButton
        | ArrowDown
        | Enter
        | Tab => FocusOptions(0)
        | Escape => Blur
        | PageUp
        | PageDown
        | Space
        | Unsupported => NoOp
        };

      send(action);
    };

    let onOptionKeyDown =
        (
          focusedIndex: int,
          options: array(Types.Option.t),
          event: ReactEvent.Keyboard.t,
        ) => {
      let key = Utils.ReactDom.keyFromEvent(event);

      let action =
        switch (key) {
        | ArrowUp when focusedIndex == 0 => FocusFilter
        | ArrowUp => FocusOptions(focusedIndex - 1)
        | ArrowDown =>
          let maxIndex = Array.size(options) - 1;
          focusedIndex == maxIndex ? NoOp : FocusOptions(focusedIndex + 1);
        | Space
        | Enter =>
          switch (options[focusedIndex]) {
          | Some(country) => SelectCountry(country, onChange)
          | None => NoOp
          }
        | Tab => FocusFilter
        // PageUp, PageDown keys have issues related to scrolling inside react-window
        // Pagination capability limited intentionally
        | PageUp =>
          focusedIndex > 4 ? FocusOptions(focusedIndex - 4) : FocusOptions(0)
        | PageDown =>
          FocusOptions(focusedIndex + 4)
        | Escape => Blur
        | Unsupported => NoOp
        };

      send(action);
    };

    let onKeyDown = (event: ReactEvent.Keyboard.t): unit => {
      switch (focusedSection) {
      | None => ()
      | Some(element) =>
        switch (element) {
        | Button => onDropdownKeyDown(event)
        | Filter => onFilterKeyDown(event)
        | Options(index) =>
          switch (options) {
          | None => ()
          | Some(options) => onOptionKeyDown(index, options, event)
          }
        }
      };
    };

    let className = Styles.root ++? className;

    <div ref={ReactDOM.Ref.domRef(rootRef)} className>
      {switch (options) {
       | None =>
         <CountrySelectDropdownButton
           text=Text.loading
           onClick=ignore
           opened=false
           onKeyDown
           onFocus=onFocusButton
           setRef={ref_ => send(SetButtonRef(ref_))}
         />
       | Some(options) =>
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
             onKeyDown
           />
           {menuOpened
            &&& <div className=Styles.menuWrapper>
                  <CountrySelectSearchFilter
                    onKeyDown
                    value=filter
                    onChange=onChangeFilter
                    onFocus=onFocusFilter
                    setRef={ref_ => send(SetFilterRef(ref_))}
                  />
                  <CountrySelectMenu.CountryList
                    onKeyDown
                    options
                    selectedCountry
                    onChangeCountry
                    focusedIndex
                  />
                </div>}
         </>
       }}
    </div>;
  };
};

include FunctorComponent(CountrySelectAPI.Request);
