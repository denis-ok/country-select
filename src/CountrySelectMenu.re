module Styles = {
  open Css;

  let wrapper = style([paddingTop(px(36))]);
};

let component: ReactSelect.CustomComponent.t =
  props =>
    <div className=Styles.wrapper>
      {ReactSelect.DefaultComponents.menu(props)}
    </div>;
