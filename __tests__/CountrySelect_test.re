open Jest;
open Expect;
open ReactTestingLibrary;

module Promise = {
  let map: ('a => 'b, Js.Promise.t('a)) => Js.Promise.t('b) =
    (f, p) => p |> Js.Promise.then_(a => Js.Promise.resolve(f(a)));

  let flatMap:
    ('a => Js.Promise.t('b), Js.Promise.t('a)) => Js.Promise.t('b) =
    (f, p) => p |> Js.Promise.then_(a => f(a));
};

module FakeOptions = {
  let opt1 = {ReactSelect.Option.label: "Argentina", value: "ar"};
  let opt2 = {ReactSelect.Option.label: "Bangladesh", value: "bd"};
  let opt3 = {ReactSelect.Option.label: "Chile", value: "cl"};
  let opt4 = {ReactSelect.Option.label: "France", value: "fr"};
  let opt5 = {ReactSelect.Option.label: "Latvia", value: "lv"};
  let opt6 = {ReactSelect.Option.label: "Sweden", value: "se"};
  let opt7 = {ReactSelect.Option.label: "Venezuela", value: "ve"};

  let options = [|opt1, opt2, opt3|];

  let manyOptions = [|opt1, opt2, opt3, opt4, opt5, opt6, opt7|];
};

module FakeRequest: CountrySelectAPI.Request = {
  let getCountriesIO = _url =>
    Relude.IO.pure(FakeOptions.options) |> Relude.IO.withDelay(500);
};

module CountrySelect = CountrySelect.Functor(FakeRequest);

let getByText = str => getByText(~matcher=`Str(str), ~options=?None);

let getByRole = str => getByRole(~matcher=`Str(str), ~options=?None);

let getAllByRole = str => getAllByRole(~matcher=`Str(str), ~options=?None);

let findByText = str => findByText(~matcher=`Str(str), ~options=?None);

let findByRole = str => findByRole(~matcher=`Str(str), ~options=?None);

let findAllRole = str => findAllByRole(~matcher=`Str(str), ~options=?None);

let findByPlaceholderText = str =>
  findByPlaceholderText(~matcher=`Str(str), ~options=?None);

let renderSelector = () =>
  render(<CountrySelect country=None onChange=ignore />);

module Event = {
  let pressKeyDown =
    FireEvent.keyDown(
      ~eventInit={"key": "ArrowDown", "keyCode": 40, "code": 40},
    );

  let pressEnter =
    FireEvent.keyDown(
      ~eventInit={"key": "Enter", "keyCode": 13, "code": 13},
    );

  let pressTab =
    FireEvent.keyDown(~eventInit={"key": "Tab", "keyCode": 9, "code": 9});

  let pressC =
    FireEvent.keyPress(
      ~eventInit={"key": "c", "keyCode": 67, "code": "KeyC"},
    );

  let click = FireEvent.click(~eventInit=?None);

  let focus = FireEvent.focus(~eventInit=?None);
};

describe("CountrySelect", () => {
  test("Render", () =>
    expect(renderSelector()) |> toMatchSnapshot
  );

  testPromise("Pass country prop", () =>
    render(<CountrySelect country={Some("bd")} onChange=ignore />)
    |> findByText("Bangladesh")
    |> Promise.map(el => expect(el) |> toMatchSnapshot)
  );

  testPromise("Loading placeholder", () =>
    renderSelector()
    |> findByText("Loading...")
    |> Promise.map(el => expect(el) |> toMatchSnapshot)
  );

  testPromise("Select Country placeholder", () =>
    renderSelector()
    |> findByText("Select Country")
    |> Promise.map(el => expect(el) |> toMatchSnapshot)
  );

  testPromise("Open menu by click", () => {
    let rendered = renderSelector();

    rendered
    |> findByText("Select Country")
    |> Promise.map(button => act(() => button |> Event.click))
    |> Promise.flatMap(() => rendered |> findByPlaceholderText("Search"))
    |> Promise.map(el => expect(el) |> toMatchSnapshot);
  });

  testPromise("Open menu and select country", () => {
    let rendered = renderSelector();

    rendered
    |> findByText("Select Country")
    |> Promise.map(button => act(() => button |> Event.click))
    |> Promise.flatMap(() => rendered |> findByText("Bangladesh"))
    |> Promise.map(option => act(() => option |> Event.click))
    |> Promise.map(el => expect(el) |> toMatchSnapshot);
  });

  testPromise("List has role attribute", () => {
    let rendered = renderSelector();

    rendered
    |> findByText("Select Country")
    |> Promise.map(button => act(() => button |> Event.click))
    |> Promise.map(() => rendered |> getByRole("listbox"))
    |> Promise.map(el => expect(el) |> toMatchSnapshot);
  });

  testPromise("List with 7 items has role attribute", () => {
    let rendered = renderSelector();

    rendered
    |> findByText("Select Country")
    |> Promise.map(button => act(() => button |> Event.click))
    |> Promise.map(() => rendered |> getByRole("listbox"))
    |> Promise.map(el => expect(el) |> toMatchSnapshot);
  });

  testPromise("Options have role attribute", () => {
    let rendered = renderSelector();

    rendered
    |> findByText("Select Country")
    |> Promise.map(button => act(() => button |> Event.click))
    |> Promise.map(() => rendered |> getAllByRole("option"))
    |> Promise.map(el => expect(el) |> toMatchSnapshot);
  });
});
