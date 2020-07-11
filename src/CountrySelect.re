let placeholderLoading = React.string("Loading...");

let placeholder = React.string("Select Country");

module Functor = (Request: CountrySelectAPI.Request) => {
  [@react.component]
  let make = (~optionsUrl: option(string)=?) => {
    let (options, setOptions) = React.useState(() => None);

    ReludeReact.Effect.useIOOnMount(
      Request.getCountriesIO(optionsUrl),
      options => setOptions(_ => Some(options)),
      error => Js.log(ReludeFetch.Error.show(e => e, error)),
    );

    switch (options) {
    | None =>
      <ReactSelect
        isLoading=true
        isDisabled=true
        placeholder=placeholderLoading
        options=[||]
      />
    | Some(options) => <ReactSelect options placeholder />
    };
  };
};

include Functor(CountrySelectAPI.Request);
