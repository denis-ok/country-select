open Jest;
open Expect;
open ReactTestingLibrary;

describe("CountrySelect", () => {
  let rendered = render(<CountrySelect />);

  test("Render", () => {
    expect(rendered) |> toMatchSnapshot
  });
});
