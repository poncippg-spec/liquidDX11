static bool    g_cp_open  = false;
static float*  g_cp_rgb   = nullptr;

bool ColorButton(const char* label, float rgb[3]) {
    ImGui::PushID(label);
    const float rowh = 46.0f;
    float w = ImGui::GetContentRegionAvail().x;
    ImVec2 cur = ImGui::GetCursorScreenPos();
    ImGui::InvisibleButton("##cprow", ImVec2(w, rowh));
    bool hov = ImGui::IsItemHovered();
    if (ImGui::IsItemDeactivated() && hov) { g_cp_open = true; g_cp_rgb = rgb; }

    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImVec2 lts = ImGui::CalcTextSize(label);
    const float sw = 58, sh = 30;
    ImVec2 swp(cur.x + 2, cur.y + (rowh - sh) * 0.5f);
    Primitive bd; bd.cx = swp.x + sw*0.5f; bd.cy = swp.y + sh*0.5f; bd.hw = sw*0.5f; bd.hh = sh*0.5f;
    bd.corner_radius = 9.0f; bd.fade = 1.0f; bd.material = Material::Thin; bd.elevate = hov ? 1.15f : 1.0f;
    g->Submit(bd);
    dl->AddRectFilled(ImVec2(swp.x+5, swp.y+5), ImVec2(swp.x+sw-5, swp.y+sh-5),
                      IM_COL32((int)(rgb[0]*255),(int)(rgb[1]*255),(int)(rgb[2]*255),255), 6.0f);
    dl->AddText(ImVec2(swp.x + sw + 12, cur.y + (rowh - lts.y) * 0.5f), kInk, label);
    ImGui::PopID();
    return false;
}

void DrawColorPickerModal() {
    if (!g_cp_rgb) return;
    float* rgb = g_cp_rgb;
    ImVec2 wmin = ImGui::GetWindowPos(); ImVec2 wsz = ImGui::GetWindowSize();
    ImGuiID mid = ImGui::GetID("##cpmodal");
    Spring& sp = g->springs().Get((uint32_t)mid, 13, SpringStyle::Bouncy, 0.0f);
    sp.stiffness = 200.0f; sp.damping = 17.0f;
    const bool live = g_cp_open;
    sp.target = live ? 1.0f : 0.0f; sp.Tick(Dt());
    float a = Clampf(sp.x, 0.0f, 1.0f);
    if (!live && a < 0.01f) { g_cp_rgb = nullptr; return; }
    const float TWO_PI = 2.0f * kPI;

    ImGui::SetNextWindowPos(wmin); ImGui::SetNextWindowSize(wsz); ImGui::SetNextWindowBgAlpha(0.0f);
    if (a < 0.92f) ImGui::SetNextWindowFocus();
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::Begin("##cpmodal_win", nullptr,
                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                 ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoBackground |
                 ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse);
    ImDrawList* dl = ImGui::GetWindowDrawList();

    ImGui::SetCursorScreenPos(wmin);
    ImGui::InvisibleButton("##cpdim", wsz, ImGuiButtonFlags_AllowOverlap);

    float pw = 320.0f, ph = pw * 4.0f/3.0f;

    ImVec2 pc(wmin.x + wsz.x*0.5f, wmin.y + wsz.y*0.5f - (1.0f - sp.x) * 50.0f);
    ImVec2 pmin(pc.x - pw*0.5f, pc.y - ph*0.5f);
    { Primitive bg; bg.cx = pc.x; bg.cy = pc.y; bg.hw = pw*0.5f; bg.hh = ph*0.5f;
      bg.corner_radius = 24.0f; bg.fade = a; bg.material = Material::Thick; g->SubmitOverlay(bg); }

    DrawUIBlurPanel(dl, pmin, ImVec2(pmin.x+pw, pmin.y+ph), 24.0f, a, IM_COL32(22,23,29,110));
    ImGui::SetCursorScreenPos(pmin);
    ImGui::InvisibleButton("##cppanel", ImVec2(pw, ph), ImGuiButtonFlags_AllowOverlap);

    const float pad = 20.0f;
    float hh, ss, vv; ImGui::ColorConvertRGBtoHSV(rgb[0], rgb[1], rgb[2], hh, ss, vv);
    bool changed = false;
    dl->AddText(ImVec2(pmin.x + pad, pmin.y + 15.0f), kInk, "Color");

    float vbx = pmin.x + pad, vbw = pw - pad*2.0f;

    float wheelD = 196.0f, R = wheelD * 0.5f;
    ImVec2 wc(pmin.x + pw*0.5f, pmin.y + 44.0f + R);
    ImGui::SetCursorScreenPos(ImVec2(wc.x - R, wc.y - R));
    ImGui::InvisibleButton("##wheel", ImVec2(wheelD, wheelD));
    bool wheelActive = ImGui::IsItemActive();
    if (wheelActive) { ImVec2 mp = ImGui::GetIO().MousePos; float dx = mp.x-wc.x, dy = mp.y-wc.y;
        float ang = atan2f(dy,dx); if (ang<0) ang += TWO_PI; hh = ang/TWO_PI; ss = Clampf(sqrtf(dx*dx+dy*dy)/R,0,1); changed = true; }
    const int NSEG = 80; ImVec2 uvw = ImGui::GetIO().Fonts->TexUvWhitePixel;
    ImU32 cen = IM_COL32((int)(vv*255),(int)(vv*255),(int)(vv*255),255);
    dl->PrimReserve(NSEG*3, NSEG*3);
    for (int s=0;s<NSEG;++s){ float a0=(s/(float)NSEG)*TWO_PI, a1=((s+1)/(float)NSEG)*TWO_PI; float r0,g0,b0,r1,g1,b1;
        ImGui::ColorConvertHSVtoRGB(s/(float)NSEG,1,vv,r0,g0,b0); ImGui::ColorConvertHSVtoRGB((s+1)/(float)NSEG,1,vv,r1,g1,b1);
        dl->PrimVtx(wc, uvw, cen);
        dl->PrimVtx(ImVec2(wc.x+cosf(a0)*R, wc.y+sinf(a0)*R), uvw, IM_COL32((int)(r0*255),(int)(g0*255),(int)(b0*255),255));
        dl->PrimVtx(ImVec2(wc.x+cosf(a1)*R, wc.y+sinf(a1)*R), uvw, IM_COL32((int)(r1*255),(int)(g1*255),(int)(b1*255),255)); }
    dl->AddCircle(wc, R+1.5f, IM_COL32(0,0,0,70), 80, 1.0f);
    dl->AddCircle(wc, R, IM_COL32(255,255,255,150), 80, 2.0f);
    ImVec2 wt(wc.x+cosf(hh*TWO_PI)*ss*R, wc.y+sinf(hh*TWO_PI)*ss*R);
    Spring& wsx = g->springs().Get((uint32_t)mid, 30, SpringStyle::Bouncy, wt.x); wsx.stiffness=220.0f; wsx.damping=20.0f; wsx.target=wt.x; wsx.Tick(Dt());
    Spring& wsy = g->springs().Get((uint32_t)mid, 31, SpringStyle::Bouncy, wt.y); wsy.stiffness=220.0f; wsy.damping=20.0f; wsy.target=wt.y; wsy.Tick(Dt());
    Spring& wgr = g->springs().Get((uint32_t)mid, 32, SpringStyle::Critical, 0.0f); wgr.target = wheelActive ? 1.0f : 0.0f; wgr.Tick(Dt());
    ImVec2 wts(wsx.x, wsy.x); float wtr = 7.0f + 2.5f*Clampf(wgr.x,0.0f,1.0f);
    dl->AddCircleFilled(wts, wtr, IM_COL32((int)(rgb[0]*255),(int)(rgb[1]*255),(int)(rgb[2]*255),255));
    dl->AddCircle(wts, wtr+1.0f, IM_COL32(255,255,255,255), 0, 2.5f); dl->AddCircle(wts, wtr+1.0f, IM_COL32(0,0,0,90), 0, 1.0f);

    float vby = wc.y + R + 16.0f;
    ImGui::SetCursorScreenPos(ImVec2(vbx, vby));
    ImGui::InvisibleButton("##val", ImVec2(vbw, 18.0f));
    bool valActive = ImGui::IsItemActive();
    if (valActive) { ImVec2 mp=ImGui::GetIO().MousePos; vv = Clampf((mp.x-vbx)/vbw,0,1); changed=true; }
    float fr,fg,fb; ImGui::ColorConvertHSVtoRGB(hh,ss,1.0f,fr,fg,fb);
    ImU32 fullc = IM_COL32((int)(fr*255),(int)(fg*255),(int)(fb*255),255);
    dl->AddRectFilledMultiColor(ImVec2(vbx,vby), ImVec2(vbx+vbw,vby+18), IM_COL32(0,0,0,255), fullc, fullc, IM_COL32(0,0,0,255));
    dl->AddRect(ImVec2(vbx-1.5f,vby-1.5f), ImVec2(vbx+vbw+1.5f,vby+19.5f), IM_COL32(0,0,0,70), 7.0f,0,1.0f);
    dl->AddRect(ImVec2(vbx,vby), ImVec2(vbx+vbw,vby+18), IM_COL32(255,255,255,150), 6,0,1.5f);
    float vx = vbx + vv*vbw;
    Spring& vsx = g->springs().Get((uint32_t)mid, 33, SpringStyle::Bouncy, vx); vsx.stiffness=220.0f; vsx.damping=20.0f; vsx.target=vx; vsx.Tick(Dt());
    Spring& vgr = g->springs().Get((uint32_t)mid, 34, SpringStyle::Critical, 0.0f); vgr.target = valActive ? 1.0f : 0.0f; vgr.Tick(Dt());
    float vgw = 3.0f + 1.5f*Clampf(vgr.x,0.0f,1.0f), vge = 3.0f*Clampf(vgr.x,0.0f,1.0f);
    dl->AddRectFilled(ImVec2(vsx.x-vgw,vby-3-vge), ImVec2(vsx.x+vgw,vby+21+vge), IM_COL32(255,255,255,255), 3.0f);
    dl->AddRect(ImVec2(vsx.x-vgw,vby-3-vge), ImVec2(vsx.x+vgw,vby+21+vge), IM_COL32(0,0,0,60), 3.0f,0,1.0f);

    if (changed) ImGui::ColorConvertHSVtoRGB(hh,ss,vv,rgb[0],rgb[1],rgb[2]);

    float infoY = vby + 32.0f;
    dl->AddRectFilled(ImVec2(vbx,infoY), ImVec2(vbx+44,infoY+30), IM_COL32((int)(rgb[0]*255),(int)(rgb[1]*255),(int)(rgb[2]*255),255), 9.0f);
    dl->AddRect(ImVec2(vbx,infoY), ImVec2(vbx+44,infoY+30), IM_COL32(255,255,255,50), 9,0,1.0f);
    char hex[16]; std::snprintf(hex,sizeof(hex),"#%02X%02X%02X",(int)(rgb[0]*255),(int)(rgb[1]*255),(int)(rgb[2]*255));
    dl->AddText(ImVec2(vbx+54, infoY+1), kInk, hex);
    char rgbtxt[40]; std::snprintf(rgbtxt,sizeof(rgbtxt),"R %d  G %d  B %d",(int)(rgb[0]*255),(int)(rgb[1]*255),(int)(rgb[2]*255));
    dl->AddText(ImVec2(vbx+54, infoY+16), kInkSoft, rgbtxt);
    ImVec2 cbp(vbx+vbw-58, infoY+2);
    ImGui::SetCursorScreenPos(cbp);
    ImGui::InvisibleButton("##copy", ImVec2(58,26));
    bool cph = ImGui::IsItemHovered();
    { Primitive cb; cb.cx=cbp.x+29; cb.cy=cbp.y+13; cb.hw=29; cb.hh=13; cb.corner_radius=8; cb.fade=1; cb.material=cph?Material::Accent:Material::Thin; g->SubmitOverlay(cb);
      const char* ct="Copy"; ImVec2 cts=ImGui::CalcTextSize(ct); dl->AddText(ImVec2(cbp.x+(58-cts.x)*0.5f, cbp.y+(26-cts.y)*0.5f), cph?IM_COL32(255,255,255,255):kInk, ct); }
    if (ImGui::IsItemDeactivated() && cph) ImGui::SetClipboardText(hex);

    float gridY = infoY + 42.0f;
    static const ImU32 pr[] = { IM_COL32(255,59,48,255),IM_COL32(255,149,0,255),IM_COL32(255,204,0,255),IM_COL32(52,199,89,255),IM_COL32(0,199,190,255),IM_COL32(0,122,255,255),IM_COL32(88,86,214,255),IM_COL32(175,82,222,255),IM_COL32(255,45,85,255),IM_COL32(142,142,147,255) };
    float chip=24.0f, cg=(vbw-10*chip)/9.0f;
    for (int i=0;i<10;i++){ ImGui::PushID(3000+i); ImVec2 pcc(vbx + i*(chip+cg), gridY); ImGui::SetCursorScreenPos(pcc); ImGui::InvisibleButton("##pp", ImVec2(chip,chip)); bool ph2=ImGui::IsItemHovered(); ImVec2 ctr(pcc.x+chip*0.5f,pcc.y+chip*0.5f); dl->AddCircleFilled(ctr, chip*0.5f*(ph2?1.12f:1.0f), pr[i]); if(ph2)dl->AddCircle(ctr,chip*0.5f+2.5f,IM_COL32(255,255,255,180),0,1.5f); if(ImGui::IsItemDeactivated()&&ph2){ImU32 cl=pr[i]; rgb[0]=(cl&0xFF)/255.f; rgb[1]=((cl>>8)&0xFF)/255.f; rgb[2]=((cl>>16)&0xFF)/255.f;} ImGui::PopID(); }

    ImVec2 dbp(vbx, pmin.y + ph - pad - 40.0f);
    ImGui::SetCursorScreenPos(dbp);
    ImGui::InvisibleButton("##done", ImVec2(vbw, 40));
    bool dc = ImGui::IsItemDeactivated() && ImGui::IsItemHovered();
    { Primitive db; db.cx=dbp.x+vbw*0.5f; db.cy=dbp.y+20; db.hw=vbw*0.5f; db.hh=20; db.corner_radius=13; db.fade=1; db.material=Material::Accent; g->SubmitOverlay(db);
      const char* dt="Done"; ImVec2 dts=ImGui::CalcTextSize(dt); dl->AddText(ImVec2(dbp.x+(vbw-dts.x)*0.5f, dbp.y+(40-dts.y)*0.5f), IM_COL32(255,255,255,255), dt); }

    ImGuiIO& cio = ImGui::GetIO();
    bool inPanel = cio.MousePos.x >= pmin.x && cio.MousePos.x <= pmin.x + pw &&
                   cio.MousePos.y >= pmin.y && cio.MousePos.y <= pmin.y + ph;
    if (live && (dc || ImGui::IsKeyPressed(ImGuiKey_Escape) || (cio.MouseClicked[0] && !inPanel))) {
        g_cp_open = false;
    }
    ImGui::End();
    ImGui::PopStyleVar();
}

ProfileAction ProfileMenu(const char* id, const char* name, const char* email,
                          const char* initials, ImU32 tint,
                          float x, float y, float w,
                          int* status, int* theme, float master, bool active) {
    ImGui::PushID(id);
    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImGuiStorage* st = ImGui::GetStateStorage();
    ImGuiID gid = ImGui::GetID("pm");
    auto SA = [](ImU32 c, float a) -> ImU32 {
        a = Clampf(a, 0.0f, 1.0f);
        ImU32 al = (ImU32)(((c >> 24) & 0xFF) * a);
        return (c & 0x00FFFFFFu) | (al << 24);
    };
    ProfileAction action = ProfileAction::None;
    master = Clampf(master, 0.0f, 1.0f);

    bool open = st->GetBool(gid, false);
    ImGuiID subKey = gid ^ 0xB00Bu;
    int openSub = st->GetInt(subKey, 0);

    Spring& osp = g->springs().Get((uint32_t)gid, 20, SpringStyle::Bouncy,  open ? 1.0f : 0.0f);
    osp.target = open ? 1.0f : 0.0f; osp.Tick(Dt());
    float ot = Clampf(osp.x, 0.0f, 1.0f);
    Spring& bhov = g->springs().Get((uint32_t)gid, 22, SpringStyle::Critical, 0.0f);
    Spring& ssp = g->springs().Get((uint32_t)gid, 21, SpringStyle::Bouncy, 0.0f);
    ssp.target = (open && openSub != 0) ? 1.0f : 0.0f; ssp.Tick(Dt());
    float ot2 = Clampf(ssp.x, 0.0f, 1.0f);

    const float bh = 56.0f;
    ImVec2 cardMin = ImGui::GetWindowPos(), cardSz = ImGui::GetWindowSize();
    bool wantClose = false;

    ImGui::SetCursorScreenPos(ImVec2(x, y));
    ImGui::InvisibleButton("##pm_btn", ImVec2(w, bh));
    bool bh_hov = ImGui::IsItemHovered();
    if (!open && ImGui::IsItemDeactivated() && bh_hov) { open = true; st->SetBool(gid, open); }
    bhov.target = (bh_hov || open || active) ? 1.0f : 0.0f; bhov.Tick(Dt());
    {
        Primitive b; b.cx = x + w*0.5f; b.cy = y + bh*0.5f; b.hw = w*0.5f; b.hh = bh*0.5f;
        b.corner_radius = 14.0f; b.fade = master * (0.5f + 0.5f*bhov.x); b.material = (open || active) ? Material::Accent : Material::Thin;
        b.elevate = 1.0f + 0.18f*bhov.x; g->SubmitOverlay(b);
    }
    {
        char shortName[40]; { int i=0; for (; name[i] && name[i] != ' ' && i < 38; ++i) shortName[i]=name[i]; shortName[i]=0; }
        float av = 36.0f; ImVec2 ac(x + 12.0f + av*0.5f, y + bh*0.5f);
        dl->AddCircleFilled(ac, av*0.5f, SA(tint, master));
        dl->AddCircle(ac, av*0.5f, SA(IM_COL32(255,255,255,60), master), 0, 1.5f);
        ImVec2 is = ImGui::CalcTextSize(initials);
        dl->AddText(ImVec2(ac.x - is.x*0.5f, ac.y - is.y*0.5f), SA(IM_COL32(255,255,255,255), master), initials);
        float tx = x + 12.0f + av + 10.0f;
        dl->AddText(ImVec2(tx, y + 11.0f), SA(kInk, master), shortName);
        dl->AddText(ImVec2(tx, y + 30.0f), SA(kInkSoft, master), email);
        ImVec2 cc(x + w - 18.0f, y + bh*0.5f);
        float ang = ot * kPI, ca = cosf(ang), sa2 = sinf(ang);
        auto rt = [&](float ox, float oy){ return ImVec2(cc.x + ox*ca - oy*sa2, cc.y + ox*sa2 + oy*ca); };
        ImU32 chc = SA(kInkSoft, master*(0.7f + 0.3f*bhov.x));
        dl->AddLine(rt(-5,-2), rt(0,3), chc, 2.2f);
        dl->AddLine(rt( 5,-2), rt(0,3), chc, 2.2f);
    }

    const float mw = 250.0f, pad = 12.0f, hh = 58.0f, rh = 38.0f, dgap = 12.0f, endpad = 12.0f;
    float H = pad + hh + dgap + 2*rh + dgap + 3*rh + dgap + 1*rh + endpad + 36.0f;
    float left = x, right = x + mw;
    float bottom = y - 8.0f;
    float top = bottom - H;
    float h2 = H * (0.40f + 0.60f*ot);
    float clipTop = bottom - h2;

    const char*   mLbl[6]  = { "View Profile", "Account Settings", "Status", "Appearance", "Glass Settings", "Sign Out" };
    Icon          mIco[6]  = { Icon::Person, Icon::Gear, Icon::Wifi, Icon::Display, Icon::Palette, Icon::Lock };
    int           mKind[6] = { 0, 0, 1, 2, 0, 0 };
    ProfileAction mAct[6]  = { ProfileAction::ViewProfile, ProfileAction::AccountSettings,
                               ProfileAction::None, ProfileAction::None,
                               ProfileAction::GlassSettings, ProfileAction::SignOut };
    ImU32 stCol[4] = { IM_COL32(52,209,88,255), IM_COL32(255,159,10,255), IM_COL32(255,69,58,255), IM_COL32(142,142,147,255) };
    const char* stName[4] = { "Online", "Away", "Do Not Disturb", "Invisible" };
    const char* thName[3] = { "Dark", "Light", "Auto" };
    Icon        thIco[3]  = { Icon::Moon, Icon::Sun, Icon::Display };

    float rowY[6]; float cy = top + pad + hh + dgap;
    for (int i = 0; i < 6; ++i) {
        if (i == 2 || i == 5) cy += dgap;
        rowY[i] = cy; cy += rh;
    }

    if (ot > 0.003f) {

        ImGui::SetNextWindowPos(cardMin); ImGui::SetNextWindowSize(cardSz); ImGui::SetNextWindowBgAlpha(0.0f);
        if (open) ImGui::SetNextWindowFocus();
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::Begin("##pm_win", nullptr,
                     ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                     ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoBackground |
                     ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse);
        ImDrawList* mdl = ImGui::GetWindowDrawList();

        ImGui::SetCursorScreenPos(cardMin);
        ImGui::InvisibleButton("##pm_catch", cardSz, ImGuiButtonFlags_AllowOverlap);
        if (ImGui::IsItemDeactivated() && ImGui::IsItemHovered()) wantClose = true;

        bool live = ot > 0.85f;

        Primitive bg; bg.cx = (left+right)*0.5f; bg.cy = (clipTop+bottom)*0.5f;
        bg.hw = mw*0.5f; bg.hh = h2*0.5f; bg.corner_radius = 17.0f; bg.fade = master*ot; bg.material = Material::Thick;
        g->SubmitOverlay(bg);
        DrawUIBlurPanel(mdl, ImVec2(left, clipTop), ImVec2(right, bottom), 17.0f, master*ot, IM_COL32(22,23,29,118));

        ImGui::PushClipRect(ImVec2(left, clipTop), ImVec2(right, bottom), true);

        {
            float ax = left + pad + 19.0f, ay = top + pad + 19.0f;
            mdl->AddCircleFilled(ImVec2(ax, ay), 19.0f, SA(tint, master*ot));
            ImVec2 is = ImGui::CalcTextSize(initials);
            mdl->AddText(ImVec2(ax - is.x*0.5f, ay - is.y*0.5f), SA(IM_COL32(255,255,255,255), master*ot), initials);
            mdl->AddText(ImVec2(ax + 28.0f, top + pad + 6.0f),  SA(kInk, master*ot), name);
            mdl->AddText(ImVec2(ax + 28.0f, top + pad + 24.0f), SA(kInkSoft, master*ot), email);
        }
        auto divat = [&](float yy){ mdl->AddLine(ImVec2(left+12, yy), ImVec2(right-12, yy), SA(IM_COL32(255,255,255,26), master*ot), 1.0f); };
        divat(top + pad + hh + dgap*0.5f);
        divat(rowY[2] - dgap*0.5f);
        divat(rowY[5] - dgap*0.5f);

        int hov = -1;
        for (int i = 0; i < 6; ++i) {
            ImGui::PushID(i);
            ImGui::SetCursorScreenPos(ImVec2(left + 8, rowY[i]));
            ImGui::InvisibleButton("##r", ImVec2(mw - 16, rh - 2));
            bool rh_hov = ImGui::IsItemHovered();
            if (rh_hov) hov = i;
            if (live && rh_hov && ImGui::IsItemDeactivated()) {
                if (mKind[i] == 0) { action = mAct[i]; open = false; openSub = 0; }
                else { openSub = (openSub == mKind[i]) ? 0 : mKind[i]; }
            }
            ImGui::PopID();
        }
        if (live && hov >= 0 && mKind[hov] != 0) openSub = mKind[hov];

        ImGuiID hyk = gid ^ 0x1111u, hak = gid ^ 0x2222u;
        float gy = st->GetFloat(hyk, rowY[0]);
        float ty = (hov >= 0) ? rowY[hov] : gy;
        gy += (ty - gy) * Clampf(Dt()*22.0f, 0.0f, 1.0f); st->SetFloat(hyk, gy);
        float ha = st->GetFloat(hak, 0.0f);
        ha += (((hov >= 0) ? 1.0f : 0.0f) - ha) * Clampf(Dt()*16.0f, 0.0f, 1.0f); st->SetFloat(hak, ha);
        if (ha > 0.01f)
            mdl->AddRectFilled(ImVec2(left+8, gy), ImVec2(right-8, gy + rh-2), SA(AccentCol(), master*ot*ha), 10.0f);

        int sClamp = *status < 0 ? 0 : (*status > 3 ? 3 : *status);
        int tClamp = *theme  < 0 ? 0 : (*theme  > 2 ? 2 : *theme);
        for (int i = 0; i < 6; ++i) {
            float prox = ha * (1.0f - Clampf(fabsf(gy - rowY[i]) / rh, 0.0f, 1.0f));
            bool isOut = (i == 5);
            ImU32 lc = isOut ? SA(IM_COL32(255,99,97,255), master*ot) : SA(kInk, master*ot);
            if (prox > 0.01f && !isOut) lc = SA(IM_COL32(255,255,255,255), master*ot);
            float iy = rowY[i] + (rh-2)*0.5f;
            ImVec2 ip(left + 8 + 20.0f, iy);
            ImU32 icc = isOut ? SA(IM_COL32(255,99,97,255), master*ot)
                              : SA(prox > 0.5f ? IM_COL32(255,255,255,255) : kInkSoft, master*ot);
            if (mKind[i] == 1)       mdl->AddCircleFilled(ip, 6.0f, SA(stCol[sClamp], master*ot));
            else if (mKind[i] == 2)  DrawIcon(mdl, thIco[tClamp], ip, 17.0f, icc, 2.0f);
            else                     DrawIcon(mdl, mIco[i], ip, 17.0f, icc, 2.0f);
            ImVec2 ls = ImGui::CalcTextSize(mLbl[i]);
            mdl->AddText(ImVec2(left + 8 + 38.0f, iy - ls.y*0.5f), lc, mLbl[i]);
            if (mKind[i] != 0)
                DrawIcon(mdl, Icon::ChevronR, ImVec2(right - 18.0f, iy), 13.0f,
                         SA(prox > 0.5f ? IM_COL32(255,255,255,255) : kInkSoft, master*ot), 2.0f);
        }
        {
            ImFont* ff = ImGui::GetFont();
            float credY = rowY[5] + rh + 8.0f, mid = (left + right) * 0.5f;
            mdl->AddLine(ImVec2(left + 12, credY), ImVec2(right - 12, credY), SA(IM_COL32(255,255,255,26), master*ot), 1.0f);
            const char* c1 = "Created by poncipp \xC2\xB7 @pondot";
            const char* c2 = "github.com/poncippg-spec";
            ImVec2 s1 = ff->CalcTextSizeA(13.0f, 99999.0f, 0.0f, c1);
            ImVec2 s2 = ff->CalcTextSizeA(13.0f, 99999.0f, 0.0f, c2);
            mdl->AddText(ff, 13.0f, ImVec2(mid - s1.x*0.5f, credY + 8.0f),  SA(kInkSoft, master*ot), c1);
            mdl->AddText(ff, 13.0f, ImVec2(mid - s2.x*0.5f, credY + 23.0f), SA(AccentCol(), master*ot), c2);
        }
        ImGui::PopClipRect();

        if (ot2 > 0.003f && openSub != 0) {
            int sc = (openSub == 1) ? 4 : 3;
            const float sw = 200.0f, srh = 36.0f, spad = 8.0f;
            float sH = spad*2 + sc*srh;
            int parent = (openSub == 1) ? 2 : 3;
            float sTop = rowY[parent] - spad;
            if (sTop + sH > bottom - 6) sTop = bottom - 6 - sH;
            if (sTop < top + 6) sTop = top + 6;
            float sLeft = right + 6.0f - (1.0f - ot2)*12.0f;
            float sBottom = sTop + sH;

            Primitive sb; sb.cx = sLeft + sw*0.5f; sb.cy = sTop + sH*0.5f; sb.hw = sw*0.5f; sb.hh = sH*0.5f;
            sb.corner_radius = 15.0f; sb.fade = master*ot2; sb.material = Material::Thick; g->SubmitOverlay(sb);

            DrawUIBlurPanel(mdl, ImVec2(sLeft, sTop), ImVec2(sLeft+sw, sBottom), 15.0f, master*ot2, IM_COL32(22,23,29,118));

            ImGui::SetCursorScreenPos(ImVec2(sLeft, sTop));
            ImGui::InvisibleButton("##pm_subpanel", ImVec2(sw, sH), ImGuiButtonFlags_AllowOverlap);

            ImGui::PushClipRect(ImVec2(sLeft, sTop), ImVec2(sLeft+sw, sBottom), true);
            int shov = -1;
            for (int i = 0; i < sc; ++i) {
                ImGui::PushID(100 + i);
                float ry = sTop + spad + i*srh;
                ImGui::SetCursorScreenPos(ImVec2(sLeft + 6, ry));
                ImGui::InvisibleButton("##sr", ImVec2(sw - 12, srh - 2));
                bool rhov = ImGui::IsItemHovered();
                if (rhov) shov = i;
                if (live && rhov && ImGui::IsItemDeactivated()) {
                    if (openSub == 1) { *status = i; action = ProfileAction::StatusChanged; }
                    else              { *theme  = i; action = ProfileAction::ThemeChanged;  }
                    open = false; openSub = 0;
                }
                ImGui::PopID();
            }
            ImGuiID syk = gid ^ 0x3333u, sak = gid ^ 0x4444u;
            float sgy = st->GetFloat(syk, sTop + spad);
            float sty = (shov >= 0) ? (sTop + spad + shov*srh) : sgy;
            sgy += (sty - sgy) * Clampf(Dt()*22.0f, 0.0f, 1.0f); st->SetFloat(syk, sgy);
            float sha = st->GetFloat(sak, 0.0f);
            sha += (((shov >= 0) ? 1.0f : 0.0f) - sha) * Clampf(Dt()*16.0f, 0.0f, 1.0f); st->SetFloat(sak, sha);
            if (sha > 0.01f)
                mdl->AddRectFilled(ImVec2(sLeft+6, sgy), ImVec2(sLeft+sw-6, sgy + srh-2), SA(AccentCol(), master*ot2*sha), 9.0f);
            for (int i = 0; i < sc; ++i) {
                float ry = sTop + spad + i*srh, iy = ry + (srh-2)*0.5f;
                float prox = sha * (1.0f - Clampf(fabsf(sgy - ry)/srh, 0.0f, 1.0f));
                ImU32 lc = SA(prox > 0.5f ? IM_COL32(255,255,255,255) : kInk, master*ot2);
                ImU32 tick = SA(prox > 0.5f ? IM_COL32(255,255,255,255) : AccentCol(), master*ot2);
                if (openSub == 1) {
                    mdl->AddCircleFilled(ImVec2(sLeft + 18, iy), 5.0f, SA(stCol[i], master*ot2));
                    mdl->AddText(ImVec2(sLeft + 32, iy - ImGui::CalcTextSize(stName[i]).y*0.5f), lc, stName[i]);
                    if (i == *status) DrawIcon(mdl, Icon::Check, ImVec2(sLeft + sw - 18, iy), 14.0f, tick, 2.0f);
                } else {
                    DrawIcon(mdl, thIco[i], ImVec2(sLeft + 18, iy), 16.0f, SA(prox > 0.5f ? IM_COL32(255,255,255,255) : kInkSoft, master*ot2), 2.0f);
                    mdl->AddText(ImVec2(sLeft + 34, iy - ImGui::CalcTextSize(thName[i]).y*0.5f), lc, thName[i]);
                    if (i == *theme) DrawIcon(mdl, Icon::Check, ImVec2(sLeft + sw - 18, iy), 14.0f, tick, 2.0f);
                }
            }
            ImGui::PopClipRect();
        }

        ImGui::End();
        ImGui::PopStyleVar();
    }

    if (ImGui::IsKeyPressed(ImGuiKey_Escape) && open) { open = false; openSub = 0; }
    if (wantClose) { open = false; openSub = 0; }
    st->SetBool(gid, open);
    st->SetInt(subKey, openSub);
    ImGui::PopID();
    return action;
}

static bool g_lastFieldFocused = false;
bool FieldWasFocused() { return g_lastFieldFocused; }
static bool TextCore(const char* label, char* buf, int buflen, const char* hint,
                     Icon leading, bool pill, bool showClear, bool password = false,
                     bool drawBg = true, float fixedH = 0.0f, float fixedW = 0.0f) {
    ImGui::PushID(label);
    const float h = fixedH > 0.0f ? fixedH : 44.0f;
    float w = fixedW > 0.0f ? fixedW : ImGui::GetContentRegionAvail().x;
    ImVec2 cur = ImGui::GetCursorScreenPos();
    ImGuiID id = ImGui::GetID("tf");
    float rad = pill ? h * 0.5f : 12.0f;

    ImGuiStorage* st = ImGui::GetStateStorage();
    int prevLen = st->GetInt(id, -1);
    int curLen = (int)std::strlen(buf);

    bool hovPill = pill && ImGui::IsMouseHoveringRect(cur, ImVec2(cur.x + w, cur.y + h));
    float focusPrev = g->springs().Get((uint32_t)id, 8, SpringStyle::Critical, 0.0f).x;
    Spring& bub = g->springs().Get((uint32_t)id, 9, SpringStyle::Bouncy, 0.0f);
    bub.target = (pill && (hovPill || focusPrev > 0.5f)) ? 1.0f : 0.0f; bub.Tick(Dt());
    float sw = Clampf(bub.x, 0.0f, 1.25f);

    if (drawBg) {
        float gw = 1.0f + 0.045f * sw;
        Primitive f; f.cx = cur.x + w * 0.5f; f.cy = cur.y + h * 0.5f;
        f.hw = w * 0.5f * gw; f.hh = h * 0.5f * gw; f.corner_radius = pill ? f.hh : rad;
        f.fade = 1.0f; f.material = Material::Thin; f.elevate = 1.0f + 0.10f * sw;
        g->Submit(f);
    }

    ImDrawList* dl = ImGui::GetWindowDrawList();
    float textX = cur.x + (pill ? 18.0f : 15.0f);
    bool hasIcon = (leading != Icon::None);
    if (hasIcon) textX = cur.x + (pill ? 44.0f : 40.0f);

    (void)rad;
    float clearW = (showClear && curLen > 0) ? 26.0f : 8.0f;
    float innerL = textX;
    float innerR = cur.x + w - clearW - 8.0f;
    float availT = innerR - innerL; if (availT < 10.0f) availT = 10.0f;
    float fh = ImGui::GetFontSize();
    float ty = cur.y + (h - fh) * 0.5f - 1.0f;

    ImGui::SetCursorScreenPos(ImVec2(innerL, ty));
    ImGui::SetNextItemWidth(availT);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(0,0,0,0));
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0,0,0,0));
    ImGui::PushStyleColor(ImGuiCol_TextSelectedBg, IM_COL32(10,132,255,70));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0,0));
    ImGuiInputTextFlags flags = password ? ImGuiInputTextFlags_Password : 0;
    bool changed = ImGui::InputText("##in", buf, (size_t)buflen, flags);
    bool active = ImGui::IsItemActive();
    ImGui::PopStyleVar(); ImGui::PopStyleColor(3);

    Spring& fg = g->springs().Get((uint32_t)id, 8, SpringStyle::Critical, 0.0f);
    fg.target = active ? 1.0f : 0.0f; fg.Tick(Dt());
    float fa = Clampf(fg.x, 0.0f, 1.0f);
    if (hasIcon) {
        ImU32 acc = AccentCol();
        int ir = 120 + (int)(((int)(acc & 0xFF) - 120) * fa);
        int ig = 122 + (int)(((int)((acc >> 8) & 0xFF) - 122) * fa);
        int ib = 132 + (int)(((int)((acc >> 16) & 0xFF) - 132) * fa);
        DrawIcon(dl, leading, ImVec2(cur.x + (pill ? 22.0f : 20.0f), cur.y + h * 0.5f), 18.0f, IM_COL32(ir, ig, ib, 255), 2.0f);
    }
    if (drawBg && fa > 0.01f) {
        float ul = (w * 0.5f - 12.0f) * fa, uy = cur.y + h - 2.5f;
        dl->AddLine(ImVec2(cur.x + w * 0.5f - ul, uy), ImVec2(cur.x + w * 0.5f + ul, uy),
                    (AccentCol() & 0x00FFFFFF) | ((int)(220 * fa) << 24), 2.0f);
    }

    char disp[256];
    int n = curLen < 255 ? curLen : 255;
    if (password) { for (int k = 0; k < n; ++k) disp[k] = '*'; disp[n] = 0; }
    else { std::memcpy(disp, buf, (size_t)n); disp[n] = 0; }

    Spring& pop = g->springs().Get((uint32_t)id, 9, SpringStyle::Bouncy, 0.0f);
    if (prevLen >= 0 && curLen > prevLen) pop.x = 1.0f;
    pop.target = 0.0f; pop.Tick(Dt());
    float popv = Clampf(pop.x, 0.0f, 1.0f);

    ImGuiID ckey = id ^ 0x9e3779b9u;

    if (hint && *hint && !pill) {
        Spring& lift = g->springs().Get((uint32_t)id, 11, SpringStyle::Bouncy, (active || curLen > 0) ? 1.0f : 0.0f);
        lift.target = (active || curLen > 0) ? 1.0f : 0.0f; lift.Tick(Dt());
        float lf = Clampf(lift.x, 0.0f, 1.2f);
        float ls = 1.0f - 0.40f * Clampf(lf, 0.0f, 1.0f);
        float ly = ty + ((cur.y + 4.0f) - ty) * lf;
        int v = 122 + (int)(70.0f * Clampf(lf, 0.0f, 1.0f));
        dl->AddText(ImGui::GetFont(), fh * ls, ImVec2(innerL, ly), IM_COL32(v, v + 2, v + 10, 255), hint);
    }
    if (curLen == 0 && !active) {
        if (pill && hint && *hint) dl->AddText(ImVec2(innerL, ty), IM_COL32(120,122,132,255), hint);
        st->SetFloat(ckey, innerL);
    } else {
        float tw = ImGui::CalcTextSize(disp).x;
        float scroll = tw > availT ? (tw - availT) : 0.0f;
        float baseX = innerL - scroll;
        dl->PushClipRect(ImVec2(innerL - 1.0f, cur.y + 3.0f), ImVec2(innerR + 2.0f, cur.y + h - 3.0f), true);

        if (n > 0 && popv > 0.02f) {
            char head[256]; int hn = n - 1;
            std::memcpy(head, disp, (size_t)hn); head[hn] = 0;
            float headW = ImGui::CalcTextSize(head).x;
            dl->AddText(ImVec2(baseX, ty), kInk, head);
            char last[2] = { disp[n - 1], 0 };
            float ps = 1.0f + popv * 1.15f;
            float fsz = fh * ps;
            float lw = ImGui::CalcTextSize(last).x;
            float lx = baseX + headW - lw * (ps - 1.0f) * 0.5f;
            float lly = cur.y + (h - fsz) * 0.5f - 1.0f;
            int la = (int)(255.0f * (0.55f + 0.45f * (1.0f - popv)));
            dl->AddText(ImGui::GetFont(), fsz, ImVec2(lx, lly), (kInk & 0x00FFFFFF) | (la << 24), last);
        } else {
            dl->AddText(ImVec2(baseX, ty), kInk, disp);
        }

        if (active) {
            float target = baseX + tw;
            float caretX = st->GetFloat(ckey, target);
            caretX += (target - caretX) * Clampf(Dt() * 22.0f, 0.0f, 1.0f);
            st->SetFloat(ckey, caretX);
            float blink = 0.5f + 0.5f * sinf((float)ImGui::GetTime() * 6.0f);
            int ca = (int)(255.0f * (0.35f + 0.65f * blink));
            dl->AddLine(ImVec2(caretX, cur.y + (h - fh) * 0.5f), ImVec2(caretX, cur.y + (h + fh) * 0.5f - 2.0f), IM_COL32(80,150,255,ca), 2.4f);
        } else {
            st->SetFloat(ckey, innerL);
        }
        dl->PopClipRect();
    }
    st->SetInt(id, curLen);

    if (showClear && curLen > 0) {
        ImVec2 xp(cur.x + w - 22.0f, cur.y + h * 0.5f);
        ImGui::SetCursorScreenPos(ImVec2(xp.x - 11.0f, xp.y - 11.0f));
        ImGui::InvisibleButton("##clr", ImVec2(22, 22));
        bool xh = ImGui::IsItemHovered();
        dl->AddCircleFilled(xp, 9.0f, xh ? IM_COL32(110,110,120,255) : IM_COL32(150,150,160,210));
        dl->AddLine(ImVec2(xp.x-3,xp.y-3), ImVec2(xp.x+3,xp.y+3), IM_COL32(255,255,255,255), 1.7f);
        dl->AddLine(ImVec2(xp.x-3,xp.y+3), ImVec2(xp.x+3,xp.y-3), IM_COL32(255,255,255,255), 1.7f);
        if (ImGui::IsItemDeactivated() && xh) { buf[0] = 0; changed = true; }
    }

    ImGui::SetCursorScreenPos(ImVec2(cur.x, cur.y + h));
    ImGui::Dummy(ImVec2(w, 0));
    g_lastFieldFocused = active;
    ImGui::PopID();
    return changed;
}

bool TextField(const char* label, char* buf, int buflen, const char* hint, Icon leading) {
    return TextCore(label, buf, buflen, hint, leading, false, false);
}
bool SearchField(char* buf, int buflen, const char* hint, float width) {
    return TextCore("##search", buf, buflen, hint ? hint : "Search", Icon::Search, true, true, false, true, 40.0f, width);
}
bool PasswordField(const char* label, char* buf, int buflen, const char* hint) {
    return TextCore(label, buf, buflen, hint, Icon::Lock, false, false, true);
}
bool FieldRaw(const char* label, char* buf, int buflen, const char* hint, Icon leading, bool password, float height) {
    return TextCore(label, buf, buflen, hint, leading, false, false, password, false, height);
}
