module Styles = {
  open Css;

  let a = style([textDecoration(`none)]);
};

[@react.component]
let make = (~route, ~className, ~children) => {
  <a
    href={route->Route.toString}
    className={Css.merge([Styles.a, className])}
    onClick={event =>
      if (!event->ReactEvent.Mouse.defaultPrevented
          && event->ReactEvent.Mouse.button == 0
          && !event->ReactEvent.Mouse.altKey
          && !event->ReactEvent.Mouse.ctrlKey
          && !event->ReactEvent.Mouse.metaKey
          && !event->ReactEvent.Mouse.shiftKey) {
        event->ReactEvent.Mouse.preventDefault;
        route->Route.redirect;
      }
    }>
    children
  </a>;
};
