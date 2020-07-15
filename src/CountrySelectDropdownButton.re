module Styles = {
  open Css;

  module Const = CountrySelectConstants.Style;

  let iconSvg = (opened: bool): React.element => {
    <svg
      width="8px"
      height="5px"
      viewBox="0 0 8 5"
      version="1.1"
      xmlns="http://www.w3.org/2000/svg"
      xmlnsXlink="http://www.w3.org/1999/xlink"
      transform={opened ? "rotate(-180)" : ""}>
      <g
        id="Landing"
        stroke="none"
        strokeWidth="1"
        fill="none"
        fillRule="evenodd">
        <g id="Artboard" transform="translate(-286.000000, -197.000000)">
          <path
            d="M300.998894,186.5 C301.689815,186.5 302.315313,186.780058 302.767917,187.233691 C303.220426,187.687229 303.5,188.314156 303.5,189.00793 L303.5,189.00793 L303.5,208.99207 C303.5,209.685536 303.221091,210.313478 302.76798,210.767619 C302.316298,211.220328 301.69164,211.5 300.998894,211.5 L300.998894,211.5 L188.001106,211.5 C187.310185,211.5 186.684687,211.219942 186.232083,210.766309 C185.779574,210.312771 185.5,209.685844 185.5,208.99207 L185.5,208.99207 L185.5,189.00793 C185.5,188.314464 185.778909,187.686522 186.23202,187.232381 C186.683702,186.779672 187.30836,186.5 188.001106,186.5 L188.001106,186.5 Z"
            id="bg"
            strokeOpacity="0.2"
            stroke="#000000"
            fill="#FFFFFF"
          />
          <g id="Group" transform="translate(286.000000, 195.000000)">
            <rect id="box" x="0" y="0" width="8" height="8" />
            <polygon id="icon" fill="#333333" points="0 2 8 2 4 7" />
          </g>
        </g>
      </g>
    </svg>;
  };

  let buttonHover =
    hover([cursor(pointer), backgroundColor(Const.Color.hover)]);

  let buttonFocus = focus([borderColor(Const.Color.focus)]);

  let button =
    style([
      boxSizing(borderBox),
      display(flexBox),
      flexDirection(row),
      justifyContent(spaceBetween),
      alignItems(center),
      paddingLeft(px(10)),
      paddingRight(px(10)),
      maxWidth(px(Const.Size.menuWidthPx)),
      height(px(26)),
      borderStyle(`solid),
      borderColor(Const.Color.dropdownBorder),
      borderRadius(px(3)),
      borderWidth(px(1)),
      fontSize(Const.fontSizePx),
      fontFamily(Const.fontFamily),
      focus([outlineStyle(`none)]),
      backgroundColor(white),
      buttonHover,
      buttonFocus,
    ]);

  let text =
    style([
      boxSizing(borderBox),
      whiteSpace(`nowrap),
      overflow(`hidden),
      textOverflow(`ellipsis),
      paddingRight(`px(6)),
    ]);
};

[@react.component]
let make =
    (
      ~text: string,
      ~opened: bool,
      ~onClick: unit => unit,
      ~onFocus: unit => unit,
      ~setRef: React.ref(Js.Nullable.t(Dom.element)) => unit,
    ) => {
  let onClick = (event: ReactEvent.Mouse.t) => {
    ReactEvent.Mouse.preventDefault(event);
    onClick();
  };

  let buttonRef: React.ref(Js.Nullable.t(Dom.element)) =
    React.useRef(Js.Nullable.null);

  ReludeReact.Effect.useOnMount(() => setRef(buttonRef));

  <button
    ref={ReactDOMRe.Ref.domRef(buttonRef)}
    className=Styles.button
    onClick
    onFocus={Functions.omit(onFocus)}>
    <div className=Styles.text> {React.string(text)} </div>
    {Styles.iconSvg(opened)}
  </button>;
};
