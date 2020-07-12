let getStringValueFromEvent = (event: ReactEvent.Form.t) =>
  event->ReactEvent.Form.target##value->Belt.Option.getWithDefault("");
