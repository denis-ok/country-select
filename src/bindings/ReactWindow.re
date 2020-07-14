type childProps = {
  index: int,
  style: ReactDOM.Style.t,
};

type child = childProps => React.element;

module FixedSizeList = {
  [@react.component] [@bs.module "react-window"]
  external make:
    (
      ~height: int,
      ~width: int,
      ~itemSize: int,
      ~itemCount: int,
      ~className: string=?,
      ~children: child
    ) =>
    React.element =
    "FixedSizeList";
};
