open Jest;
open Expect;
open ReactTestingLibrary;

module Promise = {
  let map: ('a => 'b, Js.Promise.t('a)) => Js.Promise.t('b) =
    (f, p) => p |> Js.Promise.then_(a => Js.Promise.resolve(f(a)));
};

module FakeOptions = {
  let opt1 = {ReactSelect.Option.label: "Argentina", value: "ar"};
  let opt2 = {ReactSelect.Option.label: "Bangladesh", value: "bd"};
  let opt3 = {ReactSelect.Option.label: "Chile", value: "cl"};

  let options = [|opt1, opt2, opt3|];
};

module FakeRequest: CountrySelectAPI.Request = {
  let getCountriesIO = _url =>
    Relude.IO.pure(FakeOptions.options) |> Relude.IO.withDelay(500);
};

module CountrySelect = CountrySelect.Functor(FakeRequest);

let getByText = str => getByText(~matcher=`Str(str), ~options=?None);

let findByText = str => findByText(~matcher=`Str(str), ~options=?None);

let renderSelector = () =>
  render(<CountrySelect country=None onChange=ignore />);

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
});
