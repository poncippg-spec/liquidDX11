int SegmentedControl(const char* id, const char* const* labels, int count, int current) {
    if (count <= 0) return current;
    ImGui::PushID(id);
    const float h = US(36.0f);
    float w = ImGui::GetContentRegionAvail().x * 0.9f;
    ImVec2 cur = ImGui::GetCursorScreenPos();
    ImGui::InvisibleButton("##seg", ImVec2(w, h));
    bool hov = ImGui::IsItemHovered(), act = ImGui::IsItemActive();
    bool released = ImGui::IsItemDeactivated() && hov;
    if (ImGui::IsItemActivated()) PlaySfx(Sfx::SegmentSelect);
    float segw = w / (float)count;
    int result = current;
    int mouseSeg = -1;
    if (hov) {
        int s = (int)((ImGui::GetIO().MousePos.x - cur.x) / segw);
        mouseSeg = s < 0 ? 0 : (s >= count ? count - 1 : s);
    }
    if (released) result = mouseSeg < 0 ? current : mouseSeg;
    ImGuiID gid = ImGui::GetID("##seg");
    Spring& prs = g->springs().Get((uint32_t)gid, 20, SpringStyle::Critical, 1.0f);
    prs.target = act ? 0.97f : 1.0f; prs.Tick(Dt());
    float press = Clampf(prs.x, 0.94f, 1.0f);
    float bgHW = w * 0.5f * press, bgHH = h * 0.5f * press;
    Primitive bg; bg.cx = cur.x + w * 0.5f; bg.cy = cur.y + h * 0.5f;
    bg.hw = bgHW; bg.hh = bgHH; bg.corner_radius = h * 0.5f; bg.fade = 1.0f; bg.material = Material::Thin;
    g->Submit(bg);
    ImGuiID sid = ImGui::GetID("sel");
    Spring& sp = g->springs().Get((uint32_t)sid, 5, SpringStyle::Critical, (float)current);
    sp.target = (float)current; sp.Tick(Dt());
    Spring& tail = g->springs().Get((uint32_t)sid, 6, SpringStyle::Overdamped, (float)current);
    tail.target = sp.x; tail.Tick(Dt());
    const float pad = 3.0f;
    float pcy = cur.y + h * 0.5f, phw = (segw * 0.5f - pad) * press, phh = (h * 0.5f - pad) * press;
    float headX = cur.x + segw * (sp.x + 0.5f);
    float tailX = cur.x + segw * (tail.x + 0.5f);
    if (std::fabs(headX - tailX) > 1.5f) {
        SubmitBlob(headX, pcy, phw, phh, tailX, pcy, phw, phh, phh, 16.0f, 1.0f, Material::Accent);
    } else {
        Primitive pill;
        pill.cx = headX; pill.cy = pcy; pill.hw = phw; pill.hh = phh; pill.corner_radius = phh;
        pill.fade = 1.0f; pill.material = Material::Accent;
        g->Submit(pill);
    }
    ImGuiID hxk = sid ^ 0x2B1Du, hak = sid ^ 0x4C7Fu;
    ImGuiStorage* sst = ImGui::GetStateStorage();
    bool wantHL = hov && mouseSeg >= 0 && mouseSeg != current;
    float ghx = sst->GetFloat(hxk, (float)(mouseSeg < 0 ? current : mouseSeg));
    float thx = (float)(mouseSeg < 0 ? ghx : mouseSeg);
    ghx += (thx - ghx) * Clampf(Dt() * 20.0f, 0.0f, 1.0f); sst->SetFloat(hxk, ghx);
    float gha = sst->GetFloat(hak, 0.0f);
    gha += ((wantHL ? 1.0f : 0.0f) - gha) * Clampf(Dt() * 16.0f, 0.0f, 1.0f); sst->SetFloat(hak, gha);
    if (gha > 0.01f) {
        Primitive hl; hl.cx = cur.x + segw * (ghx + 0.5f); hl.cy = pcy;
        hl.hw = phw; hl.hh = phh; hl.corner_radius = phh; hl.fade = gha * 0.5f; hl.material = Material::Thin;
        g->Submit(hl);
    }
    ImDrawList* dl = ImGui::GetWindowDrawList();
    for (int i = 0; i < count; ++i) {
        ImVec2 ts = ImGui::CalcTextSize(labels[i]);
        float cx = cur.x + segw * (i + 0.5f);
        ImGuiID lcid = sid ^ ((uint32_t)(i + 1) * 0x9E3779B1u);
        ImU32 col = SmoothCol((uint32_t)lcid, 0, (i == current) ? IM_COL32(255,255,255,255) : kInk);
        dl->AddText(ImVec2(cx - ts.x * 0.5f, cur.y + (h - ts.y) * 0.5f), col, labels[i]);
    }
    ImGui::PopID();
    return result;
}
bool Checkbox(const char* label, bool* v) {
    ImGui::PushID(label);
    const float s = US(24.0f);
    ImVec2 cur = ImGui::GetCursorScreenPos();
    ImGui::InvisibleButton("##cb", ImVec2(s, s));
    bool hov = ImGui::IsItemHovered(), act = ImGui::IsItemActive();
    bool clicked = ImGui::IsItemDeactivated() && hov;
    if (ImGui::IsItemActivated()) PlaySfx(*v ? Sfx::CheckOff : Sfx::CheckOn);
    if (clicked) *v = !*v;
    ImGuiID id = ImGui::GetID("k");
    Spring& k = g->springs().Get((uint32_t)id, 2, SpringStyle::Critical, *v ? 1.0f : 0.0f);
    k.target = *v ? 1.0f : 0.0f; k.Tick(Dt());
    float kc = Clampf(k.x, 0.0f, 1.0f);
    Spring& pr = g->springs().Get((uint32_t)id, 25, SpringStyle::Critical, 1.0f);
    pr.target = act ? 0.95f : 1.0f; pr.Tick(Dt());
    float boxSc = Clampf(pr.x, 0.93f, 1.0f);
    Spring& tp = g->springs().Get((uint32_t)id, 26, SpringStyle::CriticalFast, *v ? 1.0f : 0.0f);
    tp.target = *v ? 1.0f : 0.0f; tp.Tick(Dt());
    float tick = Clampf(tp.x, 0.0f, 1.0f);
    float bhw = s * 0.5f * boxSc, bhh = s * 0.5f * boxSc;
    Primitive box; box.cx = cur.x + s * 0.5f; box.cy = cur.y + s * 0.5f;
    box.hw = bhw; box.hh = bhh; box.corner_radius = US(7.0f); box.fade = 1.0f; box.material = Material::Thin;
    g->Submit(box);
    Primitive on = box; on.material = Material::Accent; on.fade = kc; g->Submit(on);
    if (tick > 0.05f) {
        ImDrawList* dl = ImGui::GetWindowDrawList();
        ImVec2 c = ImVec2(cur.x + s * 0.5f, cur.y + s * 0.5f);
        float k2 = (0.55f + 0.45f * tick) * boxSc * g_ui;
        ImVec2 p0 = ImVec2(c.x - 5.5f * k2, c.y + 0.5f * k2);
        ImVec2 p1 = ImVec2(c.x - 1.5f * k2, c.y + 4.5f * k2);
        ImVec2 p2 = ImVec2(c.x + 6.0f * k2, c.y - 4.5f * k2);
        ImU32 col = SmoothCol((uint32_t)id, 28, IM_COL32(255, 255, 255, (int)(255 * tick)));
        dl->AddLine(p0, p1, col, US(2.4f));
        dl->AddLine(p1, p2, col, US(2.4f));
    }
    ImGui::SameLine(0.0f, 10.0f);
    ImGui::AlignTextToFramePadding();
    ImGui::TextUnformatted(label);
    ImGui::PopID();
    return clicked;
}
int RadioGroup(const char* id, const char* const* opts, int count, int current) {
    ImGui::PushID(id);
    int result = current;
    const float r = 11.0f;
    ImDrawList* dl = ImGui::GetWindowDrawList();
    for (int i = 0; i < count; ++i) {
        ImGui::PushID(i);
        float rowh = (ImGui::GetFontSize() + 10.0f);
        if (rowh < r * 2.0f) rowh = r * 2.0f;
        ImVec2 cur = ImGui::GetCursorScreenPos();
        ImGui::InvisibleButton("##rb", ImVec2(ImGui::GetContentRegionAvail().x, rowh));
        if (ImGui::IsItemActivated()) PlaySfx(Sfx::RadioSelect);
        if (ImGui::IsItemDeactivated() && ImGui::IsItemHovered()) result = i;
        float cy = cur.y + rowh * 0.5f;
        Primitive ring; ring.cx = cur.x + r; ring.cy = cy; ring.hw = r; ring.hh = r;
        ring.corner_radius = r; ring.fade = 1.0f; ring.material = (i == current) ? Material::Accent : Material::Thin;
        g->Submit(ring);
        ImGuiID did = ImGui::GetID("dot");
        Spring& dsp = g->springs().Get((uint32_t)did, 6, SpringStyle::Bouncy, (i == current) ? 1.0f : 0.0f);
        dsp.target = (i == current) ? 1.0f : 0.0f; dsp.Tick(Dt());
        float dd = Clampf(dsp.x, 0.0f, 1.0f);
        if (dd > 0.02f) {
            Primitive dot; dot.cx = cur.x + r; dot.cy = cy; dot.hw = r * 0.44f * dd; dot.hh = dot.hw;
            dot.corner_radius = dot.hw; dot.fade = 1.0f; dot.material = Material::Knob;
            g->Submit(dot);
        }
        ImVec2 ts = ImGui::CalcTextSize(opts[i]);
        dl->AddText(ImVec2(cur.x + r * 2.0f + 10.0f, cy - ts.y * 0.5f), kInk, opts[i]);
        ImGui::PopID();
    }
    ImGui::PopID();
    return result;
}
void Stepper(const char* label, int* v, int step, int vmin, int vmax) {
    ImGui::PushID(label);
    ImGui::AlignTextToFramePadding();
    ImGui::TextUnformatted(label);
    ImGui::SameLine(0, 16);
    if (Button("-", false, ImVec2(34, 32))) { *v -= step; if (*v < vmin) *v = vmin; }
    ImGui::SameLine(0, 8);
    char buf[32]; std::snprintf(buf, sizeof(buf), "%d", *v);
    const float bw = 46, bh = 32;
    ImVec2 cur = ImGui::GetCursorScreenPos();
    ImGui::Dummy(ImVec2(bw, bh));
    Primitive box; box.cx = cur.x + bw * 0.5f; box.cy = cur.y + bh * 0.5f;
    box.hw = bw * 0.5f; box.hh = bh * 0.5f; box.corner_radius = 9.0f; box.fade = 1.0f; box.material = Material::Thin;
    g->Submit(box);
    ImVec2 ts = ImGui::CalcTextSize(buf);
    ImGui::GetWindowDrawList()->AddText(ImVec2(cur.x + (bw - ts.x) * 0.5f, cur.y + (bh - ts.y) * 0.5f), kInk, buf);
    ImGui::SameLine(0, 8);
    if (Button("+", false, ImVec2(34, 32))) { *v += step; if (*v > vmax) *v = vmax; }
    ImGui::PopID();
}
void ProgressBar(float frac, ImVec2 size) {
    frac = Clampf(frac, 0.0f, 1.0f);
    if (size.x <= 0) size.x = ImGui::GetContentRegionAvail().x;
    if (size.y <= 0) size.y = 8.0f;
    ImVec2 cur = ImGui::GetCursorScreenPos();
    ImGui::Dummy(size);
    Primitive track; track.cx = cur.x + size.x * 0.5f; track.cy = cur.y + size.y * 0.5f;
    track.hw = size.x * 0.5f; track.hh = size.y * 0.5f; track.corner_radius = size.y * 0.5f;
    track.fade = 1.0f; track.material = Material::Thin;
    g->Submit(track);
    float fw = size.x * frac;
    if (fw > size.y) {
        Primitive fill; fill.cx = cur.x + fw * 0.5f; fill.cy = cur.y + size.y * 0.5f;
        fill.hw = fw * 0.5f; fill.hh = size.y * 0.5f; fill.corner_radius = size.y * 0.5f;
        fill.fade = 1.0f; fill.material = Material::Accent;
        g->Submit(fill);
        float sweep = fmodf((float)ImGui::GetTime() * 0.55f, 1.5f) - 0.25f;
        float sx = cur.x + fw * sweep, bw = size.y * 2.4f;
        ImU32 c0 = IM_COL32(255,255,255,0), cw = IM_COL32(255,255,255,75);
        ImDrawList* dl = ImGui::GetWindowDrawList();
        dl->PushClipRect(ImVec2(cur.x, cur.y), ImVec2(cur.x + fw, cur.y + size.y), true);
        dl->AddRectFilledMultiColor(ImVec2(sx - bw, cur.y), ImVec2(sx, cur.y + size.y), c0, cw, cw, c0);
        dl->AddRectFilledMultiColor(ImVec2(sx, cur.y), ImVec2(sx + bw, cur.y + size.y), cw, c0, c0, cw);
        dl->PopClipRect();
    }
}
bool Chip(const char* label, bool selected) {
    ImGui::PushID(label);
    ImVec2 ts = ImGui::CalcTextSize(label);
    float w = ts.x + US(26.0f), h = US(30.0f);
    ImVec2 cur = ImGui::GetCursorScreenPos();
    ImGui::InvisibleButton("##chip", ImVec2(w, h));
    bool clicked = ImGui::IsItemDeactivated() && ImGui::IsItemHovered();
    if (ImGui::IsItemActivated()) PlaySfx(Sfx::ChipTap);
    ImGuiID id = ImGui::GetID("c");
    Spring& sc = g->springs().Get((uint32_t)id, 1, SpringStyle::Bouncy, 1.0f);
    sc.target = ImGui::IsItemActive() ? 0.94f : (ImGui::IsItemHovered() ? 1.05f : 1.0f); sc.Tick(Dt());
    float gel = 1.0f - sc.x;
    Primitive p; p.cx = cur.x + w * 0.5f; p.cy = cur.y + h * 0.5f;
    p.hw = w * 0.5f * sc.x * (1.0f + gel * 0.7f); p.hh = h * 0.5f * sc.x * (1.0f - gel * 0.5f); p.corner_radius = h * 0.5f; p.fade = 1.0f;
    p.material = selected ? Material::Accent : Material::Thin;
    g->Submit(p);
    ImU32 col = SmoothCol((uint32_t)id, 20, selected ? IM_COL32(255,255,255,255) : kInk);
    ImGui::GetWindowDrawList()->AddText(ImVec2(cur.x + (w - ts.x) * 0.5f, cur.y + (h - ts.y) * 0.5f), col, label);
    ImGui::PopID();
    return clicked;
}
bool IconButton(const char* id, Icon ic, float size) {
    ImGui::PushID(id);
    const float bs = size * g_ui;
    ImVec2 cur = ImGui::GetCursorScreenPos();
    ImGui::InvisibleButton("##ib", ImVec2(bs, bs));
    bool hov = ImGui::IsItemHovered(), act = ImGui::IsItemActive();
    bool clicked = ImGui::IsItemDeactivated() && hov;
    if (ImGui::IsItemActivated()) PlaySfx(Sfx::IconTap);
    ImGuiID iid = ImGui::GetID("##ib");
    Spring& sc = g->springs().Get((uint32_t)iid, 1, SpringStyle::Bouncy, 1.0f);
    sc.target = act ? 0.9f : (hov ? 1.07f : 1.0f); sc.Tick(Dt());
    float gel = 1.0f - sc.x;
    Primitive p; p.cx = cur.x + bs*0.5f; p.cy = cur.y + bs*0.5f;
    p.hw = bs*0.5f*sc.x*(1.0f + gel*0.55f); p.hh = bs*0.5f*sc.x*(1.0f - gel*0.45f);
    p.corner_radius = std::min(p.hw, p.hh); p.fade = 1.0f; p.material = hov ? Material::Accent : Material::Thin;
    g->Submit(p);
    ImU32 icc = SmoothCol((uint32_t)iid, 20, hov ? IM_COL32(255,255,255,255) : kInk);
    DrawIcon(ImGui::GetWindowDrawList(), ic, ImVec2(cur.x + bs*0.5f, cur.y + bs*0.5f), size*0.46f,
             icc, 2.0f);
    ImGui::PopID();
    return clicked;
}
static ImGuiID g_kb_capturing = 0;
bool Keybind(const char* label, int* key) {
    ImGui::PushID(label);
    const float rowh = 40.0f;
    float w = ImGui::GetContentRegionAvail().x;
    float rowW = std::min(w, 320.0f);
    ImVec2 cur = ImGui::GetCursorScreenPos();
    ImGui::InvisibleButton("##kb", ImVec2(rowW, rowh), ImGuiButtonFlags_AllowOverlap);
    ImGuiID id = ImGui::GetID("##kb");
    if (ImGui::IsItemDeactivated() && ImGui::IsItemHovered()) g_kb_capturing = id;
    bool capturing = (g_kb_capturing == id);
    bool changed = false;
    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImGuiStorage* st = ImGui::GetStateStorage();
    ImVec2 lts = ImGui::CalcTextSize(label);
    dl->AddText(ImVec2(cur.x + 2, cur.y + (rowh - lts.y) * 0.5f), kInk, label);
    const char* kn = capturing ? "Press a key" : ImGui::GetKeyName((ImGuiKey)*key);
    if (!kn || !*kn) kn = "None";
    ImVec2 kts = ImGui::CalcTextSize(kn);
    float cwTarget = kts.x + 28.0f; if (cwTarget < 66.0f) cwTarget = 66.0f;
    ImGuiID cwk = id ^ 0x1357u;
    float cw = st->GetFloat(cwk, cwTarget);
    cw += (cwTarget - cw) * Clampf(Dt() * 18.0f, 0.0f, 1.0f); st->SetFloat(cwk, cw);
    Spring& pop = g->springs().Get((uint32_t)id, 16, SpringStyle::Bouncy, 0.0f);
    pop.target = 0.0f; pop.Tick(Dt());
    float breathe = capturing ? (1.0f + 0.05f * sinf((float)ImGui::GetTime() * 7.0f)) : 1.0f;
    float sc2 = breathe * (1.0f + Clampf(pop.x, 0.0f, 1.0f) * 0.26f);
    const float ch2 = 28.0f;
    ImVec2 cc(cur.x + rowW - cw * 0.5f, cur.y + rowh * 0.5f);
    if (capturing) {
        float rp = 0.5f + 0.5f * sinf((float)ImGui::GetTime() * 7.0f);
        dl->AddRect(ImVec2(cc.x - cw*0.5f*sc2 - 3, cc.y - ch2*0.5f*sc2 - 3), ImVec2(cc.x + cw*0.5f*sc2 + 3, cc.y + ch2*0.5f*sc2 + 3),
                    (AccentCol() & 0x00FFFFFF) | ((int)(130 * rp) << 24), 13.0f, 0, 2.0f);
    }
    Primitive cb; cb.cx = cc.x; cb.cy = cc.y; cb.hw = cw * 0.5f * sc2; cb.hh = ch2 * 0.5f * sc2;
    cb.corner_radius = std::min(cb.hw, cb.hh); cb.fade = 1.0f; cb.material = capturing ? Material::Accent : Material::Thin;
    g->Submit(cb);
    dl->AddText(ImVec2(cc.x - kts.x * 0.5f, cc.y - kts.y * 0.5f), capturing ? IM_COL32(255,255,255,255) : kInk, kn);
    if (capturing) {
        const float xr = 9.0f;
        ImVec2 xc(cc.x - cw * 0.5f * sc2 - xr - 7.0f, cc.y);
        ImGui::SetCursorScreenPos(ImVec2(xc.x - xr, xc.y - xr));
        ImGui::InvisibleButton("##kbx", ImVec2(xr * 2.0f, xr * 2.0f));
        bool xhov = ImGui::IsItemHovered();
        if (ImGui::IsItemDeactivated() && xhov) { g_kb_capturing = 0; capturing = false; }
        Primitive xb; xb.cx = xc.x; xb.cy = xc.y; xb.hw = xr; xb.hh = xr;
        xb.corner_radius = xr; xb.fade = 1.0f; xb.material = xhov ? Material::Accent : Material::Thin;
        g->Submit(xb);
        ImU32 xcol = IM_COL32(255, 255, 255, 235);
        dl->AddLine(ImVec2(xc.x - 3.2f, xc.y - 3.2f), ImVec2(xc.x + 3.2f, xc.y + 3.2f), xcol, 2.0f);
        dl->AddLine(ImVec2(xc.x - 3.2f, xc.y + 3.2f), ImVec2(xc.x + 3.2f, xc.y - 3.2f), xcol, 2.0f);
    }
    if (capturing) {
        for (int k = ImGuiKey_NamedKey_BEGIN; k < ImGuiKey_NamedKey_END; ++k) {
            if (k >= ImGuiKey_MouseLeft && k <= ImGuiKey_MouseWheelY) continue;
            if (ImGui::IsKeyPressed((ImGuiKey)k, false)) {
                if ((ImGuiKey)k != ImGuiKey_Escape) { *key = k; changed = true; pop.x = 1.0f; }
                g_kb_capturing = 0; break;
            }
        }
    }
    ImGui::PopID();
    return changed;
}
void StatRow(const char* label, const char* value) {
    const float rowh = 24.0f;
    float w = ImGui::GetContentRegionAvail().x;
    float rowW = std::min(w, 300.0f);
    ImVec2 cur = ImGui::GetCursorScreenPos();
    ImGui::Dummy(ImVec2(rowW, rowh));
    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImVec2 lts = ImGui::CalcTextSize(label);
    dl->AddText(ImVec2(cur.x + 2, cur.y + (rowh - lts.y) * 0.5f), kInkSoft, label);
    ImVec2 vts = ImGui::CalcTextSize(value);
    dl->AddText(ImVec2(cur.x + rowW - vts.x - 2, cur.y + (rowh - vts.y) * 0.5f), kInk, value);
}
void SectionHeader(const char* text) {
    ImGui::PushStyleColor(ImGuiCol_Text, kInkSoft);
    ImGui::TextUnformatted(text);
    ImGui::PopStyleColor();
}
struct PanelInfo { size_t idx; float x, w, y0; };
static std::vector<PanelInfo> g_panel_stack;
static const float kPanelPadX = 13.0f, kPanelPadT = 17.0f, kPanelPadB = 12.0f;
static const float kPanelRowGap = 9.0f, kPanelTitleGap = 11.0f, kPanelGap = 2.0f;
void BeginPanel(const char* title) {
    float w = ImGui::GetContentRegionAvail().x;
    ImVec2 cur = ImGui::GetCursorScreenPos();
    if (title && *title)
        ImGui::GetWindowDrawList()->AddText(ImVec2(cur.x + 2.0f, cur.y), kInkSoft, title);
    ImGui::Dummy(ImVec2(0, ImGui::GetFontSize() + kPanelTitleGap * g_density));
    ImGui::Indent(kPanelPadX);
    ImGui::PushItemWidth(w - 2.0f * kPanelPadX);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(12.0f, kPanelRowGap * g_density));
    float top = ImGui::GetCursorScreenPos().y;
    size_t idx = g ? g->Submit(Primitive{}) : 0;
    g_panel_stack.push_back({ idx, cur.x, w, top });
}
void EndPanel() {
    if (g_panel_stack.empty()) return;
    PanelInfo p = g_panel_stack.back(); g_panel_stack.pop_back();
    float y1 = ImGui::GetCursorScreenPos().y;
    ImGui::PopStyleVar();
    ImGui::PopItemWidth();
    ImGui::Unindent(kPanelPadX);
    float bgTop = p.y0 + kPanelRowGap * g_density - kPanelPadT * g_density;
    float bgBot = y1 + kPanelPadB * g_density;
    if (g && bgBot - bgTop > 1.0f) {
        Primitive& pr = g->At(p.idx);
        pr.cx = p.x + p.w * 0.5f; pr.cy = (bgTop + bgBot) * 0.5f;
        pr.hw = p.w * 0.5f; pr.hh = (bgBot - bgTop) * 0.5f; pr.corner_radius = 15.0f;
        pr.fade = g->SubmitFadeValue(); pr.material = Material::Thin;
    }
    ImGui::SetCursorScreenPos(ImVec2(p.x, bgBot + kPanelGap));
    ImGui::Dummy(ImVec2(0.0f, 0.0f));
}
void Divider() {
    ImVec2 cur = ImGui::GetCursorScreenPos();
    float w = ImGui::GetContentRegionAvail().x;
    ImGui::Dummy(ImVec2(w, 9));
    ImDrawList* dl = ImGui::GetWindowDrawList();
    dl->AddLine(ImVec2(cur.x, cur.y + 4), ImVec2(cur.x + w, cur.y + 4), IM_COL32(0,0,0,26), 1.0f);
    dl->AddLine(ImVec2(cur.x, cur.y + 5), ImVec2(cur.x + w, cur.y + 5), IM_COL32(255,255,255,46), 1.0f);
}
bool Dropdown(const char* label, const char* const* items, int count, int* current) {
    ImGui::PushID(label);
    const float h = 42.0f;
    float w = ImGui::GetContentRegionAvail().x * 0.9f;
    ImGuiID id = ImGui::GetID("dd");
    ImGuiStorage* st = ImGui::GetStateStorage();
    bool open = st->GetBool(id, false);
    ImVec2 cur = ImGui::GetCursorScreenPos();
    ImGui::InvisibleButton("##field", ImVec2(w, h));
    bool hov = ImGui::IsItemHovered(), act = ImGui::IsItemActive();
    if (ImGui::IsItemActivated()) PlaySfx(open ? Sfx::Dismiss : Sfx::MenuOpen);
    if (ImGui::IsItemDeactivated() && hov) { open = !open; st->SetBool(id, open); }
    Spring& sp = g->springs().Get((uint32_t)id, 7, SpringStyle::Critical, open ? 1.0f : 0.0f);
    sp.target = open ? 1.0f : 0.0f; sp.Tick(Dt());
    float ot = Clampf(sp.x, 0.0f, 1.0f);
    Spring& hv = g->springs().Get((uint32_t)id, 20, SpringStyle::Critical, 0.0f);
    hv.target = (hov || act || open) ? 1.0f : 0.0f; hv.Tick(Dt());
    Spring& pr = g->springs().Get((uint32_t)id, 21, SpringStyle::Critical, 1.0f);
    pr.target = act ? 0.98f : 1.0f; pr.Tick(Dt());
    float lift = Clampf(hv.x, 0.0f, 1.0f);
    float fSc = Clampf(pr.x, 0.96f, 1.0f);
    Primitive f; f.cx = cur.x + w * 0.5f; f.cy = cur.y + h * 0.5f;
    f.hw = w * 0.5f * fSc; f.hh = h * 0.5f * fSc; f.corner_radius = 12.0f; f.fade = 1.0f;
    f.material = Material::Thin; f.elevate = 1.0f + 0.15f * lift;
    g->Submit(f);
    ImDrawList* dl = ImGui::GetWindowDrawList();
    const char* curtxt = (*current >= 0 && *current < count) ? items[*current] : "Select";
    ImVec2 ts = ImGui::CalcTextSize(curtxt);
    ImU32 txRest = IM_COL32(214, 216, 226, 255);
    ImU32 txc = SmoothCol((uint32_t)id, 24, (hov || open) ? kInk : txRest);
    dl->AddText(ImVec2(cur.x + 15, cur.y + (h - ts.y) * 0.5f), txc, curtxt);
    ImVec2 cc(cur.x + w - 20 + lift * 2.0f, cur.y + h * 0.5f);
    float ang = ot * kPI, ca = cosf(ang), sa = sinf(ang);
    auto rt = [&](float ox, float oy) { return ImVec2(cc.x + ox * ca - oy * sa, cc.y + ox * sa + oy * ca); };
    ImU32 chc = SmoothCol((uint32_t)id, 28, (hov || open) ? kInk : kInkSoft);
    dl->AddLine(rt(-5, -2), rt(0, 3), chc, 2.2f);
    dl->AddLine(rt( 5, -2), rt(0, 3), chc, 2.2f);
    bool changed = false;
    const float rowh = 40.0f;
    float listH = (count * rowh + 8.0f) * ot;
    if (listH > 1.0f) {
        ImGui::Dummy(ImVec2(0, 5));
        ImVec2 lp = ImGui::GetCursorScreenPos();
        Primitive bg; bg.cx = lp.x + w * 0.5f; bg.cy = lp.y + listH * 0.5f;
        bg.hw = w * 0.5f; bg.hh = listH * 0.5f; bg.corner_radius = 14.0f; bg.fade = ot; bg.material = Material::Thick;
        g->Submit(bg);
        ImGui::PushClipRect(lp, ImVec2(lp.x + w, lp.y + listH), true);
        int hoveredIdx = -1;
        for (int i = 0; i < count; ++i) {
            ImGui::PushID(i);
            ImVec2 rc(lp.x + 5, lp.y + 4 + i * rowh);
            ImGui::SetCursorScreenPos(rc);
            ImGui::InvisibleButton("##it", ImVec2(w - 10, rowh - 2));
            if (ImGui::IsItemHovered()) hoveredIdx = i;
            if (ImGui::IsItemDeactivated() && ImGui::IsItemHovered()) { *current = i; changed = true; open = false; st->SetBool(id, false); }
            ImGui::PopID();
        }
        ImGuiID hyk = id ^ 0x51EDu, hak = id ^ 0x77C1u;
        float gy = st->GetFloat(hyk, (float)(hoveredIdx < 0 ? 0 : hoveredIdx));
        float ty = (float)(hoveredIdx < 0 ? gy : hoveredIdx);
        gy += (ty - gy) * Clampf(Dt() * 20.0f, 0.0f, 1.0f); st->SetFloat(hyk, gy);
        float ha = st->GetFloat(hak, 0.0f);
        ha += ((hoveredIdx >= 0 ? 1.0f : 0.0f) - ha) * Clampf(Dt() * 16.0f, 0.0f, 1.0f); st->SetFloat(hak, ha);
        if (ha > 0.01f) {
            Primitive hl; hl.cx = lp.x + 5 + (w - 10) * 0.5f; hl.cy = lp.y + 4 + (gy + 0.5f) * rowh - 1.0f;
            hl.hw = (w - 10) * 0.5f; hl.hh = (rowh - 2) * 0.5f - 1.0f; hl.corner_radius = 10.0f; hl.fade = ot * ha; hl.material = Material::Accent;
            g->Submit(hl);
        }
        for (int i = 0; i < count; ++i) {
            ImVec2 rc(lp.x + 5, lp.y + 4 + i * rowh);
            float bf = Clampf(ha * (1.0f - Clampf(fabsf(gy - i), 0.0f, 1.0f)), 0.0f, 1.0f);
            ImU32 tc = IM_COL32(236 + (int)(19 * bf), 237 + (int)(18 * bf), 243 + (int)(12 * bf), 255);
            ImVec2 its = ImGui::CalcTextSize(items[i]);
            dl->AddText(ImVec2(rc.x + 12, rc.y + (rowh - 2 - its.y) * 0.5f), tc, items[i]);
            if (i == *current)
                DrawIcon(dl, Icon::Check, ImVec2(rc.x + (w - 10) - 16, rc.y + (rowh - 2) * 0.5f), 15.0f,
                         bf > 0.5f ? IM_COL32(255,255,255,255) : IM_COL32(120,170,255,255), 2.0f);
        }
        ImGui::PopClipRect();
        ImGui::SetCursorScreenPos(ImVec2(lp.x, lp.y + listH));
        ImGui::Dummy(ImVec2(w, 0));
    }
    ImGui::PopID();
    return changed;
}
