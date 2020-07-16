module Const = CountrySelectConstants.Style;

module ListItems = {
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

module ListItemsReactWindow = {
  [@react.component]
  let make = (~children: array(React.element)) => {
    <ReactWindow.FixedSizeList
      className=ListItems.Styles.className
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
        ~onKeyDown: ReactEvent.Keyboard.t => unit,
        ~focusedIndex: option(int),
      ) => {
    let elements =
      options->Belt.Array.mapWithIndex(
        (index, {value, label} as option: CountrySelectTypes.Option.t) =>
        <CountrySelectOption
          key=value
          value
          label
          isFocused=Belt.Option.(
            map(focusedIndex, idx => idx == index)->getWithDefault(false)
          )
          isSelected=Belt.Option.(
            map(selectedCountry, c => c.value == value)->getWithDefault(false)
          )
          onClick={() => onChangeCountry(option)}
          onKeyDown
        />
      );

    if (Array.length(options) <= 6) {
      <ListItems> {React.array(elements)} </ListItems>;
    } else {
      <ListItemsReactWindow> elements </ListItemsReactWindow>;
    };
  };
};
