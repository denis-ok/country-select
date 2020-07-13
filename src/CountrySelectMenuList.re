module Styles = {
  open Css;

  let wrapper =
    style([
      width(`percent(100.0)),
      height(`percent(100.0)),
      overflowX(`hidden),
      overflowY(`auto),
    ]);
};

let component: ReactSelect.CustomComponent.t =
  props =>
    <div ref={props.innerRef} className=Styles.wrapper>
      {props.children}
    </div>;
