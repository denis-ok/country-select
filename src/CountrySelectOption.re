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
    backgroundColor(Const.Color.menuItemFocused),
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
      ~onFocus: unit => unit,
    ) => {
  let wrapperClass =
    switch (isFocused, isSelected) {
    | (true, true) => {j|$(wrapper) $(focused) $(selected)|j}
    | (false, true) => {j|$(wrapper) $(selected)|j}
    | (true, false) => {j|$(wrapper) $(focused)|j}
    | (false, false) => Styles.wrapper
    };

  let onClick = (event: ReactEvent.Mouse.t) => {
    ReactEvent.Mouse.preventDefault(event);
    onClick();
  };

  let onFocus = event => {
    ReactEvent.Focus.preventDefault(event);
    let target = ReactEvent.Focus.target(event);
    onFocus();

    Js.log2("Target:", target);
  };

  <div tabIndex=0 className=wrapperClass onClick onFocus role="option">
    <FlagIconCss countryCode=value />
    <p className=Styles.paragraph> {React.string(label)} </p>
  </div>;
};
