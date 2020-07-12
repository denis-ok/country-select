[%raw {|require('flag-icon-css/css/flag-icon.css')|}];

[@react.component]
let make = (~countryCode: string) => {
  <span
    className={"flag-icon flag-icon-" ++ countryCode}
    style={ReactDOM.Style.make(~width="14px", ~height="10px", ())}
  />;
};
