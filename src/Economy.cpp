#include "Economy.hpp"
#include <cstdio>
#include <algorithm>

using namespace geode::prelude;

namespace gcu {
    std::string Price::str() const {
        std::string s;
        auto add = [&](std::string const& x) { if (!s.empty()) s += " + "; s += x; };
        if (cents) add(Eco::money(cents));
        if (gems) add(std::to_string(gems) + " gems");
        if (orbs) add(std::to_string(orbs) + " orbs");
        if (diamonds) add(std::to_string(diamonds) + " diamonds");
        if (keys) add(std::to_string(keys) + " keys");
        if (shards) add(std::to_string(shards) + " shards");
        return s.empty() ? "Free" : s;
    }

    int64_t Eco::cents() { return Store::get("cents", 500); }  // $5.00 starter wallet
    std::string Eco::money(int64_t c) {
        char buf[48]; std::snprintf(buf, sizeof buf, "$%lld.%02lld", (long long)(c / 100), (long long)(std::abs(c) % 100));
        return buf;
    }
    void Eco::addCents(int64_t c) { Store::set("cents", std::max<int64_t>(0, cents() + c)); }
    int64_t Eco::cur(char const* n) { return Store::get(std::string("cur.") + n, 0); }
    void Eco::addCur(char const* n, int64_t d) { Store::set(std::string("cur.") + n, std::max<int64_t>(0, cur(n) + d)); }

    int Eco::energy() {
        int e = (int)Store::get("energy", ENERGY_MAX);
        auto last = Store::get("energy.t", Store::now());
        auto now = Store::now();
        if (e < ENERGY_MAX) {
            auto gain = (now - last) / ENERGY_REGEN_SEC;
            if (gain > 0) {
                e = (int)std::min<int64_t>(ENERGY_MAX, e + gain);
                Store::set("energy", e);
                Store::set("energy.t", e >= ENERGY_MAX ? now : last + gain * ENERGY_REGEN_SEC);
            }
        } else if (last != now) Store::set("energy.t", now);
        return e;
    }
    void Eco::addEnergy(int d) {
        int e = energy();
        if (e >= ENERGY_MAX && d < 0) Store::set("energy.t", Store::now());   // regen clock starts when we drop below max
        Store::set("energy", std::clamp(e + d, 0, ENERGY_MAX));
    }
    int Eco::secondsToNextEnergy() {
        if (energy() >= ENERGY_MAX) return 0;
        return (int)std::max<int64_t>(0, ENERGY_REGEN_SEC - (Store::now() - Store::get("energy.t", Store::now())));
    }

    bool Eco::canAfford(Price const& p) {
        return cents() >= p.cents && cur("gems") >= p.gems && cur("orbs") >= p.orbs && cur("diamonds") >= p.diamonds
            && cur("keys") >= p.keys && cur("shards") >= p.shards;
    }
    std::string Eco::missing(Price const& p) {
        std::string s;
        auto chk = [&](char const* label, int64_t have, int64_t need, bool money) {
            if (have >= need) return;
            if (!s.empty()) s += ", ";
            s += money ? Eco::money(need - have) : std::to_string(need - have) + " " + label;
        };
        chk("", cents(), p.cents, true);
        chk("gems", cur("gems"), p.gems, false); chk("orbs", cur("orbs"), p.orbs, false);
        chk("diamonds", cur("diamonds"), p.diamonds, false); chk("keys", cur("keys"), p.keys, false);
        chk("shards", cur("shards"), p.shards, false);
        return s;
    }
    void Eco::charge(Price const& p) {
        addCents(-p.cents); addCur("gems", -p.gems); addCur("orbs", -p.orbs);
        addCur("diamonds", -p.diamonds); addCur("keys", -p.keys); addCur("shards", -p.shards);
        trackSpend(p.cents);
    }
    bool Eco::bought(std::string const& id) { return Store::get("own." + id, 0) != 0; }
    void Eco::markBought(std::string const& id) { Store::set("own." + id, 1); }
    void Eco::trackSpend(int64_t c) { if (c > 0) Store::add("stat.spent", c); }

    static int64_t periodSec() { return (int64_t)Mod::get()->getSettingValue<int64_t>("sub-period-minutes") * 60; }
    bool Eco::subActive(std::string const& id) { return Store::get("sub." + id, 0) > Store::now(); }
    int64_t Eco::subSecondsLeft(std::string const& id) { return std::max<int64_t>(0, Store::get("sub." + id, 0) - Store::now()); }
    void Eco::subStart(std::string const& id, int64_t price) {
        Store::set("sub." + id, Store::now() + periodSec());
        Store::set("subprice." + id, price);
        Store::set("subcancel." + id, 0);
    }
    void Eco::subCancel(std::string const& id) { Store::set("subcancel." + id, 1); }
    std::vector<std::string> Eco::tickSubs() {
        std::vector<std::string> out;
        for (auto id : {"mod", "editor", "pass"}) {
            std::string sid = id;
            auto end = Store::get("sub." + sid, 0);
            if (end == 0 || end > Store::now()) continue;
            auto price = Store::get("subprice." + sid, 0);
            if (Store::get("subcancel." + sid, 0)) { Store::set("sub." + sid, 0); out.push_back(sid + " subscription ended."); continue; }
            Price p; p.cents = price;
            if (canAfford(p)) { charge(p); Store::set("sub." + sid, Store::now() + periodSec()); out.push_back(sid + " renewed for " + money(price) + "."); }
            else { Store::set("sub." + sid, 0); out.push_back(sid + " lapsed: wallet could not cover " + money(price) + "."); }
        }
        return out;
    }

    int64_t rewardCents(Tier t) {
        static const int64_t r[] = {10, 25, 50, 100, 200, 500, 750, 1000, 1500, 2000, 2500};
        return r[(int)t];
    }
    char const* tierName(Tier t) {
        static const char* n[] = {"Auto", "Easy", "Normal", "Hard", "Harder", "Insane", "Easy Demon", "Medium Demon", "Hard Demon", "Insane Demon", "Extreme Demon"};
        return n[(int)t];
    }
    int tierStars(Tier t) { static const int s[] = {1, 2, 3, 4, 6, 8, 10, 10, 10, 10, 10}; return s[(int)t]; }
}
