open Jest;
open Expect;
open ReactTestingLibrary;

module FakeOptions = {
  let opt1 = {ReactSelect.Option.label: "Argentina", value: "ar"};
  let opt2 = {ReactSelect.Option.label: "Bangladesh", value: "bd"};
  let opt3 = {ReactSelect.Option.label: "Chile", value: "cl"};

  let options = [|opt1, opt2, opt3|];
};

module FakeRequest: CountrySelectAPI.Request = {
  let getCountriesIO = _url =>
    Relude.IO.pure(FakeOptions.options) |> Relude.IO.withDelay(1000);
};

module CountrySelect = CountrySelect.Functor(FakeRequest);

describe("CountrySelect", () => {
  let rendered = render(<CountrySelect />);

  test("Render", () => {
    expect(rendered) |> toMatchSnapshot
  });
});
