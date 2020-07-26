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
  filteredOptions: option(array(Types.Option.t)),
  selectedCountry: option(Types.Option.t),
  filter: string,
  focusedSection: Types.VisualState.t,
  rootRef: option(React.ref(Js.Nullable.t(Dom.element))),
};

let initialState = {
  options: None,
  filteredOptions: None,
  selectedCountry: None,
  filter: "",
  focusedSection: MenuClosed(`Unfocused),
  rootRef: None,
};

type action =
  | FetchCountriesSuccess(array(Types.Option.t))
  | FetchCountriesFailure(ReludeFetch.Error.t(string))
  | SetCountry(option(Types.Option.t))
  | SelectCountry(Types.Option.t, string => unit)
  | ChangeFilter(string)
  | SetFocusedSection(Types.VisualState.t)
  | SetRootRef(React.ref(Js.Nullable.t(Dom.element)))
  | ToggleMenu
  | Blur
  | HighlightOption(int)
  | NoOp;

let reducer =
    (state: state, action: action): ReludeReact.Reducer.update(action, state) =>
  switch (action) {
  | FetchCountriesSuccess(countries) =>
    Update({
      ...state,
      options: Some(countries),
      filteredOptions: Some(countries),
    })

  | FetchCountriesFailure(error) =>
    SideEffect(_ => Js.log(ReludeFetch.Error.show(e => e, error)))

  | SetCountry(country) => Update({...state, selectedCountry: country})

  | SelectCountry(country, callback) =>
    UpdateWithSideEffect(
      {
        ...state,
        selectedCountry: Some(country),
        focusedSection: MenuClosed(`Focused),
      },
      _ => callback(country.value),
    )

  | ChangeFilter(filter) =>
    let filteredOptions = {
      switch (state.options) {
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

    let focusedSection =
      switch (filteredOptions) {
      | None => Types.VisualState.MenuOpened(`Options)
      | _ => state.focusedSection
      };

    Update({...state, filter, filteredOptions, focusedSection});

  | ToggleMenu =>
    Update(
      switch (state.focusedSection) {
      | MenuClosed(`Unfocused)
      | MenuClosed(`Focused) => {
          ...state,
          focusedSection: MenuOpened(`Options),
        }
      | MenuOpened(`Options)
      | MenuOpened(`HighlightedOption(_)) => {
          ...state,
          focusedSection: MenuClosed(`Focused),
        }
      },
    )

  | SetFocusedSection(section) => Update({...state, focusedSection: section})

  | SetRootRef(ref_) => Update({...state, rootRef: Some(ref_)})

  | Blur =>
    Update({
      ...state,
      focusedSection: MenuClosed(`Unfocused),
      filter: "",
      filteredOptions: state.options,
    })

  | HighlightOption(index) =>
    Update({
      ...state,
      focusedSection: MenuOpened(`HighlightedOption(index)),
    })

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
      {options, filteredOptions, selectedCountry, filter, focusedSection}: state,
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

    let onChangeCountry =
      React.useCallback0((country: Types.Option.t) =>
        SelectCountry(country, onChange)->send
      );

    let menuOpened =
      switch (focusedSection) {
      | MenuOpened(`Options)
      | MenuOpened(`HighlightedOption(_)) => true
      | MenuClosed(_) => false
      };

    let buttonFocused =
      switch (focusedSection) {
      | MenuClosed(`Focused) => true
      | MenuClosed(`Unfocused)
      | MenuOpened(`Options)
      | MenuOpened(`HighlightedOption(_)) => false
      };

    let highlightedIndex =
      switch (focusedSection) {
      | MenuOpened(`HighlightedOption(index)) => Some(index)
      | MenuOpened(`Options)
      | MenuClosed(_) => None
      };

    let highlightOption = index => {
      switch (filteredOptions) {
      | None => NoOp
      | Some(options) =>
        let maxIndex = Array.size(options) - 1;

        if (index < 0) {
          HighlightOption(0);
        } else if (index > maxIndex) {
          HighlightOption(maxIndex);
        } else {
          HighlightOption(index);
        };
      };
    };

    let onButtonKeyDown = (key: Types.KeyboardButton.t) => {
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

    let onFilterKeyDown = (key: Types.KeyboardButton.t) => {
      let action =
        switch (key) {
        | ArrowUp => ToggleMenu
        | ArrowDown
        | Enter =>
          switch (filteredOptions) {
          | None => NoOp
          | Some(_) => highlightOption(0)
          }
        | Tab => Blur
        | Escape => ChangeFilter("")
        | PageUp
        | PageDown
        | Space
        | Unsupported => NoOp
        };

      send(action);
    };

    let onOptionKeyDown =
        (key: Types.KeyboardButton.t, options, highlightedIndex: int) => {
      let action =
        switch (key) {
        | ArrowUp when highlightedIndex == 0 =>
          SetFocusedSection(MenuOpened(`Options))
        | ArrowUp => highlightOption(highlightedIndex - 1)
        | ArrowDown => highlightOption(highlightedIndex + 1)
        | Space
        | Tab
        | Enter =>
          switch (options[highlightedIndex]) {
          | Some(country) => SelectCountry(country, onChange)
          | None => NoOp
          }
        | PageUp => highlightOption(highlightedIndex - 4)
        | PageDown => highlightOption(highlightedIndex + 4)
        | Escape => Blur
        | Unsupported => NoOp
        };

      send(action);
    };

    let onKeyDown = (event: ReactEvent.Keyboard.t): unit => {
      let key = Utils.ReactDom.keyFromEvent(event);

      switch (focusedSection) {
      | MenuClosed(`Focused) => onButtonKeyDown(key)
      | MenuOpened(`Options) => onFilterKeyDown(key)
      | MenuOpened(`HighlightedOption(highlightedIndex)) =>
        switch (filteredOptions) {
        | Some(options) => onOptionKeyDown(key, options, highlightedIndex)
        | None => ()
        }
      | MenuClosed(`Unfocused) => ()
      };
    };

    let className = Styles.root ++? className;

    <div ref={ReactDOM.Ref.domRef(rootRef)} className>
      {switch (options) {
       | None =>
         <CountrySelectButton
           text=Text.loading
           opened=false
           focused=buttonFocused
           onClick=ignore
           onFocus=ignore
           onKeyDown=ignore
         />
       | Some(_loadedOptions) =>
         <>
           <CountrySelectButton
             text=Option.(
               map(selectedCountry, c => c.label)
               ->getWithDefault(Text.selectCountry)
             )
             opened=menuOpened
             focused=buttonFocused
             onClick={() => ToggleMenu->send}
             onFocus={() => SetFocusedSection(MenuClosed(`Focused))->send}
             onKeyDown
           />
           {menuOpened
            &&& <div
                  className=Styles.menuWrapper
                  onClick=ReactEvent.Mouse.stopPropagation>
                  <CountrySelectFilter
                    onKeyDown
                    value=filter
                    onChange={str => ChangeFilter(str)->send}
                    onFocus={() =>
                      SetFocusedSection(MenuOpened(`Options))->send
                    }
                  />
                  {switch (filteredOptions) {
                   | Some(filteredOptions) =>
                     <CountrySelectOptions.Memo
                       options=filteredOptions
                       selectedCountry
                       onChangeCountry
                       highlightedIndex
                     />
                   | None => <CountrySelectOptions.CountryNotFound />
                   }}
                </div>}
         </>
       }}
    </div>;
  };
};
