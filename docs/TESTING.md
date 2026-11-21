# Test results (honest)

## Passed
* Logic tests (`Economy.cpp` + `Store.cpp`, built with g++ against a stub of the Geode header):
  all 11 reward values exact incl. Extreme Demon = $25.00; wallet charge and shortfall text; purchases persist in the save string;
  energy regeneration and 25 cap; $100 subscription renews, then lapses when the wallet is short; reset clears everything.
* Symbol check: every hooked method exists with a Windows address in the 2.2091 bindings; `m_level`, `m_isPracticeMode`, `m_isTestMode` live on `GJBaseGameLayer`;
  `GJSearchObject::m_searchType`, `GJGameLevel::{m_demonDifficulty,m_ratings,m_ratingsSum,m_autoLevel,m_levelType}` exist.

## NOT tested (no compiler for Geode, no game)
Checklist items needing a real build: mod loads on Alpha 1, menus open/close repeatedly without crashes, popups render, ads, hooks fire,
purchase flow on screen, hidden RobTop button, cosmetics actually unlocking in the icon kit, `.geode` packaging.

## Likely first compile errors / risks
1. `CCMenuItemExt::createSpriteExtra` and `ButtonSprite::create(text,width,minWidth,scale,absolute,font,texture,height)` overloads: confirm signatures in the Alpha 1 headers.
2. `"name.png"_spr` and `"resources": {"sprites": [...]}` packaging: if sprites do not load, the code falls back to nothing (backgrounds skip, icons use a drawn square).
3. `Notification::create(..., NotificationIcon::Success, ...)` enum scope under alpha.1.
4. Online Levels quick-search gating pops the browser on the next frame (visible flicker). A cleaner hook needs those button handlers bound.
5. Level difficulty detection is best effort (`m_ratings/m_ratingsSum`, then `m_difficulty`); unrated levels pay as Auto. Check on real downloaded levels.
6. Editor Premium gates **playtesting only**; no other editor feature is limited yet.
7. Windows only (`mod.json`), because other platforms lack addresses for several hooked functions.
8. Bindings pin: see README step 2.
