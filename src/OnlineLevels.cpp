#include "Screens.hpp"
#include "Ads.hpp"
#include <Geode/modify/CreatorLayer.hpp>
#include <Geode/modify/LevelBrowserLayer.hpp>
#include <Geode/binding/GJSearchObject.hpp>

// Pay-per-button paywall. Only the client UI is gated; no requests, accounts or servers are touched.
namespace gcu {
    struct Feat { char const* key; char const* label; };
    static const Feat FEATS[] = {
        {"search", "Search"}, {"featured", "Featured"}, {"recent", "Recent"}, {"downloaded", "Most Downloaded"}, {"liked", "Most Liked"},
        {"trending", "Trending"}, {"daily", "Daily Level"}, {"weekly", "Weekly Demon"}, {"gauntlets", "Gauntlets"}, {"lists", "Lists"},
        {"saved", "Saved Levels"}, {"followed", "Followed Creators"},
        // extra buttons that exist in the real 2.2091 menus, monetized too
        {"magic", "Magic"}, {"awarded", "Latest Stars"}, {"friends", "Friends"}, {"fame", "Hall of Fame"}, {"event", "Event Level"},
        {"mappacks", "Map Packs"}, {"leaderboards", "Leaderboards"},
    };
    static constexpr int64_t FEATURE_CENTS = 299, ENTRY_CENTS = 499;

    static bool hasEntry() { return Eco::bought("online.entry"); }
    static bool hasFeat(std::string const& k) { return Eco::bought("online.f." + k); }
    static char const* labelOf(std::string const& k) { for (auto& f : FEATS) if (k == f.key) return f.label; return "Feature"; }

    static void buyFeature(std::string const& key) {
        Price p; p.cents = FEATURE_CENTS;
        confirmPurchase(std::string("Online Levels: ") + labelOf(key), p, [key]() {
            Eco::markBought("online.f." + key);
            toast(std::string(labelOf(key)) + " unlocked permanently. Press it again.");
        });
    }
    // true => allowed to proceed
    static bool gate(std::string const& key) {
        if (!hasEntry()) { toast("Buy Online Levels access first ($4.99)", false); return false; }
        if (hasFeat(key)) return true;
        buyFeature(key);
        return false;
    }
    static void buyEntry(std::function<void()> then) {
        Price p; p.cents = ENTRY_CENTS;
        confirmPurchase("Online Levels access", p, [then]() { Eco::markBought("online.entry"); toast("Online Levels unlocked. Features sold separately."); if (then) then(); });
    }

    void openOnlineHub() {
        auto p = MenuPopup::create("Online Levels Features", []() {
            std::vector<Row> r;
            r.push_back({"Online Levels access", hasEntry() ? "Unlocked" : "Required to enter the menu", hasEntry() ? "Owned" : "$4.99", []() { if (!hasEntry()) buyEntry(nullptr); }, {255, 230, 120}});
            for (auto& f : FEATS) {
                bool own = hasFeat(f.key);
                r.push_back({f.label, own ? "Unlocked" : "Each button is sold separately", own ? "Owned" : "$2.99", [k = std::string(f.key)]() {
                    if (!hasEntry()) { toast("Buy access first ($4.99)", false); return; }
                    if (!hasFeat(k)) buyFeature(k); else toast("Already unlocked. Use it from the Creator menu.");
                }});
            }
            return r;
        }, []() { return "Wallet " + Eco::money(Eco::cents()); }, {255, 230, 120});
        openPopup(p);
    }

    static std::string featureForSearch(SearchType t) {
        switch (t) {
            case SearchType::Search: case SearchType::Users: return "search";
            case SearchType::Downloaded: return "downloaded"; case SearchType::MostLiked: return "liked";
            case SearchType::Trending: return "trending"; case SearchType::Recent: return "recent";
            case SearchType::Featured: return "featured"; case SearchType::Magic: return "magic";
            case SearchType::Awarded: return "awarded"; case SearchType::Followed: return "followed";
            case SearchType::Friends: return "friends"; case SearchType::HallOfFame: return "fame";
            case SearchType::DailySafe: return "daily"; case SearchType::WeeklySafe: return "weekly";
            case SearchType::EventSafe: return "event"; case SearchType::SavedLevels: return "saved";
            case SearchType::LevelListsOnClick: return "lists";
            default: return "";
        }
    }

    // Creator button = "Online Levels" entrance; called from the MenuLayer hook in main.cpp
    void onlineEntrance(std::function<void()> proceed) {
        if (hasEntry()) { proceed(); return; }
        buyEntry(proceed);
    }

    // Geode 6.0.0-alpha.1 does not expose hook addresses for the Daily, Weekly, Event,
    // Gauntlets, or Saved Levels CreatorLayer callbacks. Those overrides are intentionally
    // omitted so this file compiles; browser-backed searches remain gated in GCUBrowser.
    class $modify(GCUCreatorLayer, CreatorLayer) {
        void onOnlineLevels(CCObject* s) { if (gate("search")) CreatorLayer::onOnlineLevels(s); }
        void onFeaturedLevels(CCObject* s) { if (gate("featured")) CreatorLayer::onFeaturedLevels(s); }
        void onTopLists(CCObject* s) { if (gate("lists")) CreatorLayer::onTopLists(s); }
        void onMapPacks(CCObject* s) { if (gate("mappacks")) CreatorLayer::onMapPacks(s); }
        void onLeaderboards(CCObject* s) { if (gate("leaderboards")) CreatorLayer::onLeaderboards(s); }
        // Paths are replaced by the monthly battle pass
        void onPaths(CCObject*) { openPass(); }
        // Treasure Room is reworked
        void onTreasureRoom(CCObject*) { openTreasure(); }
    };

    // Quick-search buttons (Most Liked, Trending, Recent, ...) have no hookable address in these bindings,
    // so they are gated where they all converge: the browser opens, and we back out next frame if locked.
    class $modify(GCUBrowser, LevelBrowserLayer) {
        bool init(GJSearchObject* obj) {
            if (!LevelBrowserLayer::init(obj)) return false;
            if (obj) {
                auto key = featureForSearch(obj->m_searchType);
                if (!key.empty() && !hasFeat(key)) {
                    Ref<LevelBrowserLayer> self = this;
                    geode::queueInMainThread([self, key]() {
                        self->keyBackClicked();
                        if (hasEntry()) buyFeature(key); else toast("Buy Online Levels access first ($4.99)", false);
                    });
                }
            }
            return true;
        }
    };
}
