module Option = {
  type t = {
    value: string,
    label: string,
  };
};

module Section = {
  type t =
    | Button
    | Filter
    | Options(int);
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
