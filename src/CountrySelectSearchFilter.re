module Styles = {
  open Css;

  let searchIcon =
    <svg
      xmlns="http://www.w3.org/2000/svg"
      width="14"
      height="14"
      viewBox="0 0 24 24">
      <path
        d="M23.809 21.646l-6.205-6.205c1.167-1.605 1.857-3.579 1.857-5.711 0-5.365-4.365-9.73-9.731-9.73-5.365 0-9.73 4.365-9.73 9.73 0 5.366 4.365 9.73 9.73 9.73 2.034 0 3.923-.627 5.487-1.698l6.238 6.238 2.354-2.354zm-20.955-11.916c0-3.792 3.085-6.877 6.877-6.877s6.877 3.085 6.877 6.877-3.085 6.877-6.877 6.877c-3.793 0-6.877-3.085-6.877-6.877z"
      />
    </svg>;

  let input =
    style([
      boxSizing(borderBox),
      display(flexBox),
      flexDirection(row),
      alignItems(center),
      paddingLeft(px(32)),
      width(px(230)),
      height(px(36)),
      borderStyle(`solid),
      borderColor(hex("cccccc")),
      borderRadius(px(0)),
      borderTopLeftRadius(px(2)),
      borderTopRightRadius(px(2)),
      borderWidth(px(1)),
      marginTop(px(2)),
      marginBottom(px(2)),
      fontSize(px(14)),
      fontFamily(`custom("Arial")),
      focus([outlineStyle(`none)]),
      placeholder([color(`hex("D8D8D8"))]),
    ]);

  let wrapper = style([position(relative)]);

  let icon = style([position(absolute), left(px(11)), top(px(11))]);
};

let placeholder = "Search";

[@react.component]
let make = (~value: string, ~onChange: string => unit) => {
  let onChange = event => {
    let value = Utils.getStringValueFromEvent(event);
    onChange(value);
  };

  let onClick = (event: ReactEvent.Mouse.t) => {
    ReactEvent.Mouse.stopPropagation(event);
    ReactEvent.Mouse.preventDefault(event);
  };

  <div className=Styles.wrapper>
    <div className=Styles.icon> Styles.searchIcon </div>
    <input
      name="searchCountry"
      value
      onChange
      onClick
      placeholder
      className=Styles.input
    />
  </div>;
};
