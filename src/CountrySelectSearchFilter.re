let icon =
  <svg
    xmlns="http://www.w3.org/2000/svg"
    width="16"
    height="16"
    viewBox="0 0 24 24">
    <path
      d="M23.809 21.646l-6.205-6.205c1.167-1.605 1.857-3.579 1.857-5.711 0-5.365-4.365-9.73-9.731-9.73-5.365 0-9.73 4.365-9.73 9.73 0 5.366 4.365 9.73 9.73 9.73 2.034 0 3.923-.627 5.487-1.698l6.238 6.238 2.354-2.354zm-20.955-11.916c0-3.792 3.085-6.877 6.877-6.877s6.877 3.085 6.877 6.877-3.085 6.877-6.877 6.877c-3.793 0-6.877-3.085-6.877-6.877z"
    />
  </svg>;

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

  <div>
    icon
    <input name="searchCountry" value onChange onClick placeholder />
  </div>;
};
