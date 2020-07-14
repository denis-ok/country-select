open Belt;
open Functions;
open Utils.React;

module Text = {
  let loading = "Loading...";
  let selectCountry = "Select Country";
};

let filterOptions =
    (options: array(CountrySelectTypes.Option.t), filterString) => {
  let searchSubstring = Utils.String.normalizeString(filterString);

  let hasSubstring = Utils.String.hasSubstring(~search=searchSubstring);

  if (Relude.String.isEmpty(searchSubstring)) {
    options;
  } else {
    options->Array.keep(({value, label}) =>
      value->hasSubstring || label->hasSubstring
    );
  };
};

module Functor = (Request: CountrySelectAPI.Request) => {
  [@react.component]
  let make =
      (
        ~country: option(string),
        ~onChange: string => unit,
        ~optionsUrl: option(string)=?,
        ~className: option(string)=?,
      ) => {
    let className = Option.getWithDefault(className, "");

    let (options, setOptions) = React.useState(() => None);

    let (
      selectedCountry: option(CountrySelectTypes.Option.t),
      setSelectedCountry,
    ) =
      React.useState(() => None);

    let (filterString, setFilterString) = React.useState(() => "");

    let onChangeFilterString = str => setFilterString(const(str));

    let (menuOpened, toggleMenu) = React.useState(() => false);

    let toggleMenu = () => toggleMenu(opened => !opened);

    ReludeReact.Effect.useIOOnMount(
      Request.getCountriesIO(optionsUrl),
      options => setOptions(const(Some(options))),
      error => Js.log(ReludeFetch.Error.show(e => e, error)),
    );

    React.useEffect2(
      () => {
        switch (options, country) {
        | (Some(options), Some(country)) =>
          let mbSelectedValue =
            options->Array.getBy(opt => opt.value == country);

          setSelectedCountry(const(mbSelectedValue));
        | _ => ()
        };
        None;
      },
      (options, country),
    );

    let onChangeCountry = (selectedCountry: CountrySelectTypes.Option.t, _) => {
      onChange(selectedCountry.value);
      setSelectedCountry(_ => Some(selectedCountry));
    };

    switch (options) {
    | None =>
      <CountrySelectDropdownButton
        text=Text.loading
        onClick=ignore
        opened=false
      />
    | Some(options) =>
      let filteredOptions = filterOptions(options, filterString);

      <div className>
        <CountrySelectDropdownButton
          text=Option.(
            map(selectedCountry, c => c.label)
            ->getWithDefault(Text.selectCountry)
          )
          onClick=toggleMenu
          opened=menuOpened
        />
        {menuOpened
         &&& <CountrySelectMenu.Wrapper>
               <CountrySelectSearchFilter
                 value=filterString
                 onChange=onChangeFilterString
               />
               <CountrySelectMenu.List>
                 {filteredOptions
                  ->Array.map(
                      ({value, label} as option: CountrySelectTypes.Option.t) =>
                      <CountrySelectOption
                        key=value
                        value
                        label
                        isFocused=false
                        isSelected=false
                        onClick={() => onChangeCountry(option, ())}
                      />
                    )
                  ->React.array}
               </CountrySelectMenu.List>
             </CountrySelectMenu.Wrapper>}
      </div>;
    };
  };
};

include Functor(CountrySelectAPI.Request);
