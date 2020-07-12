module Styles = {
  open Css;

  let paragraph =
    style([
      margin(px(0)),
      marginLeft(px(8)),
      fontSize(px(14)),
      fontFamily(`custom("Arial")),
    ]);

  let wrapper =
    style([
      boxSizing(borderBox),
      display(flexBox),
      width(px(230)),
      height(px(26)),
      flexDirection(`row),
      alignItems(`center),
      paddingLeft(px(12)),
      hover([backgroundColor(`rgb((245, 245, 245))), cursor(pointer)]),
    ]);

  let focused = style([backgroundColor(`rgb((235, 235, 235)))]);

  let selected = style([fontWeight(`bold)]);
};

open Styles;

let component: ReactSelect.CustomComponent.t =
  ({isFocused, isSelected, innerProps, innerRef, value, label}) => {
    let wrapperClass =
      switch (isFocused, isSelected) {
      | (true, true) => {j|$(wrapper) $(focused) $(selected)|j}
      | (false, true) => {j|$(wrapper) $(selected)|j}
      | (true, false) => {j|$(wrapper) $(focused)|j}
      | (false, false) => Styles.wrapper
      };

    <div ref=innerRef className=wrapperClass onClick={innerProps.onClick}>
      <FlagIconCss countryCode=value />
      <p className=Styles.paragraph> {React.string(label)} </p>
    </div>;
  };
