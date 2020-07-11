module D = Decode.AsResult.OfParseError;

module Response = {
  let ((<$>), (<*>)) = D.(map, apply);

  let make = (label, value): ReactSelect.Option.t => {label, value};

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

module Request = {
  open Relude.IO;

  let getCountriesIO = url =>
    ReludeFetch.get(url)
    >>= ReludeFetch.Response.StatusCode.ensure2xx
    >>= ReludeFetch.Response.json
    >>= Response.decode(url)
    |> Relude.IO.withDelay(2000);
};
