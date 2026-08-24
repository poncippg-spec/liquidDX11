#pragma once
#include <cmath>
#include <unordered_map>
#include <cstdint>

namespace Glass {

enum class SpringStyle { CriticalFast, Critical, Bouncy, Overdamped };

struct Spring {
    float x = 0.0f, v = 0.0f, target = 0.0f, stiffness = 280.0f, damping = 0.0f, mass = 1.0f;

    static Spring Make(SpringStyle s, float start = 0.0f) {
        Spring sp; sp.x = start; sp.target = start;
        switch (s) {
            case SpringStyle::CriticalFast: sp.stiffness = 450.0f; sp.damping = 2.0f * std::sqrt(sp.stiffness * sp.mass);          break;
            case SpringStyle::Critical:     sp.stiffness = 280.0f; sp.damping = 2.0f * std::sqrt(sp.stiffness * sp.mass);          break;
            case SpringStyle::Bouncy:       sp.stiffness = 320.0f; sp.damping = 0.55f * 2.0f * std::sqrt(sp.stiffness * sp.mass); break;
            case SpringStyle::Overdamped:   sp.stiffness = 220.0f; sp.damping = 1.6f  * 2.0f * std::sqrt(sp.stiffness * sp.mass); break;
        }
        return sp;
    }

    void Tick(float dt) {
        if (damping <= 0.0f) damping = 2.0f * std::sqrt(stiffness * mass);
        const int steps = (dt > 1.0f/120.0f) ? 2 : 1;
        const float h = dt / (float)steps;
        for (int i = 0; i < steps; ++i) {
            const float F = -stiffness * (x - target) - damping * v;
            v += (F / mass) * h; x += v * h;
        }
    }
    void Snap(float to) { x = to; v = 0.0f; target = to; }
};

struct SpringCache {
    std::unordered_map<uint64_t, Spring> map;

    Spring& Get(uint32_t imgui_id, uint32_t slot, SpringStyle style, float initial = 0.0f) {
        const uint64_t key = ((uint64_t)imgui_id << 32) | (uint64_t)slot;
        auto it = map.find(key);
        if (it == map.end()) return map.emplace(key, Spring::Make(style, initial)).first->second;
        return it->second;
    }
};

}
