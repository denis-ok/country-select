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

module ReactDom = {
  [@bs.val] [@bs.scope ("window", "document")]
  external _addEventListener: (string, 'a => unit, bool) => unit =
    "addEventListener";

  [@bs.val] [@bs.scope ("window", "document")]
  external _removeEventListener: (string, 'a => unit, bool) => unit =
    "removeEventListener";

  let addMouseDownListener = (handler: ReactEvent.Mouse.t => unit) =>
    _addEventListener("mousedown", handler, false);

  let removeMouseDownListener = (handler: ReactEvent.Mouse.t => unit) =>
    _removeEventListener("mousedown", handler, false);

  let getStringValueFromEvent = (event: ReactEvent.Form.t) =>
    event->ReactEvent.Form.target##value->Belt.Option.getWithDefault("");

  let keyFromEvent =
    ReactEvent.Keyboard.key >> CountrySelectTypes.KeyboardButton.fromString;

  let contains: (Dom.element, Js.t({..})) => bool =
    (parent, child) => ReactDOM.domElementToObj(parent)##contains(child);

  let useClickOutside =
      (
        rootRef: React.ref(Js.Nullable.t(Dom.element)),
        handler: unit => unit,
      ) => {
    let onDocumentClick = event => {
      let target = ReactEvent.Mouse.target(event);

      switch (rootRef.current->Js.Nullable.toOption) {
      | Some(node) when contains(node, target) => ()
      | _ => handler()
      };
    };

    React.useEffect0(() => {
      addMouseDownListener(onDocumentClick);
      Some(() => removeMouseDownListener(onDocumentClick));
    });
  };

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

  let extractDomElementFromRef = (reactRef: React.ref(Js.Nullable.t('a))) =>
    reactRef.current->Js.Nullable.toOption;
};

module Infix = {
  let (+++) = (x, y) => x ++ " " ++ y;

  let (++?) = (x, y) =>
    switch (y) {
    | Some(y) => x +++ y
    | None => x
    };

  let (&&&) = (condition: bool, element: React.element) =>
    if (condition) {element} else {React.null};
};
