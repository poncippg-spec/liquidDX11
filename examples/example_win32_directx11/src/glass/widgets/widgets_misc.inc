int SidebarNav(const char* id, const Icon* icons, const char* const* labels,
               const ImU32* tints, int count, int current, float w, float itemH) {
    ImGui::PushID(id);
    ImVec2 base = ImGui::GetCursorScreenPos();
    ImGuiID sid = ImGui::GetID("sel");
    Spring& sp = g->springs().Get((uint32_t)sid, 10, SpringStyle::Critical, (float)(current < 0 ? 0 : current));
    if (current >= 0) sp.target = (float)current;
    sp.Tick(Dt());
    Spring& navtail = g->springs().Get((uint32_t)sid, 11, SpringStyle::Overdamped, sp.x);
    navtail.target = sp.x; navtail.Tick(Dt());
    if (current >= 0) {
        float phw = w * 0.5f, phh = itemH * 0.5f - 3.0f;
        float cyH = base.y + (sp.x + 0.5f) * itemH, cyT = base.y + (navtail.x + 0.5f) * itemH;
        if (std::fabs(cyH - cyT) > 1.5f) {
            SubmitBlob(base.x + phw, cyH, phw, phh, base.x + phw, cyT, phw, phh, 12.0f, 16.0f, 1.0f, Material::Thin);
        } else {
            Primitive hl; hl.cx = base.x + phw; hl.cy = cyH;
            hl.hw = phw; hl.hh = phh; hl.corner_radius = 12.0f; hl.fade = 1.0f; hl.material = Material::Thin;
            g->Submit(hl);
        }
    }
    int result = current;
    ImDrawList* dl = ImGui::GetWindowDrawList();
    for (int i = 0; i < count; ++i) {
        ImGui::PushID(i);
        ImVec2 ip(base.x, base.y + i * itemH);
        ImGui::SetCursorScreenPos(ip);
        ImGui::InvisibleButton("##nav", ImVec2(w, itemH));
        bool ih  = ImGui::IsItemHovered();
        bool act = ImGui::IsItemActive();
        if (ImGui::IsItemActivated())            PlaySfx(Sfx::SliderGrab);
        if (ImGui::IsItemDeactivated() && ih) result = i;
        float tsz = US(30.0f); (void)tints;
        uint32_t navId = (uint32_t)ImGui::GetID("##nav");
        Spring& prs = g->springs().Get(navId, 18, SpringStyle::Critical, 1.0f);
        prs.target = act ? 0.92f : 1.0f; prs.Tick(Dt());
        Spring& lif = g->springs().Get(navId, 19, SpringStyle::Critical, 0.0f);
        lif.target = (ih || act) ? 1.0f : 0.0f; lif.Tick(Dt());
        Spring& pop = g->springs().Get(navId, 20, SpringStyle::Critical, 0.0f);
        pop.target = (i == current) ? 1.0f : 0.0f; pop.Tick(Dt());
        float press = Clampf(prs.x, 0.78f, 1.12f);
        float lift  = Clampf(lif.x, 0.0f, 1.0f);
        float popv  = Clampf(pop.x, 0.0f, 1.2f);
        float iScale = press * (1.0f + 0.10f*lift + 0.10f*popv);
        float shift  = lift * US(5.0f);
        ImU32 icc = SmoothCol(navId, 10, (i == current || ih) ? kInk : kInkSoft);
        DrawIcon(dl, icons[i], ImVec2(ip.x + US(10.0f) + tsz * 0.5f + shift, ip.y + itemH * 0.5f), 30.0f * 0.62f * iScale, icc, 2.2f);
        ImU32 lc = SmoothCol(navId, 14, (i == current) ? kInk : kInkSoft);
        ImVec2 lts = ImGui::CalcTextSize(labels[i]);
        dl->AddText(ImVec2(ip.x + tsz + US(22.0f) + shift, ip.y + (itemH - lts.y) * 0.5f), lc, labels[i]);
        if (i == current) {
            float chev = US(12.0f) - lift * US(3.0f);
            DrawIcon(dl, Icon::ChevronR, ImVec2(ip.x + w - chev, ip.y + itemH * 0.5f), 13.0f * (1.0f + 0.08f*popv), IM_COL32(150,150,160,200), 2.0f);
        }
        ImGui::PopID();
    }
    ImGui::SetCursorScreenPos(ImVec2(base.x, base.y + count * itemH));
    ImGui::Dummy(ImVec2(w, 0));
    ImGui::PopID();
    return result;
}
static ImU32 AccentCol() {
    const MaterialParams& m = Params(Material::Accent);
    return IM_COL32((int)(m.tint_rgb[0]*255), (int)(m.tint_rgb[1]*255), (int)(m.tint_rgb[2]*255), 255);
}
void ProgressRing(const char* id, float frac, float radius, const char* center) {
    frac = Clampf(frac, 0.0f, 1.0f);
    ImGui::PushID(id);
    ImVec2 cur = ImGui::GetCursorScreenPos();
    float d = radius * 2.0f;
    ImGui::Dummy(ImVec2(d, d));
    ImVec2 c(cur.x + radius, cur.y + radius);
    ImDrawList* dl = ImGui::GetWindowDrawList();
    float th = radius * 0.16f; if (th < 3.0f) th = 3.0f;
    float rr = radius - th * 0.5f;
    dl->PathArcTo(c, rr, 0.0f, 2.0f * kPI, 56); dl->PathStroke(IM_COL32(255,255,255,32), 0, th);
    ImGuiID aid = ImGui::GetID("f");
    Spring& sp = g->springs().Get((uint32_t)aid, 14, SpringStyle::Critical, frac); sp.target = frac; sp.Tick(Dt());
    float f = Clampf(sp.x, 0.0f, 1.0f);
    float a0 = -kPI * 0.5f, a1 = a0 + 2.0f * kPI * f;
    ImU32 acc = AccentCol();
    if (f > 0.001f) { dl->PathArcTo(c, rr, a0, a1, 56); dl->PathStroke(acc, 0, th);
        dl->AddCircleFilled(ImVec2(c.x + cosf(a0)*rr, c.y + sinf(a0)*rr), th*0.5f, acc);
        dl->AddCircleFilled(ImVec2(c.x + cosf(a1)*rr, c.y + sinf(a1)*rr), th*0.5f, acc); }
    if (center && *center) { ImVec2 ts = ImGui::CalcTextSize(center); dl->AddText(ImVec2(c.x - ts.x*0.5f, c.y - ts.y*0.5f), kInk, center); }
    ImGui::PopID();
}
void Badge(const char* text, ImU32 color) {
    ImVec2 ts = ImGui::CalcTextSize(text);
    float w = ts.x + 14.0f, h = ts.y + 6.0f;
    ImVec2 cur = ImGui::GetCursorScreenPos();
    ImGui::Dummy(ImVec2(w, h));
    ImDrawList* dl = ImGui::GetWindowDrawList();
    dl->AddRectFilled(cur, ImVec2(cur.x + w, cur.y + h), color, h * 0.5f);
    dl->AddText(ImVec2(cur.x + 7.0f, cur.y + 3.0f), IM_COL32(255,255,255,255), text);
}
void Avatar(const char* initials, float size, ImU32 tint) {
    ImVec2 cur = ImGui::GetCursorScreenPos();
    ImGui::Dummy(ImVec2(size, size));
    ImVec2 c(cur.x + size*0.5f, cur.y + size*0.5f);
    ImDrawList* dl = ImGui::GetWindowDrawList();
    dl->AddCircleFilled(c, size*0.5f, tint);
    dl->AddCircle(c, size*0.5f, IM_COL32(255,255,255,55), 0, 1.5f);
    ImVec2 ts = ImGui::CalcTextSize(initials);
    dl->AddText(ImVec2(c.x - ts.x*0.5f, c.y - ts.y*0.5f), IM_COL32(255,255,255,255), initials);
}
int Rating(const char* id, int stars, int maxstars) {
    ImGui::PushID(id);
    const float s = 26.0f;
    int result = stars;
    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImU32 acc = AccentCol();
    for (int i = 0; i < maxstars; ++i) {
        ImGui::PushID(i);
        ImVec2 cur = ImGui::GetCursorScreenPos();
        ImGui::InvisibleButton("##star", ImVec2(s, s));
        bool hov = ImGui::IsItemHovered(), act = ImGui::IsItemActive();
        if (ImGui::IsItemDeactivated() && hov) result = i + 1;
        bool filled = (i < stars);
        uint32_t starId = (uint32_t)ImGui::GetID("##star");
        Spring& sp = g->springs().Get(starId, 1, SpringStyle::Bouncy, 1.0f);
        sp.target = act ? 0.84f : (hov ? 1.22f : (filled ? 1.06f : 1.0f)); sp.Tick(Dt());
        float sc = Clampf(sp.x, 0.7f, 1.4f);
        ImU32 starc = SmoothCol(starId, 20, filled ? acc : IM_COL32(255,255,255,45));
        DrawIcon(dl, Icon::Star, ImVec2(cur.x + s*0.5f, cur.y + s*0.5f), s*0.72f*sc, starc, 2.0f);
        if (i < maxstars - 1) ImGui::SameLine(0.0f, 4.0f);
        ImGui::PopID();
    }
    ImGui::PopID();
    return result;
}
bool VSlider(const char* id, float* v, float vmin, float vmax, ImVec2 size) {
    ImGui::PushID(id);
    ImVec2 cur = ImGui::GetCursorScreenPos();
    ImGui::InvisibleButton("##vs", size);
    bool active = ImGui::IsItemActive();
    bool hov = ImGui::IsItemHovered();
    uint32_t vsId = (uint32_t)ImGui::GetID("##vs");
    float t = (*v - vmin) / (vmax - vmin);
    if (active) { float my = ImGui::GetIO().MousePos.y; t = Clampf(1.0f - (my - cur.y) / size.y, 0.0f, 1.0f); *v = vmin + t * (vmax - vmin); }
    float cx = cur.x + size.x * 0.5f;
    Primitive tr; tr.cx = cx; tr.cy = cur.y + size.y*0.5f; tr.hw = 4.0f; tr.hh = size.y*0.5f; tr.corner_radius = 4.0f; tr.fade = 1.0f; tr.material = Material::Thin; g->Submit(tr);
    float ky = cur.y + (1.0f - t) * size.y;
    if (cur.y + size.y - ky > 1.0f) { Primitive fl; fl.cx = cx; fl.cy = (ky + cur.y + size.y)*0.5f; fl.hw = 4.0f; fl.hh = (cur.y + size.y - ky)*0.5f; fl.corner_radius = 4.0f; fl.fade = 1.0f; fl.material = Material::Accent; g->Submit(fl); }
    Spring& krs = g->springs().Get(vsId, 20, SpringStyle::Critical, 10.0f);
    krs.target = active ? 12.0f : (hov ? 11.0f : 10.0f); krs.Tick(Dt());
    float kr = Clampf(krs.x, 9.0f, 13.0f);
    Primitive kn; kn.cx = cx; kn.cy = ky; kn.hw = kr; kn.hh = kr; kn.corner_radius = kr; kn.fade = 1.0f; kn.material = Material::Knob; kn.elevate = 1.0f + 0.18f*Clampf((krs.x-10.0f)*0.5f, 0.0f, 1.0f); g->Submit(kn);
    ImGui::PopID();
    return active;
}
int SegmentedIcons(const char* id, const Icon* icons, int count, int current) {
    if (count <= 0) return current;
    ImGui::PushID(id);
    const float h = 38.0f;
    float w = ImGui::GetContentRegionAvail().x;
    ImVec2 cur = ImGui::GetCursorScreenPos();
    ImGui::InvisibleButton("##segi", ImVec2(w, h));
    bool segHov = ImGui::IsItemHovered(), segAct = ImGui::IsItemActive();
    bool released = ImGui::IsItemDeactivated() && segHov;
    float segw = w / (float)count;
    int result = current;
    if (released) { int s = (int)((ImGui::GetIO().MousePos.x - cur.x) / segw); result = s < 0 ? 0 : (s >= count ? count-1 : s); }
    int hovSeg = -1;
    if (segHov || segAct) { int s = (int)((ImGui::GetIO().MousePos.x - cur.x) / segw); hovSeg = s < 0 ? 0 : (s >= count ? count-1 : s); }
    Primitive bg; bg.cx = cur.x + w*0.5f; bg.cy = cur.y + h*0.5f; bg.hw = w*0.5f; bg.hh = h*0.5f; bg.corner_radius = h*0.5f; bg.fade = 1.0f; bg.material = Material::Thin; g->Submit(bg);
    ImGuiID sid = ImGui::GetID("sel");
    Spring& sp = g->springs().Get((uint32_t)sid, 5, SpringStyle::Critical, (float)current); sp.target = (float)current; sp.Tick(Dt());
    Spring& tail = g->springs().Get((uint32_t)sid, 6, SpringStyle::Overdamped, (float)current); tail.target = sp.x; tail.Tick(Dt());
    const float pad = 3.0f;
    float pcy = cur.y + h*0.5f, phw = segw*0.5f - pad, phh = h*0.5f - pad;
    float hX = cur.x + segw*(sp.x+0.5f), tX = cur.x + segw*(tail.x+0.5f);
    if (std::fabs(hX - tX) > 1.5f) SubmitBlob(hX, pcy, phw, phh, tX, pcy, phw, phh, phh, 16.0f, 1.0f, Material::Accent);
    else { Primitive pill; pill.cx = hX; pill.cy = pcy; pill.hw = phw; pill.hh = phh; pill.corner_radius = phh; pill.fade = 1.0f; pill.material = Material::Accent; g->Submit(pill); }
    ImDrawList* dl = ImGui::GetWindowDrawList();
    for (int i = 0; i < count; ++i) {
        uint32_t segId = sid ^ ((uint32_t)(i + 1) * 0x9E3779B1u);
        Spring& slif = g->springs().Get(segId, 24, SpringStyle::Critical, 0.0f);
        slif.target = (i == hovSeg && !segAct) ? 1.0f : 0.0f; slif.Tick(Dt());
        Spring& sprs = g->springs().Get(segId, 25, SpringStyle::Critical, 1.0f);
        sprs.target = (i == hovSeg && segAct) ? 0.92f : 1.0f; sprs.Tick(Dt());
        float isc = Clampf(sprs.x, 0.85f, 1.1f) * (1.0f + 0.08f*Clampf(slif.x, 0.0f, 1.0f));
        ImU32 sic = SmoothCol(segId, 28, (i==current) ? IM_COL32(255,255,255,255) : kInk);
        DrawIcon(dl, icons[i], ImVec2(cur.x + segw*(i+0.5f), cur.y + h*0.5f), 18.0f*isc, sic, 2.0f);
    }
    ImGui::PopID();
    return result;
}
void Tooltip(const char* text) {
    if (!ImGui::IsItemHovered()) return;
    ImVec2 mp = ImGui::GetIO().MousePos;
    ImVec2 ts = ImGui::CalcTextSize(text);
    float w = ts.x + 20.0f, h = ts.y + 12.0f;
    ImVec2 p(mp.x + 16.0f, mp.y + 10.0f);
    Primitive bg; bg.cx = p.x + w*0.5f; bg.cy = p.y + h*0.5f; bg.hw = w*0.5f; bg.hh = h*0.5f; bg.corner_radius = 9.0f; bg.fade = 1.0f; bg.material = Material::Thick; g->SubmitOverlay(bg);
    ImGui::GetForegroundDrawList()->AddText(ImVec2(p.x + 10.0f, p.y + 6.0f), kInk, text);
}
void SeparatorText(const char* text) {
    float w = ImGui::GetContentRegionAvail().x;
    ImVec2 cur = ImGui::GetCursorScreenPos();
    ImGui::Dummy(ImVec2(w, 22.0f));
    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImVec2 ts = ImGui::CalcTextSize(text);
    float cy = cur.y + 11.0f;
    float side = (w - ts.x - 20.0f) * 0.5f; if (side < 0) side = 0;
    dl->AddLine(ImVec2(cur.x, cy), ImVec2(cur.x + side, cy), IM_COL32(255,255,255,34), 1.0f);
    dl->AddText(ImVec2(cur.x + side + 10.0f, cy - ts.y*0.5f), kInkSoft, text);
    dl->AddLine(ImVec2(cur.x + w - side, cy), ImVec2(cur.x + w, cy), IM_COL32(255,255,255,34), 1.0f);
}
bool NumberDrag(const char* label, float* v, float speed, float vmin, float vmax, const char* fmt) {
    ImGui::PushID(label);
    const float h = 40.0f;
    float w = ImGui::GetContentRegionAvail().x;
    ImVec2 cur = ImGui::GetCursorScreenPos();
    ImGui::InvisibleButton("##nd", ImVec2(w, h));
    bool active = ImGui::IsItemActive(), hov = ImGui::IsItemHovered();
    uint32_t ndId = (uint32_t)ImGui::GetID("##nd");
    bool changed = false;
    if (active) { float dx = ImGui::GetIO().MouseDelta.x; if (dx != 0.0f) { *v = Clampf(*v + dx * speed, vmin, vmax); changed = true; } }
    Spring& lif = g->springs().Get(ndId, 20, SpringStyle::Critical, 0.0f);
    lif.target = (hov || active) ? 1.0f : 0.0f; lif.Tick(Dt());
    Spring& prs = g->springs().Get(ndId, 21, SpringStyle::Critical, 1.0f);
    prs.target = active ? 0.985f : 1.0f; prs.Tick(Dt());
    float liftv = Clampf(lif.x, 0.0f, 1.0f), pressv = Clampf(prs.x, 0.95f, 1.02f);
    float drawScale = pressv * (1.0f + 0.01f*liftv);
    Primitive f; f.cx = cur.x + w*0.5f; f.cy = cur.y + h*0.5f; f.hw = w*0.5f*drawScale; f.hh = h*0.5f*drawScale; f.corner_radius = 11.0f; f.fade = 1.0f; f.material = Material::Thin; f.elevate = 1.0f + 0.15f*liftv; g->Submit(f);
    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImVec2 lts = ImGui::CalcTextSize(label);
    ImU32 lc = SmoothCol(ndId, 24, (hov||active) ? kInk : kInkSoft);
    dl->AddText(ImVec2(cur.x + 14.0f, cur.y + (h - lts.y)*0.5f), lc, label);
    char buf[40]; std::snprintf(buf, sizeof(buf), fmt, *v);
    ImVec2 vts = ImGui::CalcTextSize(buf);
    ImU32 vc = SmoothCol(ndId, 28, active ? IM_COL32(255,255,255,255) : AccentCol());
    dl->AddText(ImVec2(cur.x + w - vts.x - 14.0f, cur.y + (h - vts.y)*0.5f), vc, buf);
    ImGui::PopID();
    return changed;
}
void StatusPill(const char* text, ImU32 color) {
    ImVec2 ts = ImGui::CalcTextSize(text);
    float w = ts.x + 34.0f, h = ts.y + 12.0f;
    ImVec2 cur = ImGui::GetCursorScreenPos();
    ImGui::InvisibleButton("##sp", ImVec2(w, h));
    Primitive bg; bg.cx = cur.x + w*0.5f; bg.cy = cur.y + h*0.5f; bg.hw = w*0.5f; bg.hh = h*0.5f; bg.corner_radius = h*0.5f; bg.fade = 1.0f; bg.material = Material::Thin; g->Submit(bg);
    ImDrawList* dl = ImGui::GetWindowDrawList();
    dl->AddCircleFilled(ImVec2(cur.x + 16.0f, cur.y + h*0.5f), 4.0f, color);
    dl->AddText(ImVec2(cur.x + 26.0f, cur.y + (h - ts.y)*0.5f), kInk, text);
}
static const ImGuiWindowFlags kOverlayWinFlags =
    ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
    ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoBackground |
    ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse;
static inline ImU32 SubAlpha(ImU32 c, float a) {
    a = Clampf(a, 0.0f, 1.0f);
    ImU32 al = (ImU32)(((c >> 24) & 0xFF) * a);
    return (c & 0x00FFFFFFu) | (al << 24);
}
void SubmitBlob(float cx1, float cy1, float hw1, float hh1,
                float cx2, float cy2, float hw2, float hh2,
                float radius, float k, float fade, Material m) {
    if (!g) return;
    float minx = std::min(cx1 - hw1, cx2 - hw2), maxx = std::max(cx1 + hw1, cx2 + hw2);
    float miny = std::min(cy1 - hh1, cy2 - hh2), maxy = std::max(cy1 + hh1, cy2 + hh2);
    Primitive p;
    p.cx = (minx + maxx) * 0.5f; p.cy = (miny + maxy) * 0.5f;
    p.hw = (maxx - minx) * 0.5f + k; p.hh = (maxy - miny) * 0.5f + k;
    p.corner_radius = radius; p.fade = fade; p.material = m;
    p.b1x = cx1; p.b1y = cy1; p.b1w = hw1; p.b1h = hh1;
    p.b2x = cx2; p.b2y = cy2; p.b2w = hw2; p.b2h = hh2;
    p.blob_k = k < 0.5f ? 0.5f : k;
    g->Submit(p);
}
void Spinner(const char* id, float radius, ImU32 col) {
    ImGui::PushID(id);
    if (col == 0) col = AccentCol();
    ImVec2 cur = ImGui::GetCursorScreenPos();
    ImGui::Dummy(ImVec2(radius * 2.0f, radius * 2.0f));
    ImVec2 c(cur.x + radius, cur.y + radius);
    ImDrawList* dl = ImGui::GetWindowDrawList();
    float rot = (float)ImGui::GetTime() * 5.2f;
    const int N = 9;
    for (int i = 0; i < N; ++i) {
        float ang = rot + (float)i / N * 2.0f * kPI;
        float f = (float)i / (float)(N - 1);
        ImU32 cc = (col & 0x00FFFFFFu) | ((ImU32)(45 + 210 * f) << 24);
        ImVec2 p0(c.x + cosf(ang) * radius * 0.52f, c.y + sinf(ang) * radius * 0.52f);
        ImVec2 p1(c.x + cosf(ang) * radius,         c.y + sinf(ang) * radius);
        dl->AddLine(p0, p1, cc, 2.6f);
    }
    ImGui::PopID();
}
struct DiscInfo { size_t idx; float x, w, y0; bool open; float elevate; };
static std::vector<DiscInfo> g_disc_stack;
bool BeginDisclosure(const char* title, bool* open) {
    ImGui::PushID(title);
    float w = ImGui::GetContentRegionAvail().x;
    ImVec2 cur = ImGui::GetCursorScreenPos();
    const float hh = 42.0f;
    ImGui::InvisibleButton("##disc", ImVec2(w, hh));
    ImGuiID id = ImGui::GetID("##disc");
    bool hov = ImGui::IsItemHovered(), act = ImGui::IsItemActive();
    if (ImGui::IsItemDeactivated() && hov) *open = !*open;
    Spring& chs = g->springs().Get((uint32_t)id, 24, SpringStyle::Critical, *open ? 1.0f : 0.0f);
    chs.target = *open ? 1.0f : 0.0f; chs.Tick(Dt());
    Spring& lif = g->springs().Get((uint32_t)id, 26, SpringStyle::Critical, 0.0f);
    lif.target = (hov || act) ? 1.0f : 0.0f; lif.Tick(Dt());
    Spring& prs = g->springs().Get((uint32_t)id, 28, SpringStyle::Critical, 1.0f);
    prs.target = act ? 0.9f : 1.0f; prs.Tick(Dt());
    float liftv = Clampf(lif.x, 0.0f, 1.0f), pressv = Clampf(prs.x, 0.82f, 1.05f);
    float shift = liftv * US(3.0f);
    size_t idx = g ? g->Submit(Primitive{}) : 0;
    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImVec2 cc(cur.x + 18.0f + shift, cur.y + hh * 0.5f);
    float ang = chs.x * (kPI * 0.5f), ca = cosf(ang), sa = sinf(ang);
    auto rt = [&](float ox, float oy) { return ImVec2(cc.x + (ox*ca - oy*sa)*pressv, cc.y + (ox*sa + oy*ca)*pressv); };
    ImU32 chc = SmoothCol((uint32_t)id, 30, (hov||act) ? kInk : kInkSoft);
    dl->AddLine(rt(-3,-5), rt(4,0), chc, 2.4f);
    dl->AddLine(rt(-3, 5), rt(4,0), chc, 2.4f);
    ImVec2 ts = ImGui::CalcTextSize(title);
    dl->AddText(ImVec2(cur.x + 34.0f + shift, cur.y + (hh - ts.y) * 0.5f), kInk, title);
    g_disc_stack.push_back({ idx, cur.x, w, cur.y, *open, 1.0f + 0.16f*liftv });
    if (*open) { ImGui::Indent(kPanelPadX); ImGui::PushItemWidth(w - 2*kPanelPadX);
                 ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(12, kPanelRowGap)); ImGui::Dummy(ImVec2(0, 4)); }
    return *open;
}
void EndDisclosure() {
    if (g_disc_stack.empty()) return;
    DiscInfo d = g_disc_stack.back(); g_disc_stack.pop_back();
    const float hh = 42.0f;
    if (d.open) { ImGui::PopStyleVar(); ImGui::PopItemWidth(); ImGui::Unindent(kPanelPadX); ImGui::Dummy(ImVec2(0, kPanelPadB)); }
    float y1 = d.open ? ImGui::GetCursorScreenPos().y : (d.y0 + hh);
    float h = y1 - d.y0;
    if (g && h > 1.0f) { Primitive& pr = g->At(d.idx); pr.cx = d.x + d.w*0.5f; pr.cy = d.y0 + h*0.5f;
        pr.hw = d.w*0.5f; pr.hh = h*0.5f; pr.corner_radius = 15.0f; pr.fade = g->SubmitFadeValue(); pr.material = Material::Thin; pr.elevate = d.elevate; }
    ImGui::PopID();
    ImGui::Dummy(ImVec2(0, kPanelGap + 4.0f));
}
