open Belt;
open Utils.React;

module Text = {
  let loading = "Loading...";
  let selectCountry = "Select Country";
};

type state = {
  options: option(array(CountrySelectTypes.Option.t)),
  selectedCountry: option(CountrySelectTypes.Option.t),
  filter: string,
  menuOpened: bool,
};

let initialState = {
  options: None,
  selectedCountry: None,
  filter: "",
  menuOpened: false,
};

type action =
  | FetchCountriesSuccess(array(CountrySelectTypes.Option.t))
  | FetchCountriesFailure(ReludeFetch.Error.t(string))
  | SetCountry(option(CountrySelectTypes.Option.t))
  | ChangeCountry(CountrySelectTypes.Option.t, string => unit)
  | SetFilter(string)
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
    let ({options, selectedCountry, filter, menuOpened}: state, send) =
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

    let onChangeCountry = (country: CountrySelectTypes.Option.t) => {
      ChangeCountry(country, onChange)->send;
    };

    let className = Option.getWithDefault(className, "");

    switch (options) {
    | None =>
      <CountrySelectDropdownButton
        text=Text.loading
        onClick=ignore
        opened=false
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
        />
        {menuOpened
         &&& <CountrySelectMenu.Wrapper>
               <CountrySelectSearchFilter
                 value=filter
                 onChange=onChangeFilter
               />
               <CountrySelectMenu.CountryList
                 options=filteredOptions
                 onChangeCountry
               />
             </CountrySelectMenu.Wrapper>}
      </div>;
    };
  };
};

include Functor(CountrySelectAPI.Request);
