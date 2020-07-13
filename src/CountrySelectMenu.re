module Styles = {
  open Css;

  let wrapper =
    style([
      boxSizing(borderBox),
      position(`absolute),
      left(px(-2)),
      top(px(40)),
      paddingTop(px(38)),
      width(px(230)),
      height(px(200)),
      borderStyle(`solid),
      borderColor(`rgba((0, 0, 0, 0.08))),
      borderRadius(px(2)),
      borderWidth(px(1)),
      boxShadow(
        Shadow.box(
          ~x=px(0),
          ~y=px(1),
          ~blur=px(3),
          `rgba((0, 0, 0, 0.08)),
        ),
      ),
    ]);
};

let component: ReactSelect.CustomComponent.t =
  props =>
    <div ref={props.innerRef} className=Styles.wrapper>
      {props.children}
    </div>;
