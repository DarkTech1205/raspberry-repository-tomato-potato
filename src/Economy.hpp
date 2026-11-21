#pragma once
#include "Store.hpp"
#include <string>
#include <vector>

namespace gcu {
    // Fictional wallet is stored in cents. No real money is ever involved.
    struct Price {
        int64_t cents = 0, gems = 0, orbs = 0, diamonds = 0, keys = 0, shards = 0;
        std::string str() const;
        bool free() const { return !(cents || gems || orbs || diamonds || keys || shards); }
    };

    struct Eco {
        static constexpr int ENERGY_MAX = 25;
        static constexpr int ENERGY_REGEN_SEC = 120;

        static int64_t cents();
        static std::string money(int64_t cents);
        static void addCents(int64_t c);

        static int64_t cur(char const* name);         // "gems","orbs","diamonds","keys","shards","stars","moons","coins"
        static void addCur(char const* name, int64_t d);

        static int energy();                           // applies regeneration first
        static void addEnergy(int d);
        static int secondsToNextEnergy();

        static bool canAfford(Price const& p);
        static std::string missing(Price const& p);
        static void charge(Price const& p);            // caller must check canAfford

        static bool bought(std::string const& id);
        static void markBought(std::string const& id);

        // Subscriptions: "mod" $100, "editor" $5, "pass" $14.99. A month is a configurable number of real minutes.
        static bool subActive(std::string const& id);
        static int64_t subSecondsLeft(std::string const& id);
        static void subStart(std::string const& id, int64_t priceCents);
        static void subCancel(std::string const& id);
        static std::vector<std::string> tickSubs();    // renews or lapses; returns messages

        static void trackSpend(int64_t cents);         // for the fake monetization dashboard
    };

    // Fixed reward table. GJDifficulty tiers map onto these exactly.
    enum class Tier { Auto, Easy, Normal, Hard, Harder, Insane, EasyDemon, MediumDemon, HardDemon, InsaneDemon, ExtremeDemon };
    int64_t rewardCents(Tier t);
    char const* tierName(Tier t);
    int tierStars(Tier t);
}
