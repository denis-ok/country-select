module Option = {
  type t = {
    value: string,
    label: string,
  };
};

[@react.component] [@bs.module "react-select"]
external make:
  (
    ~id: string=?,
    ~isDisabled: bool=?,
    ~isLoading: bool=?,
    ~isSearchable: bool=?,
    ~isMulti: bool=?,
    ~name: string=?,
    ~onChange: (Option.t, string) => unit=?,
    ~options: array(Option.t),
    ~value: Option.t=?,
    ~placeholder: React.element=?
  ) =>
  React.element =
  "default";
