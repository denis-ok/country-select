module Styles = {
  open Css;

  module Const = CountrySelectConstants.Style;

  let searchIcon =
    <svg
      width="13px"
      height="13px"
      viewBox="0 0 13 13"
      version="1.1"
      xmlns="http://www.w3.org/2000/svg"
      xmlnsXlink="http://www.w3.org/1999/xlink">
      <defs>
        <path
          d="M188,245.001858 C188,243.896262 188.90277,243 190.005817,243 L415.994183,243 C417.101965,243 418,243.887227 418,245.001858 L418,439.998142 C418,441.103738 417.09723,442 415.994183,442 L190.005817,442 C188.898035,442 188,441.112773 188,439.998142 L188,245.001858 Z"
          id="path-1"
        />
        <filter
          x="-2.6%"
          y="-2.5%"
          width="105.2%"
          height="106.0%"
          filterUnits="objectBoundingBox"
          id="filter-2">
          <feMorphology
            radius="1"
            operator="dilate"
            result="shadowSpreadOuter1"
          />
          <feOffset dx="0" dy="1" result="shadowOffsetOuter1" />
          <feGaussianBlur stdDeviation="1.5" result="shadowBlurOuter1" />
          <feComposite
            in2="SourceAlpha"
            operator="out"
            result="shadowBlurOuter1"
          />
          <feColorMatrix
            values="0 0 0 0 0   0 0 0 0 0   0 0 0 0 0  0 0 0 0.1 0"
            type_="matrix"
          />
        </filter>
      </defs>
      <g
        id="Landing"
        stroke="none"
        strokeWidth="1"
        fill="none"
        fillRule="evenodd">
        <g id="Artboard" transform="translate(-199.000000, -254.000000)">
          <g id="bg">
            <use
              fill="black"
              fillOpacity="1"
              filter="url(#filter-2)"
              xlinkHref="#path-1"
            />
            <path
              strokeOpacity="0.08"
              stroke="#000000"
              strokeWidth="1"
              d="M415.994183,242.5 C416.687368,242.5 417.314477,242.778691 417.767928,243.231246 C418.219958,243.682384 418.5,244.306575 418.5,245.001858 L418.5,245.001858 L418.5,439.998142 C418.5,440.688422 418.218797,441.313612 417.765105,441.766408 C417.310836,442.21978 416.683774,442.5 415.994183,442.5 L415.994183,442.5 L190.005817,442.5 C189.312632,442.5 188.685523,442.221309 188.232072,441.768754 C187.780042,441.317616 187.5,440.693425 187.5,439.998142 L187.5,439.998142 L187.5,245.001858 C187.5,244.311578 187.781203,243.686388 188.234895,243.233592 C188.689164,242.78022 189.316226,242.5 190.005817,242.5 L190.005817,242.5 Z"
              fillRule="evenodd"
            />
          </g>
          <g id="Group" transform="translate(198.000000, 253.000000)">
            <rect id="box" x="0" y="0" width="14" height="14" />
            <path
              d="M6,1 C8.76142375,1 11,3.23857625 11,6 C11,7.01954236 10.6948479,7.96781264 10.1708943,8.75846025 L13.4700003,12.0600004 L12.0600004,13.4700003 L8.75846025,10.1708943 C7.96781264,10.6948479 7.01954236,11 6,11 C3.23857625,11 1,8.76142375 1,6 C1,3.23857625 3.23857625,1 6,1 Z M6,2.79999995 C4.23268877,2.79999995 2.79999995,4.23268877 2.79999995,6 C2.79999995,7.76731123 4.23268877,9.20000005 6,9.20000005 C7.76731123,9.20000005 9.20000005,7.76731123 9.20000005,6 C9.20000005,4.23268877 7.76731123,2.79999995 6,2.79999995 Z"
              id="icon"
              fill="#333333"
            />
          </g>
        </g>
      </g>
    </svg>;

  let inputFocus =
    focus([
      outlineStyle(`none),
      borderStyle(`solid),
      borderWidth(px(1)),
      borderRadius(px(1)),
      borderColor(Const.Color.focus),
    ]);

  let input =
    style([
      boxSizing(borderBox),
      display(flexBox),
      flexDirection(row),
      alignItems(center),
      margin(px(0)),
      padding(px(0)),
      paddingLeft(px(31)),
      paddingRight(px(10)),
      width(`percent(100.0)),
      height(px(Const.Size.searchHeightPx)),
      borderWidth(px(0)),
      borderBottomWidth(px(1)),
      borderBottomColor(Const.Color.menuBorder),
      fontSize(px(14)),
      fontFamily(`custom("Arial")),
      focus([outlineStyle(`none)]),
      placeholder([color(`hex("D8D8D8"))]),
      inputFocus,
    ]);

  let wrapper = style([top(px(38)), left(px(-2))]);

  let iconInputWrapper = style([position(relative)]);

  let icon = style([position(absolute), left(px(11)), top(px(10))]);
};

let placeholder = "Search";

[@react.component]
let make =
    (
      ~value: string,
      ~onChange: string => unit,
      ~onFocus: unit => unit,
      ~onKeyDown: ReactEvent.Keyboard.t => unit,
      ~focused: bool,
    ) => {
  let onChange = event => {
    let value = Utils.ReactDom.getStringValueFromEvent(event);
    onChange(value);
  };

  let inputRef: React.ref(Js.Nullable.t(Dom.element)) =
    React.useRef(Js.Nullable.null);

  React.useEffect1(
    () => {
      if (focused) {
        Utils.ReactDom.focusRef(inputRef);
      };

      None;
    },
    [|focused|],
  );

  <div className=Styles.wrapper>
    <div className=Styles.iconInputWrapper>
      <div className=Styles.icon> Styles.searchIcon </div>
      <input
        autoFocus=true
        ref={ReactDOMRe.Ref.domRef(inputRef)}
        name="searchCountry"
        value
        onChange
        onKeyDown
        placeholder
        className=Styles.input
        onFocus={Functions.omit(onFocus)}
      />
    </div>
  </div>;
};
