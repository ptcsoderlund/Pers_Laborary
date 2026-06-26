# PL — contributor guide

This document is for people working *on* PL.

## Conventions (follow these for any new module)

These patterns are already established across the codebase — match them.
- **Naming** `pl_<module>_<verb>`, example: `pl_arena_new`,`pl_arena_alloc`.
  Types are `pl_<thing>`. Enums and constants are SNAKE_UPPER_CASE.
  Everything else is snake_lower_case.
  Macros follow the same rules and adds a _x or _X suffix.
- **Opaque types:** use as handles for consumers which never see the header fields.
- **Header-before-payload**: Headers under opaque types are always first and at the position of the opaque handle.
  May be exposed with functions, never included in consumer types.
- **Error handling:** functions return a `pl_error` at runtime. PL_ERROR_NONE is good/ok/success result.
  Asserts are used in must not break code at code/dev time. Errors at runtime.
  This makes out parameters heavily used "T** out_x".
- **Visibility:** We dont care about private,internal or public access.
  If a function is only used in module, keep it private. Everything else is visible to everyone.
- **Philosophy:** Explicit is better then implicit. Static type systems are better then dynamic.
  Be explicit and do the work, avoid implicit work flows as much as possible (try/catch).
