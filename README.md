# Level Bucks: Economy Simulator

An intentionally overcomplicated, friendly satire of free-to-play game economies. “Totally Legit GD Moderator” is a fictional local mod role. Nothing in this project grants server privileges, changes account data, rates or uploads levels, or uses real money. There are no ads, checkout flows, or network requests for currency.

## Research and feasibility

Geode v6.0.0-alpha.1 release notes announce Geometry Dash 2.209x support. The manifest uses `6.0.0-alpha.1` without a `v` prefix to match the SDK's `GEODE_VERSION_FULL` value used by the CMake compatibility check. The GitHub action still selects the release tag as `v6.0.0-alpha.1`. The official binding file names `PlayLayer::levelComplete`, but its binding is Windows-only; the completion payout is therefore not implemented for this macOS/Android build. The game did not open for live menu inspection, so the native Online Levels controls remain untouched and a separate mod-only mock portal is used instead. No external mod dependencies are required.

| Feature | Status |
| --- | --- |
| MacOS and Android64 build workflow | Configured; requires a successful GitHub Actions run to verify |
| $100 local wallet and persistent one-time purchases | Implemented with Geode saved values |
| Online Levels button ($5.99 Level Bucks) | Implemented as an unlock to a fictional local mock portal; native online browser is not intercepted |
| Shop prices, details, insufficient funds, already-owned checks | Implemented |
| Fake moderator stamp, star counter, spike gag, reversible clutter theme | Implemented as local mod-panel gags only |
| In-level MOD shortcut and pretend suggest/feature/remove/ban controls | Implemented as local parody dialogs; no server action or real moderator access |
| GD Colon video-inspired ad/energy/loot chest/season pass parody | Implemented as optional fake dashboard; no ads, payments, gameplay gating, or real rewards |
| Developer drawer, unlock everything in this mod, reset | Implemented; resets ask for confirmation |
| 2.3 Objects | Original text preview gallery only; it does not register or save editor objects |
| 5x speed | Disabled mock that says it is unsupported; gameplay is unchanged |
| First-completion rewards | Not implemented: current binding has `PlayLayer::levelComplete` for Windows only, not macOS/Android |
| Native Online Levels button gating | Not implemented: target menu was unavailable for inspection, and native navigation is left intact |

### Proposed transparent reward table

Rewards are in fictional **Level Bucks**, never real dollars. This table is design intent only until completion detection is verified.

| Official level difficulty | Reward |
| --- | ---: |
| Easy | $1 |
| Normal | $2 |
| Hard | $3 |
| Harder | $5 |
| Insane | $8 |
| Demon (non-Extreme) | $15 |
| Extreme Demon | $25 |

The categories follow Geometry Dash’s official rating labels; the reward amounts are original balancing choices. They are shown as a preview only because the cross-platform completion hook is unavailable in the pinned bindings.

## Implemented now

- Geode v6 alpha mod targeting macOS and Android / GD 2.2091.
- GitHub Actions workflow for macOS and Android 64-bit builds, pinned to Geode v6.0.0-alpha.1 and CLI v4.0.0.
- Main-menu entry and multi-page, mod-only UI.
- In-level `MOD` shortcut with a pretend moderator panel and harmless parody dialogs.
- Optional mobile-game parody dashboard for fake ads, energy, loot chests, and a season pass.
- Local wallet seed at $100 Level Bucks, with persistent one-time purchases.
- Shop, shortcut page, fake Online Levels portal, gag panel, preview gallery, rewards preview, and developer drawer.
- Settings entries for economy, sound/UI gags, and experimental features. The economy switch is used; sound and experimental settings are placeholders until those features are supported.
- No network calls, purchases with real money, account changes, or server interactions.

The UI and local economy are implemented, but the code has not been compiled or run in Geometry Dash. Completion rewards are not paid in this build. The mod has no other mod dependencies, so it builds on its own.

## Build requirements

- Geometry Dash 2.2091.
- Geode v6.0.0-alpha.1 SDK and CLI v4.0.0 or newer.
- CMake 3.21 or newer and the platform’s Geode build toolchain.
- Build from this folder with `geode build` after installing/configuring the SDK and CLI.
- The top-level CMake file disables C++ module dependency scanning because this mod does not use modules; this avoids the Android NDK build trying to scan the runner's host Linux standard-library module sources.

No build has been run in this environment because CMake and the Geode CLI/SDK are absent. Push this repository to GitHub, then open **Actions → Build Geode Mod**. The workflow uploads a **Build Output** artifact containing the platform packages if both builds succeed. Platform compatibility is not claimed until those builds complete and the mod is launched against the target game.

## Reset

The home page includes a confirmation-protected reset that restores the $100 wallet and clears this mod’s shop unlocks and gag toggles. It does not change Geometry Dash data.

## Sources

- [Geode v6.0.0-alpha.1 release](https://github.com/geode-sdk/geode/releases/tag/v6.0.0-alpha.1)
- [Geode mod creation and build guide](https://github.com/geode-sdk/docs/blob/main/getting-started/create-mod.md)
- [Geode savedata guide](https://github.com/geode-sdk/docs/blob/main/mods/savedata.md)
- [Geode settings guide](https://github.com/geode-sdk/docs/blob/main/mods/settings.md)
- [RobTop’s official Geometry Dash rating guide](https://www.robtopgames.com/files/GDRating.pdf)
- [Geometry Dash 2.209 bindings](https://raw.githubusercontent.com/geode-sdk/bindings/main/bindings/2.209/GeometryDash.bro)
- [Official Geode multi-platform GitHub Actions example](https://github.com/geode-sdk/build-geode-mod/blob/main/examples/multi-platform.yml)
- [GD Colon: “How to Ruin Geometry Dash”](https://www.youtube.com/watch?v=5TlxFXFi8UU)
- [Geometry Dash official Moderator FAQ](https://www.robtopgames.com/faq/en/answers/moderator/)
- [Geometry Dash official level rating guide](https://www.robtopgames.com/files/GDRating.pdf)
