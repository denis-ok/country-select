open Jest;
open Expect;
open ReactTestingLibrary;
open TestUtils;

module FakeOptions = {
  let opt1 = {CountrySelectTypes.Option.label: "Argentina", value: "ar"};
  let opt2 = {CountrySelectTypes.Option.label: "Bangladesh", value: "bd"};
  let opt3 = {CountrySelectTypes.Option.label: "Chile", value: "cl"};
  let opt4 = {CountrySelectTypes.Option.label: "France", value: "fr"};
  let opt5 = {CountrySelectTypes.Option.label: "Latvia", value: "lv"};
  let opt6 = {CountrySelectTypes.Option.label: "Sweden", value: "se"};

  let options = [|opt1, opt2, opt3, opt4, opt5, opt6|];
};

module FakeRequest: CountrySelectAPI.Request = {
  let getCountriesIO = _url =>
    Relude.IO.pure(FakeOptions.options) |> Relude.IO.withDelay(200);
};

module CountrySelect = CountrySelect.FunctorComponent(FakeRequest);

let renderSelector = () =>
  render(<CountrySelect country=None onChange=ignore />);

describe("CountrySelect", () => {
  test("Render", () =>
    expect(renderSelector()) |> toMatchSnapshot
  );

  testPromise("Pass country prop", () =>
    render(<CountrySelect country={Some("bd")} onChange=ignore />)
    |> findByText'("Bangladesh")
    |> Promise.map(el => expect(el) |> toMatchSnapshot)
  );

  testPromise("Loading text -> Select Country text", () => {
    let rendered = renderSelector();

    rendered
    |> findByText'("Loading...")
    |> Promise.flatMap(_ => rendered |> findByText'("Select Country"))
    |> Promise.map(el => expect(el) |> toMatchSnapshot);
  });

  testPromise("Options list has role attribute", () => {
    let rendered = renderSelector();

    rendered
    |> findByText'("Select Country")
    |> Promise.map(button => act(() => button |> Event.click))
    |> Promise.map(() => rendered |> getByRole'("listbox"))
    |> Promise.map(el => expect(el) |> toMatchSnapshot);
  });

  testPromise("Options have role attribute", () => {
    let rendered = renderSelector();

    rendered
    |> findByText'("Select Country")
    |> Promise.map(button => act(() => button |> Event.click))
    |> Promise.map(() => rendered |> getAllByRole'("option"))
    |> Promise.map(el => expect(el) |> toMatchSnapshot);
  });
});

describe("Filter countries", () => {
  testPromise("Case insensitive", () => {
    let rendered = renderSelector();

    rendered
    |> findByText'("Select Country")
    |> Promise.map(button => act(() => button |> Event.click))
    |> Promise.flatMap(() => rendered |> findByPlaceholderText'("Search"))
    |> Promise.map(input => UserEvent.typeText(input, "chil"))
    |> Promise.map(() => rendered |> getAllByRole'("option"))
    |> Promise.map(es => expect(Array.length(es)) |> toEqual(1));
  });

  testPromise("Country not found", () => {
    let rendered = renderSelector();

    rendered
    |> findByText'("Select Country")
    |> Promise.map(button => act(() => button |> Event.click))
    |> Promise.flatMap(() => rendered |> findByPlaceholderText'("Search"))
    |> Promise.map(input => UserEvent.typeText(input, "lala"))
    |> Promise.map(() => getByDisplayValue'("lala"))
    |> Promise.flatMap(_ => rendered |> findByText'("Country not found"))
    |> Promise.map(el => expect(el) |> toMatchSnapshot);
  });
});

describe("Click handling", () => {
  testPromise("Open menu", () => {
    let rendered = renderSelector();

    rendered
    |> findByText'("Select Country")
    |> Promise.map(button => act(() => button |> Event.click))
    |> Promise.flatMap(() => rendered |> findByPlaceholderText'("Search"))
    |> Promise.map(el => expect(el) |> toMatchSnapshot);
  });

  testPromise("Open menu and select country", () => {
    let rendered = renderSelector();

    rendered
    |> findByText'("Select Country")
    |> Promise.map(button => act(() => button |> Event.click))
    |> Promise.flatMap(() => rendered |> findByText'("Bangladesh"))
    |> Promise.map(option => act(() => option |> Event.click))
    |> Promise.map(el => expect(el) |> toMatchSnapshot);
  });
});

describe("KeyDown handling", () => {
  open! JestDom;

  describe("Focus button ->", () => {
    let focusButton = () => {
      let rendered = renderSelector();

      rendered
      |> findByText'("Select Country")
      |> Promise.map(_ => {
           let button = rendered |> getByRole'("button");
           Event.focus(button);
           button;
         });
    };

    testPromise("Check attribute", () =>
      focusButton()
      |> Promise.map(btn =>
           expect(btn) |> toHaveAttribute("aria-expanded", ~value="false")
         )
    );

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
           Event.pressArrowDown(btn);

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

  describe("Focused filter ->", () => {
    let renderFocusedFilter = () => {
      let rendered = renderSelector();

      rendered
      |> findByText'("Select Country")
      |> Promise.map(_ => {
           let button = rendered |> getByRole'("button");
           Event.focus(button);
           Event.pressEnter(button);
           let input = getByPlaceholderText'("Search", rendered);

           (rendered, input);
         });
    };

    testPromise("ArrowDown -> Enter", () => {
      renderFocusedFilter()
      |> Promise.map(((rendered, input)) => {
           Event.pressArrowDown(input);
           Event.pressEnter(input);
           rendered |> getByRole'("button");
         })
      |> Promise.map(el =>
           expect(el)
           |> toHaveTextContent(`Str("Argentina"), ~options=?None)
         )
    });

    testPromise("ArrowDown -> ArrowDown -> Space", () => {
      renderFocusedFilter()
      |> Promise.map(((rendered, input)) => {
           Event.pressArrowDown(input);
           Event.pressArrowDown(input);
           Event.pressSpace(input);
           rendered |> getByRole'("button");
         })
      |> Promise.map(el =>
           expect(el)
           |> toHaveTextContent(`Str("Bangladesh"), ~options=?None)
         )
    });

    testPromise("ArrowDown -> ArrowUp -> ArrowUp -> Menu closed", () => {
      renderFocusedFilter()
      |> Promise.map(((rendered, input)) => {
           Event.pressArrowDown(input);
           Event.pressArrowUp(input);
           Event.pressArrowUp(input);
           rendered |> getByRole'("button");
         })
      |> Promise.map(el =>
           expect(el) |> toHaveAttribute("aria-expanded", ~value="false")
         )
    });
  });
});
