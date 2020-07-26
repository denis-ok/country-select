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
  visualState: Types.VisualState.t,
  rootRef: option(React.ref(Js.Nullable.t(Dom.element))),
};

let initialState = {
  options: None,
  filteredOptions: None,
  selectedCountry: None,
  filter: "",
  visualState: MenuClosed(`Unfocused),
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
        visualState: MenuClosed(`Focused),
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

    let visualState =
      switch (filteredOptions) {
      | None => Types.VisualState.MenuOpened(`NoOptions)
      | Some(countries) => Types.VisualState.MenuOpened(`Options(countries))
      };

    Update({...state, filter, filteredOptions, visualState});

  | ToggleMenu =>
    Update(
      switch (state.visualState) {
      | MenuClosed(`Unfocused)
      | MenuClosed(`Focused) => {
          ...state,
          visualState:
            switch (state.options) {
            | Some(options) => MenuOpened(`Options(options))
            | None => state.visualState
            },
        }
      | MenuOpened(`Options(_))
      | MenuOpened(`NoOptions)
      | MenuOpened(`OptionsWithHighlight(_)) => {
          ...state,
          visualState: MenuClosed(`Focused),
        }
      },
    )

  | SetFocusedSection(section) => Update({...state, visualState: section})

  | SetRootRef(ref_) => Update({...state, rootRef: Some(ref_)})

  | Blur =>
    Update({
      ...state,
      visualState: MenuClosed(`Unfocused),
      filter: "",
      filteredOptions: state.options,
    })

  | HighlightOption(index) =>
    switch (state.visualState) {
    | MenuOpened(`Options(options))
    | MenuOpened(`OptionsWithHighlight(options, _)) =>
      Update({
        ...state,
        visualState: MenuOpened(`OptionsWithHighlight((options, index))),
      })

    | _ => NoUpdate
    }

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
      {options, filteredOptions, selectedCountry, filter, visualState}: state,
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
      switch (visualState) {
      | MenuOpened(`Options(_))
      | MenuOpened(`NoOptions)
      | MenuOpened(`OptionsWithHighlight(_)) => true
      | MenuClosed(`Focused)
      | MenuClosed(`Unfocused) => false
      };

    let buttonFocused =
      switch (visualState) {
      | MenuClosed(`Focused) => true
      | MenuClosed(`Unfocused)
      | MenuOpened(`Options(_))
      | MenuOpened(`NoOptions)
      | MenuOpened(`OptionsWithHighlight(_)) => false
      };

    let highlightedIndex =
      switch (visualState) {
      | MenuOpened(`OptionsWithHighlight(_, index)) => Some(index)
      | MenuOpened(`Options(_))
      | MenuOpened(`NoOptions)
      | MenuClosed(`Focused)
      | MenuClosed(`Unfocused) => None
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
          SetFocusedSection(MenuOpened(`Options(options)))
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

      switch (visualState) {
      | MenuClosed(`Focused) => onButtonKeyDown(key)
      | MenuOpened(`Options(_))
      | MenuOpened(`NoOptions) => onFilterKeyDown(key)
      | MenuOpened(`OptionsWithHighlight(_, highlightedIndex)) =>
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
       | Some(loadedOptions) =>
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
                      SetFocusedSection(MenuOpened(`Options(loadedOptions)))
                      ->send
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
