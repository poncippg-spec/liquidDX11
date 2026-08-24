int TabBar(const char* id, const Icon* icons, const char* const* labels, int count, int current) {
    if (count <= 0) return current;
    ImGui::PushID(id);
    float w = ImGui::GetContentRegionAvail().x;
    const float h = 46.0f;
    ImVec2 cur = ImGui::GetCursorScreenPos();
    ImGui::Dummy(ImVec2(w, h));
    float seg = w / (float)count;
    { Primitive bg; bg.cx = cur.x + w*0.5f; bg.cy = cur.y + h*0.5f; bg.hw = w*0.5f; bg.hh = h*0.5f;
      bg.corner_radius = h*0.5f; bg.fade = 1.0f; bg.material = Material::Thin; g->Submit(bg); }
    ImGuiID sid = ImGui::GetID("sel");
    Spring& sp = g->springs().Get((uint32_t)sid, 29, SpringStyle::Bouncy, (float)current);
    sp.target = (float)current; sp.Tick(Dt());
    Spring& tail = g->springs().Get((uint32_t)sid, 30, SpringStyle::Overdamped, (float)current);
    tail.target = sp.x; tail.Tick(Dt());
    { float scy = cur.y + h*0.5f, shw = seg*0.5f - 4.0f, shh = h*0.5f - 4.0f;
      float hX = cur.x + (sp.x + 0.5f)*seg, tX = cur.x + (tail.x + 0.5f)*seg;
      if (std::fabs(hX - tX) > 1.5f) SubmitBlob(hX, scy, shw, shh, tX, scy, shw, shh, shh, 16.0f, 1.0f, Material::Accent);
      else { Primitive sel; sel.cx = hX; sel.cy = scy; sel.hw = shw; sel.hh = shh;
             sel.corner_radius = shh; sel.fade = 1.0f; sel.material = Material::Accent; g->Submit(sel); } }
    int result = current;
    ImDrawList* dl = ImGui::GetWindowDrawList();
    float cy = cur.y + h*0.5f;
    for (int i = 0; i < count; ++i) {
        ImGui::PushID(i); ImGui::SetCursorScreenPos(ImVec2(cur.x + i*seg, cur.y));
        ImGui::InvisibleButton("##t", ImVec2(seg, h));
        if (ImGui::IsItemActivated()) PlaySfx(Sfx::TabChange);
        if (ImGui::IsItemDeactivated() && ImGui::IsItemHovered()) result = i;
        ImGui::PopID();
        float sx = cur.x + (i + 0.5f)*seg;
        ImVec2 ls = ImGui::CalcTextSize(labels[i]);
        float iconSz = 18.0f, tot = iconSz + 7.0f + ls.x, startX = sx - tot*0.5f;
        ImU32 col = SmoothCol((uint32_t)sid + (uint32_t)i*64u, 20, (i == current) ? IM_COL32(255,255,255,255) : kInkSoft);
        DrawIcon(dl, icons[i], ImVec2(startX + iconSz*0.5f, cy), iconSz, col, 2.2f);
        dl->AddText(ImVec2(startX + iconSz + 7.0f, cy - ls.y*0.5f), col, labels[i]);
    }
    ImGui::PopID();
    return result;
}
bool ControlTile(Icon ic, const char* label, bool* v, float size) {
    ImGui::PushID(label);
    ImVec2 cur = ImGui::GetCursorScreenPos();
    ImGui::InvisibleButton("##tile", ImVec2(size, size));
    bool hov = ImGui::IsItemHovered(), act = ImGui::IsItemActive();
    if (ImGui::IsItemActivated()) PlaySfx(Sfx::SwitchTick);
    bool clk = ImGui::IsItemDeactivated() && hov; if (clk) *v = !*v;
    ImGuiID id = ImGui::GetID("##tile");
    Spring& k  = g->springs().Get((uint32_t)id, 2, SpringStyle::Bouncy, *v ? 1.0f : 0.0f); k.target = *v ? 1.0f : 0.0f; k.Tick(Dt());
    Spring& sc = g->springs().Get((uint32_t)id, 1, SpringStyle::Bouncy, 1.0f); sc.target = act ? 0.94f : (hov ? 1.06f : 1.0f); sc.Tick(Dt());
    float kc = Clampf(k.x, 0.0f, 1.0f);
    ImVec2 c(cur.x + size*0.5f, cur.y + size*0.5f);
    float gel = 1.0f - sc.x;
    float hw = size*0.5f*sc.x*(1.0f + gel*0.6f);
    float hh = size*0.5f*sc.x*(1.0f - gel*0.5f);
    { Primitive bg; bg.cx = c.x; bg.cy = c.y; bg.hw = hw; bg.hh = hh; bg.corner_radius = 18.0f; bg.fade = 1.0f; bg.material = Material::Thin; g->Submit(bg); }
    { Primitive on; on.cx = c.x; on.cy = c.y; on.hw = hw; on.hh = hh; on.corner_radius = 18.0f; on.fade = kc; on.material = Material::Accent; g->Submit(on); }
    ImDrawList* dl = ImGui::GetWindowDrawList();
    DrawIcon(dl, ic, ImVec2(cur.x + 22.0f, cur.y + 22.0f), 22.0f, SmoothCol((uint32_t)id, 20, kc > 0.5f ? IM_COL32(255,255,255,255) : kInk), 2.3f);
    ImVec2 ls = ImGui::CalcTextSize(label);
    dl->AddText(ImVec2(cur.x + 16.0f, cur.y + size - ls.y - 14.0f), SmoothCol((uint32_t)id, 24, kc > 0.5f ? IM_COL32(255,255,255,255) : kInkSoft), label);
    ImGui::PopID();
    return clk;
}
int DrumPicker(const char* id, const char* const* items, int count, int current, ImVec2 size) {
    if (count <= 0) return current;
    ImGui::PushID(id);
    ImVec2 cur = ImGui::GetCursorScreenPos();
    ImGui::InvisibleButton("##drum", size);
    bool active = ImGui::IsItemActive();
    ImGuiStorage* st = ImGui::GetStateStorage();
    ImGuiID ofk = ImGui::GetID("off");
    float off = st->GetFloat(ofk, (float)current);
    const float rowh = 34.0f;
    if (active) off -= ImGui::GetIO().MouseDelta.y / rowh;
    else { float tgt = (float)(int)(off + (off >= 0 ? 0.5f : -0.5f)); off += (tgt - off) * Clampf(Dt()*12.0f, 0.0f, 1.0f); }
    off = Clampf(off, 0.0f, (float)(count - 1));
    st->SetFloat(ofk, off);
    int sel = (int)(off + 0.5f); if (sel < 0) sel = 0; if (sel >= count) sel = count - 1;
    ImVec2 c(cur.x + size.x*0.5f, cur.y + size.y*0.5f);
    { Primitive bg; bg.cx = c.x; bg.cy = c.y; bg.hw = size.x*0.5f; bg.hh = size.y*0.5f; bg.corner_radius = 14.0f; bg.fade = 1.0f; bg.material = Material::Thin; g->Submit(bg); }
    { Primitive band; band.cx = c.x; band.cy = c.y; band.hw = size.x*0.5f - 6.0f; band.hh = rowh*0.5f; band.corner_radius = 9.0f; band.fade = 1.0f; band.material = Material::Accent; g->Submit(band); }
    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImGui::PushClipRect(cur, ImVec2(cur.x + size.x, cur.y + size.y), true);
    int span = (int)(size.y / rowh / 2.0f) + 1;
    int lo = (int)off - span, hi = (int)off + span + 1;
    for (int i = lo; i <= hi; ++i) {
        if (i < 0 || i >= count) continue;
        float y = c.y + (i - off) * rowh;
        float dist = fabsf(i - off);
        float al = Clampf(1.0f - dist * 0.32f, 0.15f, 1.0f);
        bool oncenter = (i == sel);
        ImVec2 ts = ImGui::CalcTextSize(items[i]);
        ImU32 col = oncenter ? SubAlpha(IM_COL32(255,255,255,255), al) : SubAlpha(kInk, al);
        dl->AddText(ImVec2(c.x - ts.x*0.5f, y - ts.y*0.5f), col, items[i]);
    }
    ImGui::PopClipRect();
    ImGui::PopID();
    return sel;
}
void DynamicIsland(Icon ic, const char* title, const char* trailing, bool expanded, float center_x, float top_y, float max_w) {
    ImGuiID id = ImGui::GetID("##island");
    Spring& e = g->springs().Get((uint32_t)id, 30, SpringStyle::Bouncy, expanded ? 1.0f : 0.0f);
    e.target = expanded ? 1.0f : 0.0f; e.Tick(Dt());
    float t = Clampf(e.x, 0.0f, 1.0f);
    float wC = 156.0f, wE = std::min(360.0f, max_w - 20.0f), hC = 34.0f, hE = 60.0f;
    float w = wC + (wE - wC)*t, h = hC + (hE - hC)*t;
    float cx = center_x, cy = top_y + h*0.5f;
    { Primitive p; p.cx = cx; p.cy = cy; p.hw = w*0.5f; p.hh = h*0.5f; p.corner_radius = h*0.5f; p.fade = 1.0f; p.material = Material::Thick; g->SubmitOverlay(p); }
    ImDrawList* dl = ImGui::GetForegroundDrawList();
    float left = cx - w*0.5f;
    DrawIcon(dl, ic, ImVec2(left + 22.0f, cy), 18.0f + 4.0f*t, AccentCol(), 2.4f);
    if (title && *title) dl->AddText(ImVec2(left + 42.0f, cy - ImGui::GetFontSize()*0.5f), kInk, title);
    if (trailing && *trailing) {
        ImVec2 ts = ImGui::CalcTextSize(trailing);
        dl->AddText(ImVec2(cx + w*0.5f - ts.x - 20.0f, cy - ts.y*0.5f), SubAlpha(kInkSoft, 0.4f + 0.6f*t), trailing);
    }
}
bool Scrubber(const char* id, float* v, float vmin, float vmax, float width) {
    ImGui::PushID(id);
    const float h = 28.0f, tr = 6.0f;
    float w = width > 0.0f ? width : ImGui::GetContentRegionAvail().x;
    if (width <= 0.0f && w > 360.0f) w = 360.0f;
    ImVec2 cur = ImGui::GetCursorScreenPos();
    ImGui::InvisibleButton("##scrub", ImVec2(w, h));
    bool active = ImGui::IsItemActive(), hov = ImGui::IsItemHovered();
    if (ImGui::IsItemActivated()) PlaySfx(Sfx::ScrubGrab);
    float t = (*v - vmin) / (vmax - vmin);
    if (active) { float mx = ImGui::GetIO().MousePos.x; t = Clampf((mx - cur.x) / std::max(1.0f, w), 0.0f, 1.0f); *v = vmin + t * (vmax - vmin); }
    ImGuiStorage* st = ImGui::GetStateStorage();
    ImGuiID tk = ImGui::GetID("ts");
    float ts2 = st->GetFloat(tk, t); ts2 += (t - ts2) * Clampf(Dt()*20.0f, 0.0f, 1.0f); st->SetFloat(tk, ts2);
    float cy = cur.y + h*0.5f, fx = cur.x + w*ts2;
    { Primitive trk; trk.cx = cur.x + w*0.5f; trk.cy = cy; trk.hw = w*0.5f; trk.hh = tr*0.5f; trk.corner_radius = tr*0.5f; trk.fade = 1.0f; trk.material = Material::Thin; g->Submit(trk); }
    if (fx > cur.x + tr) { Primitive fl; fl.cx = (cur.x + fx)*0.5f; fl.cy = cy; fl.hw = (fx - cur.x)*0.5f; fl.hh = tr*0.5f; fl.corner_radius = tr*0.5f; fl.fade = 1.0f; fl.material = Material::Accent; g->Submit(fl); }
    ImGuiID gid = ImGui::GetID("g");
    Spring& grow = g->springs().Get((uint32_t)gid, 4, SpringStyle::Bouncy, 1.0f);
    grow.target = active ? 1.55f : (hov ? 1.2f : 1.0f); grow.Tick(Dt());
    { Primitive th; th.cx = fx; th.cy = cy; th.hw = 3.0f; th.hh = (h*0.5f - 2.0f) * grow.x; th.corner_radius = 3.0f; th.fade = 1.0f; th.material = Material::Knob; g->Submit(th); }
    ImGui::PopID();
    return active;
}
void Skeleton(const char* id, ImVec2 size) {
    ImGui::PushID(id);
    ImVec2 cur = ImGui::GetCursorScreenPos();
    ImGui::Dummy(size);
    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImVec2 mx(cur.x + size.x, cur.y + size.y);
    dl->AddRectFilled(cur, mx, IM_COL32(255,255,255,16), 8.0f);
    float t = fmodf((float)ImGui::GetTime() * 0.6f, 1.4f) / 1.4f;
    float x = cur.x - 60.0f + (size.x + 120.0f) * t;
    dl->PushClipRect(cur, mx, true);
    ImU32 c0 = IM_COL32(255,255,255,0), c1 = IM_COL32(255,255,255,42);
    dl->AddRectFilledMultiColor(ImVec2(x-55,cur.y), ImVec2(x,cur.y+size.y), c0, c1, c1, c0);
    dl->AddRectFilledMultiColor(ImVec2(x,cur.y), ImVec2(x+55,cur.y+size.y), c1, c0, c0, c1);
    dl->PopClipRect();
    ImGui::PopID();
}
void AttentionRing(const char* id, ImVec2 center, float radius) {
    (void)id;
    ImDrawList* dl = ImGui::GetWindowDrawList();
    float t = fmodf((float)ImGui::GetTime(), 1.2f) / 1.2f;
    float r = radius * (1.0f + t * 0.9f);
    int a = (int)(170 * (1.0f - t));
    dl->AddCircle(center, r, (AccentCol() & 0x00FFFFFFu) | ((ImU32)a << 24), 48, 2.6f);
}
void LevelIndicator(float frac, int segments, ImVec2 size) {
    if (segments < 1) segments = 1;
    frac = Clampf(frac, 0.0f, 1.0f);
    ImVec2 cur = ImGui::GetCursorScreenPos();
    ImGui::Dummy(size);
    ImDrawList* dl = ImGui::GetWindowDrawList();
    float gap = 3.0f, sw = (size.x - gap * (segments - 1)) / (float)segments;
    int filled = (int)(frac * segments + 0.5f);
    ImU32 acc = AccentCol();
    for (int i = 0; i < segments; ++i) {
        float x = cur.x + i * (sw + gap);
        ImU32 c = (i < filled) ? acc : IM_COL32(255,255,255,38);
        dl->AddRectFilled(ImVec2(x, cur.y), ImVec2(x + sw, cur.y + size.y), c, 3.0f);
    }
}
int PageDots(const char* id, int count, int current) {
    ImGui::PushID(id);
    const float dot = 7.0f, gap = 8.0f, activeW = 18.0f, h = dot + 6.0f;
    float w = count * dot + (count - 1) * gap + (activeW - dot);
    ImVec2 cur = ImGui::GetCursorScreenPos();
    ImGui::Dummy(ImVec2(w, h));
    ImDrawList* dl = ImGui::GetWindowDrawList();
    int result = current;
    float x = cur.x, cy = cur.y + h * 0.5f;
    for (int i = 0; i < count; ++i) {
        ImGui::PushID(i);
        ImGuiStorage* st = ImGui::GetStateStorage();
        ImGuiID wk = ImGui::GetID("w");
        float target = (i == current) ? activeW : dot;
        float dw = st->GetFloat(wk, target); dw += (target - dw) * Clampf(Dt()*16.0f, 0.0f, 1.0f); st->SetFloat(wk, dw);
        ImGui::SetCursorScreenPos(ImVec2(x, cur.y));
        ImGui::InvisibleButton("##d", ImVec2(dw, h));
        if (ImGui::IsItemDeactivated() && ImGui::IsItemHovered()) result = i;
        ImU32 c = SmoothCol((uint32_t)ImGui::GetID("c"), 20, (i == current) ? AccentCol() : IM_COL32(255,255,255,70));
        dl->AddRectFilled(ImVec2(x, cy - dot*0.5f), ImVec2(x + dw, cy + dot*0.5f), c, dot*0.5f);
        x += dw + gap;
        ImGui::PopID();
    }
    ImGui::PopID();
    return result;
}
void ActivityRings(const char* id, const float* fracs, const ImU32* cols, int count, float radius) {
    ImGui::PushID(id);
    ImVec2 cur = ImGui::GetCursorScreenPos();
    ImGui::Dummy(ImVec2(radius * 2.0f, radius * 2.0f));
    ImVec2 c(cur.x + radius, cur.y + radius);
    ImDrawList* dl = ImGui::GetWindowDrawList();
    float thick = radius * 0.16f;
    for (int i = 0; i < count; ++i) {
        float rr = radius - thick * 0.5f - i * (thick + 3.0f);
        if (rr < thick) break;
        dl->AddCircle(c, rr, (cols[i] & 0x00FFFFFFu) | ((ImU32)55 << 24), 64, thick);
        float f = Clampf(fracs[i], 0.0f, 1.0f);
        if (f > 0.001f) {
            dl->PathArcTo(c, rr, -kPI * 0.5f, -kPI * 0.5f + f * 2.0f * kPI, 80);
            dl->PathStroke(cols[i], 0, thick);
        }
    }
    ImGui::PopID();
}
float Gauge(const char* id, float v, float vmin, float vmax, float radius, const char* fmt) {
    ImGui::PushID(id);
    ImVec2 cur = ImGui::GetCursorScreenPos();
    ImGui::Dummy(ImVec2(radius * 2.0f, radius * 1.5f));
    ImVec2 c(cur.x + radius, cur.y + radius);
    ImDrawList* dl = ImGui::GetWindowDrawList();
    const float a0 = kPI * 0.75f, a1 = kPI * 2.25f;
    float t = Clampf((v - vmin) / (vmax - vmin), 0.0f, 1.0f);
    float rr = radius * 0.82f;
    dl->PathArcTo(c, rr, a0, a1, 80); dl->PathStroke(IM_COL32(255,255,255,40), 0, 6.0f);
    dl->PathArcTo(c, rr, a0, a0 + (a1 - a0) * t, 80); dl->PathStroke(AccentCol(), 0, 6.0f);
    float na = a0 + (a1 - a0) * t;
    dl->AddLine(c, ImVec2(c.x + cosf(na) * rr * 0.82f, c.y + sinf(na) * rr * 0.82f), kInk, 2.6f);
    dl->AddCircleFilled(c, 4.0f, kInk);
    char b[32]; std::snprintf(b, sizeof(b), fmt, v);
    ImVec2 ts = ImGui::CalcTextSize(b);
    dl->AddText(ImVec2(c.x - ts.x * 0.5f, c.y + rr * 0.45f), kInk, b);
    ImGui::PopID();
    return v;
}
bool MorphButton(const char* id, MorphIcon kind, bool* state, float size) {
    ImGui::PushID(id);
    ImVec2 cur = ImGui::GetCursorScreenPos();
    ImGui::InvisibleButton("##mb", ImVec2(size, size));
    bool hov = ImGui::IsItemHovered(), act = ImGui::IsItemActive();
    if (ImGui::IsItemActivated()) PlaySfx(kind==MorphIcon::PlayPause ? (*state ? Sfx::Pause : Sfx::Play) : Sfx::IconTap);
    bool clk = ImGui::IsItemDeactivated() && hov; if (clk) *state = !*state;
    ImGuiID bid = ImGui::GetID("##mb");
    Spring& m  = g->springs().Get((uint32_t)bid, 31, SpringStyle::Bouncy, *state ? 1.0f : 0.0f); m.target = *state ? 1.0f : 0.0f; m.Tick(Dt());
    Spring& sc = g->springs().Get((uint32_t)bid, 1, SpringStyle::Bouncy, 1.0f); sc.target = act ? 0.9f : (hov ? 1.06f : 1.0f); sc.Tick(Dt());
    float t = Clampf(m.x, 0.0f, 1.0f);
    ImVec2 c(cur.x + size*0.5f, cur.y + size*0.5f); float r = size*0.5f*sc.x;
    { Primitive bg; bg.cx = c.x; bg.cy = c.y; bg.hw = r; bg.hh = r; bg.corner_radius = r; bg.fade = 1.0f; bg.material = hov ? Material::Accent : Material::Thin; g->Submit(bg); }
    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImU32 col = SmoothCol((uint32_t)bid, 20, hov ? IM_COL32(255,255,255,255) : kInk);
    float s = size * 0.24f;
    if (kind == MorphIcon::PlusClose) {
        float ang = t * kPI * 0.25f, ca = cosf(ang), sa = sinf(ang);
        auto R = [&](float ox, float oy) { return ImVec2(c.x + ox*ca - oy*sa, c.y + ox*sa + oy*ca); };
        dl->AddLine(R(-s,0), R(s,0), col, 2.6f); dl->AddLine(R(0,-s), R(0,s), col, 2.6f);
    } else if (kind == MorphIcon::PlayPause) {
        ImU32 cP = SubAlpha(col, 1.0f - t);
        dl->AddTriangleFilled(ImVec2(c.x-s*0.7f,c.y-s), ImVec2(c.x-s*0.7f,c.y+s), ImVec2(c.x+s,c.y), cP);
        ImU32 cB = SubAlpha(col, t);
        dl->AddRectFilled(ImVec2(c.x-s*0.6f,c.y-s), ImVec2(c.x-s*0.2f,c.y+s), cB, 1.5f);
        dl->AddRectFilled(ImVec2(c.x+s*0.2f,c.y-s), ImVec2(c.x+s*0.6f,c.y+s), cB, 1.5f);
    } else {
        ImU32 cM = SubAlpha(col, 1.0f - t);
        for (int k = -1; k <= 1; ++k) dl->AddLine(ImVec2(c.x-s, c.y+k*s*0.66f), ImVec2(c.x+s, c.y+k*s*0.66f), cM, 2.4f);
        ImU32 cX = SubAlpha(col, t);
        dl->AddLine(ImVec2(c.x-s,c.y-s), ImVec2(c.x+s,c.y+s), cX, 2.6f);
        dl->AddLine(ImVec2(c.x-s,c.y+s), ImVec2(c.x+s,c.y-s), cX, 2.6f);
    }
    ImGui::PopID();
    return clk;
}
void Odometer(const char* id, int value, float fontScale) {
    ImGui::PushID(id);
    ImGuiStorage* st = ImGui::GetStateStorage(); ImGuiID k = ImGui::GetID("v");
    float disp = st->GetFloat(k, (float)value);
    disp += ((float)value - disp) * Clampf(Dt() * 7.0f, 0.0f, 1.0f);
    if (fabsf(disp - (float)value) < 0.45f) disp = (float)value;
    st->SetFloat(k, disp);
    char b[32]; std::snprintf(b, sizeof(b), "%d", (int)(disp + 0.5f));
    float fs = ImGui::GetFontSize() * fontScale;
    ImVec2 cur = ImGui::GetCursorScreenPos();
    ImVec2 ts = ImGui::CalcTextSize(b); float wpx = ts.x * fontScale;
    ImGui::Dummy(ImVec2(wpx, fs));
    ImGui::GetWindowDrawList()->AddText(ImGui::GetFont(), fs, cur, kInk, b);
    ImGui::PopID();
}
void SuccessHUD(const char* id, bool trigger) {
    ImGui::PushID(id);
    ImGuiStorage* st = ImGui::GetStateStorage(); ImGuiID k = ImGui::GetID("t");
    float t = st->GetFloat(k, 2.0f);
    if (trigger) { t = 0.0f; PlaySfx(Sfx::Success); }
    if (t < 1.5f) t += Dt() * 1.5f;
    st->SetFloat(k, t);
    if (t < 1.3f) {
        ImVec2 wmin = ImGui::GetWindowPos(), wsz = ImGui::GetWindowSize();
        ImVec2 c(wmin.x + wsz.x*0.5f, wmin.y + wsz.y*0.5f);
        float a = (t < 0.2f) ? (t/0.2f) : (t > 1.0f ? (1.3f - t)/0.3f : 1.0f); a = Clampf(a, 0.0f, 1.0f);
        ImDrawList* dl = ImGui::GetForegroundDrawList();
        float pe = Clampf(t/0.28f, 0.0f, 1.0f);
        const float c1 = 1.70158f, c3 = c1 + 1.0f, u = pe - 1.0f;
        float pop = 0.55f + 0.45f * (1.0f + c3*u*u*u + c1*u*u);
        dl->AddCircleFilled(c, 46.0f*pop, SubAlpha(AccentCol(), a*0.92f));
        float p = Clampf((t - 0.15f) / 0.4f, 0.0f, 1.0f);
        ImVec2 p0(c.x-16, c.y+2), p1(c.x-5, c.y+13), p2(c.x+18, c.y-14);
        ImU32 w = SubAlpha(IM_COL32(255,255,255,255), a);
        if (p > 0.0f) { float pp = Clampf(p*2.0f,0,1); dl->AddLine(p0, ImVec2(p0.x+(p1.x-p0.x)*pp, p0.y+(p1.y-p0.y)*pp), w, 4.0f); }
        if (p > 0.5f) { float pp = (p-0.5f)*2.0f; dl->AddLine(p1, ImVec2(p1.x+(p2.x-p1.x)*pp, p1.y+(p2.y-p1.y)*pp), w, 4.0f); }
    }
    ImGui::PopID();
}
bool TokenField(const char* id, char tokens[][24], int* count, int maxTokens, char* input, int inputLen) {
    ImGui::PushID(id);
    bool changed = false;
    for (int i = 0; i < *count; ++i) {
        ImGui::PushID(i);
        if (Glass::Chip(tokens[i], true)) {
            for (int j = i; j < *count - 1; ++j) SafeCopy(tokens[j], tokens[j+1], 24);
            (*count)--; changed = true; ImGui::PopID(); break;
        }
        ImGui::SameLine(); ImGui::PopID();
    }
    ImGui::SetNextItemWidth(130.0f);
    if (ImGui::InputText("##tok", input, inputLen, ImGuiInputTextFlags_EnterReturnsTrue)) {
        if (input[0] && *count < maxTokens) { SafeCopy(tokens[*count], input, 24); (*count)++; input[0] = 0; changed = true; }
    }
    ImGui::PopID();
    return changed;
}
bool ComboBox(const char* label, char* buf, int buflen, const char* const* items, int count) {
    ImGui::PushID(label);
    bool changed = Glass::TextField(label, buf, buflen, "Type or pick\xE2\x80\xA6");
    int cur = -1;
    for (int i = 0; i < count; ++i) if (std::strcmp(buf, items[i]) == 0) { cur = i; break; }
    if (Glass::Dropdown("##pick", items, count, &cur)) { if (cur >= 0) { SafeCopy(buf, items[cur], buflen); changed = true; } }
    ImGui::PopID();
    return changed;
}
static int CalLeap(int y) { return ((y % 4 == 0 && y % 100 != 0) || y % 400 == 0) ? 1 : 0; }
static int CalDaysInMonth(int m, int y) {
    static const int d[] = { 31,28,31,30,31,30,31,31,30,31,30,31 };
    return (m == 2) ? 28 + CalLeap(y) : d[(m - 1) % 12];
}
static int CalDow(int y, int m, int d) {
    static const int t[] = { 0,3,2,5,0,3,5,1,4,6,2,4 };
    if (m < 3) y -= 1;
    return (y + y/4 - y/100 + y/400 + t[m-1] + d) % 7;
}
bool Calendar(const char* id, int* day, int* month, int* year) {
    ImGui::PushID(id);
    bool changed = false;
    float w = ImGui::GetContentRegionAvail().x; if (w > 300.0f) w = 300.0f;
    ImVec2 cur = ImGui::GetCursorScreenPos();
    ImDrawList* dl = ImGui::GetWindowDrawList();
    const float headH = 34.0f, cw = w / 7.0f, cellH = 30.0f;
    ImGui::SetCursorScreenPos(cur); ImGui::InvisibleButton("##prev", ImVec2(34, headH));
    bool prevHov = ImGui::IsItemHovered(); ImGuiID prevId = ImGui::GetID("##prev");
    if (ImGui::IsItemDeactivated() && prevHov) { (*month)--; if (*month < 1) { *month = 12; (*year)--; } changed = true; }
    {
      Spring& pn = g->springs().Get((uint32_t)prevId, 20, SpringStyle::Critical, 0.0f); pn.target = prevHov ? 1.0f : 0.0f; pn.Tick(Dt());
      ImVec2 c2(cur.x + 17 - pn.x*3.0f, cur.y + headH*0.5f);
      ImU32 pc = SmoothCol((uint32_t)prevId, 24, prevHov ? kInk : kInkSoft);
      dl->AddLine(ImVec2(c2.x+3, c2.y-5), ImVec2(c2.x-3, c2.y), pc, 2.0f);
      dl->AddLine(ImVec2(c2.x+3, c2.y+5), ImVec2(c2.x-3, c2.y), pc, 2.0f); }
    ImGui::SetCursorScreenPos(ImVec2(cur.x + w - 34, cur.y)); ImGui::InvisibleButton("##next", ImVec2(34, headH));
    bool nextHov = ImGui::IsItemHovered(); ImGuiID nextId = ImGui::GetID("##next");
    if (ImGui::IsItemDeactivated() && nextHov) { (*month)++; if (*month > 12) { *month = 1; (*year)++; } changed = true; }
    {
      Spring& nn = g->springs().Get((uint32_t)nextId, 20, SpringStyle::Critical, 0.0f); nn.target = nextHov ? 1.0f : 0.0f; nn.Tick(Dt());
      DrawIcon(dl, Icon::ChevronR, ImVec2(cur.x + w - 17 + nn.x*3.0f, cur.y + headH*0.5f), 13.0f, SmoothCol((uint32_t)nextId, 24, nextHov ? kInk : kInkSoft), 2.0f); }
    static const char* mn[] = { "January","February","March","April","May","June","July","August","September","October","November","December" };
    char tt[40]; std::snprintf(tt, sizeof(tt), "%s %d", mn[(*month-1)%12], *year);
    ImVec2 tts = ImGui::CalcTextSize(tt);
    dl->AddText(ImVec2(cur.x + (w - tts.x)*0.5f, cur.y + (headH - tts.y)*0.5f), kInk, tt);
    float gy = cur.y + headH + 2.0f;
    static const char* wd[] = { "S","M","T","W","T","F","S" };
    for (int i = 0; i < 7; ++i) { ImVec2 ws = ImGui::CalcTextSize(wd[i]); dl->AddText(ImVec2(cur.x + i*cw + (cw-ws.x)*0.5f, gy), kInkSoft, wd[i]); }
    float gridY = gy + 20.0f;
    int dim = CalDaysInMonth(*month, *year), fw = CalDow(*year, *month, 1);
    int rows = (fw + dim + 6) / 7;
    for (int dd = 1; dd <= dim; ++dd) {
        int idx = fw + dd - 1, row = idx / 7, colc = idx % 7;
        float cx = cur.x + colc*cw, cyy = gridY + row*cellH;
        ImGui::PushID(dd); ImGui::SetCursorScreenPos(ImVec2(cx, cyy)); ImGui::InvisibleButton("##day", ImVec2(cw, cellH));
        bool hov = ImGui::IsItemHovered(), act = ImGui::IsItemActive(); ImGuiID dayId = ImGui::GetID("##day");
        if (ImGui::IsItemDeactivated() && hov) { *day = dd; changed = true; } ImGui::PopID();
        bool sel = (dd == *day);
        ImVec2 cc(cx + cw*0.5f, cyy + cellH*0.5f);
        Spring& dsc = g->springs().Get((uint32_t)dayId, 20, SpringStyle::Critical, 1.0f);
        dsc.target = act ? 0.96f : (hov ? 1.06f : 1.0f); dsc.Tick(Dt());
        Spring& dhl = g->springs().Get((uint32_t)dayId, 21, SpringStyle::Critical, 0.0f);
        dhl.target = (hov && !sel) ? 1.0f : 0.0f; dhl.Tick(Dt());
        if (dhl.x > 0.01f) { Primitive ph; ph.cx = cc.x; ph.cy = cc.y; ph.hw = 14.0f*dsc.x; ph.hh = 14.0f*dsc.x; ph.corner_radius = 14.0f; ph.fade = dhl.x*0.5f; ph.material = Material::Thin; g->Submit(ph); }
        if (sel) { Primitive p; p.cx = cc.x; p.cy = cc.y; p.hw = 14.0f*dsc.x; p.hh = 14.0f*dsc.x; p.corner_radius = 14.0f; p.fade = 1.0f; p.elevate = act ? 0.7f : (hov ? 1.25f : 1.0f); p.material = Material::Accent; g->Submit(p); }
        char db[8]; std::snprintf(db, sizeof(db), "%d", dd); ImVec2 ds = ImGui::CalcTextSize(db);
        dl->AddText(ImVec2(cc.x - ds.x*0.5f, cc.y - ds.y*0.5f), SmoothCol((uint32_t)dayId, 24, (sel || hov) ? IM_COL32(255,255,255,255) : kInk), db);
    }
    ImGui::SetCursorScreenPos(cur);
    ImGui::Dummy(ImVec2(w, headH + 22.0f + rows*cellH + 4.0f));
    ImGui::PopID();
    return changed;
}
int OutlineTree(const char* id, const char* const* labels, const int* depth, const Icon* icons, int count, int current) {
    ImGui::PushID(id);
    ImGuiStorage* st = ImGui::GetStateStorage();
    int result = current;
    float w = ImGui::GetContentRegionAvail().x; if (w > 300.0f) w = 300.0f;
    ImDrawList* dl = ImGui::GetWindowDrawList();
    const float rowh = 30.0f;
    int i = 0;
    while (i < count) {
        int d = depth[i];
        bool hasChild = (i + 1 < count) && depth[i + 1] > d;
        ImGuiID ok = ImGui::GetID(i + 1000);
        bool open = st->GetBool(ok, true);
        ImVec2 cur = ImGui::GetCursorScreenPos();
        ImGui::PushID(i); ImGui::InvisibleButton("##row", ImVec2(w, rowh));
        bool hov = ImGui::IsItemHovered(), act = ImGui::IsItemActive(); bool clk = ImGui::IsItemDeactivated() && hov; ImGui::PopID();
        if (clk) { if (hasChild) { open = !open; st->SetBool(ok, open); } result = i; }
        bool sel = (i == current);
        Spring& hl = g->springs().Get((uint32_t)ok, 20, SpringStyle::Critical, 0.0f); hl.target = (hov && !sel) ? 1.0f : 0.0f; hl.Tick(Dt());
        if (hl.x > 0.01f) { Primitive ph; ph.cx = cur.x + w*0.5f; ph.cy = cur.y + rowh*0.5f; ph.hw = w*0.5f; ph.hh = rowh*0.5f - 1.0f; ph.corner_radius = 9.0f; ph.fade = hl.x*0.4f; ph.material = Material::Thin; g->Submit(ph); }
        if (sel) { Primitive p; p.cx = cur.x + w*0.5f; p.cy = cur.y + rowh*0.5f; p.hw = w*0.5f; p.hh = rowh*0.5f - 1.0f; p.corner_radius = 9.0f; p.fade = 1.0f; p.elevate = act ? 0.7f : (hov ? 1.25f : 1.0f); p.material = Material::Accent; g->Submit(p); }
        Spring& nx = g->springs().Get((uint32_t)ok, 21, SpringStyle::Critical, 0.0f); nx.target = hov ? 1.0f : 0.0f; nx.Tick(Dt());
        float x = cur.x + 8.0f + d * 16.0f + nx.x*3.0f, cy = cur.y + rowh*0.5f;
        ImU32 col = SmoothCol((uint32_t)ok, 28, (sel || hov) ? IM_COL32(255,255,255,255) : kInk);
        if (hasChild) {
            Spring& ch = g->springs().Get((uint32_t)ok, 24, SpringStyle::Critical, open ? 1.0f : 0.0f); ch.target = open ? 1.0f : 0.0f; ch.Tick(Dt());
            float ang = ch.x * (kPI*0.5f), ca = cosf(ang), sa = sinf(ang); ImVec2 cc(x + 5, cy);
            auto R = [&](float ox, float oy) { return ImVec2(cc.x + ox*ca - oy*sa, cc.y + ox*sa + oy*ca); };
            dl->AddLine(R(-3,-4), R(3,0), col, 2.0f); dl->AddLine(R(-3,4), R(3,0), col, 2.0f);
            x += 16.0f;
        } else x += 6.0f;
        if (icons[i] != Icon::None) { DrawIcon(dl, icons[i], ImVec2(x + 9, cy), 15.0f, col, 2.0f); x += 24.0f; }
        ImVec2 ls = ImGui::CalcTextSize(labels[i]);
        dl->AddText(ImVec2(x, cy - ls.y*0.5f), col, labels[i]);
        i++;
        if (!open && hasChild) while (i < count && depth[i] > d) i++;
    }
    ImGui::PopID();
    return result;
}
