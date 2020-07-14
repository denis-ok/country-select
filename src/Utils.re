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

  let focusRef = (inputRef: React.ref(Js.Nullable.t(Dom.element))) => {
    inputRef.current
    ->Js.Nullable.toOption
    ->Belt.Option.map(ref => ReactDOMRe.domElementToObj(ref))
    ->Belt.Option.map(node => Js.Global.setTimeout(() => node##focus(), 10))
    ->ignore;
  };

  let focusOptRef = ref_ =>
    switch (ref_) {
    | None => ()
    | Some(ref_) => focusRef(ref_)
    };
};

module Dom = {
  let getStringValueFromEvent = (event: ReactEvent.Form.t) =>
    event->ReactEvent.Form.target##value->Belt.Option.getWithDefault("");

  let keyFromEvent =
    ReactEvent.Keyboard.key >> CountrySelectTypes.KeyboardButton.fromString;
};
