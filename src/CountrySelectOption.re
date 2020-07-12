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
};

let component: ReactSelect.CustomComponent.t =
  props =>
    <div className=Styles.wrapper onClick={props.innerProps.onClick}>
      <FlagIconCss countryCode={props.value} />
      <p className=Styles.paragraph> {React.string(props.label)} </p>
    </div>;
