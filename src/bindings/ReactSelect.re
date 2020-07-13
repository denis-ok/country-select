module Option = {
  type t = {
    value: string,
    label: string,
  };
};

module CustomComponent = {
  module ProvidedProps = {
    module InnerProps = {
      type t = {
        id: string,
        onClick: ReactEvent.Mouse.t => unit,
        onMouseMove: ReactEvent.Mouse.t => unit,
        onMouseOver: ReactEvent.Mouse.t => unit,
      };
    };

    type t = {
      innerRef: ReactDOM.domRef,
      innerProps: InnerProps.t,
      label: string,
      value: string,
      isSelected: bool,
      isFocused: bool,
    };
  };

  type t = ProvidedProps.t => React.element;
};

module Components = {
  // Every value of type below obviously must be option(CustomComponent.t)
  // Unfortunately, passing None just broke everything with an exception
  // Probably because we have { Menu: undefined } object instead of ommited key
  // Mayybe this can be avoided with usage of Js.Dict API
  // Or some kind of util that rejects reduntand keys
  type t = {
    [@bs.as "Option"]
    option: CustomComponent.t,
    [@bs.as "Menu"]
    menu: CustomComponent.t,
  };
};

module DefaultComponents = {
  module Component = {
    type t = CustomComponent.ProvidedProps.t => React.element;
  };

  type t;
  [@bs.module "react-select"] external components: t = "components";

  [@bs.get] external _option: t => Component.t = "Option";
  let option = components->_option;

  [@bs.get] external _menu: t => Component.t = "Menu";
  let menu = components->_menu;
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
    ~onChange: (Option.t, string) => unit=?,
    ~onFocus: ReactEvent.Focus.t => unit=?,
    ~onKeyDown: ReactEvent.Keyboard.t => unit=?,
    ~onMenuOpen: unit => unit=?,
    ~onMenuClose: unit => unit=?,
    ~options: array(Option.t),
    ~value: Option.t=?,
    ~placeholder: React.element=?
  ) =>
  React.element =
  "default";
