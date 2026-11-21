#include "Screens.hpp"
#include "Ads.hpp"
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/EditorUI.hpp>

using namespace geode::prelude;

namespace gcu {
    // Main menu: Shop button, status bar, banner ad, hidden RobTop button.
    class $modify(GCUMenu, MenuLayer) {
        struct Fields { CCLabelBMFont* status = nullptr; int taps = 0; int64_t firstTap = 0; };

        bool init() {
            if (!MenuLayer::init()) return false;
            for (auto& m : Eco::tickSubs()) toast(m, false);
            auto win = CCDirector::get()->getWinSize();

            auto menu = CCMenu::create(); menu->setPosition({0, 0}); this->addChild(menu, 50);

            auto shop = CCMenuItemExt::createSpriteExtra(ButtonSprite::create("SHOP", 70, 0, .6f, false, "goldFont.fnt", "GJ_button_01.png", 28.f),
                [](CCMenuItemSpriteExtra*) { openMainShop(); });
            shop->setPosition({46, win.height - 24}); menu->addChild(shop);

            auto subs = CCMenuItemExt::createSpriteExtra(ButtonSprite::create("MOD $100", 80, 0, .55f, false, "goldFont.fnt", "GJ_button_06.png", 28.f),
                [](CCMenuItemSpriteExtra*) { if (Eco::subActive("mod")) openModPanel(); else openSubs(); });
            subs->setPosition({46, win.height - 56}); menu->addChild(subs);

            auto online = CCMenuItemExt::createSpriteExtra(ButtonSprite::create("ONLINE PRICES", 110, 0, .5f, false, "goldFont.fnt", "GJ_button_06.png", 26.f),
                [](CCMenuItemSpriteExtra*) { openOnlineHub(); });
            online->setPosition({62, win.height - 86}); menu->addChild(online);

            m_fields->status = CCLabelBMFont::create("", "bigFont.fnt");
            m_fields->status->setScale(.4f); m_fields->status->setAnchorPoint({1, .5f});
            m_fields->status->setPosition({win.width - 8, win.height - 12});
            this->addChild(m_fields->status, 50);
            if (auto logo = CCSprite::create("gcu-logo.png"_spr)) { logo->setScale(.28f); logo->setPosition({win.width - 70, win.height - 60}); this->addChild(logo, 50); }
            if (auto promo = CCSprite::create("gcu-promo.png"_spr)) { promo->setScale(.22f); promo->setPosition({win.width - 62, 90}); this->addChild(promo, 50); }
            auto gem = artOrFallback("gcu-gem.png"_spr, 16.f, {255, 79, 163}); gem->setPosition({win.width - 232, win.height - 12}); this->addChild(gem, 50);
            auto en = artOrFallback("gcu-energy.png"_spr, 16.f, {255, 225, 77}); en->setPosition({win.width - 212, win.height - 12}); this->addChild(en, 50);
            this->schedule(schedule_selector(GCUMenu::updateStatus), 1.f);
            this->updateStatus(0);

            // hidden button: 7 quick taps on the dim square in the bottom-left corner
            auto hidden = CCMenuItemExt::createSpriteExtra(CCSprite::createWithSpriteFrameName("square02_small.png"),
                [this](CCMenuItemSpriteExtra*) {
                    auto now = Store::now();
                    if (now - m_fields->firstTap > 4) { m_fields->taps = 0; m_fields->firstTap = now; }
                    if (++m_fields->taps >= 7) { m_fields->taps = 0; toast("You found something..."); openRobTop(false); }
                });
            hidden->setOpacity(18); hidden->setScale(.6f); hidden->setPosition({14, 14}); menu->addChild(hidden);

            if (adsEnabled() && Store::get("banner.hide", 0) < Store::now()) {
                auto banner = makeBanner(); banner->setPosition({win.width / 2 - 150, 2}); this->addChild(banner, 60);
            }
            return true;
        }

        void updateStatus(float) {
            if (!m_fields->status) return;
            m_fields->status->setString(fmt::format("{}  {} gems  energy {}/25{}", Eco::money(Eco::cents()), Eco::cur("gems"), Eco::energy(), Eco::subActive("mod") ? "  [MOD]" : "").c_str());
        }

        void onCreator(CCObject* s) { onlineEntrance([this, s]() { MenuLayer::onCreator(s); }); }
    };

    // Editor: building and saving are free. Playtesting needs Editor Premium ($5/month).
    class $modify(GCUEditorUI, EditorUI) {
        void onPlaytest(CCObject* s) {
            if (Eco::subActive("editor")) { EditorUI::onPlaytest(s); return; }
            Price p; p.cents = 500;
            confirmPurchase("Editor Premium (monthly): playtesting", p, [s, this]() {
                Eco::subStart("editor", 500); toast("Editor Premium active"); EditorUI::onPlaytest(s);
            });
        }
    };
}
