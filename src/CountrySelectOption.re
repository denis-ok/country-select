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
      maxWidth(px(180))
    ]);

  let wrapper =
    style([
      boxSizing(borderBox),
      display(flexBox),
      width(px(Const.Size.menuWidthPx)),
      minHeight(px(Const.Size.menuOptionHeightPx)),
      flexDirection(`row),
      alignItems(`center),
      paddingLeft(px(12)),
      hover([backgroundColor(Const.Color.hover), cursor(pointer)]),
    ]);

  let focused = style([backgroundColor(Const.Color.menuItemFocused)]);

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

  let onClick = (event: ReactEvent.Mouse.t) => {
    ReactEvent.Mouse.preventDefault(event);
    onClick();
  };

  <div className=wrapperClass onClick>
    <FlagIconCss countryCode=value />
    <p className=Styles.paragraph> {React.string(label)} </p>
  </div>;
};
