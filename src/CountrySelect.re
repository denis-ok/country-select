open Belt;

let placeholderLoading = React.string("Loading...");

let placeholder = React.string("Select Country");

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
        placeholder=placeholderLoading
        options=[||]
      />
    | Some(options) =>
      <ReactSelect
        options
        placeholder
        value=?selectedCountry
        onChange=onChangeCountry
      />
    };
  };
};

include Functor(CountrySelectAPI.Request);
