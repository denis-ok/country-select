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
      | Some(Button) => {...state, focusedSection: Some(Filter)}
      | Some(Filter)
      | Some(Options(_)) => {...state, focusedSection: Some(Button)}
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

module Functor = (Request: CountrySelectAPI.Request) => {
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

    let focusOption = newIndex => {
      switch (filteredOptions) {
      | None => NoOp
      | Some(options) =>
        let maxIndex = Array.size(options) - 1;

        if (newIndex < 0) {
          FocusOption(0);
        } else if (newIndex > maxIndex) {
          FocusOption(maxIndex);
        } else {
          FocusOption(newIndex);
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
          | None => SetFocusedSection(Button)
          | Some(_) => focusOption(0)
          }
        | Tab
        | Escape => Blur
        | PageUp
        | PageDown
        | Space
        | Unsupported => NoOp
        };

      send(action);
    };

    let onOptionKeyDown =
        (key: Types.KeyboardButton.t, options, focusedIndex: int) => {
      let action =
        switch (key) {
        | ArrowUp when focusedIndex == 0 => SetFocusedSection(Filter)
        | ArrowUp => focusOption(focusedIndex - 1)
        | ArrowDown => focusOption(focusedIndex + 1)
        | Space
        | Tab
        | Enter =>
          switch (options[focusedIndex]) {
          | Some(country) => SelectCountry(country, onChange)
          | None => NoOp
          }
        | PageUp => focusOption(focusedIndex - 4)
        | PageDown => focusOption(focusedIndex + 4)
        | Escape => Blur
        | Unsupported => NoOp
        };

      send(action);
    };

    let onKeyDown = (event: ReactEvent.Keyboard.t): unit => {
      let key = Utils.ReactDom.keyFromEvent(event);

      switch (focusedSection) {
      | Some(Button) => onButtonKeyDown(key)
      | Some(Filter) => onFilterKeyDown(key)
      | Some(Options(focusedIndex)) =>
        switch (filteredOptions) {
        | Some(options) => onOptionKeyDown(key, options, focusedIndex)
        | None => ()
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
             onClick=toggleMenu
             onFocus=onFocusButton
             onKeyDown
           />
           {menuOpened
            &&& <div
                  className=Styles.menuWrapper
                  onClick=ReactEvent.Mouse.stopPropagation>
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
