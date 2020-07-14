module D = Decode.AsResult.OfParseError;

module Response = {
  let ((<$>), (<*>)) = D.(map, apply);

  let make = (label, value): CountrySelectTypes.Option.t => {label, value};

  let decodeOne =
    make <$> D.field("label", D.string) <*> D.field("value", D.string);

  let decodeMany = D.array(decodeOne);

  let decode = (url, json) => {
    decodeMany(json)
    |> Relude.IO.fromResult
    |> Relude.IO.mapRight(e => {
         let readableError = D.ParseError.failureToDebugString(e);
         ReludeFetch.Error.decodeError({url, innerError: readableError});
       });
  };
};

module type Request = {
  let getCountriesIO:
    option(string) =>
    Relude.IO.t(
      array(CountrySelectTypes.Option.t),
      ReludeFetch.Error.t(string),
    );
};

module Request: Request = {
  open Relude.IO;

  let optionsUrl = "https://gist.githubusercontent.com/rusty-key/659db3f4566df459bd59c8a53dc9f71f/raw/4127f9550ef063121c564025f6d27dceeb279623/counties.json";

  let getCountriesIO = (url: option(string)) => {
    let url = Belt.Option.getWithDefault(url, optionsUrl);

    ReludeFetch.get(url)
    >>= ReludeFetch.Response.StatusCode.ensure2xx
    >>= ReludeFetch.Response.json
    >>= Response.decode(url)
    |> Relude.IO.withDelay(500);
  };
};
