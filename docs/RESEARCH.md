# Research notes and feature checklist

## What I could verify (done in the build sandbox)
* Geode SDK tag **v6.0.0-alpha.1** cloned; `VERSION` file reads `6.0.0-alpha.1`.
* Bindings for **GD 2.2091** inspected (commit 4e6c6d2, 2026-10-31). Every hooked function was grepped and has a Windows address:
  `MenuLayer::onCreator`, `CreatorLayer::{onOnlineLevels,onFeaturedLevels,onDailyLevel,onWeeklyLevel,onEventLevel,onGauntlets,onTopLists,onSavedLevels,onMapPacks,onLeaderboards,onPaths,onTreasureRoom}`,
  `LevelBrowserLayer::init(GJSearchObject*)`, `PlayLayer::{init(GJGameLevel*,bool),resetLevel,levelComplete}`, `EditorUI::onPlaytest`.
* Real menus in 2.2091 that exist beyond the prompt's 12-button list: Magic, Latest Stars, Friends, Hall of Fame, Event Level, Map Packs, Leaderboards. All are monetized.
* Real quick-search buttons (Most Downloaded, Most Liked, Recent, Trending, Followed) have **no bound address**, so they cannot be hooked directly.
  They are gated when `LevelBrowserLayer` opens with the matching `SearchType`.
* Popup API in alpha.1 is the non-template `geode::Popup` (read from the SDK's own `ConfirmUninstallPopup`).

## What I could NOT do
* **The video.** `youtube.com` was not fetchable from the sandbox, so I have no transcript, captions or timestamps. I did not watch it.
  The "directly demonstrated" column below is therefore EMPTY on purpose. Nothing is claimed as appearing in the video.
  Action for you: paste the transcript or timestamps and I will fill this in.
* **Geode v6 migration guide** (docs.geode-sdk.org/tutorials/migrate-v6) was blocked by the fetch allowlist; I used the SDK source instead.
* **Image generation** was unavailable. All art is procedural (docs/make_art.py), original, and not AI-generated.
* **Compile / run**: no Windows toolchain, no Geometry Dash.

## Feature checklist
| Feature | Source | Status |
|---|---|---|
| Features shown in the video | video | UNKNOWN (no transcript) |
| Reward table by difficulty ($0.10 to $25) | prompt | implemented, unit tested |
| $4.99 Online Levels entrance | prompt | implemented, untested in game |
| $2.99 per Online Levels button (12 listed + 7 more found) | prompt | implemented, untested in game |
| $100/month Moderator Membership + panel with all 9 listed buttons | prompt | implemented, untested in game |
| ROBTop's Secret Menu + second hidden menu | prompt | implemented, untested in game |
| Gems, energy (25 max), reworked currencies, exchange | prompt | implemented |
| Rewarded / forced / banner ads | prompt | implemented |
| 15-style cosmetics, 6 rarities, rotation, collection score | prompt | implemented (colors/particles/decorations are mod-only flags) |
| Treasure Room, GeoPass, objectives, creator program, $20 top comment | prompt | implemented |
| Editor Premium $5/month | prompt | playtest gate only (see TESTING.md) |
| Save + reset | prompt | implemented, unit tested |

## Sources
* https://github.com/geode-sdk/geode (tag v6.0.0-alpha.1)
* https://github.com/geode-sdk/bindings (commit 4e6c6d24b09502337f22c9db2aadb371fe7ee8ba, `bindings/2.2091/GeometryDash.bro`)
