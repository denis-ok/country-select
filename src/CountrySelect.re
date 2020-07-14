open Belt;
open Utils.React;

module Types = CountrySelectTypes;

module Text = {
  let loading = "Loading...";
  let selectCountry = "Select Country";
};

type state = {
  options: option(array(Types.Option.t)),
  selectedCountry: option(Types.Option.t),
  filter: string,
  menuOpened: bool,
  focusedElement: option(Types.Element.t),
};

let initialState = {
  options: None,
  selectedCountry: None,
  filter: "",
  menuOpened: false,
  focusedElement: None,
};

type action =
  | FetchCountriesSuccess(array(Types.Option.t))
  | FetchCountriesFailure(ReludeFetch.Error.t(string))
  | SetCountry(option(Types.Option.t))
  | ChangeCountry(Types.Option.t, string => unit)
  | SetFilter(string)
  | SetFocusedElement(Types.Element.t)
  | ToggleMenu;

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

  | SetFocusedElement(element) =>
    Update({...state, focusedElement: Some(element)})
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
      {options, selectedCountry, filter, menuOpened, focusedElement}: state,
      send,
    ) =
      ReludeReact.Reducer.useReducer(reducer, initialState);

    ReludeReact.Effect.useIOOnMount(
      Request.getCountriesIO(optionsUrl),
      options => FetchCountriesSuccess(options)->send,
      error => FetchCountriesFailure(error)->send,
    );

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
      SetFocusedElement(Types.Element.Button)->send;
    };

    let onFocusFilter = () => {
      SetFocusedElement(Types.Element.Filter)->send;
    };

    let onFocusList = () => {
      switch (focusedElement) {
      | Some(Options) => ()
      | _ => SetFocusedElement(Types.Element.Options)->send
      };
    };

    let className = Option.getWithDefault(className, "");

    switch (options) {
    | None =>
      <CountrySelectDropdownButton
        text=Text.loading
        onClick=ignore
        opened=false
        onFocus=onFocusButton
      />
    | Some(options) =>
      let filteredOptions = Utils.filterOptions(options, filter);

      <div className>
        <CountrySelectDropdownButton
          text=Option.(
            map(selectedCountry, c => c.label)
            ->getWithDefault(Text.selectCountry)
          )
          onClick=toggleMenu
          opened=menuOpened
          onFocus=onFocusButton
        />
        {menuOpened
         &&& <CountrySelectMenu.Wrapper>
               <CountrySelectSearchFilter
                 value=filter
                 onChange=onChangeFilter
                 onFocus=onFocusFilter
               />
               <CountrySelectMenu.CountryList
                 options=filteredOptions
                 onChangeCountry
                 onFocus=onFocusList
               />
             </CountrySelectMenu.Wrapper>}
      </div>;
    };
  };
};

include Functor(CountrySelectAPI.Request);
