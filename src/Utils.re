let getStringValueFromEvent = (event: ReactEvent.Form.t) =>
  event->ReactEvent.Form.target##value->Belt.Option.getWithDefault("");

module String = {
  let normalizeString = string =>
    Js.String.trim(string)->Js.String.toLowerCase;

  let hasSubstring = (~search, string) =>
    Relude.String.toLowerCase(string) |> Relude.String.contains(~search);
};

module React = {
  let (&&&) = (condition: bool, element: React.element) =>
    if (condition) {element} else {React.null};
};
