# Test results (honest)

## Passed (run here)
* Logic tests, g++ against a stub Geode header (`Economy.cpp`, `Store.cpp` only):
  all 11 reward values exact incl. Extreme Demon = $25.00; wallet charge and shortfall text; purchases persist;
  energy regen and 25 cap; $100 subscription renews then lapses when the wallet is short; reset clears everything;
  corrupted save strings are skipped key by key without losing valid keys; `Store::version()` bumps on writes; `Store::fnv` is deterministic.
* Symbol check against bindings `78d5bb4` (2.2091): every hooked function, every enumerator (`SearchType`, `GJDifficulty`, `GJLevelType`,
  `GJTimedLevelType`, `IconType`) and every member used exists. Per-platform address availability was read from the bindings and drives `Platform.hpp`.
* Geode 6.0.0-beta.1 headers checked for: `Popup::init(float,float,...)`, `setTitle`, `m_closeBtn`, `m_size`, `Notification::create`,
  `CCMenuItemExt::createSpriteExtra`, `addChildAtPosition` overloads, `Mod::expandSpriteName`, `queueInMainThread`, `Ref`, `getChildByIDRecursive`.
* `build.yml.txt` parses as YAML and uses only inputs that exist in the current `build-geode-mod` `action.yml`.

## NOT tested
No compile against Geode and no game run. So these are unverified: it loads, menus render, ads, hooks fire, hidden button, cosmetics unlocking,
macOS/Android behaviour, `.geode` packaging, and co-existence with your other mods.

## Crash hardening in this version (written, not yet proven in game)
* Popups redraw only when saved data changed (version counter), never mid-click on a timer.
* Every GD sprite frame goes through `safeFrame` (blank box if missing); mod PNGs are optional and skipped if absent.
* Purchase-then-continue flows hold the layer and sender in `Ref` and re-enter the hook instead of keeping raw `this`.
* The banner is found by node ID after an ad instead of captured (no dangling pointer, no retain cycle).
* Forced ads hide the popup X and ignore back/Escape; rewarded ads can be skipped.
* Online Levels: direct button hooks where the platform has them. Where it does not, the screen is left with `backOutNextFrame`, which only acts if
  the layer is still alive, still running and (for scenes) is the current scene. A stale layer can never pop the scene stack.
* Hooks that have no address on a platform are not compiled for it (`GCU_*` macros), instead of hooking something unbound.
* `GameManager::unlockIcon` (Windows-only inline) is no longer called; purchases are honoured through the `isIconUnlocked` hook.

## Known risks / first things to check
1. Calling the original from a lambda: the code re-enters the hook after a purchase rather than calling the base directly, which is the safest pattern I know, but unverified.
2. "Gated when the page opens" on macOS shows a one-frame flash of the target screen.
3. `"resources": {"sprites": [...]}` packaging and `expandSpriteName` paths: if textures fail to load the UI still works without them.
4. Level difficulty detection is best effort (`m_ratings/m_ratingsSum`, then `m_difficulty`); unrated levels pay as Auto.
5. Editor Premium gates playtesting only.
6. iOS is not supported.
