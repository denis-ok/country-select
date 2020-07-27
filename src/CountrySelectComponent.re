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
  visualState: Types.VisualState.t,
  rootRef: option(React.ref(Js.Nullable.t(Dom.element))),
};

let initialState = {
  options: None,
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
  | SetVisualState(Types.VisualState.t)
  | SetRootRef(React.ref(Js.Nullable.t(Dom.element)))
  | ToggleMenu
  | Blur
  | HighlightOption(int)
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

    let visualState: Types.VisualState.t =
      switch (filteredOptions) {
      | Some(countries) => MenuOpened(`Options((countries, None)))
      | None => MenuOpened(`NoOptions)
      };

    Update({...state, filter, visualState});

  | ToggleMenu =>
    Update(
      switch (state.visualState) {
      | MenuClosed(`Unfocused)
      | MenuClosed(`Focused) => {
          ...state,
          filter: "",
          visualState:
            switch (state.options) {
            | Some(options) => MenuOpened(`Options((options, None)))
            | None => state.visualState
            },
        }
      | MenuOpened(`Options(_))
      | MenuOpened(`NoOptions) => {
          ...state,
          filter: "",
          visualState: MenuClosed(`Focused),
        }
      },
    )

  | SetVisualState(section) => Update({...state, visualState: section})

  | SetRootRef(ref_) => Update({...state, rootRef: Some(ref_)})

  | Blur =>
    Update({...state, visualState: MenuClosed(`Unfocused), filter: ""})

  | HighlightOption(index) =>
    switch (state.visualState) {
    | MenuOpened(`Options(options, _)) =>
      let maxIndex = Array.size(options) - 1;

      let highlightedIndex =
        if (index < 0) {
          0;
        } else if (index > maxIndex) {
          maxIndex;
        } else {
          index;
        };

      Update({
        ...state,
        visualState:
          MenuOpened(`Options((options, Some(highlightedIndex)))),
      });
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
    let ({options, selectedCountry, filter, visualState}: state, send) =
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
      | MenuOpened(`NoOptions) => true
      | MenuClosed(`Focused)
      | MenuClosed(`Unfocused) => false
      };

    let buttonFocused =
      switch (visualState) {
      | MenuClosed(`Focused) => true
      | MenuClosed(`Unfocused)
      | MenuOpened(`Options(_))
      | MenuOpened(`NoOptions) => false
      };

    let onFocusedButtonKeyDown = (key: Types.KeyboardButton.t) => {
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
        | Enter => HighlightOption(0)
        | Tab => Blur
        | Escape => ChangeFilter("")
        | PageUp
        | PageDown
        | Space
        | Unsupported => NoOp
        };

      send(action);
    };

    let onFilterWithHighlightedOptionKeyDown =
        (
          key: Types.KeyboardButton.t,
          options: array(CountrySelectTypes.Option.t),
          highlightedIndex: int,
        ) => {
      let action =
        switch (key) {
        | ArrowUp when highlightedIndex == 0 =>
          SetVisualState(MenuOpened(`Options((options, None))))
        | ArrowUp => HighlightOption(highlightedIndex - 1)
        | ArrowDown => HighlightOption(highlightedIndex + 1)
        | Space
        | Tab
        | Enter =>
          switch (options[highlightedIndex]) {
          | Some(country) => SelectCountry(country, onChange)
          | None => NoOp
          }
        | PageUp => HighlightOption(highlightedIndex - 4)
        | PageDown => HighlightOption(highlightedIndex + 4)
        | Escape => Blur
        | Unsupported => NoOp
        };

      send(action);
    };

    let onKeyDown = (event: ReactEvent.Keyboard.t): unit => {
      let key = Utils.ReactDom.keyFromEvent(event);

      switch (visualState) {
      | MenuClosed(`Focused) => onFocusedButtonKeyDown(key)
      | MenuClosed(`Unfocused) => ()
      | MenuOpened(`Options(_, None))
      | MenuOpened(`NoOptions) => onFilterKeyDown(key)
      | MenuOpened(`Options(options, Some(highlightedIndex))) =>
        onFilterWithHighlightedOptionKeyDown(key, options, highlightedIndex)
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
             onFocus={() => SetVisualState(MenuClosed(`Focused))->send}
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
                      SetVisualState(
                        MenuOpened(`Options((loadedOptions, None))),
                      )
                      ->send
                    }
                  />
                  {switch (visualState) {
                   | MenuOpened(`Options(options, highlightedIndex)) =>
                     <CountrySelectOptions.Memo
                       options
                       selectedCountry
                       onChangeCountry
                       highlightedIndex
                     />
                   | MenuOpened(`NoOptions) =>
                     <CountrySelectOptions.CountryNotFound />
                   | _ => React.null
                   }}
                </div>}
         </>
       }}
    </div>;
  };
};
