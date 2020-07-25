[@bs.module "scroll-into-view-if-needed"]
external _scrollIntoViewIfNeeded: (Dom.element, Js.t({..})) => unit =
  "default";

let scrollIntoViewIfNeeded = el =>
  _scrollIntoViewIfNeeded(el, {"scrollMode": "if-needed"});
