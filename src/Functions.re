let (>>) = (f, g, x) => g(f(x));

let const = (v, _) => v;

let omit = (f, _) => f();
