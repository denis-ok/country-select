open Jest;
open Expect;
open ReactTestingLibrary;

let getByString = text => getByText(~matcher=`Str(text), ~options=?None);

describe("Component", () => {
  let rendered = render(<Component />);

  test("Render", () => {
    expect(rendered |> getByString("Hello World!")) |> toMatchSnapshot
  });
});
