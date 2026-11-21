# Research notes and feature checklist

## What I could verify (done in the build sandbox)
* Geode SDK tag **v6.0.0-beta.1** (commit 0bf4857, 2026-11-20) cloned; `VERSION` reads `6.0.0-beta.1`. beta.1 changelog is two small fixes on top of alpha.3; API used by this mod is unchanged.
* Bindings for **GD 2.2091** inspected at commit 78d5bb4 (2026-11-20, one minute before the beta.1 tag). Windows, macOS and Android availability was checked per function (Android links by symbol for every hooked class). Hooked functions:
  `MenuLayer::onCreator`, `CreatorLayer::{onOnlineLevels,onFeaturedLevels,onDailyLevel,onWeeklyLevel,onEventLevel,onGauntlets,onTopLists,onSavedLevels,onMapPacks,onLeaderboards,onPaths,onTreasureRoom}`,
  `LevelBrowserLayer::init(GJSearchObject*)`, `PlayLayer::{init(GJGameLevel*,bool),resetLevel,levelComplete}`, `EditorUI::onPlaytest`.
* Real menus in 2.2091 that exist beyond the prompt's 12-button list: Magic, Latest Stars, Friends, Hall of Fame, Event Level, Map Packs, Leaderboards. All are monetized.
* Quick-search buttons (Most Downloaded, Most Liked, Recent, Trending, Magic, Latest Stars, Followed) are bound on Windows (and by symbol on Android) in this bindings commit, but NOT on macOS.
  macOS gates them when `LevelBrowserLayer` opens with the matching `SearchType`.
* Popup API in beta.1 is the non-template `geode::Popup` with `init(float, float, ...)` (read from the SDK header).

## What I could NOT do
* **The video.** `youtube.com` was not fetchable from the sandbox, so I have no transcript, captions or timestamps. I did not watch it.
  The "directly demonstrated" column below is therefore EMPTY on purpose. Nothing is claimed as appearing in the video.
  Action for you: paste the transcript or timestamps and I will fill this in.
* **Geode v6 migration guide** (docs.geode-sdk.org) was blocked by the fetch allowlist; I used the SDK headers and changelog instead.
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
* https://github.com/geode-sdk/geode (tag v6.0.0-beta.1)
* https://github.com/geode-sdk/build-geode-mod (action.yml and examples/multi-platform.yml, basis for build.yml.txt)
* https://github.com/geode-sdk/bindings (commit 78d5bb4eb0732dd636ae16d4dcf6e5ce84ec1221, `bindings/2.2091/GeometryDash.bro`)
