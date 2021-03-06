open Jest;
open Expect;
open ReactTestingLibrary;
open TestUtils;

module FakeRequest: CountrySelectAPI.Request = {
  let getCountriesIO = _url =>
    CountrySelectAPI.Response.decode("fakeurl", FakeResponse.json)
    |> Relude.IO.withDelay(200);
};

module CountrySelect = CountrySelectComponent.Functor(FakeRequest);

let renderSelector = () =>
  render(<CountrySelect country=None onChange=ignore />);

describe("CountrySelect", () => {
  testPromise("Render", () => {
    let rendered = renderSelector();

    rendered
    |> findByText'("Select Country")
    |> Promise.map(_ => rendered |> container |> expect |> toMatchSnapshot);
  });

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

  // testPromise("Options list has role attribute", () => {
  //   let rendered = renderSelector();

  //   rendered
  //   |> findByText'("Select Country")
  //   |> Promise.map(button => act(() => button |> Event.click))
  //   |> Promise.map(() => rendered |> getByRole'("listbox"))
  //   |> Promise.map(el => expect(el) |> toMatchSnapshot);
  // });

  testPromise("Options have role attribute", () => {
    let rendered = renderSelector();

    rendered
    |> findByText'("Select Country")
    |> Promise.map(button => act(() => button |> Event.click))
    |> Promise.map(() => rendered |> getAllByRole'("option"))
    |> Promise.map(el => expect(el) |> toMatchSnapshot);
  });

  testPromise("Pass className prop", () => {
    open! JestDom;

    let rendered =
      render(<CountrySelect country=None onChange=ignore className="lala" />);

    rendered
    |> findByText'("Select Country")
    |> Promise.map(_ =>
         rendered
         |> container
         |> unsafeFirstChild
         |> expect
         |> toHaveClass(`Str("lala"), ~options=?None)
       );
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

  describe("Focus filter ->", () => {
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

    testPromise("Escape", () => {
      renderFocusedFilter()
      |> Promise.map(((rendered, input)) => {
           Event.pressEscape(input);
           rendered |> getByRole'("button") |> expect |> not_ |> toHaveFocus;
         })
    });

    testPromise("Tab", () => {
      renderFocusedFilter()
      |> Promise.map(((rendered, input)) => {
           Event.pressTab(input);
           rendered
           |> getByRole'("button")
           |> expect
           |> toHaveAttribute("aria-expanded", ~value="false");
         })
    });

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

    testPromise("ArrowDown -> Escape", () => {
      renderFocusedFilter()
      |> Promise.map(((rendered, input)) => {
           Event.pressArrowDown(input);
           Event.pressEscape(input);
           rendered
           |> getByRole'("button")
           |> expect
           |> toHaveAttribute("aria-expanded", ~value="false");
         })
    });

    testPromise("ArrowDown -> Type > Escape -> Type", () => {
      renderFocusedFilter()
      |> Promise.map(((rendered, input)) => {
           Event.pressArrowDown(input);
           UserEvent.typeText(input, "lala");
           Event.pressEscape(input);
           UserEvent.typeText(input, "sweden");
           Event.pressArrowDown(input);
           Event.pressEnter(input);
           rendered
           |> getByRole'("button")
           |> expect
           |> toHaveTextContent(`Str("Sweden"), ~options=?None);
         })
    });

    testPromise("ArrowDown -> Tab", () => {
      renderFocusedFilter()
      |> Promise.map(((rendered, input)) => {
           Event.pressArrowDown(input);
           Event.pressTab(input);
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

    testPromise("ArrowDown -> PageDown -> PageDown", () => {
      renderFocusedFilter()
      |> Promise.map(((rendered, input)) => {
           Event.pressArrowDown(input);
           Event.pressPageDown(input);
           Event.pressPageDown(input);
           Event.pressEnter(input);
           rendered |> getByRole'("button");
         })
      |> Promise.map(el =>
           expect(el) |> toHaveTextContent(`Str("Zimbabwe"), ~options=?None)
         )
    });

    testPromise("ArrowDown -> PgDown -> PgDown -> PageUp -> PageUp ", () => {
      renderFocusedFilter()
      |> Promise.map(((rendered, input)) => {
           Event.pressArrowDown(input);
           Event.pressPageDown(input);
           Event.pressPageDown(input);
           Event.pressPageUp(input);
           Event.pressPageUp(input);
           Event.pressEnter(input);
           rendered |> getByRole'("button");
         })
      |> Promise.map(el =>
           expect(el)
           |> toHaveTextContent(`Str("Argentina"), ~options=?None)
         )
    });
  });
});
