module Option = {
  type t = {
    value: string,
    label: string,
  };
};

module Components = {
  module CustomComponent = {
    module ProvidedProps = {
      type t = {
        innerRef: Js.Json.t,
        innerProps: Js.Json.t,
        label: string,
        value: string,
      };
      // isMulti,
      // clearValue
      // getStyles
      // getValue
      // hasValue
      // isMulti
      // isRtl
      // options
      // selectOption
      // setValue
      // selectProps
    };

    type t = ProvidedProps.t => React.element;
  };

  type t = {
    [@bs.as "Option"]
    option: CustomComponent.t,
  };
};

module DefaultComponents = {
  module Component = {
    type t = Components.CustomComponent.ProvidedProps.t => React.element;
  };

  type t;
  [@bs.module "react-select"] external components: t = "components";

  [@bs.get] external _option: t => Component.t = "Option";
  let option = components->_option;
};

[@react.component] [@bs.module "react-select"]
external make:
  (
    ~blurInputOnSelect: bool=?,
    ~components: Components.t=?,
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
