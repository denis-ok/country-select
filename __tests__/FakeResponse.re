let data = {j|[
  {"label": "Argentina", "value": "ar"},
  {"label": "Bangladesh", "value": "bd"},
  {"label": "Chile", "value": "cl"},
  {"label": "France", "value": "fr"},
  {"label": "Latvia", "value": "lv"},
  {"label": "Sweden", "value": "se"},
  {"label": "Zimbabwe", "value": "zw"}
]|j};

let json = Js.Json.parseExn(data);
