open Functions;

module String = {
  let normalizeString = string =>
    Js.String.trim(string)->Js.String.toLowerCase;

  let hasSubstring = (~search, string) =>
    Relude.String.toLowerCase(string) |> Relude.String.contains(~search);
};

let filterOptions =
    (options: array(CountrySelectTypes.Option.t), filter: string) => {
  let searchSubstring = String.normalizeString(filter);

  let hasSubstring = String.hasSubstring(~search=searchSubstring);

  if (Relude.String.isEmpty(searchSubstring)) {
    options;
  } else {
    options->Belt.Array.keep(({value, label}) =>
      value->hasSubstring || label->hasSubstring
    );
  };
};

module React = {
  let (&&&) = (condition: bool, element: React.element) =>
    if (condition) {element} else {React.null};
};

module Dom = {
  let getStringValueFromEvent = (event: ReactEvent.Form.t) =>
    event->ReactEvent.Form.target##value->Belt.Option.getWithDefault("");

  let keyFromEvent =
    ReactEvent.Keyboard.key >> CountrySelectTypes.KeyboardButton.fromString;
};
