[@react.component]
let make = (~countryCode: string) => {
  <span style={ReactDOM.Style.make(~width="14px", ())}>
    <FlagIconCssBuild countryCode />
  </span>;
};
