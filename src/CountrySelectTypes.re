module Option = {
  type t = ReactSelect.Option.t;
};

module Section = {
  type t =
    | Button
    | Filter
    | Options;
};

module KeyboardButton = {
  type t =
    | ArrowUp
    | ArrowDown
    | Enter
    | Escape
    | Tab
    | Space
    | Unsupported;

  let fromString =
    fun
    | "ArrowUp" => ArrowUp
    | "ArrowDown" => ArrowDown
    | "Enter" => Enter
    | "Escape" => Escape
    | "Tab" => Tab
    | "Space" => Space
    | _ => Unsupported;
};
