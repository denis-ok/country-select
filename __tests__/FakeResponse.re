let data = {j|[
  {"label": "Argentina", "value": "ar"},
  {"label": "Bangladesh", "value": "bd"},
  {"label": "Chile", "value": "cl"},
  {"label": "France", "value": "fr"},
  {"label": "Latvia", "value": "lv"},
  {"label": "Sweden", "value": "se"}
]|j};

let json = Js.Json.parseExn(data);
