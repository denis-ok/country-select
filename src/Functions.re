let (>>) = (f, g, x) => g(f(x));

let const = (v, _) => v;

let omit = (f, _) => f();

let both = (mx, my) =>
  switch (mx, my) {
  | (Some(x), Some(y)) => Some((x, y))
  | _ => None
  };
