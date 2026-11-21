# How To Ruin Geometry Dash (Geode mod, parody)

Satirical mod for **Geometry Dash 2.2091** on **Geode 6.0.0-beta.1**, for **Windows, macOS and Android**.
Every button costs fictional wallet credit. No real payments, no accounts, no server traffic.
The "moderator" and "RobTop" tools act on local demo data only.

> STATUS: source only. **Not compiled and not run in the game** (the authoring sandbox has no Geode toolchain or GD).
> See docs/TESTING.md for what was and was not verified.

## Build
Easiest: GitHub Actions. Copy `build.yml.txt` to `.github/workflows/build.yml` in your repo, push, and download the combined
`.geode` (Windows + macOS + Android32 + Android64) from the workflow artifacts.

Local build (one platform at a time):
1. Clone `https://github.com/geode-sdk/geode` at tag `v6.0.0-beta.1`, set `GEODE_SDK` to it.
2. Reproducible bindings: clone `geode-sdk/bindings`, check out `78d5bb4eb0732dd636ae16d4dcf6e5ce84ec1221`, set `GEODE_BINDINGS_REPO_PATH` to it.
   (The SDK tag otherwise downloads bindings `main`, which keeps moving.)
3. `geode build`, then `geode package`.

## Platform notes
| Feature | Windows | Android | macOS |
|---|---|---|---|
| $4.99 entrance, Search, Featured, Lists, Map Packs, Leaderboards, Friends | button hooks | button hooks | button hooks |
| Daily, Gauntlets, Saved | button hooks | button hooks | gated when the page opens |
| Weekly, Event | button hooks | button hooks | Intel: button hooks, Apple Silicon: gated when the page opens |
| Most Liked / Downloaded / Recent / Trending / Magic / Latest Stars / Followed | button hooks | button hooks | gated when the browser opens |
| Hall of Fame | gated when the browser opens | same | same |
"Gated when the page opens" means the screen opens and closes again on the next frame, then the purchase dialog shows.
Reason: those buttons have no address in the macOS bindings, so they cannot be hooked directly.

## Your other mods
Node IDs, ImagePlus, Good Grid, Editor Touch API, Editor Tab API, Alpha's UI Pack and Alpha's Geode Utils are **not** dependencies.
The mod uses none of their APIs and loads without them. Overlap is limited to `MenuLayer`, `CreatorLayer`, `LevelBrowserLayer`,
`LevelSearchLayer` and `EditorUI::onPlaytest`, and every hook calls the original. I could not test them together; if one conflicts, tell me which.

## What is where
| File | Purpose |
|---|---|
| src/Platform.hpp | Per-platform binding macros, safe "leave this screen" helper, sprite path helper |
| src/Store.* | Save system (one string in Geode saved values, per-key tolerant parser, version counter, reset) |
| src/Economy.* | Wallet (cents), currencies, 25-cap energy with regen, subscriptions, exact reward table |
| src/UI.* | Purchase confirmation, paged list popup used by every screen, cash rain, toasts |
| src/Ads.* | Rewarded / forced / banner ads |
| src/Rewards.cpp | PlayLayer hooks: level-complete payout, attempt counter, energy, GeoPass XP |
| src/Cosmetics.cpp | Cosmetic shop, rotating stock, collection score, `isIconUnlocked` hook |
| src/Shop.cpp | Store bundles, exchange, wallet/reset, subscriptions, GeoPass, objectives, Treasure Room, creator program |
| src/OnlineLevels.cpp | $4.99 entrance and $2.99 per button (19 buttons) |
| src/Moderator.cpp | $100/month parody moderator panel (local demo data) |
| src/RobTopMenu.cpp | Hidden menu (7 taps on the dim square, bottom-left of the main menu) plus a second secret menu |
| src/main.cpp | Main menu buttons, status bar, banner, editor playtest gate |
| docs/make_art.py | Generates resources/*.png (procedural art) |
