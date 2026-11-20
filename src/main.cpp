#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <algorithm>
#include <iterator>
#include <string>
#include <vector>

using namespace geode::prelude;

namespace {
constexpr int64_t STARTING_BALANCE = 10000;
constexpr auto WALLET = "wallet-cents";
constexpr auto OWNED = "owned-items";
constexpr auto APPROVED = "local-approval-stamp";
constexpr auto SPIKE = "local-spike-ban";
constexpr auto CHAOS = "local-chaos-theme";

struct Product { char const* id; char const* name; char const* use; int64_t cents; };
constexpr Product PRODUCTS[] = {
    {"online", "Online Levels button", "Opens a local mock portal; native online menus remain untouched.", 599},
    {"stars", "9000 Star Shower", "Temporary cosmetic counter only; never changes game stats.", 999},
    {"spike", "Spike Amnesty", "Reversibly hides a decorative spike in this mod panel.", 250},
    {"chaos", "Menu Worse-ifier", "Reversible clutter theme for this mod panel.", 777},
    {"objects", "2.3 Objects Preview Pack", "Original gallery art; no editor objects or level data.", 1299},
    {"dev", "Developer Buttons Drawer", "Fake local gags, this-mod-only unlock-all, and reset controls.", 2000},
};
std::string money(int64_t cents) { return fmt::format("{}{}.{:02} Level Bucks", "$", cents / 100, cents % 100); }
int64_t balance() {
    auto n = Mod::get()->getSavedValue<int64_t>(WALLET, STARTING_BALANCE);
    return n < 0 ? STARTING_BALANCE : n;
}
void saveBalance(int64_t n) { Mod::get()->setSavedValue<int64_t>(WALLET, std::max<int64_t>(0, n)); }
std::vector<std::string> inventory() { return Mod::get()->getSavedValue<std::vector<std::string>>(OWNED, {}); }
bool hasItem(char const* id) {
    auto v = inventory();
    return std::find(v.begin(), v.end(), id) != v.end();
}
void grant(char const* id) {
    auto v = inventory();
    if (std::find(v.begin(), v.end(), id) == v.end()) {
        v.emplace_back(id);
        Mod::get()->setSavedValue<std::vector<std::string>>(OWNED, v);
    }
}

class BucksPopup : public geode::Popup {
protected:
    enum Page { Home, Shop, Shortcuts, Gags, Gallery, Dev, Portal, Rewards };
    bool init() {
        if (!Popup::init(370.f, 310.f)) return false;
        setTitle("Level Bucks - LOCAL MOD GAGS");
        m_body = CCNode::create();
        m_body->setContentSize({340.f, 245.f});
        m_mainLayer->addChildAtPosition(m_body, Anchor::Center);
        showPage(Home);
        return true;
    }
public:
    static BucksPopup* create() {
        auto p = new BucksPopup();
        if (p->init()) { p->autorelease(); return p; }
        delete p;
        return nullptr;
    }
    void clicked(CCObject* obj) {
        int n = obj->getTag();
        switch (n) {
            case 1: showPage(Shop); break;
            case 2: showPage(Shortcuts); break;
            case 3: showPage(Gags); break;
            case 4: showPage(Gallery); break;
            case 5: showPage(Dev); break;
            case 6: showPage(Rewards); break;
            case 7: reset(); break;
            case 8: showPage(Home); break;
            case 9: showPage(Portal); break;
            case 10: approval(); break;
            case 11: stars(); break;
            case 12: spike(); break;
            case 13: chaos(); break;
            case 14: unlockAll(); break;
            case 15: restoreLook(); break;
            case 20: showPage(Gallery); break;
            case 21: showPage(Dev); break;
            case 22: speedMock(); break;
            default:
                if (n >= 100 && n < 100 + static_cast<int>(std::size(PRODUCTS))) buy(n - 100);
        }
    }
private:
    CCNode* m_body = nullptr;
    int m_stars = 0;

    void label(std::string s, float y, float scale = .4f) {
        auto l = CCLabelBMFont::create(s.c_str(), "bigFont.fnt");
        if (!l) return;
        l->setScale(scale);
        l->setPosition({170.f, y});
        m_body->addChild(l);
    }
    void button(std::string s, int tag, float y) {
        auto l = CCLabelBMFont::create(s.c_str(), "goldFont.fnt");
        if (!l) return;
        l->setScale(.42f);
        auto item = CCMenuItemLabel::create(l, this, menu_selector(BucksPopup::clicked));
        item->setTag(tag);
        auto menu = CCMenu::create();
        menu->setPosition({170.f, y});
        menu->addChild(item);
        m_body->addChild(menu);
    }
    void showPage(Page p) {
        m_body->removeAllChildren();
        float y = 222.f;
        auto line = [&](std::string s, float size = .37f) { label(s, y, size); y -= 24.f; };
        auto action = [&](std::string s, int tag) { button(s, tag, y); y -= 26.f; };
        switch (p) {
            case Home:
                line("WALLET: " + money(balance()), .49f);
                line("Totally Legit GD Moderator (local role)", .34f);
                action("Shop: buy mod-only buttons", 1);
                action("Owned shortcuts", 2);
                action("LOCAL MOD GAGS", 3);
                action("2.3 Objects preview gallery", 4);
                action("Developer Buttons drawer", 5);
                action("Reward table and status", 6);
                action("Reset wallet and unlocks...", 7);
                break;
            case Shop:
                line("ONE-TIME UNLOCKS - LEVEL BUCKS ONLY", .36f);
                line("Choose an item to inspect details and confirm.", .30f);
                for (size_t i = 0; i < std::size(PRODUCTS); ++i) {
                    auto const& item = PRODUCTS[i];
                    action(std::string(hasItem(item.id) ? "OWNED " : "BUY ") + item.name + " " + money(item.cents), 100 + static_cast<int>(i));
                }
                action("5x Speed - NOT SUPPORTED (mock)", 22);
                action("Back", 8);
                break;
            case Shortcuts:
                line("MOD-ONLY SHORTCUTS", .43f);
                if (hasItem("online")) action("Open Online Levels mock portal", 9);
                if (hasItem("objects")) action("Open preview gallery", 20);
                if (hasItem("dev")) action("Open fake Developer Buttons", 21);
                if (!hasItem("online") && !hasItem("objects") && !hasItem("dev")) line("No shortcuts owned. Visit the Shop.", .33f);
                line("Native Online Levels menu is untouched.", .29f);
                action("Back", 8);
                break;
            case Gags:
                line("LOCAL MOD GAGS - THIS PANEL ONLY", .38f);
                action(Mod::get()->getSavedValue<bool>(APPROVED, false) ? "Remove fake approval stamp" : "Approve my own level (fake stamp)", 10);
                action(std::string("9000 Star Shower ") + (hasItem("stars") ? "(owned)" : "(Shop unlock)"), 11);
                action(std::string(Mod::get()->getSavedValue<bool>(SPIKE, false) ? "Unban decorative spike" : "Ban decorative spike") + (hasItem("spike") ? "" : " (Shop unlock)"), 12);
                action(std::string(Mod::get()->getSavedValue<bool>(CHAOS, false) ? "Restore tidy panel" : "Make this panel worse") + (hasItem("chaos") ? "" : " (Shop unlock)"), 13);
                if (m_stars) line("TEMPORARY COSMETIC COUNTER: 9000 stars", .3f);
                if (Mod::get()->getSavedValue<bool>(APPROVED, false)) line("LOCAL FAKE APPROVAL STAMP", .31f);
                line(Mod::get()->getSavedValue<bool>(SPIKE, false) ? "Decorative spike: banned from this panel." : "Decorative spike: ^", .30f);
                action("Back", 8);
                break;
            case Gallery:
                line("2.3 OBJECTS - FAN-MADE PREVIEW", .38f);
                line("Art and labels only, not editor objects.", .29f);
                line("REFUND PORTAL | AUTO-RENEWAL PAD", .34f);
                line("SUBSCRIPTION SAW | PREMIUM CHECKPOINT", .32f);
                line("CANCEL-ANYTIME SPIKE | TERMS ORB", .34f);
                line(hasItem("objects") ? "Pack owned; creates no level data." : "Locked: buy the Preview Pack in Shop.", .30f);
                action("5x Speed - NOT SUPPORTED (mock)", 22);
                action("Back", 8);
                break;
            case Dev:
                line("FAKE DEVELOPER BUTTONS - LOCAL MOD GAGS", .35f);
                if (!hasItem("dev")) line("Locked: unlock this drawer in the Shop.", .33f);
                else {
                    action("Unlock Everything (in this menu)", 14);
                    action("Restore this panel's default look", 15);
                    action("LOCAL MOD GAGS", 3);
                    line("No real moderation, uploads, or account changes.", .28f);
                }
                action("Back", 8);
                break;
            case Portal:
                line("ONLINE LEVELS BUTTON - LOCAL MOCK", .40f);
                line("Not the native online browser.", .33f);
                line("TRENDING: Expensive Jump | FRIENDS: You (offline)", .27f);
                line("SEARCH: free button | FEATURED: Premium Square", .28f);
                line("No network or level downloads.", .32f);
                action("Back", 8);
                break;
            case Rewards:
                line("LEVEL BUCKS REWARD TABLE (PREVIEW)", .39f);
                line("Easy $1 | Normal $2 | Hard $3 | Harder $5", .34f);
                line("Insane $8 | Demon $15 | Extreme Demon $25", .34f);
                line("Completion payout is pending platform bindings.", .30f);
                line("No completion rewards are currently issued.", .30f);
                action("Back", 8);
                break;
        }
        if (Mod::get()->getSavedValue<bool>(CHAOS, false))
            label("ADVERTISEMENT SPACE: STILL PAYING RENT", 240.f, .27f);
    }
    void buy(int i) {
        auto const& item = PRODUCTS[i];
        auto details = fmt::format("{}\n\n{}\n\nPrice: {}\nFictional in-game currency: Level Bucks.", item.name, item.use, money(item.cents));
        if (hasItem(item.id)) return FLAlertLayer::create("Already owned", details.c_str(), "OK")->show();
        if (!Mod::get()->getSettingValue<bool>("economy-enabled"))
            return FLAlertLayer::create("Economy disabled", "Enable the economy in this mod's settings before buying.", "OK")->show();
        Ref<BucksPopup> self(this);
        std::string id = item.id;
        auto price = item.cents;
        createQuickPopup("Confirm local unlock", details, "Cancel", "Buy", [self, id, price](auto, bool confirm) {
            if (!confirm) return;
            if (hasItem(id.c_str())) return;
            auto current = balance();
            if (current < price) {
                FLAlertLayer::create("Not enough Level Bucks", fmt::format("Need {}. Balance: {}.", money(price), money(current)).c_str(), "OK")->show();
                return;
            }
            saveBalance(current - price);
            grant(id.c_str());
            self->showPage(Shop);
        });
    }
    void approval() {
        auto next = !Mod::get()->getSavedValue<bool>(APPROVED, false);
        Mod::get()->setSavedValue<bool>(APPROVED, next);
        FLAlertLayer::create("LOCAL MOD GAG", next ? "Fake stamp added to this panel only. Nothing was rated, uploaded, or sent." : "Fake local stamp removed.", "OK")->show();
        showPage(Gags);
    }
    void stars() {
        if (!hasItem("stars")) return FLAlertLayer::create("Locked gag", "Unlock Star Shower in the Shop. It never changes game stats.", "OK")->show();
        m_stars = 9000;
        showPage(Gags);
    }
    void spike() {
        if (!hasItem("spike")) return FLAlertLayer::create("Locked gag", "Unlock Spike Amnesty in the Shop first.", "OK")->show();
        Mod::get()->setSavedValue<bool>(SPIKE, !Mod::get()->getSavedValue<bool>(SPIKE, false));
        showPage(Gags);
    }
    void chaos() {
        if (!hasItem("chaos")) return FLAlertLayer::create("Locked gag", "Unlock Menu Worse-ifier in the Shop first.", "OK")->show();
        Mod::get()->setSavedValue<bool>(CHAOS, !Mod::get()->getSavedValue<bool>(CHAOS, false));
        showPage(Gags);
    }
    void unlockAll() {
        if (!hasItem("dev")) return FLAlertLayer::create("Locked drawer", "Unlock Developer Buttons Drawer in the Shop first.", "OK")->show();
        Ref<BucksPopup> self(this);
        createQuickPopup("LOCAL MOD GAG", "Unlock every shop item for this mod only? Nothing in Geometry Dash itself changes.", "Cancel", "Unlock", [self](auto, bool confirm) {
            if (!confirm) return;
            for (auto const& item : PRODUCTS) grant(item.id);
            self->showPage(Shortcuts);
        });
    }
    void restoreLook() {
        Mod::get()->setSavedValue<bool>(APPROVED, false);
        Mod::get()->setSavedValue<bool>(SPIKE, false);
        Mod::get()->setSavedValue<bool>(CHAOS, false);
        m_stars = 0;
        FLAlertLayer::create("Panel restored", "Local gags are off. Purchased unlocks remain owned.", "OK")->show();
        showPage(Dev);
    }
    void reset() {
        Ref<BucksPopup> self(this);
        createQuickPopup("Reset local economy?", "This clears the wallet and this mod's unlocks. It cannot be undone. Game account, native menus, stats, and levels are unchanged.", "Cancel", "Reset", [self](auto, bool confirm) {
            if (!confirm) return;
            saveBalance(STARTING_BALANCE);
            Mod::get()->setSavedValue<std::vector<std::string>>(OWNED, {});
            Mod::get()->setSavedValue<bool>(APPROVED, false);
            Mod::get()->setSavedValue<bool>(SPIKE, false);
            Mod::get()->setSavedValue<bool>(CHAOS, false);
            self->m_stars = 0;
            self->showPage(Home);
        });
    }
    void speedMock() {
        FLAlertLayer::create("Not supported on this build", "5x speed is a mock button only. Gameplay speed is unchanged.", "OK")->show();
    }
};
}

class $modify(LevelBucksMenu, MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;
        auto size = CCDirector::sharedDirector()->getWinSize();
        auto label = CCLabelBMFont::create("LEVEL BUCKS", "goldFont.fnt");
        if (!label) return true;
        label->setScale(.52f);
        auto item = CCMenuItemLabel::create(label, this, menu_selector(LevelBucksMenu::onOpenBucks));
        auto menu = CCMenu::create();
        menu->addChild(item);
        menu->setPosition({size.width * .5f, size.height * .20f});
        menu->setID("level-bucks-entry");
        this->addChild(menu, 1000);
        return true;
    }
    void onOpenBucks(CCObject*) {
        if (auto popup = BucksPopup::create()) popup->show();
    }
};

$on_mod(Loaded) {
    auto n = Mod::get()->getSavedValue<int64_t>(WALLET, STARTING_BALANCE);
    if (n < 0) n = STARTING_BALANCE;
    Mod::get()->setSavedValue<int64_t>(WALLET, n);
}
