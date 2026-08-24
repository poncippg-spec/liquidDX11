static std::string CfgTrim(const std::string& s) {
    size_t a = 0, b = s.size();
    while (a < b && std::isspace((unsigned char)s[a])) ++a;
    while (b > a && std::isspace((unsigned char)s[b - 1])) --b;
    return s.substr(a, b - a);
}
static bool CfgParseF(const std::string& s, float& out) {
    try { size_t n = 0; double d = std::stod(s, &n);
          while (n < s.size() && std::isspace((unsigned char)s[n])) ++n;
          if (n != s.size()) return false; out = (float)d; return true; }
    catch (...) { return false; }
}
static bool CfgParseI(const std::string& s, int& out) {
    try { size_t n = 0; long v = std::stol(s, &n, 10);
          while (n < s.size() && std::isspace((unsigned char)s[n])) ++n;
          if (n != s.size()) return false; out = (int)v; return true; }
    catch (...) { return false; }
}
static inline float CfgClampF(float v, float lo, float hi) { return v < lo ? lo : (v > hi ? hi : v); }
static inline int   CfgClampI(int v, int lo, int hi)       { return v < lo ? lo : (v > hi ? hi : v); }
static void CfgApply(GlassConfig& c, const std::string& key, const std::string& val) {
    float f; int i;
    if (key == "window.width")        { if (CfgParseI(val, i)) c.win_w = CfgClampI(i, 200, 16384); return; }
    if (key == "window.height")       { if (CfgParseI(val, i)) c.win_h = CfgClampI(i, 200, 16384); return; }
    if (key == "window.x")            { if (CfgParseI(val, i)) c.win_x = CfgClampI(i, -1, 16384);  return; }
    if (key == "window.y")            { if (CfgParseI(val, i)) c.win_y = CfgClampI(i, -1, 16384);  return; }
    if (key == "window.always_on_top"){ if (CfgParseI(val, i)) c.always_on_top = CfgClampI(i, 0, 1); return; }
    if (key == "window.start_page")   { if (CfgParseI(val, i)) c.start_page = CfgClampI(i, 0, 7);  return; }
    if (key == "appearance")          { if (CfgParseI(val, i)) c.appearance = CfgClampI(i, 0, 1);  return; }
    if (key == "accent_r")            { if (CfgParseF(val, f)) c.accent_r = CfgClampF(f, 0, 1);    return; }
    if (key == "accent_g")            { if (CfgParseF(val, f)) c.accent_g = CfgClampF(f, 0, 1);    return; }
    if (key == "accent_b")            { if (CfgParseF(val, f)) c.accent_b = CfgClampF(f, 0, 1);    return; }
    if (key == "font_scale")          { if (CfgParseF(val, f)) c.font_scale = CfgClampF(f, 0.6f, 2.0f); return; }
    if (key == "ui_scale")            { if (CfgParseF(val, f)) c.ui_scale = CfgClampF(f, 0.5f, 3.0f); return; }
    if (key == "density")             { if (CfgParseI(val, i)) c.density = CfgClampI(i, 0, 2);     return; }
    if (key == "light.angle")         { if (CfgParseF(val, f)) c.light_angle = CfgClampF(f, 0, 360); return; }
    if (key == "light.intensity")     { if (CfgParseF(val, f)) c.light_intensity = CfgClampF(f, 0, 3); return; }
    if (key == "light.angle2")        { if (CfgParseF(val, f)) c.light_angle2 = CfgClampF(f, 0, 360); return; }
    if (key == "light.intensity2")    { if (CfgParseF(val, f)) c.light_intensity2 = CfgClampF(f, 0, 3); return; }
    if (key == "material.blur")       { if (CfgParseF(val, f)) c.mat_blur = CfgClampF(f, 0, 1);    return; }
    if (key == "material.opacity")    { if (CfgParseF(val, f)) c.mat_opacity = CfgClampF(f, 0, 1); return; }
    if (key == "material.saturation") { if (CfgParseF(val, f)) c.mat_saturation = CfgClampF(f, 0, 4); return; }
    if (key == "material.refraction") { if (CfgParseF(val, f)) c.mat_refraction = CfgClampF(f, 0, 120); return; }
    if (key == "material.chroma")     { if (CfgParseF(val, f)) c.mat_chroma = CfgClampF(f, 0, 30);  return; }
    if (key == "material.edge")       { if (CfgParseF(val, f)) c.mat_edge = CfgClampF(f, 0, 3);     return; }
    if (key == "material.shadow")     { if (CfgParseF(val, f)) c.mat_shadow = CfgClampF(f, 0, 1);   return; }
    if (key == "material.flow")       { if (CfgParseF(val, f)) c.mat_flow = CfgClampF(f, 0, 24);    return; }
    if (key == "edge.fresnel_exp")    { if (CfgParseF(val, f)) c.edge_fresnel_exp = CfgClampF(f, 0.5f, 8.0f);  return; }
    if (key == "edge.bevel_scale")    { if (CfgParseF(val, f)) c.edge_bevel_scale = CfgClampF(f, 0.1f, 1.0f);  return; }
    if (key == "edge.lip")            { if (CfgParseF(val, f)) c.edge_lip = CfgClampF(f, 0.0f, 1.0f);          return; }
    if (key == "edge.specular_exp")   { if (CfgParseF(val, f)) c.edge_specular_exp = CfgClampF(f, 4.0f, 200.0f); return; }
    if (key == "edge.specular_amt")   { if (CfgParseF(val, f)) c.edge_specular_amt = CfgClampF(f, 0.0f, 4.0f);  return; }
    if (key == "edge.front_amt")      { if (CfgParseF(val, f)) c.edge_front_amt = CfgClampF(f, 0.0f, 4.0f);     return; }
    if (key == "edge.back_amt")       { if (CfgParseF(val, f)) c.edge_back_amt = CfgClampF(f, 0.0f, 4.0f);      return; }
    if (key == "edge.sat_pop")        { if (CfgParseF(val, f)) c.edge_sat_pop = CfgClampF(f, 0.0f, 4.0f);       return; }
    if (key == "edge.bevel_tilt")     { if (CfgParseF(val, f)) c.edge_bevel_tilt = CfgClampF(f, 0.5f, 6.0f);    return; }
    if (key == "edge.light_height")   { if (CfgParseF(val, f)) c.edge_light_height = CfgClampF(f, 0.2f, 2.5f);  return; }
    if (key == "edge.shadow")         { if (CfgParseF(val, f)) c.edge_shadow = CfgClampF(f, 0.0f, 2.0f);        return; }
    if (key == "edge.cursor_size")    { if (CfgParseF(val, f)) c.edge_cursor_size = CfgClampF(f, 40.0f, 400.0f); return; }
    if (key == "edge.cursor_glow")    { if (CfgParseF(val, f)) c.edge_cursor_glow = CfgClampF(f, 0.0f, 3.0f);   return; }
    if (key == "edge.sheen")          { if (CfgParseF(val, f)) c.edge_sheen = CfgClampF(f, 0.0f, 3.0f);         return; }
    if (key == "edge.grain")          { if (CfgParseF(val, f)) c.edge_grain = CfgClampF(f, 0.0f, 4.0f);         return; }
    if (key == "edge.ambient_rim")    { if (CfgParseF(val, f)) c.edge_ambient_rim = CfgClampF(f, 0.0f, 2.0f);   return; }
    if (key == "edge.front_spread")   { if (CfgParseF(val, f)) c.edge_front_spread = CfgClampF(f, 30.0f, 500.0f); return; }
    if (key == "edge.back_spread")    { if (CfgParseF(val, f)) c.edge_back_spread = CfgClampF(f, 30.0f, 500.0f);  return; }
    if (key == "edge.panel_br_spread"){ if (CfgParseF(val, f)) c.edge_panel_br_spread = CfgClampF(f, 30.0f, 500.0f); return; }
    if (key == "edge.panel_br_smooth"){ if (CfgParseF(val, f)) c.edge_panel_br_smooth = CfgClampF(f, 30.0f, 400.0f); return; }
    if (key == "edge.panel_rim_angle"){ if (CfgParseF(val, f)) c.edge_panel_rim_angle = CfgClampF(f, 0.0f, 360.0f); return; }
    if (key == "edge.tbl_remove")     { if (CfgParseI(val, i)) c.edge_tbl_remove = CfgClampI(i, 0, 1);          return; }
    if (key == "edge.tbl_fill")       { if (CfgParseF(val, f)) c.edge_tbl_fill = CfgClampF(f, 0.0f, 2.0f);      return; }
    if (key == "edge.adapt_on")       { if (CfgParseI(val, i)) c.edge_adapt_on = CfgClampI(i, 0, 1);            return; }
    if (key == "edge.adapt_strength") { if (CfgParseF(val, f)) c.edge_adapt_strength = CfgClampF(f, 0.0f, 1.0f); return; }
    if (key == "edge.adapt_pivot")    { if (CfgParseF(val, f)) c.edge_adapt_pivot = CfgClampF(f, 0.0f, 1.0f);   return; }
    if (key == "edge.adapt_soft")     { if (CfgParseF(val, f)) c.edge_adapt_soft = CfgClampF(f, 0.02f, 0.6f);   return; }
    if (key == "edge.smooth_refraction"){ if (CfgParseI(val, i)) c.edge_smooth_refraction = CfgClampI(i, 0, 1); return; }
    if (key == "edge.squircle_on")    { if (CfgParseI(val, i)) c.edge_squircle_on = CfgClampI(i, 0, 1);          return; }
    if (key == "edge.squircle_power") { if (CfgParseF(val, f)) c.edge_squircle_power = CfgClampF(f, 2.0f, 8.0f);  return; }
    if (key == "edge.lens_amount")    { if (CfgParseF(val, f)) c.edge_lens_amount = CfgClampF(f, 0.0f, 1.0f);     return; }
    if (key == "edge.lens_power")     { if (CfgParseF(val, f)) c.edge_lens_power = CfgClampF(f, -4.0f, 4.0f);     return; }
    if (key == "edge.lens_a")         { if (CfgParseF(val, f)) c.edge_lens_a = CfgClampF(f, 0.0f, 5.0f);          return; }
    if (key == "edge.lens_b")         { if (CfgParseF(val, f)) c.edge_lens_b = CfgClampF(f, 0.0f, 6.0f);          return; }
    if (key == "edge.lens_c")         { if (CfgParseF(val, f)) c.edge_lens_c = CfgClampF(f, 0.0f, 6.0f);          return; }
    if (key == "edge.lens_d")         { if (CfgParseF(val, f)) c.edge_lens_d = CfgClampF(f, 0.0f, 10.0f);         return; }
    if (key == "edge.glow_weight")    { if (CfgParseF(val, f)) c.edge_glow_weight = CfgClampF(f, -1.0f, 1.0f);    return; }
    if (key == "edge.glow_phase")     { if (CfgParseF(val, f)) c.edge_glow_phase = CfgClampF(f, -6.3f, 6.3f);     return; }
    if (key == "edge.glow_edge0")     { if (CfgParseF(val, f)) c.edge_glow_edge0 = CfgClampF(f, -1.0f, 1.0f);     return; }
    if (key == "edge.glow_edge1")     { if (CfgParseF(val, f)) c.edge_glow_edge1 = CfgClampF(f, -1.0f, 1.0f);     return; }
    if (key == "edge.glow_bias")      { if (CfgParseF(val, f)) c.edge_glow_bias = CfgClampF(f, -1.0f, 1.0f);      return; }
    if (key == "edge.glow_anim")      { if (CfgParseF(val, f)) c.edge_glow_anim = CfgClampF(f, 0.0f, 4.0f);       return; }
    if (key == "fx.sound")            { if (CfgParseI(val, i)) c.fx_sound = CfgClampI(i, 0, 1);     return; }
    if (key == "fx.parallax")         { if (CfgParseI(val, i)) c.fx_parallax = CfgClampI(i, 0, 1);  return; }
    if (key == "fx.cursor")           { if (CfgParseI(val, i)) c.fx_cursor = CfgClampI(i, 0, 1);    return; }
    if (key == "fx.auto_hide")        { if (CfgParseI(val, i)) c.fx_auto_hide = CfgClampI(i, 0, 1); return; }
    if (key == "fx.show_fps")         { if (CfgParseI(val, i)) c.fx_show_fps = CfgClampI(i, 0, 1);  return; }
}
static void CfgParse(GlassConfig& c, const std::string& text) {
    size_t i = 0, n = text.size();
    while (i < n) {
        size_t e = text.find('\n', i);
        if (e == std::string::npos) e = n;
        std::string line = text.substr(i, e - i);
        i = e + 1;
        size_t h = line.find_first_of("#;");
        if (h != std::string::npos) line = line.substr(0, h);
        line = CfgTrim(line);
        if (line.empty()) continue;
        size_t eq = line.find('=');
        if (eq == std::string::npos) continue;
        std::string key = CfgTrim(line.substr(0, eq));
        std::string val = CfgTrim(line.substr(eq + 1));
        if (!key.empty() && !val.empty()) CfgApply(c, key, val);
    }
}
static std::string CfgDefaultText() {
    return
"# ============================================================================\n"
"#  Glass Overlay configuration.  Edit and relaunch.  Lines starting with # are\n"
"#  comments.  Any missing or invalid value falls back to the default shown.\n"
"#  Values are clamped to safe ranges, so the app can never be broken from here.\n"
"# ============================================================================\n"
"\n"
"# ---- Window --------------------------------------------------------------\n"
"window.width        = 1000      # logical px (before DPI scaling)\n"
"window.height       = 760\n"
"window.x            = -1        # -1 = center horizontally on the primary screen\n"
"window.y            = -1        # -1 = center vertically\n"
"window.always_on_top = 0        # 0 / 1\n"
"window.start_page   = 0         # 0 General 1 Display 2 Battery 3 Storage 4 Customize, 7 Launchpad\n"
"\n"
"# ---- Appearance ----------------------------------------------------------\n"
"appearance          = 0         # 0 dark, 1 light\n"
"accent_r            = 0.02      # accent colour (0..1)\n"
"accent_g            = 0.48\n"
"accent_b            = 1.0\n"
"font_scale          = 1.0       # text-only scale, 0.6 .. 2.0\n"
"ui_scale            = 1.0       # whole-GUI scale (text + layout), 0.5 .. 3.0\n"
"density             = 1         # 0 compact, 1 default, 2 spacious\n"
"\n"
"# ---- Light (where the bright rim outlines sit) — TWO independent lights --\n"
"light.angle         = 308.6     # light 1 angle: 0=top 90=right 180=bottom 270=left (clockwise)\n"
"light.intensity     = 0.474     # light 1 strength (0..3)\n"
"light.angle2        = 130.4     # light 2 angle\n"
"light.intensity2    = 0.490     # light 2 strength (0 = off; raise to enable the 2nd outline)\n"
"\n"
"# ---- Material (frosted glass body) --------------------------------------\n"
"material.blur       = 1.0       # 0 crisp .. 1 heavy frost\n"
"material.opacity    = 0.306     # tint opacity 0..1\n"
"material.saturation = 1.35      # vibrancy of the backdrop 0..4\n"
"material.refraction = 24.49     # edge lensing strength (px) 0..120\n"
"material.chroma     = 30.0      # chromatic dispersion at the rim (px) 0..30\n"
"material.edge       = 0.485     # edge-highlight intensity 0..3\n"
"material.shadow     = 0.046     # drop-shadow strength 0..1\n"
"material.flow       = 16.0      # animated liquid warp (px) 0..24 (0 = static)\n"
"\n"
"# ---- Glass edge (the rim light) -----------------------------------------\n"
"edge.fresnel_exp    = 2.551     # rim falloff: LOWER = wider/softer lip (0.5..8)\n"
"edge.bevel_scale    = 1.0       # bevel width as fraction of half-size (0.1..1)\n"
"edge.lip            = 0.6       # brightness of the continuous all-around lip (0..1)\n"
"edge.specular_exp   = 62.42     # specular tightness: HIGHER = sharper glint (4..200)\n"
"edge.specular_amt   = 1.071     # specular hotspot intensity (0..4)\n"
"edge.front_amt      = 1.332     # top-left reflection rim brightness (0..4)\n"
"edge.back_amt       = 2.281     # bottom-right refraction-exit rim brightness (0..4)\n"
"edge.sat_pop        = 1.439     # extra colour saturation confined to the rim (0..4)\n"
"edge.bevel_tilt     = 0.5       # bevel normal tilt / rim curvature (0.5..6)\n"
"edge.light_height   = 0.2       # light elevation: lower=grazing rim glint, higher=head-on (0.2..2.5)\n"
"edge.shadow         = 2.0       # depth dip on the dim cross-diagonal rim (0..2)\n"
"edge.cursor_size    = 40        # pointer spotlight radius in px (40..400)\n"
"edge.cursor_glow    = 0.0       # pointer spotlight intensity, brightens glass near the cursor (0=off..3)\n"
"edge.sheen          = 1.056     # global wet-glass top sheen multiplier (0..3)\n"
"edge.grain          = 4.0       # global film-grain multiplier (0..4)\n"
"edge.ambient_rim    = 2.0       # how much the all-around lip picks up border light (0..2)\n"
"edge.front_spread   = 430.5     # top-left rim wrap % (30 tight .. 500 wide; 100 = neutral)\n"
"edge.back_spread    = 341.7     # bottom-right rim wrap % (independent of the top-left)\n"
"edge.panel_br_spread = 437.7    # panel/button rim REACH % (100 = s^2; higher reaches further from the corner)\n"
"edge.panel_br_smooth = 400.0    # panel/button rim softness % (>100 softer, <100 sharper)\n"
"edge.panel_rim_angle = 330.6    # panel/button rim-reach LOCATION deg (0=top 90=right 180=bottom 270=left; 135=bottom-right)\n"
"edge.tbl_remove      = 1        # 1 = lift top-right/bottom-left corners (remove their shadow)\n"
"edge.tbl_fill        = 2.0      # strength of that corner fill (0..2)\n"
"edge.adapt_on        = 0        # 1 = adaptive opacity (brighter backdrop -> more opaque)\n"
"edge.adapt_strength  = 0.35     # max opacity boost on a bright backdrop (0..1)\n"
"edge.adapt_pivot     = 0.60     # backdrop brightness where the boost is half-applied (0..1)\n"
"edge.adapt_soft      = 0.30     # transition softness around the pivot\n"
"edge.smooth_refraction = 0      # 1 = smooth/creamy refraction (softer falloff + extra rim blur)\n"
"edge.squircle_on     = 0        # 1 = squircle corners (else circular)\n"
"edge.squircle_power  = 5.0      # squircle corner exponent (2..8; 2 = circular)\n"
"edge.lens_amount     = 1.0      # whole-surface magnify lens (0 = off .. 1)\n"
"edge.lens_power      = 4.0      # lens strength/sign (>0 magnify, <0 concave/invert)\n"
"edge.lens_a          = 1.505    # lens profile a\n"
"edge.lens_b          = 2.020    # lens profile b\n"
"edge.lens_c          = 2.143    # lens profile c\n"
"edge.lens_d          = 3.214    # lens profile d\n"
"edge.glow_weight     = 0.388    # angular edge-glow amplitude (-1..1; 0 = off)\n"
"edge.glow_phase      = 1.089    # glow lobe angle (radians)\n"
"edge.glow_edge0      = 0.102    # glow rim band start\n"
"edge.glow_edge1      = -0.571   # glow rim band end\n"
"edge.glow_bias       = 0.0      # glow flat brightness add\n"
"edge.glow_anim       = 0.857    # glow phase drift speed (rad/s; >0 sweeps)\n"
"\n"
"# ---- Effects -------------------------------------------------------------\n"
"fx.sound            = 1         # UI sound effects 0/1\n"
"fx.parallax         = 1         # motion/cursor tilt parallax 0/1\n"
"fx.cursor           = 1         # liquid cursor droplet 0/1\n"
"fx.auto_hide        = 0         # auto-hide chrome when idle 0/1\n"
"fx.show_fps         = 0         # FPS overlay 0/1\n";
}
