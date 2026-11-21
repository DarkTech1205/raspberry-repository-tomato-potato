#include "Rewards.hpp"
#include "Ads.hpp"
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/binding/GJGameLevel.hpp>
#include <ctime>

namespace gcu {
    void Rewards::onAdReward() {
        Eco::addEnergy(5);
        Eco::addCents(5);
        toast("Ad reward: +5 energy and $0.05");
    }

    static const char* SEASONS[12] = {"Frozen Frames", "Candy Circuits", "Spring Spikes", "Bloom Boosters", "Beach Blocks", "Solar Surge",
                                      "Hot Hazards", "Thunder Thrusters", "Spooky Spikes", "Neon Harvest", "Holiday Heists", "Final Fees"};
    static std::tm nowTm() { std::time_t t = std::time(nullptr); std::tm out{}; 
#ifdef _WIN32
        localtime_s(&out, &t);
#else
        localtime_r(&t, &out);
#endif
        return out; }
    std::string seasonName() { return SEASONS[nowTm().tm_mon]; }
    std::string seasonKey() { auto t = nowTm(); return std::to_string(t.tm_year + 1900) + "-" + std::to_string(t.tm_mon + 1); }
    int Rewards::dailyIndex() { return (int)(Store::now() / 86400); }

    static void ensureSeason() {
        // monthly reset: progress belongs to one season key
        auto key = Store::fnv(seasonKey()) % 1000000;
        if (Store::get("pass.season", -1) != (int64_t)key) {
            Store::set("pass.season", (int64_t)key); Store::set("pass.xp", 0);
            for (int i = 1; i <= 30; i++) { Store::set("pass.f." + std::to_string(i), 0); Store::set("pass.p." + std::to_string(i), 0); }
        }
    }
    int passXP() { ensureSeason(); return (int)Store::get("pass.xp", 0); }
    int passTier() { return std::min(30, passXP() / 250); }
    void Rewards::grantPassXP(int xp) { ensureSeason(); Store::add("pass.xp", xp); }

    static Tier tierFor(GJGameLevel* l) {
        if (l->m_autoLevel) return Tier::Auto;
        if (l->m_demon.value() > 0) {
            switch (l->m_demonDifficulty) {
                case 3: return Tier::EasyDemon;
                case 4: return Tier::MediumDemon;
                case 5: return Tier::InsaneDemon;
                case 6: return Tier::ExtremeDemon;
                default: return Tier::HardDemon;
            }
        }
        int avg = l->m_ratings > 0 ? (l->m_ratingsSum / l->m_ratings) : -1;   // 10 easy ... 50 insane
        if (avg >= 0) { if (avg >= 50) return Tier::Insane; if (avg >= 40) return Tier::Harder; if (avg >= 30) return Tier::Hard; if (avg >= 20) return Tier::Normal; if (avg >= 10) return Tier::Easy; }
        switch (l->m_difficulty) {
            case GJDifficulty::Easy: return Tier::Easy; case GJDifficulty::Normal: return Tier::Normal;
            case GJDifficulty::Hard: return Tier::Hard; case GJDifficulty::Harder: return Tier::Harder;
            case GJDifficulty::Insane: return Tier::Insane;
            case GJDifficulty::DemonEasy: return Tier::EasyDemon; case GJDifficulty::DemonMedium: return Tier::MediumDemon;
            case GJDifficulty::Demon: return Tier::HardDemon; case GJDifficulty::DemonInsane: return Tier::InsaneDemon;
            case GJDifficulty::DemonExtreme: return Tier::ExtremeDemon;
            default: return Tier::Auto;   // unrated (N/A) pays as Auto
        }
    }

    class $modify(GCUPlayLayer, PlayLayer) {
        struct Fields { bool rewarded = false; bool boosted = false; int attemptsThisSession = 0; };

        bool init(GJGameLevel* level, bool dontCreateObjects) {
            if (!PlayLayer::init(level, dontCreateObjects)) return false;
            // Reverse energy: playing with energy spends 1 and boosts bonus rewards. At zero you still play.
            if (Eco::energy() > 0) { Eco::addEnergy(-1); m_fields->boosted = true; }
            return true;
        }

        void resetLevel() {
            PlayLayer::resetLevel();
            Store::add("stat.attempts", 1);
            Store::add("quest.deaths", 1);
            m_fields->attemptsThisSession++;
            int n = (int)Mod::get()->getSettingValue<int64_t>("ad-every-n-attempts");
            if (n > 0 && m_fields->attemptsThisSession % n == 0 && adsEnabled())
                showAd(AdKind::Forced, fmt::format("Attempt {}: a short break from your progress", m_fields->attemptsThisSession));
        }

        void levelComplete() {
            PlayLayer::levelComplete();
            // Guards: one payout per play session, none in practice/testmode, none for the player's own editor levels.
            if (m_fields->rewarded || m_isPracticeMode || m_isTestMode || !m_level) return;
            if (m_level->m_levelType == GJLevelType::Editor) return;
            m_fields->rewarded = true;

            auto tier = tierFor(m_level);
            std::string lid = fmt::format("lvl.{}.{}", (int)m_level->m_levelID.value(), (int)m_level->m_levelType);
            bool first = Store::get(lid, 0) == 0;
            int64_t cents = rewardCents(tier);                 // exact table value on first completion
            if (!first) cents = std::max<int64_t>(1, cents / 10);   // replays pay 10% so restarting is not a money printer
            std::string note;
            if (Eco::energy() <= 0 && !m_fields->boosted) { cents = std::max<int64_t>(1, cents / 4); note = " (no energy: 25%)"; }
            Store::set(lid, 1);
            Eco::addCents(cents);
            Store::add("stat.completions", 1);
            Store::add("stat.earned", cents);
            Store::add("quest.complete", 1);

            // Energy bonus lands in other currencies so the cash table stays exact
            int orbs = tierStars(tier) * 25 * (m_fields->boosted ? 2 : 1);
            Eco::addCur("orbs", orbs); Eco::addCur("stars", tierStars(tier));
            if (tierStars(tier) >= 10) { Eco::addCur("keys", 1); Eco::addCur("shards", 2); }
            Rewards::grantPassXP(tierStars(tier) * 10);

            cashRain(this, "+" + Eco::money(cents) + note);
            toast(fmt::format("{} complete! {} paid {}{}", m_level->m_levelName, tierName(tier), Eco::money(cents), note));
            if (adsEnabled() && Store::get("secret.noads", 0) == 0)
                showAd(AdKind::Forced, "Level complete. Here is an ad to celebrate");
        }
    };
}
