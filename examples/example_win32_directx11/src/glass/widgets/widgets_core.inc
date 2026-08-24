ImU32 SmoothCol(uint32_t id, uint32_t slot, ImU32 target, SpringStyle st = SpringStyle::Critical);

bool Button(const char* label, bool primary, ImVec2 size) {
    ImGuiID id = ImGui::GetID(label);
    ImVec2 ts = ImGui::CalcTextSize(label);
    if (size.x == 0) size.x = ts.x + US(36.0f);
    if (size.y == 0) size.y = ts.y + US(18.0f);
    ImVec2 cur = ImGui::GetCursorScreenPos();
    ImGui::InvisibleButton(label, size);
    bool hov = ImGui::IsItemHovered();
    bool act = ImGui::IsItemActive();
    bool clicked = ImGui::IsItemDeactivated() && ImGui::IsItemHovered();
    if (ImGui::IsItemActivated()) PlaySfx(primary ? Sfx::ButtonPrimary : Sfx::ButtonSecondary);
    Spring& sc = g->springs().Get((uint32_t)id, 1, SpringStyle::Bouncy, 1.0f);
    sc.target = act ? 0.96f : (hov ? 1.03f : 1.0f); sc.Tick(Dt());
    Spring& el = g->springs().Get((uint32_t)id, 2, SpringStyle::Critical, 1.0f);
    el.target = act ? 0.6f : (hov ? 1.55f : 1.0f); el.Tick(Dt());
    float gel = 1.0f - sc.x;
    Primitive p;
    p.cx = cur.x + size.x * 0.5f; p.cy = cur.y + size.y * 0.5f;
    p.hw = size.x * 0.5f * sc.x * (1.0f + gel * 0.8f);
    p.hh = size.y * 0.5f * sc.x * (1.0f - gel * 0.5f);
    p.corner_radius = std::min(size.y * 0.5f, US(13.0f));
    p.fade = 1.0f; p.elevate = el.x;
    p.material = primary ? Material::Accent : Material::Thin;
    g->Submit(p);
    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImGuiStorage* st = ImGui::GetStateStorage();
    ImGuiID rxk = id ^ 0x9001u, ryk = id ^ 0x9002u;
    Spring& rip = g->springs().Get((uint32_t)id, 3, SpringStyle::Critical, 1.0f);
    if (ImGui::IsItemActivated()) { ImVec2 mp = ImGui::GetIO().MousePos; st->SetFloat(rxk, mp.x); st->SetFloat(ryk, mp.y); rip.x = 0.0f; rip.v = 0.0f; }
    rip.target = 1.0f; rip.Tick(Dt());
    if (rip.x < 0.99f) {
        float rr = Clampf(rip.x, 0.0f, 1.0f);
        float maxR = sqrtf(size.x*size.x + size.y*size.y);
        ImVec2 rc(st->GetFloat(rxk, p.cx), st->GetFloat(ryk, p.cy));
        ImU32 base = primary ? IM_COL32(255,255,255,255) : AccentCol();
        ImU32 col2 = (base & 0x00FFFFFFu) | ((ImU32)(70 * (1.0f - rr)) << 24);
        dl->PushClipRect(ImVec2(cur.x, cur.y), ImVec2(cur.x + size.x, cur.y + size.y), true);
        dl->AddCircleFilled(rc, rr * maxR, col2);
        dl->PopClipRect();
    }
    ImU32 colTarget = primary ? IM_COL32(255,255,255,255) : (hov ? IM_COL32(255,255,255,255) : kInk);
    ImU32 col = SmoothCol((uint32_t)id, 20, colTarget);
    dl->AddText(ImVec2(cur.x + (size.x - ts.x) * 0.5f, cur.y + (size.y - ts.y) * 0.5f), col, label);
    return clicked;
}

bool Toggle(const char* label, bool* v) {
    const float w = US(50.0f), h = US(30.0f), pad = US(3.0f);
    ImGui::PushID(label);
    ImVec2 cur = ImGui::GetCursorScreenPos();
    ImGui::InvisibleButton("##sw", ImVec2(w, h));
    bool active  = ImGui::IsItemActive();
    bool clicked = ImGui::IsItemDeactivated() && ImGui::IsItemHovered();
    if (ImGui::IsItemActivated()) PlaySfx(*v ? Sfx::ToggleOff : Sfx::ToggleOn);
    if (clicked) *v = !*v;
    ImGuiID id = ImGui::GetID("a");
    Spring& k = g->springs().Get((uint32_t)id, 2, SpringStyle::Critical, *v ? 1.0f : 0.0f);
    k.target = *v ? 1.0f : 0.0f; k.Tick(Dt());
    Spring& grab = g->springs().Get((uint32_t)id, 3, SpringStyle::Critical, 1.0f);
    grab.target = active ? 1.12f : 1.0f; grab.Tick(Dt());
    float kc = std::max(0.0f, std::min(1.0f, k.x));
    Primitive track;
    track.cx = cur.x + w * 0.5f; track.cy = cur.y + h * 0.5f;
    track.hw = w * 0.5f; track.hh = h * 0.5f; track.corner_radius = h * 0.5f;
    track.fade = 1.0f; track.material = Material::Thin;
    g->Submit(track);
    Primitive on = track; on.material = Material::Accent; on.fade = kc;
    g->Submit(on);
    float knob_r = h * 0.5f - pad;
    float speed  = std::min(0.45f, std::fabs(k.v) * 0.06f);
    float grow   = grab.x;
    Spring& lag = g->springs().Get((uint32_t)id, 4, SpringStyle::Overdamped, kc);
    lag.target = kc; lag.Tick(Dt());
    float ky  = cur.y + h * 0.5f;
    float kxh = cur.x + h * 0.5f + (w - h) * kc;
    float kxt = cur.x + h * 0.5f + (w - h) * std::max(0.0f, std::min(1.0f, lag.x));
    float khw = knob_r * (1.0f + speed) * grow, khh = knob_r * (1.0f - speed * 0.5f) * grow;
    if (std::fabs(kxh - kxt) > 2.0f) {
        SubmitBlob(kxh, ky, khw, khh, kxt, ky, khw * 0.82f, khh * 0.82f, std::min(khw, khh), 9.0f, 1.0f, Material::Knob);
    } else {
        Primitive knob;
        knob.cx = kxh; knob.cy = ky; knob.hw = khw; knob.hh = khh;
        knob.corner_radius = std::min(khw, khh); knob.fade = 1.0f; knob.material = Material::Knob;
        g->Submit(knob);
    }
    ImGui::SameLine(0.0f, 12.0f);
    ImGui::AlignTextToFramePadding();
    ImGui::TextUnformatted(label);
    ImGui::PopID();
    return clicked;
}

bool Slider(const char* label, float* v, float vmin, float vmax, const char* fmt) {
    ImGui::PushID(label);
    const float track_h = US(6.0f), knob_r = US(12.0f);
    const float row_h = std::max(knob_r * 2.0f, ImGui::GetFontSize() + US(6.0f));
    const float w = ImGui::CalcItemWidth();
    ImVec2 cur = ImGui::GetCursorScreenPos();
    ImGui::InvisibleButton("##sl", ImVec2(w, row_h));
    bool active = ImGui::IsItemActive();
    bool hov    = ImGui::IsItemHovered();
    if (ImGui::IsItemActivated())   PlaySfx(Sfx::SliderGrab);
    if (ImGui::IsItemDeactivated()) PlaySfx(Sfx::SliderRelease);
    ImGuiID gid = ImGui::GetID("g");
    Spring& grow = g->springs().Get((uint32_t)gid, 4, SpringStyle::Bouncy, 1.0f);
    grow.target = active ? 1.34f : (hov ? 1.10f : 1.0f); grow.Tick(Dt());
    float t = (*v - vmin) / (vmax - vmin);
    if (active) {
        float mx = ImGui::GetIO().MousePos.x;
        t = (mx - cur.x - knob_r) / std::max(1.0f, (w - 2.0f * knob_r));
        t = std::max(0.0f, std::min(1.0f, t));
        *v = vmin + t * (vmax - vmin);
    }
    ImGuiStorage* st = ImGui::GetStateStorage();
    ImGuiID tk = gid ^ 0x5A5Au;
    float tShown = st->GetFloat(tk, t);
    if (active) tShown = t;
    else { tShown += (t - tShown) * Clampf(Dt() * 18.0f, 0.0f, 1.0f); if (fabsf(tShown - t) < 0.0005f) tShown = t; }
    st->SetFloat(tk, tShown);
    float cy = cur.y + row_h * 0.5f;
    float kx = cur.x + knob_r + (w - 2.0f * knob_r) * tShown;
    Primitive track;
    track.cx = cur.x + w * 0.5f; track.cy = cy;
    track.hw = w * 0.5f; track.hh = track_h * 0.5f; track.corner_radius = track_h * 0.5f;
    track.fade = 1.0f; track.material = Material::Thin;
    g->Submit(track);
    if (kx > cur.x + track_h) {
        Primitive fill;
        fill.cx = (cur.x + kx) * 0.5f; fill.cy = cy;
        fill.hw = (kx - cur.x) * 0.5f; fill.hh = track_h * 0.5f; fill.corner_radius = track_h * 0.5f;
        fill.fade = 1.0f; fill.material = Material::Accent;
        g->Submit(fill);
    }
    float kr = knob_r * grow.x;
    Spring& klag = g->springs().Get((uint32_t)gid, 5, SpringStyle::Overdamped, tShown);
    klag.stiffness = 1980.0f; klag.damping   = 142.39f;
    klag.target = tShown; klag.Tick(Dt());
    float kxLag = cur.x + knob_r + (w - 2.0f * knob_r) * Clampf(klag.x, 0.0f, 1.0f);
    if (active && std::fabs(kx - kxLag) > 2.5f) {
        SubmitBlob(kx, cy, kr, kr, kxLag, cy, kr * 0.8f, kr * 0.8f, kr, 9.0f, 1.0f, Material::Knob);
    } else {
        Primitive knob;
        knob.cx = kx; knob.cy = cy; knob.hw = kr; knob.hh = kr;
        knob.corner_radius = kr; knob.fade = 1.0f; knob.material = Material::Knob;
        g->Submit(knob);
    }
    ImGui::SameLine(0.0f, 12.0f);
    ImGui::AlignTextToFramePadding();
    char buf[64]; std::snprintf(buf, sizeof(buf), fmt, *v);
    ImGui::Text("%s  %s", label, buf);
    ImGui::PopID();
    return active;
}

ImU32 SmoothCol(uint32_t id, uint32_t slot, ImU32 target, SpringStyle st) {
    float tr = (float)((target >> IM_COL32_R_SHIFT) & 0xFF);
    float tg = (float)((target >> IM_COL32_G_SHIFT) & 0xFF);
    float tb = (float)((target >> IM_COL32_B_SHIFT) & 0xFF);
    float ta = (float)((target >> IM_COL32_A_SHIFT) & 0xFF);
    Spring& sr = g->springs().Get(id, slot+0, st, tr); sr.target = tr; sr.Tick(Dt());
    Spring& sg = g->springs().Get(id, slot+1, st, tg); sg.target = tg; sg.Tick(Dt());
    Spring& sb = g->springs().Get(id, slot+2, st, tb); sb.target = tb; sb.Tick(Dt());
    Spring& sa = g->springs().Get(id, slot+3, st, ta); sa.target = ta; sa.Tick(Dt());
    auto cl = [](float v){ int x = (int)(v + 0.5f); return x < 0 ? 0 : (x > 255 ? 255 : x); };
    return IM_COL32(cl(sr.x), cl(sg.x), cl(sb.x), cl(sa.x));
}

static void DrawSpark(ImDrawList* dl, ImVec2 c, float s, ImU32 col) {
    float a = ((col >> IM_COL32_A_SHIFT) & 0xFF) / 255.0f;
    ImU32 soft = (col & ~IM_COL32_A_MASK) | ((ImU32)(38.0f * a) << IM_COL32_A_SHIFT);
    ImU32 ray  = (col & ~IM_COL32_A_MASK) | ((ImU32)(150.0f * a) << IM_COL32_A_SHIFT);
    auto dia = [&](float dx, float dy, float L, float Wd, ImU32 cc){ float px=-dy, py=dx; ImVec2 q[4]={ ImVec2(c.x+dx*L,c.y+dy*L), ImVec2(c.x+px*Wd,c.y+py*Wd), ImVec2(c.x-dx*L,c.y-dy*L), ImVec2(c.x-px*Wd,c.y-py*Wd) }; dl->AddConvexPolyFilled(q,4,cc); };
    dia(0.0f,1.0f, s*0.62f, s*0.20f, soft); dia(1.0f,0.0f, s*0.62f, s*0.20f, soft);
    const float d=0.70710678f; dia(d,d, s*0.34f, s*0.07f, ray); dia(-d,d, s*0.34f, s*0.07f, ray);
    dia(0.0f,1.0f, s*0.50f, s*0.13f, col); dia(1.0f,0.0f, s*0.50f, s*0.13f, col);
    dl->AddCircleFilled(c, s*0.10f, col, 16);
}
void DrawIcon(ImDrawList* dl, Icon ic, ImVec2 c, float s, ImU32 col, float th) {
    s *= g_ui; th *= g_ui;
    if (ic == Icon::Spark) { DrawSpark(dl, c, s, col); return; }
    {
        ImFont* fnt = IconIsBrand(ic) ? g_icon_font_brands : g_icon_font;
        unsigned cp = IconCodepoint(ic);
        if (fnt && cp) {
            char u[4] = { (char)(0xE0 | (cp >> 12)), (char)(0x80 | ((cp >> 6) & 0x3F)), (char)(0x80 | (cp & 0x3F)), 0 };
            float fs = s * 1.15f;
            ImVec2 ts = fnt->CalcTextSizeA(fs, 99999.0f, 0.0f, u);
            ImVec2 p(c.x - ts.x * 0.5f, c.y - ts.y * 0.5f);
            float ca = ((col >> IM_COL32_A_SHIFT) & 0xFF) / 255.0f;
            dl->AddText(fnt, fs, ImVec2(p.x,        p.y + 1.5f), IM_COL32(0, 0, 0, (int)(64 * ca)),       u);
            dl->AddText(fnt, fs, ImVec2(p.x,        p.y - 1.0f), IM_COL32(255, 255, 255, (int)(30 * ca)), u);
            dl->AddText(fnt, fs, p, col, u);
            return;
        }
    }
    const float r = s * 0.5f;
    const float W = IM_COL32_WHITE == col ? th : th;
    switch (ic) {
    case Icon::Wifi: {
        ImVec2 o(c.x, c.y + r * 0.45f);
        for (int i = 0; i < 3; ++i) { dl->PathArcTo(o, r * (0.42f + 0.28f * i), -2.356f, -0.785f, 18); dl->PathStroke(col, 0, th); }
        dl->AddCircleFilled(o, th * 0.95f, col);
    } break;
    case Icon::Bluetooth: {
        float h = r * 0.9f, w = r * 0.5f;
        ImVec2 A(c.x, c.y - h), B(c.x, c.y + h), P(c.x + w, c.y - h * 0.5f), Q(c.x + w, c.y + h * 0.5f);
        ImVec2 M(c.x - w, c.y + h * 0.5f), N(c.x - w, c.y - h * 0.5f);
        dl->AddLine(A, B, col, th); dl->AddLine(A, Q, col, th); dl->AddLine(Q, N, col, th);
        dl->AddLine(B, P, col, th); dl->AddLine(P, M, col, th);
    } break;
    case Icon::Bolt: {
        dl->PathLineTo(ImVec2(c.x + r * 0.25f, c.y - r));
        dl->PathLineTo(ImVec2(c.x - r * 0.35f, c.y + r * 0.1f));
        dl->PathLineTo(ImVec2(c.x + r * 0.05f, c.y + r * 0.1f));
        dl->PathLineTo(ImVec2(c.x - r * 0.25f, c.y + r));
        dl->PathLineTo(ImVec2(c.x + r * 0.4f, c.y - r * 0.15f));
        dl->PathLineTo(ImVec2(c.x - r * 0.02f, c.y - r * 0.15f));
        dl->PathFillConcave(col);
    } break;
    case Icon::Sun: {
        dl->AddCircle(c, r * 0.42f, col, 18, th);
        for (int k = 0; k < 8; ++k) { float a = k * (kPI / 4.0f); ImVec2 d(cosf(a), sinf(a));
            dl->AddLine(ImVec2(c.x + d.x * r * 0.62f, c.y + d.y * r * 0.62f), ImVec2(c.x + d.x * r * 0.95f, c.y + d.y * r * 0.95f), col, th); }
    } break;
    case Icon::Speaker: {
        dl->AddLine(ImVec2(c.x - r * 0.7f, c.y - r * 0.28f), ImVec2(c.x - r * 0.7f, c.y + r * 0.28f), col, th);
        dl->PathLineTo(ImVec2(c.x - r * 0.7f, c.y - r * 0.28f)); dl->PathLineTo(ImVec2(c.x - r * 0.3f, c.y - r * 0.28f));
        dl->PathLineTo(ImVec2(c.x + r * 0.1f, c.y - r * 0.62f)); dl->PathLineTo(ImVec2(c.x + r * 0.1f, c.y + r * 0.62f));
        dl->PathLineTo(ImVec2(c.x - r * 0.3f, c.y + r * 0.28f)); dl->PathLineTo(ImVec2(c.x - r * 0.7f, c.y + r * 0.28f));
        dl->PathStroke(col, ImDrawFlags_Closed, th);
        dl->PathArcTo(ImVec2(c.x + r * 0.1f, c.y), r * 0.5f, -1.0f, 1.0f, 12); dl->PathStroke(col, 0, th);
        dl->PathArcTo(ImVec2(c.x + r * 0.1f, c.y), r * 0.8f, -0.9f, 0.9f, 12); dl->PathStroke(col, 0, th);
    } break;
    case Icon::Bell: {
        ImVec2 L(c.x - r * 0.55f, c.y + r * 0.4f), R(c.x + r * 0.55f, c.y + r * 0.4f), T(c.x, c.y - r * 0.65f);
        dl->PathLineTo(L);
        dl->PathBezierQuadraticCurveTo(ImVec2(c.x - r * 0.5f, c.y - r * 0.45f), T, 12);
        dl->PathBezierQuadraticCurveTo(ImVec2(c.x + r * 0.5f, c.y - r * 0.45f), R, 12);
        dl->PathStroke(col, 0, th);
        dl->AddLine(L, R, col, th);
        dl->AddCircleFilled(T, th * 0.8f, col);
        dl->AddCircleFilled(ImVec2(c.x, c.y + r * 0.62f), th * 0.9f, col);
    } break;
    case Icon::Moon: {
        dl->PathArcTo(c, r * 0.75f, kPI * 0.42f, kPI * 1.62f, 22); dl->PathStroke(col, 0, th * 1.25f);
    } break;
    case Icon::Display: {
        dl->AddRect(ImVec2(c.x - r * 0.85f, c.y - r * 0.7f), ImVec2(c.x + r * 0.85f, c.y + r * 0.42f), col, 3.0f, 0, th);
        dl->AddLine(ImVec2(c.x, c.y + r * 0.42f), ImVec2(c.x, c.y + r * 0.72f), col, th);
        dl->AddLine(ImVec2(c.x - r * 0.4f, c.y + r * 0.8f), ImVec2(c.x + r * 0.4f, c.y + r * 0.8f), col, th);
    } break;
    case Icon::Palette: {
        dl->AddCircle(c, r * 0.82f, col, 22, th);
        dl->AddCircleFilled(ImVec2(c.x - r * 0.32f, c.y - r * 0.3f), th * 1.05f, col);
        dl->AddCircleFilled(ImVec2(c.x + r * 0.36f, c.y - r * 0.12f), th * 1.05f, col);
        dl->AddCircleFilled(ImVec2(c.x + r * 0.05f, c.y + r * 0.38f), th * 1.05f, col);
    } break;
    case Icon::Person: {
        dl->AddCircleFilled(ImVec2(c.x, c.y - r * 0.4f), r * 0.32f, col);
        dl->PathArcTo(ImVec2(c.x, c.y + r * 1.0f), r * 0.62f, kPI * 1.18f, kPI * 1.82f, 18); dl->PathStroke(col, 0, th * 1.7f);
    } break;
    case Icon::Lock: {
        dl->AddRectFilled(ImVec2(c.x - r * 0.55f, c.y - r * 0.05f), ImVec2(c.x + r * 0.55f, c.y + r * 0.6f), col, 3.0f);
        dl->PathArcTo(ImVec2(c.x, c.y - r * 0.05f), r * 0.36f, kPI, kPI * 2.0f, 14); dl->PathStroke(col, 0, th);
    } break;
    case Icon::Gear: {
        for (int k = 0; k < 8; ++k) { float a = k * (kPI / 4.0f); ImVec2 d(cosf(a), sinf(a));
            dl->AddLine(ImVec2(c.x + d.x * r * 0.45f, c.y + d.y * r * 0.45f), ImVec2(c.x + d.x * r * 0.85f, c.y + d.y * r * 0.85f), col, th * 1.5f); }
        dl->AddCircleFilled(c, r * 0.5f, col); dl->AddCircleFilled(c, r * 0.2f, IM_COL32(0, 0, 0, 0));
        dl->AddCircle(c, r * 0.5f, col, 20, th);
    } break;
    case Icon::Search: {
        ImVec2 sc(c.x - r * 0.12f, c.y - r * 0.12f);
        dl->AddCircle(sc, r * 0.5f, col, 18, th);
        dl->AddLine(ImVec2(sc.x + r * 0.38f, sc.y + r * 0.38f), ImVec2(c.x + r * 0.75f, c.y + r * 0.75f), col, th * 1.3f);
    } break;
    case Icon::Star: {
        ImVec2 p[10];
        for (int k = 0; k < 10; ++k) { float a = -kPI * 0.5f + k * (kPI / 5.0f); float rr = (k & 1) ? r * 0.42f : r * 0.95f;
            p[k] = ImVec2(c.x + cosf(a) * rr, c.y + sinf(a) * rr); }
        for (int k = 0; k < 10; ++k) dl->PathLineTo(p[k]);
        dl->PathStroke(col, ImDrawFlags_Closed, th);
    } break;
    case Icon::Cloud: {
        dl->AddCircleFilled(ImVec2(c.x - r * 0.35f, c.y + r * 0.1f), r * 0.38f, col);
        dl->AddCircleFilled(ImVec2(c.x + r * 0.3f, c.y + r * 0.1f), r * 0.34f, col);
        dl->AddCircleFilled(ImVec2(c.x - r * 0.02f, c.y - r * 0.2f), r * 0.46f, col);
        dl->AddRectFilled(ImVec2(c.x - r * 0.7f, c.y + r * 0.1f), ImVec2(c.x + r * 0.62f, c.y + r * 0.5f), col, 4.0f);
    } break;
    case Icon::Heart: {
        dl->AddCircleFilled(ImVec2(c.x - r * 0.32f, c.y - r * 0.18f), r * 0.4f, col);
        dl->AddCircleFilled(ImVec2(c.x + r * 0.32f, c.y - r * 0.18f), r * 0.4f, col);
        dl->PathLineTo(ImVec2(c.x - r * 0.66f, c.y + r * 0.02f));
        dl->PathLineTo(ImVec2(c.x + r * 0.66f, c.y + r * 0.02f));
        dl->PathLineTo(ImVec2(c.x, c.y + r * 0.78f));
        dl->PathFillConvex(col);
    } break;
    case Icon::Airplane: {
        dl->PathLineTo(ImVec2(c.x, c.y - r));
        dl->PathLineTo(ImVec2(c.x + r * 0.18f, c.y - r * 0.55f));
        dl->PathLineTo(ImVec2(c.x + r * 0.9f, c.y + r * 0.1f));
        dl->PathLineTo(ImVec2(c.x + r * 0.18f, c.y - r * 0.05f));
        dl->PathLineTo(ImVec2(c.x + r * 0.12f, c.y + r * 0.55f));
        dl->PathLineTo(ImVec2(c.x + r * 0.42f, c.y + r * 0.85f));
        dl->PathLineTo(ImVec2(c.x, c.y + r * 0.6f));
        dl->PathLineTo(ImVec2(c.x - r * 0.42f, c.y + r * 0.85f));
        dl->PathLineTo(ImVec2(c.x - r * 0.12f, c.y + r * 0.55f));
        dl->PathLineTo(ImVec2(c.x - r * 0.18f, c.y - r * 0.05f));
        dl->PathLineTo(ImVec2(c.x - r * 0.9f, c.y + r * 0.1f));
        dl->PathLineTo(ImVec2(c.x - r * 0.18f, c.y - r * 0.55f));
        dl->PathStroke(col, ImDrawFlags_Closed, th);
    } break;
    case Icon::Check: {
        dl->AddLine(ImVec2(c.x - r * 0.5f, c.y), ImVec2(c.x - r * 0.1f, c.y + r * 0.45f), col, th * 1.3f);
        dl->AddLine(ImVec2(c.x - r * 0.1f, c.y + r * 0.45f), ImVec2(c.x + r * 0.55f, c.y - r * 0.45f), col, th * 1.3f);
    } break;
    case Icon::ChevronR: {
        dl->AddLine(ImVec2(c.x - r * 0.2f, c.y - r * 0.45f), ImVec2(c.x + r * 0.25f, c.y), col, th);
        dl->AddLine(ImVec2(c.x + r * 0.25f, c.y), ImVec2(c.x - r * 0.2f, c.y + r * 0.45f), col, th);
    } break;
    default: break;
    }
    (void)W;
}

bool RowToggle(Icon ic, ImU32 tile, const char* label, bool* v) {
    ImGui::PushID(label);
    const float rowh = US(40.0f), tsz = US(26.0f), sw = US(46.0f), sh = US(28.0f), pad = US(3.0f);
    float w = ImGui::GetContentRegionAvail().x;
    float rowW = std::min(w, US(320.0f));
    ImVec2 cur = ImGui::GetCursorScreenPos();
    ImGui::InvisibleButton("##row", ImVec2(rowW, rowh));
    ImGuiID id = ImGui::GetID("##row");
    bool hov = ImGui::IsItemHovered();
    bool act = ImGui::IsItemActive();
    bool clicked = ImGui::IsItemDeactivated() && ImGui::IsItemHovered();
    if (ImGui::IsItemActivated()) PlaySfx(*v ? Sfx::ToggleOff : Sfx::ToggleOn);
    if (clicked) *v = !*v;
    ImDrawList* dl = ImGui::GetWindowDrawList();
    (void)tile;
    Spring& rscl = g->springs().Get((uint32_t)id, 20, SpringStyle::Critical, 1.0f);
    rscl.target = act ? 0.98f : (hov ? 1.015f : 1.0f); rscl.Tick(Dt());
    Spring& rele = g->springs().Get((uint32_t)id, 21, SpringStyle::Critical, 0.0f);
    rele.target = act ? 0.04f : (hov ? 1.0f : 0.0f); rele.Tick(Dt());
    float rowCx = cur.x + rowW * 0.5f, rowCy = cur.y + rowh * 0.5f;
    if (rele.x > 0.01f) {
        Primitive plate;
        plate.cx = rowCx; plate.cy = rowCy;
        plate.hw = rowW * 0.5f * rscl.x; plate.hh = rowh * 0.5f * rscl.x;
        plate.corner_radius = std::min(rowh * 0.5f, US(12.0f));
        plate.fade = Clampf(rele.x, 0.0f, 1.0f); plate.elevate = 1.0f + rele.x * 0.25f;
        plate.material = Material::Thin;
        g->Submit(plate);
    }
    float rs = rscl.x;
    auto sx = [&](float x){ return rowCx + (x - rowCx) * rs; };
    auto sy = [&](float y){ return rowCy + (y - rowCy) * rs; };
    ImU32 icCol = SmoothCol((uint32_t)id, 24, *v ? kInk : kInkSoft);
    ImU32 lbCol = SmoothCol((uint32_t)id, 28, (hov || *v) ? kInk : kInkSoft);
    ImVec2 icC(sx(cur.x + tsz * 0.5f), sy(cur.y + rowh * 0.5f));
    DrawIcon(dl, ic, icC, 26.0f * 0.66f * rs, icCol, 2.1f);
    ImVec2 lts = ImGui::CalcTextSize(label);
    dl->AddText(ImVec2(sx(cur.x + tsz + US(12)), sy(cur.y + (rowh - lts.y) * 0.5f)), lbCol, label);
    ImVec2 sp(cur.x + rowW - sw, cur.y + (rowh - sh) * 0.5f);
    Spring& k = g->springs().Get((uint32_t)id, 2, SpringStyle::Critical, *v ? 1.0f : 0.0f);
    k.target = *v ? 1.0f : 0.0f; k.Tick(Dt());
    float kc = Clampf(k.x, 0.0f, 1.0f);
    Spring& grab = g->springs().Get((uint32_t)id, 22, SpringStyle::Critical, 1.0f);
    grab.target = act ? 1.12f : 1.0f; grab.Tick(Dt());
    Spring& lag = g->springs().Get((uint32_t)id, 23, SpringStyle::Overdamped, kc);
    lag.target = kc; lag.Tick(Dt());
    float swCx = sx(sp.x + sw * 0.5f), swCy = sy(sp.y + sh * 0.5f);
    float swHw = sw * 0.5f * rs * 1.10f, swHh = sh * 0.5f * rs;
    Primitive tr; tr.cx = swCx; tr.cy = swCy; tr.hw = swHw; tr.hh = swHh;
    tr.corner_radius = sh * 0.5f * rs; tr.fade = 1.0f; tr.material = Material::Thin; g->Submit(tr);
    Primitive on = tr; on.material = Material::Accent; on.fade = kc; g->Submit(on);
    float kr = (sh * 0.5f - pad) * rs * grab.x, speed = Clampf(std::fabs(k.v) * 0.06f, 0.0f, 0.45f);
    float kny = swCy;
    float knxh = sx(sp.x + sh * 0.5f + (sw - sh) * kc);
    float knxt = sx(sp.x + sh * 0.5f + (sw - sh) * Clampf(lag.x, 0.0f, 1.0f));
    float knhw = kr * (1.0f + speed), knhh = kr * (1.0f - speed * 0.5f);
    if (std::fabs(knxh - knxt) > 2.0f) {
        SubmitBlob(knxh, kny, knhw, knhh, knxt, kny, knhw * 0.82f, knhh * 0.82f, std::min(knhw, knhh), 9.0f, 1.0f, Material::Knob);
    } else {
        Primitive kn; kn.cx = knxh; kn.cy = kny; kn.hw = knhw; kn.hh = knhh;
        kn.corner_radius = std::min(kn.hw, kn.hh); kn.fade = 1.0f; kn.material = Material::Knob; g->Submit(kn);
    }
    ImGui::PopID();
    return clicked;
}

void RowValue(Icon ic, ImU32 tile, const char* label, const char* value) {
    ImGui::PushID(label);
    const float rowh = US(48.0f), tsz = US(30.0f);
    float w = ImGui::GetContentRegionAvail().x;
    ImVec2 cur = ImGui::GetCursorScreenPos();
    ImGui::InvisibleButton("##row", ImVec2(w, rowh));
    ImGuiID id = ImGui::GetID("##row");
    bool hov = ImGui::IsItemHovered();
    bool act = ImGui::IsItemActive();
    ImDrawList* dl = ImGui::GetWindowDrawList();
    (void)tile;
    Spring& rscl = g->springs().Get((uint32_t)id, 20, SpringStyle::Critical, 1.0f);
    rscl.target = act ? 0.98f : (hov ? 1.015f : 1.0f); rscl.Tick(Dt());
    Spring& rele = g->springs().Get((uint32_t)id, 21, SpringStyle::Critical, 0.0f);
    rele.target = act ? 0.04f : (hov ? 1.0f : 0.0f); rele.Tick(Dt());
    Spring& chv = g->springs().Get((uint32_t)id, 22, SpringStyle::Critical, 0.0f);
    chv.target = (hov || act) ? 1.0f : 0.0f; chv.Tick(Dt());
    float rowCx = cur.x + w * 0.5f, rowCy = cur.y + rowh * 0.5f;
    if (rele.x > 0.01f) {
        Primitive plate;
        plate.cx = rowCx; plate.cy = rowCy;
        plate.hw = w * 0.5f * rscl.x; plate.hh = rowh * 0.5f * rscl.x;
        plate.corner_radius = std::min(rowh * 0.5f, US(12.0f));
        plate.fade = Clampf(rele.x, 0.0f, 1.0f); plate.elevate = 1.0f + rele.x * 0.25f;
        plate.material = Material::Thin;
        g->Submit(plate);
    }
    float rs = rscl.x;
    auto sx = [&](float x){ return rowCx + (x - rowCx) * rs; };
    auto sy = [&](float y){ return rowCy + (y - rowCy) * rs; };
    ImU32 icCol = SmoothCol((uint32_t)id, 24, hov ? kInk : kInkSoft);
    ImU32 lbCol = SmoothCol((uint32_t)id, 28, kInk);
    DrawIcon(dl, ic, ImVec2(sx(cur.x + tsz * 0.5f), sy(cur.y + rowh * 0.5f)), 30.0f * 0.6f * rs, icCol, 2.1f);
    ImVec2 lts = ImGui::CalcTextSize(label);
    dl->AddText(ImVec2(sx(cur.x + tsz + US(13)), sy(cur.y + (rowh - lts.y) * 0.5f)), lbCol, label);
    if (value && *value) {
        ImU32 vlCol = SmoothCol((uint32_t)id, 32, hov ? kInk : kInkSoft);
        ImVec2 vts = ImGui::CalcTextSize(value);
        dl->AddText(ImVec2(sx(cur.x + w - vts.x - US(22)), sy(cur.y + (rowh - vts.y) * 0.5f)), vlCol, value);
    }
    ImU32 chCol = SmoothCol((uint32_t)id, 36, hov ? IM_COL32(190,190,200,235) : IM_COL32(150,150,160,220));
    float chX = sx(cur.x + w - US(8) + US(4.0f) * chv.x);
    DrawIcon(dl, Icon::ChevronR, ImVec2(chX, sy(cur.y + rowh * 0.5f)), 14.0f * rs, chCol, 2.0f);
    ImGui::PopID();
}

struct Notif {
    Icon  ic; ImU32 tile;
    char  app[32], title[48], msg[96];
    float age; float life; Spring sp;
};
static std::vector<Notif> g_notifs;

static void SafeCopy(char* dst, const char* src, int n) {
    int i = 0; for (; src[i] && i < n - 1; ++i) dst[i] = src[i]; dst[i] = 0;
}

void Notify(Icon, ImU32, const char*, const char*, const char*) {

}

void DrawNotifications(float win_w, float ) {
    if (g_notifs.empty()) return;
    float dt = ImGui::GetIO().DeltaTime;
    ImDrawList* fdl = ImGui::GetForegroundDrawList();
    float bw = win_w * 0.30f; if (bw < 240.0f) bw = 240.0f; if (bw > 360.0f) bw = 360.0f;
    const float bh = 64.0f;
    const float right = win_w - 16.0f;
    float y = 16.0f;
    for (size_t idx = 0; idx < g_notifs.size(); ++idx) {
        Notif& n = g_notifs[idx];
        n.age += dt;
        n.sp.target = (n.age > n.life) ? 0.0f : 1.0f; n.sp.Tick(dt);
        float a = Clampf(n.sp.x, 0.0f, 1.0f);
        int A = (int)(255.0f * a);
        float ease = a * a * (3.0f - 2.0f * a);
        float x  = right - bw;
        float xx = x + (1.0f - ease) * (bw + 50.0f);
        Primitive bg; bg.cx = xx + bw * 0.5f; bg.cy = y + bh * 0.5f;
        bg.hw = bw * 0.5f; bg.hh = bh * 0.5f; bg.corner_radius = 18.0f; bg.fade = a; bg.material = Material::Thick;
        g->SubmitOverlay(bg);
        fdl->PushClipRect(ImVec2(xx, y), ImVec2(xx + bw - 8.0f, y + bh), true);
        float tsz = 38.0f;
        ImVec2 tp(xx + 12.0f, y + (bh - tsz) * 0.5f);
        fdl->AddRectFilled(tp, ImVec2(tp.x + tsz, tp.y + tsz), (n.tile & 0x00FFFFFF) | (A << 24), 11.0f);
        DrawIcon(fdl, n.ic, ImVec2(tp.x + tsz * 0.5f, tp.y + tsz * 0.5f), tsz * 0.58f, IM_COL32(255,255,255,A), 2.2f);
        float tx = xx + tsz + 22.0f;
        int as = (int)(170.0f * a);
        fdl->AddText(ImVec2(tx, y + 9.0f),  IM_COL32(151,154,168,as), n.app);
        fdl->AddText(ImVec2(tx, y + 24.0f), (kInk & 0x00FFFFFF) | (A << 24), n.title);
        fdl->AddText(ImVec2(tx, y + 42.0f), IM_COL32(151,154,168,as), n.msg);
        fdl->AddText(ImVec2(xx + bw - 40.0f, y + 9.0f), IM_COL32(140,142,156,as), "now");
        fdl->PopClipRect();
        y += bh + 12.0f;
    }
    for (int i = (int)g_notifs.size() - 1; i >= 0; --i)
        if (g_notifs[i].age > g_notifs[i].life && g_notifs[i].sp.x < 0.02f)
            g_notifs.erase(g_notifs.begin() + i);
}
