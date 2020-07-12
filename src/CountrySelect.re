open Belt;

let placeholderLoading = React.string("Loading...");

let placeholder = React.string("Select Country");

let filterOptions = (options: array(ReactSelect.Option.t), filterString) => {
  let normalizedFilterString =
    Js.String.trim(filterString)->Js.String.toLowerCase;

  if (Js.String.length(normalizedFilterString) == 0) {
    options;
  } else {
    options->Array.keep(({value, label}) =>
      label->Js.String.toLowerCase->Js.String.includes(normalizedFilterString)
      || value
         ->Js.String.toLowerCase
         ->Js.String.includes(normalizedFilterString)
    );
  };
};

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

    ReludeReact.Effect.useIOOnMount(
      Request.getCountriesIO(optionsUrl),
      options => setOptions(_ => Some(options)),
      error => Js.log(ReludeFetch.Error.show(e => e, error)),
    );

    React.useEffect2(
      () => {
        switch (options, country) {
        | (Some(options), Some(country)) =>
          let mbSelectedValue =
            options->Array.getBy(opt => opt.value == country);

          setSelectedCountry(_ => mbSelectedValue);
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
          onChange={str => setFilterString(_ => str)}
        />
        <ReactSelect
          isLoading=false
          isDisabled=false
          isSearchable=false
          onChange=onChangeCountry
          options={filterOptions(options, filterString)}
          placeholder
          value=?selectedCountry
        />
      </>
    };
  };
};

include Functor(CountrySelectAPI.Request);
