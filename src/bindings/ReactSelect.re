module Option = {
  type t = {
    value: string,
    label: string,
  };
};

[@react.component] [@bs.module "react-select"]
external make:
  (
    ~blurInputOnSelect: bool=?,
    ~id: string=?,
    ~isDisabled: bool=?,
    ~isLoading: bool=?,
    ~isSearchable: bool=?,
    ~isMulti: bool=?,
    ~menuIsOpen: bool=?,
    ~name: string=?,
    ~onBlur: ReactEvent.Focus.t => unit=?,
    ~onFocus: ReactEvent.Focus.t => unit=?,
    ~onChange: (Option.t, string) => unit=?,
    ~options: array(Option.t),
    ~value: Option.t=?,
    ~placeholder: React.element=?
  ) =>
  React.element =
  "default";
