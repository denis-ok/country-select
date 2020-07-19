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
  let opt1 = {CountrySelectTypes.Option.label: "Argentina", value: "ar"};
  let opt2 = {CountrySelectTypes.Option.label: "Bangladesh", value: "bd"};
  let opt3 = {CountrySelectTypes.Option.label: "Chile", value: "cl"};
  let opt4 = {CountrySelectTypes.Option.label: "France", value: "fr"};
  let opt5 = {CountrySelectTypes.Option.label: "Latvia", value: "lv"};
  let opt6 = {CountrySelectTypes.Option.label: "Sweden", value: "se"};
  let opt7 = {CountrySelectTypes.Option.label: "Venezuela", value: "ve"};

  let options = [|opt1, opt2, opt3|];

  let manyOptions = [|opt1, opt2, opt3, opt4, opt5, opt6, opt7|];
};

module FakeRequest: CountrySelectAPI.Request = {
  let getCountriesIO = _url =>
    Relude.IO.pure(FakeOptions.options) |> Relude.IO.withDelay(500);
};

module CountrySelect = CountrySelect.FunctorComponent(FakeRequest);
let getByPlaceholderText = str =>
  getByPlaceholderText(~matcher=`Str(str), ~options=?None);

let getByText = str => getByText(~matcher=`Str(str), ~options=?None);

let getByRole = str => getByRole(~matcher=`Str(str), ~options=?None);

let getByDisplayValue = str =>
  getByDisplayValue(~matcher=`Str(str), ~options=?None);

let getAllByRole = str => getAllByRole(~matcher=`Str(str), ~options=?None);

let findByText = str => findByText(~matcher=`Str(str), ~options=?None);

let findByRole = str => findByRole(~matcher=`Str(str), ~options=?None);

let findAllRole = str => findAllByRole(~matcher=`Str(str), ~options=?None);

let findByPlaceholderText = str =>
  findByPlaceholderText(~matcher=`Str(str), ~options=?None);

let renderSelector = () =>
  render(<CountrySelect country=None onChange=ignore />);

module Event = {
  let pressKeyUp =
    FireEvent.keyDown(~eventInit={"key": "ArrowUp", "code": "ArrowUp"});

  let pressKeyDown =
    FireEvent.keyDown(~eventInit={"key": "ArrowDown", "code": "ArrowDown"});

  let pressEnter =
    FireEvent.keyDown(~eventInit={"key": "Enter", "code": "Enter"});

  let pressSpace =
    FireEvent.keyDown(~eventInit={"key": " ", "code": "Space"});

  let pressEscape =
    FireEvent.keyDown(~eventInit={"key": "Escape", "code": "Escape"});

  let pressTab = FireEvent.keyDown(~eventInit={"key": "Tab", "code": "Tab"});

  let pressC = FireEvent.keyPress(~eventInit={"key": "c", "code": "KeyC"});

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

  describe("Filter countries", () => {
    testPromise("Case insensitive", () => {
      let rendered = renderSelector();

      rendered
      |> findByText("Select Country")
      |> Promise.map(button => act(() => button |> Event.click))
      |> Promise.flatMap(() => rendered |> findByPlaceholderText("Search"))
      |> Promise.map(input => UserEvent.typeText(input, "chil"))
      |> Promise.map(() => rendered |> getAllByRole("option"))
      |> Promise.map(es => expect(Array.length(es)) |> toEqual(1));
    });

    testPromise("Country not found", () => {
      let rendered = renderSelector();

      rendered
      |> findByText("Select Country")
      |> Promise.map(button => act(() => button |> Event.click))
      |> Promise.flatMap(() => rendered |> findByPlaceholderText("Search"))
      |> Promise.map(input => UserEvent.typeText(input, "lala"))
      |> Promise.map(() => getByDisplayValue("lala"))
      |> Promise.flatMap(_ => rendered |> findByText("Country not found"))
      |> Promise.map(el => expect(el) |> toMatchSnapshot);
    });
  });

  describe("KeyDown handling", () => {
    open! JestDom;

    describe("Focus button ->", () => {
      let focusButton = () => {
        let rendered = renderSelector();

        rendered
        |> findByText("Select Country")
        |> Promise.map(_ => {
             let button = rendered |> getByRole("button");
             Event.focus(button);
             button;
           });
      };

      testPromise("Enter", () =>
        focusButton()
        |> Promise.map(btn => {
             Event.pressEnter(btn);

             expect(btn) |> toHaveAttribute("aria-expanded", ~value="true");
           })
      );

      testPromise("Space", () =>
        focusButton()
        |> Promise.map(btn => {
             Event.pressSpace(btn);

             expect(btn) |> toHaveAttribute("aria-expanded", ~value="true");
           })
      );

      testPromise("ArrowDown", () =>
        focusButton()
        |> Promise.map(btn => {
             Event.pressKeyDown(btn);

             expect(btn) |> toHaveAttribute("aria-expanded", ~value="true");
           })
      );

      testPromise("Escape", () =>
        focusButton()
        |> Promise.map(btn => {
             Event.pressEscape(btn);

             expect(btn) |> not_ |> toHaveFocus;
           })
      );

      testPromise("Tab", () => {
        focusButton()
        |> Promise.map(btn => {
             Event.pressTab(btn);

             expect(btn) |> not_ |> toHaveFocus;
           })
      });
    });

    testPromise("Focus Filter -> ArrowDown -> Enter", () => {
      let rendered = renderSelector();
      rendered
      |> findByText("Select Country")
      |> Promise.map(button => act(() => button |> Event.click))
      |> Promise.map(() => rendered |> getByPlaceholderText("Search"))
      |> Promise.map(input => {
           Event.pressKeyDown(input);
           Event.pressEnter(input);
           rendered |> getByRole("button");
         })
      |> Promise.map(el =>
           expect(el)
           |> toHaveTextContent(`Str("Argentina"), ~options=?None)
         );
    });

    testPromise("Focus Filter -> ArrowDown -> ArrowDown -> Space", () => {
      let rendered = renderSelector();
      rendered
      |> findByText("Select Country")
      |> Promise.map(button => act(() => button |> Event.click))
      |> Promise.map(() => rendered |> getByPlaceholderText("Search"))
      |> Promise.map(input => {
           Event.pressKeyDown(input);
           Event.pressKeyDown(input);
           Event.pressSpace(input);
           rendered |> getByRole("button");
         })
      |> Promise.map(el =>
           expect(el)
           |> toHaveTextContent(`Str("Bangladesh"), ~options=?None)
         );
    });

    testPromise(
      "Focus Filter -> ArrowDown -> ArrowUp -> ArrowUp -> Menu closed", () => {
      let rendered = renderSelector();

      rendered
      |> findByText("Select Country")
      |> Promise.map(button => act(() => button |> Event.click))
      |> Promise.map(() => rendered |> getByPlaceholderText("Search"))
      |> Promise.map(input => {
           Event.pressKeyDown(input);
           Event.pressKeyUp(input);
           Event.pressKeyUp(input);
           rendered |> getByRole("button");
         })
      |> Promise.map(el =>
           expect(el) |> toHaveAttribute("aria-expanded", ~value="false")
         );
    });
  });
});
