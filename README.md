# How To Ruin Geometry Dash (Geode mod, parody)

Satirical mod for **Geometry Dash 2.2091** on **Geode 6.0.0-alpha.1** (Windows target). Every button costs fictional wallet credit.
No real payments, no accounts, no server traffic. All "moderator" and "RobTop" tools act on local demo data only.

> STATUS: source only. **Not compiled and not tested in-game.** The authoring sandbox had no Windows toolchain and no GD binary.
> See docs/TESTING.md for exactly what was and was not verified.

## Build (Windows, Geode CLI installed)
1. `git clone --branch v6.0.0-alpha.1 https://github.com/geode-sdk/geode` and set `GEODE_SDK` to it. Run `geode sdk install` / `geode sdk update` as usual and confirm `geode sdk version` prints 6.0.0-alpha.1.
2. **Pin bindings.** The SDK tag fetches `geode-sdk/bindings@main`, which moves. This mod was checked against bindings commit
   `4e6c6d24b09502337f22c9db2aadb371fe7ee8ba` (2026-10-31). Clone it and set `GEODE_BINDINGS_REPO_PATH` to that checkout before configuring.
3. `geode build` in this folder (or `cmake -B build && cmake --build build --config RelWithDebInfo`), then `geode package` to produce the `.geode`.
4. If something fails to compile, report the first error; the likely spots are listed in docs/TESTING.md.

## What is where
| File | Purpose |
|---|---|
| src/Store.* | Save system: one string in Geode saved values, per-key tolerant parser, reset |
| src/Economy.* | Wallet (cents), currencies, 25-cap energy with regen, subscriptions, exact reward table |
| src/UI.* | Purchase confirm popup, paged list popup used by every screen, cash rain, shake, toasts |
| src/Ads.* | Rewarded / forced / banner ads |
| src/Rewards.cpp | PlayLayer hooks: level-complete payout, attempt counter, energy, GeoPass XP |
| src/Cosmetics.cpp | Cosmetic shop: 14 categories, 6 rarities, rotating stock, vault, collection score |
| src/Shop.cpp | Store bundles, exchange, wallet/reset, subscriptions, GeoPass, objectives, Treasure Room, creator program |
| src/OnlineLevels.cpp | $4.99 entrance + $2.99 per feature (19 buttons) |
| src/Moderator.cpp | $100/month parody moderator panel (local demo data) |
| src/RobTopMenu.cpp | Hidden menu (7 taps on the dim square, bottom-left of main menu) + second secret menu |
| src/main.cpp | Main menu buttons, status bar, banner, editor playtest gate |
| docs/make_art.py | Generates resources/*.png (procedural art) |

## How the rules are implemented
* Rewards: Auto $0.10, Easy $0.25, Normal $0.50, Hard $1, Harder $2, Insane $5, Easy Demon $7.50, Medium Demon $10, Hard Demon $15, Insane Demon $20, Extreme Demon $25 (unit tested).
  Paid on first completion only; replays pay 10%; none in practice or testmode or for editor levels; one payout per play session; at 0 energy payouts drop to 25%.
* Energy: max 25, +1 per 120 s. Starting a level spends 1 and doubles the orb bonus. Cash stays exact so the table is honest.
* Online Levels: entrance is gated on the Creator button; each button is gated at its handler; Most Liked, Trending, Recent etc. are gated when the level browser opens.
* Subscriptions: a "month" is `sub-period-minutes` real minutes (default 30). Auto-renews from the wallet, lapses when short.
* Treasure Room contents are fixed and shown before purchase; duplicates are refunded in shards. No random paid drops.
