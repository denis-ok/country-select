open Belt;
open Functions;

module String = Utils.String;

let placeholderLoading = React.string("Loading...");

let placeholder = React.string("Select Country");

let filterOptions = (options: array(ReactSelect.Option.t), filterString) => {
  let searchSubstring = String.normalizeString(filterString);

  let hasSubstring = String.hasSubstring(~search=searchSubstring);

  if (Relude.String.isEmpty(searchSubstring)) {
    options;
  } else {
    options->Array.keep(({value, label}) =>
      value->hasSubstring || label->hasSubstring
    );
  };
};

module CustomOption = {
  let component: ReactSelect.CustomComponent.t =
    props =>
      <div onClick={props.innerProps.onClick}>
        {React.string(props.value)}
      </div>;
};

let components: ReactSelect.Components.t = {option: CustomOption.component};

module Functor = (Request: CountrySelectAPI.Request) => {
  [@react.component]
  let make =
      (
        ~country: option(string),
        ~onChange: string => unit,
        ~optionsUrl: option(string)=?,
      ) => {
    let (options, setOptions) = React.useState(() => None);

    let (selectedCountry: option(ReactSelect.Option.t), setSelectedCountry) =
      React.useState(() => None);

    let (filterString, setFilterString) = React.useState(() => "");

    let onChangeFilterString = str => setFilterString(const(str));

    let (menuOpened, toggleMenu) = React.useState(() => false);

    let toggleMenu = () => toggleMenu(opened => !opened);

    ReludeReact.Effect.useIOOnMount(
      Request.getCountriesIO(optionsUrl),
      options => setOptions(const(Some(options))),
      error => Js.log(ReludeFetch.Error.show(e => e, error)),
    );

    React.useEffect2(
      () => {
        switch (options, country) {
        | (Some(options), Some(country)) =>
          let mbSelectedValue =
            options->Array.getBy(opt => opt.value == country);

          setSelectedCountry(const(mbSelectedValue));
        | _ => ()
        };
        None;
      },
      (options, country),
    );

    let onChangeCountry = (selectedCountry: ReactSelect.Option.t, _) => {
      onChange(selectedCountry.value);
      setSelectedCountry(_ => Some(selectedCountry));
    };

    let onBlur = omit(toggleMenu);

    let onFocus = onBlur;

    switch (options) {
    | None =>
      <ReactSelect
        isLoading=true
        isDisabled=true
        isSearchable=false
        placeholder=placeholderLoading
        options=[||]
      />
    | Some(options) =>
      <>
        <CountrySelectSearchFilter
          value=filterString
          onChange=onChangeFilterString
        />
        <ReactSelect
          blurInputOnSelect=true
          isLoading=false
          isDisabled=false
          isSearchable=false
          menuIsOpen=menuOpened
          onBlur
          onFocus
          onChange=onChangeCountry
          options={filterOptions(options, filterString)}
          placeholder
          value=?selectedCountry
          components
        />
      </>
    };
  };
};

include Functor(CountrySelectAPI.Request);
