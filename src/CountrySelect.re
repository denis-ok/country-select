open Belt;
open Functions;
open Utils.React;

module String = Utils.String;

module Styles = {
  open Css;

  let wrapper = style([position(relative)]);
};

module Text = {
  let placeholderLoading = React.string("Loading...");
  let placeholder = React.string("Select Country");
};

let filterOptions = (options: array(ReactSelect.Option.t), filterString) => {
  let searchSubstring = String.normalizeString(filterString);

  let hasSubstring = String.hasSubstring(~search=searchSubstring);

  if (Relude.String.isEmpty(searchSubstring)) {
    options;
  } else {
    options->Array.keep(({value, label}) =>
      value->hasSubstring || label->hasSubstring
    );
  };
};

let customComponents: ReactSelect.Components.t = {
  option: CountrySelectOption.component,
  menu: CountrySelectMenu.component,
  menuList: CountrySelectMenuList.component,
};

module Functor = (Request: CountrySelectAPI.Request) => {
  [@react.component]
  let make =
      (
        ~country: option(string),
        ~onChange: string => unit,
        ~optionsUrl: option(string)=?,
      ) => {
    let (options, setOptions) = React.useState(() => None);

    let (selectedCountry: option(ReactSelect.Option.t), setSelectedCountry) =
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

    let onChangeCountry = (selectedCountry: ReactSelect.Option.t, _) => {
      onChange(selectedCountry.value);
      setSelectedCountry(_ => Some(selectedCountry));
    };

    let onBlur = omit(toggleMenu);

    let onFocus = onBlur;

    switch (options) {
    | None =>
      <ReactSelect
        isLoading=true
        isDisabled=true
        isSearchable=false
        placeholder=Text.placeholderLoading
        options=[||]
      />
    | Some(options) =>
      <div className=Styles.wrapper>
        <ReactSelect
          blurInputOnSelect=true
          components=customComponents
          isLoading=false
          isDisabled=false
          isSearchable=false
          menuIsOpen=menuOpened
          onBlur
          onChange=onChangeCountry
          onFocus
          options={filterOptions(options, filterString)}
          placeholder=Text.placeholder
          value=?selectedCountry
        />
        {menuOpened
         &&& <CountrySelectSearchFilter
               value=filterString
               onChange=onChangeFilterString
             />}
      </div>
    };
  };
};

include Functor(CountrySelectAPI.Request);
