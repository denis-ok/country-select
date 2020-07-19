open ReactTestingLibrary;

module Promise = {
  let map: ('a => 'b, Js.Promise.t('a)) => Js.Promise.t('b) =
    (f, p) => p |> Js.Promise.then_(a => Js.Promise.resolve(f(a)));

  let flatMap:
    ('a => Js.Promise.t('b), Js.Promise.t('a)) => Js.Promise.t('b) =
    (f, p) => p |> Js.Promise.then_(a => f(a));
};

module Event = {
  let pressArrowUp =
    FireEvent.keyDown(~eventInit={"key": "ArrowUp", "code": "ArrowUp"});

  let pressArrowDown =
    FireEvent.keyDown(~eventInit={"key": "ArrowDown", "code": "ArrowDown"});

  let pressEnter =
    FireEvent.keyDown(~eventInit={"key": "Enter", "code": "Enter"});

  let pressSpace =
    FireEvent.keyDown(~eventInit={"key": " ", "code": "Space"});

  let pressEscape =
    FireEvent.keyDown(~eventInit={"key": "Escape", "code": "Escape"});

  let pressTab = FireEvent.keyDown(~eventInit={"key": "Tab", "code": "Tab"});

  let click = FireEvent.click(~eventInit=?None);

  let focus = FireEvent.focus(~eventInit=?None);
};

let getByPlaceholderText' = str =>
  getByPlaceholderText(~matcher=`Str(str), ~options=?None);

let getByText' = str => getByText(~matcher=`Str(str), ~options=?None);

let getByRole' = str => getByRole(~matcher=`Str(str), ~options=?None);

let getByDisplayValue' = str =>
  getByDisplayValue(~matcher=`Str(str), ~options=?None);

let getAllByRole' = str => getAllByRole(~matcher=`Str(str), ~options=?None);

let findByText' = str => findByText(~matcher=`Str(str), ~options=?None);

let findByRole' = str => findByRole(~matcher=`Str(str), ~options=?None);

let findAllRole' = str => findAllByRole(~matcher=`Str(str), ~options=?None);

let findByPlaceholderText' = str =>
  findByPlaceholderText(~matcher=`Str(str), ~options=?None);
