type t;

[@bs.module "@testing-library/user-event"] external userEvent: t = "default";

[@bs.send] external _typeText: (t, Dom.element, string) => unit = "type";

let typeText = (elem, string) => userEvent->_typeText(elem, string);
