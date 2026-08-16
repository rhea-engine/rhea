# Widget content and slots

Content is a semantic view of the normal parent-child relationship; it does not introduce separate ownership.

Widgets may expose one or more named content slots. A slot describes the semantic role of child widgets without exposing 
the widget's internal presentation tree to callers.

Examples:

- A `Button` exposes a single `content` slot.
- A `Toolbar` exposes the slots `start`, `fill`, and `end`.
- A `Dialog` may expose `header`, `content`, and `actions`.

Slots do not own widgets independently. Widget ownership continues to follow the `UnitTree`: 
a parent owns its children and is responsible for their destruction. Assigning a widget to a slot therefore also parents it, 
either directly to the receiving widget or to an internal container owned by it. Moving content into an internal slot container 
is normal reparenting and must not duplicate ownership.

A slot may accept either one widget or multiple widgets:

- A single-content slot accepts at most one widget. Assigning another widget fails unless an explicit replacement operation is used.
- A multi-content slot appends widgets in their defined order.

Every widget that accepts arbitrary children may provide a default slot, normally named `content`. 
Children without an explicit slot assignment are assigned to this default slot. 
Widgets without a suitable default slot should require an explicit slot name.

`emplaceChild<T>()` creates and configures a widget and adds it as a normal child. 
`emplaceContent<T>()` additionally assigns the created widget to a content slot. Slot assignment must either succeed completely, 
including any required reparenting, or report failure without leaving ambiguous slot metadata.

`assignToSlot()` records the semantic slot assignment and delegates physical placement to the virtual `addToSlot()` hook. 
The default `Widget::addToSlot()` implementation adds content as a direct child. Composite widgets override the hook to route 
content into private containers. `removeFromSlot()` provides the corresponding removal hook.

For example, a `Button` may keep its content as a direct child, while a `Toolbar` routes `start`, `fill`, and `end` content 
into private layout containers. Callers must not depend on these containers or call `addToSlot()` directly. They use 
`assignToSlot()` or `emplaceContent()` so semantic assignment and physical parenting remain synchronized.

Slots define structure, not visual or interaction states. States such as `hover`, `active`, `focus`, and `disabled` 
remain widget states and are normally represented through CSS.

Widgets may provide convenient default content. For example, a `Button` with a `text` value creates a default `Label` 
when no explicit content has been assigned. The default label is itself assigned to the `content` slot. 
Explicit content suppresses this default representation without changing the button's interaction behaviour. 
Button layout therefore operates on the `content` slot, not specifically on the optional default label.

## Composition and lifecycle

Slot distribution is part of composition. It must complete before the recursive base `Widget::ready()`/`Unit::ready()` initializes 
the composed subtree. A composite widget may therefore perform synchronous fallback composition at the beginning of its own 
`ready()` and call its base implementation only after its private containers have been created and existing slot content has been distributed.

Before its private structure exists, an overridden `addToSlot()` may temporarily keep assigned content as a direct child of the 
receiving widget. During composition, the widget reparents all registered slot content into the appropriate private containers. 
Once composed, `addToSlot()` routes newly assigned content directly to those containers. Reparenting must preserve the semantic 
slot assignment and resource base path.

Hard-coded widgets may create private layout containers during an earlier composition/build step. A synchronous fallback during 
`ready()` is acceptable only when it cannot introduce new resource dependencies. Template discovery, asynchronous loading, and 
dependency collection belong to the build/composition phase before `ready()`.

If slot content can be assigned after a widget is ready, `addToSlot()` must route it to the appropriate internal container immediately. 
The receiving widget must also ensure that the new subtree has the required ready state and invalidate style and layout. Until 
ready-state propagation for dynamically added children is defined, a widget may restrict content assignment to the pre-ready phase 
and must document that restriction.

References returned by the content API are non-owning. They remain valid only while the referenced widget is
part of the `UnitTree`. Components that retain references to widgets owned by other components must either resolve them 
by ID when needed or be notified before their removal.

## Scene instance completion guarantee

`SceneResource::buildInstance()` must report successful completion only after:

- all transitively referenced `SceneResource` instances have been built;
- all component templates have been instantiated;
- all resource dependencies known to the resulting composed tree have been loaded; and
- the resulting instance forms a complete, detached tree that has not yet received `ready()`.

After `buildInstance()` completes successfully, neither slot distribution nor `ready()` 
should introduce new resource dependencies that were not available during the build process.

Projected content must retain the base path of the resource in which it was originally defined. 
Reparenting content into a component template must not change how its relative resource references are resolved.
