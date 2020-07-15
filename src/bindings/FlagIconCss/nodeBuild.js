const makeModule = (contryCode) => {
  const moduleName = contryCode.toUpperCase();

  const module = `module ${moduleName} = {
  [@react.component] [@bs.module "flag-icon-css/flags/4x3/${contryCode}.svg"]
  external make: unit => React.element = "default";
};`;

  return module;
};

let makeCase = (contryCode) => {
  const moduleName = contryCode.toUpperCase();

  const pmCase = `| "${contryCode}" => <${moduleName} />`;

  return pmCase;
};

const makeComponent = (countryCodes) => {
  const modules = countryCodes.map((c) => makeModule(c)).join("\n");
  const cases = countryCodes.map((c) => makeCase(c)).join("\n");

  const module = `
${modules}

[@react.component]
let make = (~countryCode: string) => {
  switch (countryCode) {
  ${cases}
  | _ => React.null
    };
  };
  `;

  return module;
};

const fs = require("fs");

const path = require("path");

const build = () => {
  const fileNames = fs.readdirSync(
    path.join(__dirname, "../../../node_modules/flag-icon-css/flags/4x3")
  );

  const countryCodes = fileNames
    .map((c) => c.split(".")[0])
    .filter((code) => code.length <= 2);

  const result = makeComponent(countryCodes);

  fs.writeFileSync(path.join(__dirname, "FlagIconCssBuild.re"), result);

  console.log("Done! :)");
};

build();
