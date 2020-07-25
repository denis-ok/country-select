module Styles = {
  open Css;

  module Const = CountrySelectConstants.Style;

  let paragraph =
    style([
      boxSizing(borderBox),
      margin(px(0)),
      marginLeft(px(8)),
      fontSize(Const.fontSizePx),
      fontFamily(Const.fontFamily),
      maxWidth(px(180)),
    ]);

  let focusStyle = [
    backgroundColor(Const.Color.activeItemBackground),
    borderWidth(px(0)),
    outlineWidth(px(0)),
  ];

  let wrapper =
    style([
      boxSizing(borderBox),
      display(flexBox),
      minHeight(px(Const.Size.menuOptionHeightPx)),
      flexDirection(`row),
      alignItems(`center),
      paddingLeft(px(12)),
      hover([backgroundColor(Const.Color.hover), cursor(pointer)]),
      focus(focusStyle),
    ]);

  let focused = style(focusStyle);

  let selected = style([fontWeight(`bold)]);
};

open Styles;

[@react.component]
let make =
    (
      ~isFocused: bool,
      ~isSelected: bool,
      ~value: string,
      ~label: string,
      ~onClick: unit => unit,
    ) => {
  let wrapperClass =
    switch (isFocused, isSelected) {
    | (true, true) => {j|$(wrapper) $(focused) $(selected)|j}
    | (false, true) => {j|$(wrapper) $(selected)|j}
    | (true, false) => {j|$(wrapper) $(focused)|j}
    | (false, false) => Styles.wrapper
    };

  let rootRef: React.ref(Js.Nullable.t(Dom.element)) =
    React.useRef(Js.Nullable.null);

  React.useEffect1(
    () => {
      if (isFocused) {
        Utils.ReactDom.scrollIntoViewIfNeeded(rootRef);
      };

      None;
    },
    [|isFocused|],
  );

  <div
    ref={ReactDOM.Ref.domRef(rootRef)}
    tabIndex=0
    className=wrapperClass
    onClick={Functions.omit(onClick)}
    role="option">
    <FlagIconCss countryCode=value />
    <p className=Styles.paragraph> {React.string(label)} </p>
  </div>;
};
