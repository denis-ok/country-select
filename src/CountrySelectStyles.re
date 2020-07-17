open Css;

module Const = CountrySelectConstants.Style;

let root =
  style([
    boxSizing(borderBox),
    position(relative),
    width(`fitContent),
    maxWidth(px(CountrySelectConstants.Style.Size.menuWidthPx)),
  ]);

let menuWrapper =
  style([
    position(absolute),
    backgroundColor(white),
    zIndex(1000),
    boxSizing(borderBox),
    display(flexBox),
    flexDirection(column),
    width(px(Const.Size.menuWidthPx)),
    maxHeight(px(Const.Size.menuHeightPx)),
    borderStyle(`solid),
    borderColor(Const.Color.menuBorder),
    borderRadius(px(2)),
    borderWidth(px(1)),
    marginTop(px(1)),
    boxShadow(
      Shadow.box(~x=px(0), ~y=px(1), ~blur=px(3), `rgba((0, 0, 0, 0.08))),
    ),
  ]);
