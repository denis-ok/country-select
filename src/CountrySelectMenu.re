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
    <div className=Styles.className role="listbox"> children </div>;
  };
};

module ListReactWindow = {
  [@react.component]
  let make = (~children: array(React.element)) => {
    <ReactWindow.FixedSizeList
      className=List.Styles.className
      height=Const.Size.menuHeightPx
      width=Const.Size.menuWidthPx
      itemSize=Const.Size.menuOptionHeightPx
      itemCount={Array.length(children)}>
      {({index, style}) => <div style> {children[index]} </div>}
    </ReactWindow.FixedSizeList>;
  };
};

module CountryList = {
  [@react.component]
  let make =
      (
        ~options: array(CountrySelectTypes.Option.t),
        ~onChangeCountry: CountrySelectTypes.Option.t => unit,
        ~selectedCountry: option(CountrySelectTypes.Option.t),
        ~onFocus: unit => unit,
      ) => {
    let elements =
      options->Belt.Array.map(
        ({value, label} as option: CountrySelectTypes.Option.t) =>
        <CountrySelectOption
          key=value
          value
          label
          isFocused=false
          isSelected=Belt.Option.(
            map(selectedCountry, c => c.value == value)->getWithDefault(false)
          )
          onClick={() => onChangeCountry(option)}
          onFocus
        />
      );

    if (Array.length(options) <= 6) {
      <List> {React.array(elements)} </List>;
    } else {
      <ListReactWindow> elements </ListReactWindow>;
    };
  };
};
