module Const = CountrySelectConstants.Style;

module Wrapper = {
  module Styles = {
    open Css;

    let className =
      style([
        boxSizing(borderBox),
        display(flexBox),
        flexDirection(column),
        width(px(Const.Size.menuWidthPx)),
        maxHeight(px(Const.Size.menuHeightPx)),
        borderStyle(`solid),
        borderColor(Const.Color.menuBorder),
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

  [@react.component]
  let make = (~children: React.element) => {
    <div className=Styles.className> children </div>;
  };
};

module List = {
  module Styles = {
    open Css;

    let className =
      style([
        overflowX(`hidden),
        overflowY(`auto),
        padding(px(0)),
        paddingTop(px(3)),
        paddingBottom(px(3)),
      ]);
  };

  [@react.component]
  let make = (~children: React.element) => {
    <div className=Styles.className> children </div>;
  };
};
