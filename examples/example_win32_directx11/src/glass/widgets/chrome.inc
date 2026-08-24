static bool g_bn_active = false; static Icon g_bn_ic = Icon::Bell; static ImU32 g_bn_tile = 0;
static char g_bn_app[32] = "", g_bn_title[48] = "", g_bn_msg[96] = ""; static float g_bn_age = 0.0f;
void BannerOpen(Icon ic, ImU32 tile, const char* app, const char* title, const char* msg) {
    g_bn_ic = ic; g_bn_tile = tile; SafeCopy(g_bn_app, app, 32); SafeCopy(g_bn_title, title, 48); SafeCopy(g_bn_msg, msg, 96);
    g_bn_age = 0.0f; g_bn_active = true; PlaySfx(Sfx::Notify);
}
void DrawBanner(float win_w) {
    if (!g_bn_active) return;
    g_bn_age += Dt();
    const float life = 3.8f;
    float t = (g_bn_age < 0.32f) ? g_bn_age/0.32f : (g_bn_age > life-0.4f ? Clampf((life-g_bn_age)/0.4f,0,1) : 1.0f);
    t = t*t*(3.0f-2.0f*t);
    if (g_bn_age > life) { g_bn_active = false; return; }
    float bw = std::min(win_w - 24.0f, 420.0f), bh = 64.0f;
    float slideT;
    if (g_bn_age < 0.34f) { float e = g_bn_age/0.34f; const float c1 = 1.70158f, c3 = c1+1.0f, u = e-1.0f; slideT = 1.0f + c3*u*u*u + c1*u*u; }
    else if (g_bn_age > life-0.4f) slideT = Clampf((life-g_bn_age)/0.4f, 0.0f, 1.0f);
    else slideT = 1.0f;
    float bx = (win_w - bw) * 0.5f, by = -bh - 4.0f + (20.0f + bh) * slideT;
    { Primitive p; p.cx = bx+bw*0.5f; p.cy = by+bh*0.5f; p.hw = bw*0.5f; p.hh = bh*0.5f; p.corner_radius = 19.0f; p.fade = t; p.material = Material::Thick; g->SubmitOverlay(p); }
    ImDrawList* dl = ImGui::GetForegroundDrawList();
    float tsz = 38.0f; ImVec2 tp(bx + 12.0f, by + (bh-tsz)*0.5f);
    dl->AddRectFilled(tp, ImVec2(tp.x+tsz, tp.y+tsz), SubAlpha(g_bn_tile, t), 10.0f);
    DrawIcon(dl, g_bn_ic, ImVec2(tp.x+tsz*0.5f, tp.y+tsz*0.5f), tsz*0.55f, SubAlpha(IM_COL32(255,255,255,255), t), 2.1f);
    dl->AddText(ImVec2(bx + 60.0f, by + 12.0f), SubAlpha(kInk, t), g_bn_title);
    dl->AddText(ImVec2(bx + 60.0f, by + 32.0f), SubAlpha(kInkSoft, t), g_bn_msg);
    ImVec2 as = ImGui::CalcTextSize(g_bn_app);
    dl->AddText(ImVec2(bx + bw - as.x - 14.0f, by + 12.0f), SubAlpha(kInkSoft, t*0.8f), g_bn_app);
}
static bool g_sh_open = false; static char g_sh_title[48] = ""; static Icon g_sh_icons[12]; static char g_sh_labels[12][24]; static int g_sh_count = 0;
void ShareSheetOpen(const char* title, const Icon* icons, const char* const* labels, int count) {
    SafeCopy(g_sh_title, title ? title : "Share", 48);
    g_sh_count = count > 12 ? 12 : count;
    for (int i = 0; i < g_sh_count; ++i) { g_sh_icons[i] = icons[i]; SafeCopy(g_sh_labels[i], labels[i], 24); }
    g_sh_open = true; PlaySfx(Sfx::SheetUp);
}
int DrawShareSheet() {
    if (!g_sh_open) return -1;
    ImVec2 wmin = ImGui::GetWindowPos(), wsz = ImGui::GetWindowSize();
    ImGuiID mid = ImGui::GetID("##sharesheet");
    Spring& sp = g->springs().Get((uint32_t)mid, 32, SpringStyle::Bouncy, 0.0f);
    sp.target = 1.0f; sp.Tick(Dt()); float a = Clampf(sp.x, 0.0f, 1.0f);
    ImGui::SetNextWindowPos(wmin); ImGui::SetNextWindowSize(wsz); ImGui::SetNextWindowBgAlpha(0.0f);
    if (a < 0.92f) ImGui::SetNextWindowFocus();
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::Begin("##sh_win", nullptr, kOverlayWinFlags);
    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImGui::SetCursorScreenPos(wmin); ImGui::InvisibleButton("##shdim", wsz, ImGuiButtonFlags_AllowOverlap);
    bool dimclk = ImGui::IsItemDeactivated() && ImGui::IsItemHovered();
    { Primitive dim; dim.cx = wmin.x+wsz.x*0.5f; dim.cy = wmin.y+wsz.y*0.5f; dim.hw = wsz.x*0.5f; dim.hh = wsz.y*0.5f; dim.corner_radius = 26.0f; dim.fade = a*0.6f; dim.material = Material::Thick; g->SubmitOverlay(dim); }
    const int cols = 4; int rows = (g_sh_count + cols - 1) / cols;
    float sw = std::min(wsz.x - 36.0f, 360.0f), cell = sw / cols, tileSz = 58.0f, gridH = rows * (tileSz + 26.0f);
    float sheetH = 50.0f + gridH + 16.0f;
    float sx = wmin.x + (wsz.x - sw) * 0.5f;
    float slide = (1.0f - sp.x) * (sheetH + 40.0f);
    float sy = wmin.y + wsz.y - sheetH - 18.0f + slide;
    { Primitive bg; bg.cx = sx+sw*0.5f; bg.cy = sy+sheetH*0.5f; bg.hw = sw*0.5f; bg.hh = sheetH*0.5f; bg.corner_radius = 20.0f; bg.fade = a; bg.material = Material::Thick; g->SubmitOverlay(bg); }
    ImVec2 tts = ImGui::CalcTextSize(g_sh_title);
    dl->AddText(ImVec2(sx + (sw-tts.x)*0.5f, sy + 14.0f), kInk, g_sh_title);
    int result = -1;
    for (int i = 0; i < g_sh_count; ++i) {
        int rr = i / cols, cc = i % cols;
        float tx = sx + cc*cell + (cell-tileSz)*0.5f, ty = sy + 44.0f + rr*(tileSz+26.0f);
        ImGui::PushID(i); ImGui::SetCursorScreenPos(ImVec2(tx, ty)); ImGui::InvisibleButton("##t", ImVec2(tileSz, tileSz));
        bool hov = ImGui::IsItemHovered(), act = ImGui::IsItemActive(); if (ImGui::IsItemDeactivated() && hov) result = i;
        ImGuiID tileId = ImGui::GetID("##t"); ImGui::PopID();
        Spring& tsc = g->springs().Get((uint32_t)tileId, 20, SpringStyle::Critical, 1.0f);
        tsc.target = act ? 0.96f : (hov ? 1.05f : 1.0f); tsc.Tick(Dt()); float tscv = Clampf(tsc.x, 0.94f, 1.07f);
        Spring& tel = g->springs().Get((uint32_t)tileId, 21, SpringStyle::Critical, 0.0f);
        tel.target = hov ? 1.0f : 0.0f; tel.Tick(Dt()); float telv = Clampf(tel.x, 0.0f, 1.0f);
        { Primitive p; p.cx = tx+tileSz*0.5f; p.cy = ty+tileSz*0.5f; p.hw = tileSz*0.5f*tscv; p.hh = p.hw; p.corner_radius = 16.0f; p.fade = a; p.material = hov ? Material::Accent : Material::Thin; p.elevate = 1.0f + 0.2f*telv; g->SubmitOverlay(p); }
        ImU32 tic = SmoothCol((uint32_t)tileId, 24, hov ? IM_COL32(255,255,255,255) : kInk);
        DrawIcon(dl, g_sh_icons[i], ImVec2(tx+tileSz*0.5f, ty+tileSz*0.5f), 26.0f*tscv, tic, 2.2f);
        ImVec2 ls = ImGui::CalcTextSize(g_sh_labels[i]);
        dl->AddText(ImVec2(tx+tileSz*0.5f-ls.x*0.5f, ty+tileSz+5.0f), SubAlpha(kInkSoft, a), g_sh_labels[i]);
    }
    ImGui::End(); ImGui::PopStyleVar();
    if (result >= 0 || ImGui::IsKeyPressed(ImGuiKey_Escape) || dimclk) { g_sh_open = false; sp.x = 0.0f; sp.target = 0.0f; PlaySfx(Sfx::SheetDown); }
    return result;
}
struct WInfo { ImVec2 pos, size; };
static std::vector<WInfo> g_widget_stack;
void GlassWidget(const char* id, const char* title, ImVec2 size) {
    ImGui::PushID(id);
    ImVec2 cur = ImGui::GetCursorScreenPos();
    { Primitive p; p.cx = cur.x+size.x*0.5f; p.cy = cur.y+size.y*0.5f; p.hw = size.x*0.5f; p.hh = size.y*0.5f; p.corner_radius = 26.0f; p.fade = 1.0f; p.material = Material::Thin; g->Submit(p); }
    if (title && *title) ImGui::GetWindowDrawList()->AddText(ImVec2(cur.x + 14.0f, cur.y + 12.0f), kInkSoft, title);
    g_widget_stack.push_back({ cur, size });
    ImGui::PushClipRect(cur, ImVec2(cur.x+size.x, cur.y+size.y), true);
    ImGui::SetCursorScreenPos(ImVec2(cur.x + 14.0f, cur.y + (title && *title ? 36.0f : 14.0f)));
}
void EndGlassWidget() {
    if (g_widget_stack.empty()) { ImGui::PopID(); return; }
    WInfo wi = g_widget_stack.back(); g_widget_stack.pop_back();
    ImGui::PopClipRect();
    ImGui::SetCursorScreenPos(wi.pos);
    ImGui::Dummy(wi.size);
    ImGui::PopID();
}
int Onboarding(const char* id, const char* const* titles, const char* const* bodies, const Icon* icons, int count) {
    ImGui::PushID(id);
    ImGuiStorage* st = ImGui::GetStateStorage(); ImGuiID pk = ImGui::GetID("pg");
    int page = st->GetInt(pk, 0); if (page < 0) page = 0;
    ImVec2 wmin = ImGui::GetWindowPos(), wsz = ImGui::GetWindowSize();
    ImGuiID mid = ImGui::GetID("##onb");
    Spring& sp = g->springs().Get((uint32_t)mid, 34, SpringStyle::Bouncy, 0.0f);
    sp.target = 1.0f; sp.Tick(Dt()); float a = Clampf(sp.x, 0.0f, 1.0f);
    ImGui::SetNextWindowPos(wmin); ImGui::SetNextWindowSize(wsz); ImGui::SetNextWindowBgAlpha(0.0f);
    if (a < 0.92f) ImGui::SetNextWindowFocus();
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::Begin("##onb_win", nullptr, kOverlayWinFlags);
    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImGui::SetCursorScreenPos(wmin); ImGui::InvisibleButton("##onbdim", wsz, ImGuiButtonFlags_AllowOverlap);
    g->SetSubmitFade(1.0f);
    { Primitive dim; dim.cx = wmin.x+wsz.x*0.5f; dim.cy = wmin.y+wsz.y*0.5f; dim.hw = wsz.x*0.5f; dim.hh = wsz.y*0.5f; dim.corner_radius = 26.0f; dim.fade = a; dim.material = Material::Thick; g->Submit(dim); }
    ImVec2 c(wmin.x+wsz.x*0.5f, wmin.y+wsz.y*0.5f);
    c.y += (1.0f - sp.x) * 26.0f;
    int p = page < count ? page : count-1;
    { Primitive t; t.cx = c.x; t.cy = c.y - 70.0f; t.hw = 44.0f; t.hh = 44.0f; t.corner_radius = 22.0f; t.fade = a; t.material = Material::Accent; g->Submit(t); }
    DrawIcon(dl, icons[p], ImVec2(c.x, c.y - 70.0f), 44.0f, SubAlpha(IM_COL32(255,255,255,255), a), 2.6f);
    ImVec2 tts = ImGui::CalcTextSize(titles[p]);
    dl->AddText(ImVec2(c.x - tts.x*0.5f, c.y + 0.0f), SubAlpha(kInk, a), titles[p]);
    dl->AddText(nullptr, 0.0f, ImVec2(c.x - 150.0f, c.y + 26.0f), SubAlpha(kInkSoft, a), bodies[p], nullptr, 300.0f);
    int result = page;
    ImGui::SetCursorScreenPos(ImVec2(c.x - 40.0f, c.y + 96.0f));
    int nd = PageDots("onbdots", count, p); if (nd != p) { st->SetInt(pk, nd); }
    float by = c.y + 130.0f;
    ImGui::SetCursorScreenPos(ImVec2(c.x - 80.0f, by));
    if (Button(p >= count-1 ? "Get Started" : "Continue", true, ImVec2(160.0f, 40.0f))) {
        if (p >= count-1) { result = count; }
        else { st->SetInt(pk, p+1); }
    }
    if (p > 0) { ImGui::SetCursorScreenPos(ImVec2(c.x - 80.0f, by + 48.0f));
        if (Button("Back", false, ImVec2(160.0f, 34.0f))) st->SetInt(pk, p-1); }
    ImGui::End(); ImGui::PopStyleVar();
    ImGui::PopID();
    return result;
}
int TrafficLights(const char* id, float x, float y) {
    ImGui::PushID(id);
    const float d = 13.0f, gap = 8.0f;
    ImU32 cols[3]  = { IM_COL32(255,95,86,255), IM_COL32(255,189,46,255), IM_COL32(39,201,63,255) };
    ImU32 glyph[3] = { IM_COL32(94,0,2,255),    IM_COL32(122,74,0,255),   IM_COL32(0,68,20,255)   };
    int result = -1;
    ImDrawList* dl = ImGui::GetWindowDrawList();
    bool anyHov = ImGui::IsMouseHoveringRect(ImVec2(x-4, y-5), ImVec2(x + 3*d + 2*gap + 4, y + d + 5));
    Spring& gsp = g->springs().Get((uint32_t)ImGui::GetID(id), 7, SpringStyle::Bouncy, 0.0f);
    gsp.target = anyHov ? 1.0f : 0.0f; gsp.Tick(Dt());
    float ga = Clampf(gsp.x, 0.0f, 1.0f);
    for (int i = 0; i < 3; ++i) {
        float bx = x + i*(d + gap), cx = bx + d*0.5f, cy = y + d*0.5f;
        ImGui::PushID(i); ImGui::SetCursorScreenPos(ImVec2(bx-1, y-2)); ImGui::InvisibleButton("##tl", ImVec2(d+2, d+4));
        bool hov = ImGui::IsItemHovered(), act = ImGui::IsItemActive();
        if (ImGui::IsItemDeactivated() && hov) result = i;
        ImGuiID tlid = ImGui::GetID("##tl");
        Spring& sp = g->springs().Get((uint32_t)tlid, 20, SpringStyle::Critical, 1.0f);
        sp.target = act ? 0.92f : (hov ? 1.12f : 1.0f); sp.Tick(Dt());
        ImGui::PopID();
        float rr = d*0.5f * Clampf(sp.x, 0.85f, 1.18f);
        auto bright = [](ImU32 c, int add){
            int r = ((c>>IM_COL32_R_SHIFT)&0xFF)+add, gg = ((c>>IM_COL32_G_SHIFT)&0xFF)+add, b = ((c>>IM_COL32_B_SHIFT)&0xFF)+add;
            auto cl=[](int v){return v>255?255:v;};
            return IM_COL32(cl(r), cl(gg), cl(b), (c>>IM_COL32_A_SHIFT)&0xFF);
        };
        ImU32 body = SmoothCol((uint32_t)tlid, 24, (hov||act) ? bright(cols[i], 42) : cols[i]);
        dl->AddCircleFilled(ImVec2(cx, cy), rr, body, 24);
        dl->AddCircleFilled(ImVec2(cx, cy - rr*0.30f), rr*0.50f, IM_COL32(255,255,255,64), 18);
        dl->AddCircle(ImVec2(cx, cy), rr, IM_COL32(0,0,0,40), 24, 1.0f);
        if (ga > 0.02f) {
            ImU32 gc = (glyph[i] & 0x00FFFFFFu) | ((ImU32)(ga * 235.0f) << 24);
            float s = rr*0.36f, th = 1.7f;
            if (i == 0) {
                dl->AddLine(ImVec2(cx-s,cy-s), ImVec2(cx+s,cy+s), gc, th);
                dl->AddLine(ImVec2(cx-s,cy+s), ImVec2(cx+s,cy-s), gc, th);
            } else if (i == 1) {
                dl->AddLine(ImVec2(cx-s,cy), ImVec2(cx+s,cy), gc, th);
            } else {
                dl->AddTriangleFilled(ImVec2(cx-s,cy-s), ImVec2(cx+s*0.18f,cy-s), ImVec2(cx-s,cy+s*0.18f), gc);
                dl->AddTriangleFilled(ImVec2(cx+s,cy+s), ImVec2(cx-s*0.18f,cy+s), ImVec2(cx+s,cy-s*0.18f), gc);
            }
        }
    }
    ImGui::PopID();
    return result;
}
int MenuBar(const char* id, const char* const* menus, int count) {
    ImGui::PushID(id);
    const float h = 28.0f;
    ImVec2 cur = ImGui::GetCursorScreenPos();
    float wfull = ImGui::GetContentRegionAvail().x;
    ImGui::Dummy(ImVec2(wfull, h));
    ImDrawList* dl = ImGui::GetWindowDrawList();
    int result = -1; float x = cur.x + 6.0f;
    for (int i = 0; i < count; ++i) {
        ImVec2 ts = ImGui::CalcTextSize(menus[i]); float bw = ts.x + 22.0f;
        ImGui::PushID(i); ImGui::SetCursorScreenPos(ImVec2(x, cur.y)); ImGui::InvisibleButton("##m", ImVec2(bw, h));
        bool hov = ImGui::IsItemHovered(), act = ImGui::IsItemActive(); if (ImGui::IsItemDeactivated() && hov) result = i;
        ImGuiID mItemId = ImGui::GetID("##m"); ImGui::PopID();
        Spring& hsp = g->springs().Get((uint32_t)mItemId, 20, SpringStyle::Critical, 0.0f);
        hsp.target = hov ? 1.0f : 0.0f; hsp.Tick(Dt()); float hv = Clampf(hsp.x, 0.0f, 1.0f);
        Spring& psp = g->springs().Get((uint32_t)mItemId, 21, SpringStyle::Critical, 1.0f);
        psp.target = act ? 0.96f : 1.0f; psp.Tick(Dt()); float pv = Clampf(psp.x, 0.9f, 1.0f);
        if (hv > 0.01f) {
            float ix = bw*0.5f*(1.0f-pv), iy = (h-6.0f)*0.5f*(1.0f-pv);
            dl->AddRectFilled(ImVec2(x+2+ix, cur.y+3+iy), ImVec2(x+bw-2-ix, cur.y+h-3-iy), SubAlpha(AccentCol(), hv), 7.0f);
        }
        ImU32 mtc = SmoothCol((uint32_t)mItemId, 24, hov ? IM_COL32(255,255,255,255) : kInk);
        dl->AddText(ImVec2(x + 11.0f, cur.y + (h - ts.y)*0.5f), mtc, menus[i]);
        x += bw;
    }
    ImGui::PopID();
    return result;
}
bool CollapsingTitle(const char* text, float scrollY) {
    float t = Clampf(scrollY / 60.0f, 0.0f, 1.0f);
    float fs = 33.0f - 13.0f * t;
    ImVec2 cur = ImGui::GetCursorScreenPos();
    ImGui::Dummy(ImVec2(ImGui::GetContentRegionAvail().x, fs + 8.0f));
    ImGui::GetWindowDrawList()->AddText(ImGui::GetFont(), fs, ImVec2(cur.x + 2.0f, cur.y + (33.0f - fs)*0.5f), kInk, text);
    return t > 0.5f;
}
int SwipeRow(const char* id, const char* label, const char* const* actions, const ImU32* actionCols, int actionCount) {
    ImGui::PushID(id);
    float w = ImGui::GetContentRegionAvail().x; if (w > 360.0f) w = 360.0f;
    const float h = 46.0f, actW = 72.0f;
    float maxOff = actionCount * actW;
    ImVec2 cur = ImGui::GetCursorScreenPos();
    ImGuiStorage* st = ImGui::GetStateStorage(); ImGuiID ok = ImGui::GetID("off");
    float off = st->GetFloat(ok, 0.0f);
    ImGui::InvisibleButton("##swrow", ImVec2(w, h), ImGuiButtonFlags_AllowOverlap);
    bool active = ImGui::IsItemActive();
    if (active) off += ImGui::GetIO().MouseDelta.x;
    if (!active) { float tgt = (off < -maxOff*0.4f) ? -maxOff : 0.0f; off += (tgt - off) * Clampf(Dt()*14.0f, 0.0f, 1.0f); }
    off = Clampf(off, -maxOff, 0.0f); st->SetFloat(ok, off);
    int result = -1;
    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImGui::PushClipRect(cur, ImVec2(cur.x + w, cur.y + h), true);
    for (int i = 0; i < actionCount; ++i) {
        float ax = cur.x + w + off + i*actW;
        ImGui::PushID(1000+i); ImGui::SetCursorScreenPos(ImVec2(ax, cur.y)); ImGui::InvisibleButton("##act", ImVec2(actW, h));
        if (ImGui::IsItemDeactivated() && ImGui::IsItemHovered()) result = i; ImGui::PopID();
        dl->AddRectFilled(ImVec2(ax, cur.y), ImVec2(ax+actW, cur.y+h), actionCols[i], 0.0f);
        ImVec2 as = ImGui::CalcTextSize(actions[i]);
        dl->AddText(ImVec2(ax + (actW-as.x)*0.5f, cur.y + (h-as.y)*0.5f), IM_COL32(255,255,255,255), actions[i]);
    }
    Spring& rel = g->springs().Get((uint32_t)ImGui::GetID("##swrow"), 24, SpringStyle::Critical, 0.0f);
    rel.target = (active || off < -1.0f) ? 1.0f : 0.0f; rel.Tick(Dt()); float relv = Clampf(rel.x, 0.0f, 1.0f);
    { Primitive p; p.cx = cur.x + w*0.5f + off; p.cy = cur.y + h*0.5f; p.hw = w*0.5f; p.hh = h*0.5f - 1.0f; p.corner_radius = 10.0f; p.fade = 1.0f; p.material = Material::Thin; p.elevate = 1.0f + 0.16f*relv; g->Submit(p); }
    ImVec2 ls = ImGui::CalcTextSize(label);
    dl->AddText(ImVec2(cur.x + 16.0f + off, cur.y + (h - ls.y)*0.5f), kInk, label);
    ImGui::PopClipRect();
    if (result >= 0) { off = 0.0f; st->SetFloat(ok, 0.0f); }
    ImGui::PopID();
    return result;
}
