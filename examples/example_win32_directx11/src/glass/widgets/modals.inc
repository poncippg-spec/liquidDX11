static bool g_alert_open = false, g_alert_destructive = false;
static char g_alert_title[64] = "", g_alert_msg[192] = "", g_alert_ok[24] = "OK", g_alert_cancel[24] = "";

void AlertOpen(const char* title, const char* msg, const char* okLabel, const char* cancelLabel, bool destructive) {
    SafeCopy(g_alert_title, title ? title : "", 64);
    SafeCopy(g_alert_msg, msg ? msg : "", 192);
    SafeCopy(g_alert_ok, okLabel ? okLabel : "OK", 24);
    SafeCopy(g_alert_cancel, cancelLabel ? cancelLabel : "", 24);
    g_alert_destructive = destructive; g_alert_open = true; PlaySfx(Sfx::ModalOpen);
}

int DrawAlert() {
    ImVec2 wmin = ImGui::GetWindowPos(), wsz = ImGui::GetWindowSize();
    ImGuiID mid = ImGui::GetID("##alertmodal");
    Spring& sp = g->springs().Get((uint32_t)mid, 25, SpringStyle::Bouncy, 0.0f);
    sp.stiffness = 200.0f; sp.damping = 17.0f;
    const bool live = g_alert_open;
    sp.target = live ? 1.0f : 0.0f; sp.Tick(Dt());
    float a = Clampf(sp.x, 0.0f, 1.0f);
    if (!live && a < 0.01f) return -1;
    ImGui::SetNextWindowPos(wmin); ImGui::SetNextWindowSize(wsz); ImGui::SetNextWindowBgAlpha(0.0f);
    if (live && a < 0.92f) ImGui::SetNextWindowFocus();
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::Begin("##alert_win", nullptr, kOverlayWinFlags);
    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImGui::SetCursorScreenPos(wmin);
    ImGui::InvisibleButton("##aldim", wsz, ImGuiButtonFlags_AllowOverlap);
    bool clickedDim = ImGui::IsItemDeactivated() && ImGui::IsItemHovered();
    const float pw = 304.0f, wrapW = pw - 40.0f;
    ImVec2 mts = ImGui::CalcTextSize(g_alert_msg, nullptr, false, wrapW);
    float ph = 24.0f + 24.0f + 10.0f + mts.y + 18.0f + 44.0f;
    ImVec2 pc(wmin.x + wsz.x*0.5f, wmin.y + wsz.y*0.5f);
    ImVec2 pmin(pc.x - pw*0.5f, pc.y - ph*0.5f);
    float scl = 0.82f + 0.18f * sp.x;
    float spw = pw*scl, sph = ph*scl; ImVec2 cmin(pc.x - spw*0.5f, pc.y - sph*0.5f);
    { Primitive bg; bg.cx = pc.x; bg.cy = pc.y; bg.hw = spw*0.5f; bg.hh = sph*0.5f;
      bg.corner_radius = 28.0f; bg.fade = a; bg.material = Material::Thick; g->SubmitOverlay(bg); }
    ImVec2 amax(cmin.x+spw, cmin.y+sph);
    DrawUIBlurPanel(dl, cmin, amax, 28.0f, a, IM_COL32(26,27,34,120));
    ImGui::SetCursorScreenPos(pmin); ImGui::InvisibleButton("##alpanel", ImVec2(pw, ph), ImGuiButtonFlags_AllowOverlap);
    ImVec2 tts = ImGui::CalcTextSize(g_alert_title);
    dl->AddText(ImVec2(pc.x - tts.x*0.5f, pmin.y + 20.0f), kInk, g_alert_title);
    dl->AddText(nullptr, 0.0f, ImVec2(pmin.x + 20.0f, pmin.y + 52.0f), kInkSoft, g_alert_msg, nullptr, wrapW);
    int result = -1;
    bool hasCancel = (g_alert_cancel[0] != 0);
    float bh = 38.0f, by = pmin.y + ph - bh - 14.0f;
    auto pillBtn = [&](const char* lbl, float bx, float bw, bool accent, ImU32 textc) -> bool {
        ImGui::SetCursorScreenPos(ImVec2(bx, by));
        ImGui::PushID(lbl); ImGui::InvisibleButton("##b", ImVec2(bw, bh)); bool hov = ImGui::IsItemHovered();
        bool clk = ImGui::IsItemDeactivated() && hov; ImGui::PopID();
        ImU32 fill = accent ? SubAlpha(AccentCol(), hov ? 1.0f : 0.9f)
                            : SubAlpha(IM_COL32(255,255,255,255), hov ? 0.20f : 0.11f);
        dl->AddRectFilled(ImVec2(bx, by), ImVec2(bx+bw, by+bh), fill, 11.0f);
        ImVec2 lts = ImGui::CalcTextSize(lbl);
        dl->AddText(ImVec2(bx + (bw-lts.x)*0.5f, by + (bh-lts.y)*0.5f), textc, lbl);
        return clk;
    };
    ImU32 okText = IM_COL32(255,255,255,255);
    bool  okAccent = !g_alert_destructive;
    ImU32 okCol = g_alert_destructive ? IM_COL32(255,99,97,255) : okText;
    if (hasCancel) {
        float gap = 10.0f, bw = (pw - 40.0f - gap) * 0.5f;
        if (pillBtn(g_alert_cancel, pmin.x + 20.0f, bw, false, kInk)) result = 1;
        if (pillBtn(g_alert_ok, pmin.x + 20.0f + bw + gap, bw, okAccent, okCol)) result = 0;
    } else {
        if (pillBtn(g_alert_ok, pmin.x + 20.0f, pw - 40.0f, okAccent, okCol)) result = 0;
    }
    ImGui::End(); ImGui::PopStyleVar();
    if (live && (result >= 0 || ImGui::IsKeyPressed(ImGuiKey_Escape) || clickedDim)) {
        if (result < 0 && clickedDim) result = hasCancel ? 1 : 0;
        g_alert_open = false; PlaySfx(Sfx::ModalClose);
    }
    if (!live) result = -1;
    return result;
}

static bool ContainsCI(const char* hay, const char* needle) {
    if (!needle || !*needle) return true;
    auto lc = [](char c) { return (c >= 'A' && c <= 'Z') ? (char)(c + 32) : c; };
    for (const char* h = hay; *h; ++h) {
        const char* a = h; const char* b = needle;
        while (*a && *b && lc(*a) == lc(*b)) { ++a; ++b; }
        if (!*b) return true;
    }
    return false;
}

static bool g_as_open = false; static char g_as_title[64] = ""; static char g_as_items[12][48];
static int  g_as_count = 0, g_as_dest = -1;
void ActionSheetOpen(const char* title, const char* const* items, int count, int destructiveIdx) {
    SafeCopy(g_as_title, title ? title : "", 64);
    g_as_count = count > 12 ? 12 : count;
    for (int i = 0; i < g_as_count; ++i) SafeCopy(g_as_items[i], items[i], 48);
    g_as_dest = destructiveIdx; g_as_open = true; PlaySfx(Sfx::SheetUp);
}
int DrawActionSheet() {
    ImVec2 wmin = ImGui::GetWindowPos(), wsz = ImGui::GetWindowSize();
    ImGuiID mid = ImGui::GetID("##asheet");
    Spring& sp = g->springs().Get((uint32_t)mid, 26, SpringStyle::Bouncy, 0.0f);
    sp.stiffness = 200.0f; sp.damping = 17.0f;
    const bool live = g_as_open;
    sp.target = live ? 1.0f : 0.0f; sp.Tick(Dt()); float a = Clampf(sp.x, 0.0f, 1.0f);
    if (!live && a < 0.01f) return -1;
    ImGui::SetNextWindowPos(wmin); ImGui::SetNextWindowSize(wsz); ImGui::SetNextWindowBgAlpha(0.0f);
    if (live && a < 0.92f) ImGui::SetNextWindowFocus();
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::Begin("##as_win", nullptr, kOverlayWinFlags);
    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImGui::SetCursorScreenPos(wmin); ImGui::InvisibleButton("##asdim", wsz, ImGuiButtonFlags_AllowOverlap);
    bool dimclk = ImGui::IsItemDeactivated() && ImGui::IsItemHovered();
    bool hasTitle = g_as_title[0] != 0;
    const float sw = std::min(wsz.x - 36.0f, 360.0f), rowh = 50.0f, pad = 12.0f, titleH = hasTitle ? 36.0f : 0.0f;
    float listH = titleH + g_as_count * rowh;
    float cancelH = rowh, sheetH = pad + listH + pad;
    float sx = wmin.x + (wsz.x - sw) * 0.5f;
    float groupBottom = wmin.y + wsz.y - 16.0f;
    float slide = (1.0f - sp.x) * (sheetH + cancelH + 30.0f);
    float cy2 = groupBottom - cancelH + slide;
    float sy = cy2 - 12.0f - sheetH + 0.0f;
    int result = -1;
    auto pill = [&](float yy, float hgt, const char* lbl, bool red, bool bold) -> bool {
        ImGui::PushID(lbl); ImGui::PushID((int)yy);
        ImGui::SetCursorScreenPos(ImVec2(sx, yy)); ImGui::InvisibleButton("##r", ImVec2(sw, hgt), ImGuiButtonFlags_AllowOverlap);
        bool hov = ImGui::IsItemHovered(); bool clk = ImGui::IsItemDeactivated() && hov;
        ImGui::PopID(); ImGui::PopID();
        if (hov) dl->AddRectFilled(ImVec2(sx, yy), ImVec2(sx+sw, yy+hgt), SubAlpha(AccentCol(), 0.85f), 0.0f);
        ImVec2 lts = ImGui::CalcTextSize(lbl);
        dl->AddText(ImVec2(sx + (sw-lts.x)*0.5f, yy + (hgt-lts.y)*0.5f), red ? IM_COL32(255,99,97,255) : (bold ? IM_COL32(255,255,255,255) : kInk), lbl);
        (void)bold; return clk;
    };
    { Primitive bg; bg.cx = sx+sw*0.5f; bg.cy = sy + sheetH*0.5f; bg.hw = sw*0.5f; bg.hh = sheetH*0.5f;
      bg.corner_radius = 18.0f; bg.fade = a; bg.material = Material::Thick; g->SubmitOverlay(bg); }
    DrawUIBlurPanel(dl, ImVec2(sx, sy), ImVec2(sx+sw, sy+sheetH), 18.0f, a, IM_COL32(26,27,34,120));
    if (hasTitle) { ImVec2 tts = ImGui::CalcTextSize(g_as_title);
        dl->AddText(ImVec2(sx + (sw-tts.x)*0.5f, sy + pad + 8.0f), kInkSoft, g_as_title); }
    ImGui::PushClipRect(ImVec2(sx, sy), ImVec2(sx+sw, sy+sheetH), true);
    for (int i = 0; i < g_as_count; ++i) {
        float ry = sy + pad + titleH + i*rowh;
        if (i > 0 || hasTitle) dl->AddLine(ImVec2(sx+8, ry), ImVec2(sx+sw-8, ry), SubAlpha(IM_COL32(255,255,255,40), a), 1.0f);
        if (pill(ry, rowh, g_as_items[i], i == g_as_dest, false)) result = i;
    }
    ImGui::PopClipRect();
    { Primitive bg; bg.cx = sx+sw*0.5f; bg.cy = cy2 + cancelH*0.5f; bg.hw = sw*0.5f; bg.hh = cancelH*0.5f;
      bg.corner_radius = 18.0f; bg.fade = a; bg.material = Material::Thick; g->SubmitOverlay(bg); }
    DrawUIBlurPanel(dl, ImVec2(sx, cy2), ImVec2(sx+sw, cy2+cancelH), 18.0f, a, IM_COL32(40,41,50,130));
    if (pill(cy2, cancelH, "Cancel", false, true)) result = g_as_count;
    ImGui::End(); ImGui::PopStyleVar();
    if (live && (result >= 0 || ImGui::IsKeyPressed(ImGuiKey_Escape) || dimclk)) {
        if (result < 0) result = g_as_count; g_as_open = false; PlaySfx(Sfx::SheetDown);
    }
    if (!live) result = -1;
    return result;
}

static bool g_cm_open = false; static float g_cm_x = 0, g_cm_y = 0; static char g_cm_items[12][48];
static int  g_cm_count = 0, g_cm_dest = -1;
void ContextMenuOpen(float x, float y, const char* const* items, int count, int destructiveIdx) {
    g_cm_x = x; g_cm_y = y; g_cm_count = count > 12 ? 12 : count;
    for (int i = 0; i < g_cm_count; ++i) SafeCopy(g_cm_items[i], items[i], 48);
    g_cm_dest = destructiveIdx; g_cm_open = true; PlaySfx(Sfx::MenuOpen);
}
int DrawContextMenu() {
    ImVec2 wmin = ImGui::GetWindowPos(), wsz = ImGui::GetWindowSize();
    ImGuiID mid = ImGui::GetID("##ctxmenu");
    Spring& sp = g->springs().Get((uint32_t)mid, 27, SpringStyle::Bouncy, 0.0f);
    sp.stiffness = 200.0f; sp.damping = 17.0f;
    const bool live = g_cm_open;
    sp.target = live ? 1.0f : 0.0f; sp.Tick(Dt()); float a = Clampf(sp.x, 0.0f, 1.0f);
    if (!live && a < 0.01f) return -1;
    ImGui::SetNextWindowPos(wmin); ImGui::SetNextWindowSize(wsz); ImGui::SetNextWindowBgAlpha(0.0f);
    if (live) ImGui::SetNextWindowFocus();
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::Begin("##cm_win", nullptr, kOverlayWinFlags);
    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImGui::SetCursorScreenPos(wmin); ImGui::InvisibleButton("##cmdim", wsz, ImGuiButtonFlags_AllowOverlap);
    bool dimclk = ImGui::IsItemDeactivated() && ImGui::IsItemHovered();
    const float mw = 214.0f, rowh = 34.0f, pad = 6.0f;
    float mh = pad*2 + g_cm_count*rowh;
    float mx = g_cm_x, my = g_cm_y;
    if (mx + mw > wmin.x + wsz.x - 8) mx = wmin.x + wsz.x - 8 - mw;
    if (my + mh > wmin.y + wsz.y - 8) my = wmin.y + wsz.y - 8 - mh;
    float pop = 0.70f + 0.30f * sp.x;
    ImVec2 pmn(mx, my), pmx(mx + mw*pop, my + mh*pop);
    { Primitive bg; bg.cx = (pmn.x+pmx.x)*0.5f; bg.cy = (pmn.y+pmx.y)*0.5f; bg.hw = (pmx.x-pmn.x)*0.5f; bg.hh = (pmx.y-pmn.y)*0.5f;
      bg.corner_radius = 26.0f; bg.fade = a; bg.material = Material::Thick; g->SubmitOverlay(bg); }
    DrawUIBlurPanel(dl, pmn, pmx, 26.0f, a, IM_COL32(22,23,29,118));
    int result = -1;
    ImGui::PushClipRect(pmn, pmx, true);
    for (int i = 0; i < g_cm_count; ++i) {
        float ry = my + pad + i*rowh;
        ImGui::PushID(i); ImGui::SetCursorScreenPos(ImVec2(mx+5, ry));
        ImGui::InvisibleButton("##r", ImVec2(mw-10, rowh-2), ImGuiButtonFlags_AllowOverlap);
        bool hov = ImGui::IsItemHovered(); if (ImGui::IsItemDeactivated() && hov) result = i; ImGui::PopID();
        if (hov) dl->AddRectFilled(ImVec2(mx+5, ry), ImVec2(mx+mw-5, ry+rowh-2), SubAlpha(AccentCol(), a), 9.0f);
        dl->AddText(ImVec2(mx+16, ry + (rowh-2-ImGui::GetFontSize())*0.5f),
                    i == g_cm_dest ? IM_COL32(255,99,97,255) : (hov ? IM_COL32(255,255,255,255) : kInk), g_cm_items[i]);
    }
    ImGui::PopClipRect();
    ImGui::End(); ImGui::PopStyleVar();
    if (live && (result >= 0 || ImGui::IsKeyPressed(ImGuiKey_Escape) || dimclk)) { g_cm_open = false; PlaySfx(Sfx::Dismiss); }
    if (!live) result = -1;
    return result;
}

static bool g_cmd_open = false, g_cmd_focus = false; static char g_cmd_q[64] = "";
void CommandPaletteToggle() { g_cmd_open = !g_cmd_open; if (g_cmd_open) { g_cmd_q[0] = 0; g_cmd_focus = true; PlaySfx(Sfx::PaletteOpen); } else PlaySfx(Sfx::Dismiss); }
bool CommandPaletteOpen() { return g_cmd_open; }
int DrawCommandPalette(const char* const* items, int count) {
    ImVec2 wmin = ImGui::GetWindowPos(), wsz = ImGui::GetWindowSize();
    ImGuiID mid = ImGui::GetID("##cmdpal");
    Spring& sp = g->springs().Get((uint32_t)mid, 28, SpringStyle::Bouncy, 0.0f);
    sp.stiffness = 200.0f; sp.damping = 17.0f;
    const bool live = g_cmd_open;
    sp.target = live ? 1.0f : 0.0f; sp.Tick(Dt()); float a = Clampf(sp.x, 0.0f, 1.0f);
    if (!live && a < 0.01f) return -1;
    ImGui::SetNextWindowPos(wmin); ImGui::SetNextWindowSize(wsz); ImGui::SetNextWindowBgAlpha(0.0f);
    if (live) ImGui::SetNextWindowFocus();
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::Begin("##cmd_win", nullptr, kOverlayWinFlags);
    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImGui::SetCursorScreenPos(wmin); ImGui::InvisibleButton("##cmddim", wsz, ImGuiButtonFlags_AllowOverlap);
    bool dimclk = ImGui::IsItemDeactivated() && ImGui::IsItemHovered();
    int matches[8]; int nm = 0;
    for (int i = 0; i < count && nm < 8; ++i) if (ContainsCI(items[i], g_cmd_q)) matches[nm++] = i;
    const float pw = 460.0f, searchH = 50.0f, rowh = 40.0f;
    float listH = nm * rowh;
    float ph = searchH + (nm ? (8.0f + listH) : 0.0f) + 8.0f;
    float px = wmin.x + (wsz.x - pw) * 0.5f, py = wmin.y + wsz.y * 0.17f - (1.0f - sp.x) * 60.0f;
    { Primitive bg; bg.cx = px+pw*0.5f; bg.cy = py+ph*0.5f; bg.hw = pw*0.5f; bg.hh = ph*0.5f;
      bg.corner_radius = 26.0f; bg.fade = a; bg.material = Material::Thick; g->SubmitOverlay(bg); }
    DrawUIBlurPanel(dl, ImVec2(px, py), ImVec2(px+pw, py+ph), 26.0f, a, IM_COL32(22,23,29,112));
    ImGui::SetCursorScreenPos(ImVec2(px + 10.0f, py + 7.0f));
    if (g_cmd_focus) { ImGui::SetKeyboardFocusHere(); g_cmd_focus = false; }
    SearchField(g_cmd_q, (int)sizeof(g_cmd_q), "Search commands", pw - 20.0f);
    int result = -1;
    for (int k = 0; k < nm; ++k) {
        int i = matches[k]; float ry = py + searchH + 8.0f + k*rowh;
        ImGui::PushID(k); ImGui::SetCursorScreenPos(ImVec2(px+8, ry));
        ImGui::InvisibleButton("##r", ImVec2(pw-16, rowh-2), ImGuiButtonFlags_AllowOverlap);
        bool hov = ImGui::IsItemHovered(); if (ImGui::IsItemDeactivated() && hov) result = i; ImGui::PopID();
        if (hov) dl->AddRectFilled(ImVec2(px+8, ry), ImVec2(px+pw-8, ry+rowh-2), SubAlpha(AccentCol(), a), 9.0f);
        DrawIcon(dl, Icon::ChevronR, ImVec2(px+24, ry+(rowh-2)*0.5f), 13.0f, hov?IM_COL32(255,255,255,255):kInkSoft, 2.0f);
        dl->AddText(ImVec2(px+42, ry + (rowh-2-ImGui::GetFontSize())*0.5f), hov?IM_COL32(255,255,255,255):kInk, items[i]);
    }
    ImGui::End(); ImGui::PopStyleVar();
    if (live && (result >= 0 || ImGui::IsKeyPressed(ImGuiKey_Escape) || dimclk)) { g_cmd_open = false; PlaySfx(Sfx::Dismiss); }
    if (!live) result = -1;
    return result;
}
