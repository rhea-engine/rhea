### Widget content and slots

Content is a semantic view of the normal parent-child relationship; it does not introduce separate ownership.

Widgets may expose one or more named content slots. A slot describes the semantic role of child widgets within their parent.

Examples:

- A `Button` exposes a single `content` slot.
- A `Toolbar` exposes the slots `start`, `fill`, and `end`.
- A `Dialog` may expose `header`, `content`, and `actions`.

Slots do not own widgets independently. Widget ownership continues to follow the `UnitTree`: 
a parent owns its children and is responsible for their destruction. Assigning a widget to a slot therefore also parents it,
either directly to the receiving widget or to an internal container owned by it.

A slot may accept either one widget or multiple widgets:

- A single-content slot replaces its previous content when new content is assigned.
- A multi-content slot appends widgets in their defined order.

Every widget that accepts arbitrary children may provide a default slot. Children without an explicit slot assignment are added to this default slot.

Slots define structure, not visual or interaction states. States such as `hover`, `active`, `focus`, and `disabled` 
remain widget states and are normally represented through CSS.

Widgets may provide convenient default content. For example, a `Button` with a `text` value creates a default `Label` 
when no explicit content has been assigned. Explicit content replaces this default representation without changing the 
button's interaction behaviour.

References returned by the content API are non-owning. They remain valid only while the referenced widget is
part of the `UnitTree`. Components that retain references to widgets owned by other components must either resolve them 
by ID when needed or be notified before their removal.

### Scene Instance Completion Guarantee

`SceneResource::buildInstance()` must report successful completion only after:

- all transitively referenced `SceneResource` instances have been built;
- all component templates have been instantiated;
- all resource dependencies known to the resulting composed tree have been loaded; and
- the resulting instance forms a complete, detached tree that has not yet received `ready()`.

After `buildInstance()` completes successfully, neither slot distribution nor `ready()` 
should introduce new resource dependencies that were not available during the build process.

Projected content must retain the base path of the resource in which it was originally defined. 
Reparenting content into a component template must not change how its relative resource references are resolved.