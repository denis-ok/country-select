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
  focusedSection: option(Types.FocusedSection.t),
  rootRef: option(React.ref(Js.Nullable.t(Dom.element))),
};

let initialState = {
  options: None,
  selectedCountry: None,
  filter: "",
  focusedSection: None,
  rootRef: None,
};

type action =
  | FetchCountriesSuccess(array(Types.Option.t))
  | FetchCountriesFailure(ReludeFetch.Error.t(string))
  | SetCountry(option(Types.Option.t))
  | SelectCountry(Types.Option.t, string => unit)
  | SetFilter(string)
  | SetFocusedSection(Types.FocusedSection.t)
  | SetRootRef(React.ref(Js.Nullable.t(Dom.element)))
  | ToggleMenu
  | Blur
  | FocusOption(int)
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
        focusedSection: Some(Button),
      },
      _ => callback(country.value),
    )

  | SetFilter(filter) => Update({...state, filter})

  | ToggleMenu =>
    Update(
      switch (state.focusedSection) {
      | None
      | Some(Filter)
      | Some(Options(_)) => {...state, focusedSection: Some(Button)}
      | Some(Button) => {...state, focusedSection: Some(Filter)}
      },
    )

  | SetFocusedSection(element) =>
    Update({...state, focusedSection: Some(element)})

  | SetRootRef(ref_) => Update({...state, rootRef: Some(ref_)})

  | Blur => Update({...state, focusedSection: None, filter: ""})

  | FocusOption(focusedIndex) =>
    Update({
      ...state,
      focusedSection: Some(Types.FocusedSection.Options(focusedIndex)),
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
    let ({options, selectedCountry, filter, focusedSection}: state, send) =
      ReludeReact.Reducer.useReducer(reducer, initialState);

    let menuOpened =
      switch (focusedSection) {
      | None
      | Some(Button) => false
      | Some(Filter) => true
      | Some(Options(_)) => true
      };

    let buttonFocused =
      switch (focusedSection) {
      | Some(Button) => true
      | Some(Filter)
      | Some(Options(_))
      | None => false
      };

    let filterFocused =
      switch (focusedSection) {
      | Some(Filter) => true
      | Some(Button)
      | Some(Options(_))
      | None => false
      };

    let filteredOptions = {
      switch (options) {
      | None => None
      | Some(options) =>
        let filtered = Utils.filterOptions(options, filter);

        if (Array.size(filtered) == 0) {
          None;
        } else {
          Some(filtered);
        };
      };
    };

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

    let onButtonKeyDown = (event: ReactEvent.Keyboard.t) => {
      let key = Utils.ReactDom.keyFromEvent(event);

      let action =
        switch (key) {
        | ArrowUp
        | ArrowDown
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
        | ArrowUp => ToggleMenu
        | ArrowDown
        | Enter
        | Tab => FocusOption(0)
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
          event: ReactEvent.Keyboard.t,
          options: array(Types.Option.t),
          focusedIndex: int,
        ) => {
      let key = Utils.ReactDom.keyFromEvent(event);

      let maxIndex = Array.size(options) - 1;

      let action =
        switch (key) {
        | ArrowUp when focusedIndex == 0 => SetFocusedSection(Filter)
        | ArrowUp => FocusOption(focusedIndex - 1)
        | ArrowDown =>
          focusedIndex == maxIndex ? NoOp : FocusOption(focusedIndex + 1)
        | Space
        | Enter =>
          switch (options[focusedIndex]) {
          | Some(country) => SelectCountry(country, onChange)
          | None => NoOp
          }
        | Tab => SetFocusedSection(Filter)
        // PageUp, PageDown keys have issues related to scrolling inside react-window
        // Pagination capability limited intentionally
        | PageUp =>
          focusedIndex > 4 ? FocusOption(focusedIndex - 4) : FocusOption(0)
        | PageDown =>
          let targetIndex = focusedIndex + 4;
          targetIndex > maxIndex
            ? FocusOption(maxIndex) : FocusOption(targetIndex);
        | Escape => Blur
        | Unsupported => NoOp
        };

      send(action);
    };

    let onKeyDown = (event: ReactEvent.Keyboard.t): unit => {
      switch (Functions.both(focusedSection, filteredOptions)) {
      | Some((focusedSection, filteredOptions)) =>
        switch (focusedSection) {
        | Button => onButtonKeyDown(event)
        | Filter => onFilterKeyDown(event)
        | Options(index) => onOptionKeyDown(event, filteredOptions, index)
        }
      | None => ()
      };
    };

    let className = Styles.root ++? className;

    <div ref={ReactDOM.Ref.domRef(rootRef)} className>
      {switch (options) {
       | None =>
         <CountrySelectButton
           text=Text.loading
           onClick=ignore
           opened=false
           focused=buttonFocused
           onKeyDown
           onFocus=onFocusButton
         />
       | Some(_options) =>
         <>
           <CountrySelectButton
             text=Option.(
               map(selectedCountry, c => c.label)
               ->getWithDefault(Text.selectCountry)
             )
             onClick=toggleMenu
             opened=menuOpened
             focused=buttonFocused
             onFocus=onFocusButton
             onKeyDown
           />
           {menuOpened
            &&& <div className=Styles.menuWrapper>
                  <CountrySelectFilter
                    onKeyDown
                    value=filter
                    onChange=onChangeFilter
                    onFocus=onFocusFilter
                    focused=filterFocused
                  />
                  {switch (filteredOptions) {
                   | Some(filteredOptions) =>
                     <CountrySelectOptions
                       onKeyDown
                       options=filteredOptions
                       selectedCountry
                       onChangeCountry
                       focusedIndex
                     />
                   | None => <CountrySelectOptions.CountryNotFound />
                   }}
                </div>}
         </>
       }}
    </div>;
  };
};

include FunctorComponent(CountrySelectAPI.Request);
