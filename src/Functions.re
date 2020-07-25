let (>>) = (f, g, x) => g(f(x));

let omit = (f, _) => f();
