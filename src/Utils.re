module Dom = {
  let getKeyName = Webapi.Dom.KeyboardEvent.key;
  let isKeyRepeated = Webapi.Dom.KeyboardEvent.repeat;

  let addKeyboardListeners = (~keyDownHandler, ~keyUpHandler, document) => {
    Webapi.Dom.Document.addKeyDownEventListener(keyDownHandler, document);
    Webapi.Dom.Document.addKeyUpEventListener(keyUpHandler, document);
  };

  let removeKeyboardListeners = (~keyDownHandler, ~keyUpHandler, document) => {
    Webapi.Dom.Document.removeKeyDownEventListener(keyDownHandler, document);
    Webapi.Dom.Document.removeKeyUpEventListener(keyUpHandler, document);
  };
};

module React = {
  let (&&&) = (condition: bool, element: React.element) =>
    if (condition) {element} else {React.null};
};
