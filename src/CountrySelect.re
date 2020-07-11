let optionsUrl = "https://gist.githubusercontent.com/rusty-key/659db3f4566df459bd59c8a53dc9f71f/raw/4127f9550ef063121c564025f6d27dceeb279623/counties.json";

let placeholderLoading = React.string("Loading...");

let placeholder = React.string("Select Country");

[@react.component]
let make = () => {
  let (options, setOptions) = React.useState(() => None);

  ReludeReact.Effect.useIOOnMount(
    CountrySelectAPI.Request.getCountriesIO(optionsUrl),
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
