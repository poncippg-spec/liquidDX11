static bool CfgReadFile(const std::string& path, std::string& out) {
    FILE* f = fopen(path.c_str(), "rb");
    if (!f) return false;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz < 0 || sz > (1 << 20)) { fclose(f); return false; }
    out.resize((size_t)sz);
    size_t rd = sz ? fread(&out[0], 1, (size_t)sz, f) : 0;
    out.resize(rd); fclose(f); return true;
}
static std::string CfgFindExisting() {
    char env[MAX_PATH] = {};
    DWORD en = ::GetEnvironmentVariableA("GLASS_CONFIG", env, MAX_PATH);
    if (en > 0 && en < MAX_PATH && FileExists(env)) return env;
    std::string up = ExeDir();
    for (int d = 0; d <= 6; ++d) {
        std::string cand = up + "\\glass.cfg";
        if (FileExists(cand)) return cand;
        up += "\\..";
    }
    return std::string();
}
static void LoadOrCreateConfig() {
    std::string path = CfgFindExisting();
    if (!path.empty()) {
        std::string text;
        if (CfgReadFile(path, text)) CfgParse(g_cfg, text);
        return;
    }
    std::string out = ExeDir() + "\\glass.cfg";
    FILE* f = fopen(out.c_str(), "wb");
    if (f) { std::string t = CfgDefaultText(); fwrite(t.data(), 1, t.size(), f); fclose(f); }
}
static std::string CfgSavePathStr() {
    std::string p = CfgFindExisting();
    return p.empty() ? (ExeDir() + "\\glass.cfg") : p;
}
static std::string PresetPathStr(int slot) {
    char name[32]; std::snprintf(name, sizeof(name), "\\glass_preset_%d.cfg", slot);
    return ExeDir() + name;
}
static bool LoadCfgFromPath(const std::string& path, GlassConfig& out) {
    std::string text;
    if (!CfgReadFile(path, text)) return false;
    out = GlassConfig{}; CfgParse(out, text); return true;
}
static bool CfgSave(const std::string& path, const GlassConfig& c) {
    char buf[4096];
    std::snprintf(buf, sizeof(buf),
        "# Glass Overlay configuration (saved from the Customize page).\n"
        "# Edit and relaunch; missing/invalid values fall back to defaults and are clamped.\n\n"
        "# ---- Window ----\n"
        "window.width        = %d\n"
        "window.height       = %d\n"
        "window.x            = %d\n"
        "window.y            = %d\n"
        "window.always_on_top = %d\n"
        "window.start_page   = %d\n\n"
        "# ---- Appearance ----\n"
        "appearance          = %d\n"
        "accent_r            = %.3f\n"
        "accent_g            = %.3f\n"
        "accent_b            = %.3f\n"
        "font_scale          = %.3f\n"
        "ui_scale            = %.3f\n"
        "density             = %d\n\n"
        "# ---- Light (rim outline location) ----\n"
        "light.angle         = %.1f\n"
        "light.intensity     = %.3f\n"
        "light.angle2        = %.1f\n"
        "light.intensity2    = %.3f\n\n"
        "# ---- Material ----\n"
        "material.blur       = %.3f\n"
        "material.opacity    = %.3f\n"
        "material.saturation = %.3f\n"
        "material.refraction = %.2f\n"
        "material.chroma     = %.2f\n"
        "material.edge       = %.3f\n"
        "material.shadow     = %.3f\n"
        "material.flow       = %.2f\n\n"
        "# ---- Glass edge ----\n"
        "edge.fresnel_exp    = %.3f\n"
        "edge.bevel_scale    = %.3f\n"
        "edge.lip            = %.3f\n"
        "edge.specular_exp   = %.2f\n"
        "edge.specular_amt   = %.3f\n"
        "edge.front_amt      = %.3f\n"
        "edge.back_amt       = %.3f\n"
        "edge.sat_pop        = %.3f\n"
        "edge.bevel_tilt     = %.3f\n"
        "edge.light_height   = %.3f\n"
        "edge.shadow         = %.3f\n"
        "edge.cursor_size    = %.1f\n"
        "edge.cursor_glow    = %.3f\n"
        "edge.sheen          = %.3f\n"
        "edge.grain          = %.3f\n"
        "edge.ambient_rim    = %.3f\n"
        "edge.front_spread   = %.1f\n"
        "edge.back_spread    = %.1f\n"
        "edge.panel_br_spread = %.1f\n"
        "edge.panel_br_smooth = %.1f\n"
        "edge.panel_rim_angle = %.1f\n"
        "edge.tbl_remove      = %d\n"
        "edge.tbl_fill        = %.3f\n"
        "edge.adapt_on        = %d\n"
        "edge.adapt_strength  = %.3f\n"
        "edge.adapt_pivot     = %.3f\n"
        "edge.adapt_soft      = %.3f\n"
        "edge.smooth_refraction = %d\n"
        "edge.squircle_on     = %d\n"
        "edge.squircle_power  = %.3f\n"
        "edge.lens_amount     = %.3f\n"
        "edge.lens_power      = %.3f\n"
        "edge.lens_a          = %.3f\n"
        "edge.lens_b          = %.3f\n"
        "edge.lens_c          = %.3f\n"
        "edge.lens_d          = %.3f\n"
        "edge.glow_weight     = %.3f\n"
        "edge.glow_phase      = %.3f\n"
        "edge.glow_edge0      = %.3f\n"
        "edge.glow_edge1      = %.3f\n"
        "edge.glow_bias       = %.3f\n"
        "edge.glow_anim       = %.3f\n\n"
        "# ---- Effects ----\n"
        "fx.sound            = %d\n"
        "fx.parallax         = %d\n"
        "fx.cursor           = %d\n"
        "fx.auto_hide        = %d\n"
        "fx.show_fps         = %d\n",
        c.win_w, c.win_h, c.win_x, c.win_y, c.always_on_top, c.start_page,
        c.appearance, c.accent_r, c.accent_g, c.accent_b, c.font_scale, c.ui_scale, c.density,
        c.light_angle, c.light_intensity, c.light_angle2, c.light_intensity2,
        c.mat_blur, c.mat_opacity, c.mat_saturation, c.mat_refraction, c.mat_chroma, c.mat_edge, c.mat_shadow, c.mat_flow,
        c.edge_fresnel_exp, c.edge_bevel_scale, c.edge_lip, c.edge_specular_exp, c.edge_specular_amt, c.edge_front_amt, c.edge_back_amt, c.edge_sat_pop,
        c.edge_bevel_tilt, c.edge_light_height, c.edge_shadow, c.edge_cursor_size, c.edge_cursor_glow, c.edge_sheen, c.edge_grain, c.edge_ambient_rim,
        c.edge_front_spread, c.edge_back_spread, c.edge_panel_br_spread, c.edge_panel_br_smooth, c.edge_panel_rim_angle,
        c.edge_tbl_remove, c.edge_tbl_fill,
        c.edge_adapt_on, c.edge_adapt_strength, c.edge_adapt_pivot, c.edge_adapt_soft,
        c.edge_smooth_refraction,
        c.edge_squircle_on, c.edge_squircle_power, c.edge_lens_amount, c.edge_lens_power,
        c.edge_lens_a, c.edge_lens_b, c.edge_lens_c, c.edge_lens_d,
        c.edge_glow_weight, c.edge_glow_phase, c.edge_glow_edge0, c.edge_glow_edge1, c.edge_glow_bias, c.edge_glow_anim,
        c.fx_sound, c.fx_parallax, c.fx_cursor, c.fx_auto_hide, c.fx_show_fps);
    FILE* f = fopen(path.c_str(), "wb");
    if (!f) return false;
    fwrite(buf, 1, strlen(buf), f); fclose(f); return true;
}
