module Option = {
  type t = {
    value: string,
    label: string,
  };
};

module VisualState = {
  type t =
    | MenuClosed([ | `Focused | `Unfocused])
    | MenuOpened([ | `Options(array(Option.t), option(int)) | `NoOptions]);
};

module KeyboardButton = {
  type t =
    | ArrowUp
    | ArrowDown
    | Enter
    | Escape
    | Tab
    | Space
    | PageUp
    | PageDown
    | Unsupported;

  let fromString =
    fun
    | "ArrowUp" => ArrowUp
    | "ArrowDown" => ArrowDown
    | "Enter" => Enter
    | "Escape" => Escape
    | "Tab" => Tab
    | " " => Space
    | "PageUp" => PageUp
    | "PageDown" => PageDown
    | _ => Unsupported;
};
