static void Draw_Calculator(float w){
    static double acc = 0.0, disp = 0.0; static int pend = 0;
    static bool fresh = true; static char shown[40] = "0";
    auto setShown = [&](double v){
        if (v == (long long)v && fabs(v) < 1e15) snprintf(shown, sizeof(shown), "%lld", (long long)v);
        else snprintf(shown, sizeof(shown), "%.6g", v);
    };
    auto digit = [&](int d){
        if (fresh){ disp = d; fresh = false; }
        else { double nv = disp*10 + (disp<0?-d:d); if (fabs(nv) < 1e15) disp = nv; }
        setShown(disp);
    };
    auto applyOp = [&](){
        if      (pend==1) acc += disp; else if (pend==2) acc -= disp;
        else if (pend==3) acc *= disp; else if (pend==4){ if (disp!=0) acc /= disp; }
        else acc = disp;
        setShown(acc); disp = acc;
    };
    float pw = w < 360.0f ? w : 360.0f;
    float x0 = ImGui::GetCursorScreenPos().x + (w - pw)*0.5f;
    float gap = 12.0f;
    float d  = (pw - 3*gap) / 4.0f;
    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImVec2 dispTL = ImGui::GetCursorScreenPos();
    float dispH = 100.0f;
    {
        ImVec2 ts = ImGui::GetFont()->CalcTextSizeA(58.0f, 99999.0f, 0.0f, shown);
        dl->AddText(ImGui::GetFont(), 58.0f, ImVec2(x0 + pw - ts.x - 8.0f, dispTL.y + dispH - ts.y - 10.0f), kInk, shown);
    }
    ImGui::Dummy(ImVec2(w, dispH + 8.0f));
    float gridTop = ImGui::GetCursorScreenPos().y + d*0.5f;
    auto cellC = [&](int col, int row){ return ImVec2(x0 + d*0.5f + col*(d+gap), gridTop + row*(d+gap)); };
    ImU32 white = IM_COL32(255,255,255,255);
    float fz = d*0.40f;
    if (CalcKey("ac", cellC(0,0), d,d, "AC",  Material::Thin,  kInk, fz)){ acc=0; disp=0; pend=0; fresh=true; strcpy(shown,"0"); }
    if (CalcKey("neg",cellC(1,0), d,d, "+/-", Material::Thin,  kInk, fz)){ disp=-disp; setShown(disp); }
    if (CalcKey("pct",cellC(2,0), d,d, "%",   Material::Thin,  kInk, fz)){ disp/=100.0; setShown(disp); fresh=true; }
    if (CalcKey("div",cellC(3,0), d,d, "\xC3\xB7", Material::Accent, white, fz)){ applyOp(); pend=4; fresh=true; }
    const char* digs[3][3] = {{"7","8","9"},{"4","5","6"},{"1","2","3"}};
    int          dv [3][3] = {{7,8,9},{4,5,6},{1,2,3}};
    const char*  opl[3] = { "\xC3\x97", "\xE2\x88\x92", "+" };
    int          opc[3] = { 3, 2, 1 };
    for (int r=0;r<3;r++){
        for (int c=0;c<3;c++)
            if (CalcKey(digs[r][c], cellC(c,r+1), d,d, digs[r][c], Material::Knob, IM_COL32(18,18,22,255), fz)) digit(dv[r][c]);
        if (CalcKey(opl[r], cellC(3,r+1), d,d, opl[r], Material::Accent, white, fz)){ applyOp(); pend=opc[r]; fresh=true; }
    }
    {
        ImVec2 cc = cellC(0,4); float wide = d*2+gap; cc.x += (wide-d)*0.5f;
        if (CalcKey("z0",  cc,          wide, d, "0", Material::Knob, IM_COL32(18,18,22,255), fz)) digit(0);
        if (CalcKey("dot", cellC(2,4),  d,   d, ".", Material::Knob, IM_COL32(18,18,22,255), fz)){  }
        if (CalcKey("eq",  cellC(3,4),  d,   d, "=", Material::Accent, white, fz)){ applyOp(); pend=0; fresh=true; }
    }
    ImGui::SetCursorScreenPos(ImVec2(x0, gridTop + 5*(d+gap)));
    ImGui::Dummy(ImVec2(w, 8.0f));
}
static void Draw_Weather(float w){
    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImFont* f = ImGui::GetFont();
    const ImU32 white = IM_COL32(255,255,255,255), wsoft = IM_COL32(255,255,255,205);
    float cw = w < 720.0f ? w : 720.0f;
    auto colX = [&](){ return ImGui::GetCursorScreenPos().x + (w - cw)*0.5f; };
    struct City{ const char* name; int temp; const char* cond; int hi; int lo; Icon ic; };
    static City cities[4]={ {"Cupertino",68,"Mostly Sunny",74,59,Icon::CloudSun},{"New York",54,"Cloudy",58,47,Icon::Cloud},{"London",49,"Light Rain",53,44,Icon::CloudRain},{"Tokyo",72,"Clear",78,65,Icon::Sun} };
    static int city=0; if(city<0||city>3) city=0;
    static int wSelDay=0;
    City& CC=cities[city]; int delta=CC.temp-68;
    { float h=38.0f; ImVec2 tl(colX(), ImGui::GetCursorScreenPos().y); float cgap=8.0f, chipw=(cw-3*cgap)/4.0f;
      for(int i=0;i<4;i++){ float x=tl.x+i*(chipw+cgap); bool hov=false;
        if(SurfaceHit(cities[i].name, ImVec2(x,tl.y), ImVec2(chipw,h), &hov)) city=i;
        Primitive p; p.cx=x+chipw*0.5f; p.cy=tl.y+h*0.5f; p.hw=chipw*0.5f; p.hh=h*0.5f; p.corner_radius=h*0.5f; p.fade=1.0f; p.material=(i==city)?Material::Accent:Material::Thin; p.elevate=hov?1.08f:1.0f; g->Submit(p);
        ImVec2 ns=f->CalcTextSizeA(14.0f,99999.0f,0,cities[i].name); dl->AddText(f,14.0f,ImVec2(x+chipw*0.5f-ns.x*0.5f,tl.y+h*0.5f-8),(i==city)?IM_COL32(255,255,255,255):kInk,cities[i].name); }
      ImGui::Dummy(ImVec2(w, h+12)); }
    {
        float h = 196.0f;
        ImVec2 tl(colX(), ImGui::GetCursorScreenPos().y);
        Primitive bg; bg.cx=tl.x+cw*0.5f; bg.cy=tl.y+h*0.5f; bg.hw=cw*0.5f; bg.hh=h*0.5f;
        bg.corner_radius=30.0f; bg.fade=1.0f; bg.material=Material::Accent; bg.elevate=1.2f; g->Submit(bg);
        char tb[8]; snprintf(tb,8,"%d\xC2\xB0",CC.temp);
        char hl[28]; snprintf(hl,28,"H:%d\xC2\xB0   L:%d\xC2\xB0",CC.hi,CC.lo);
        dl->AddText(f, 24.0f, ImVec2(tl.x+26, tl.y+18),  white, CC.name);
        dl->AddText(f, 78.0f, ImVec2(tl.x+22, tl.y+40),  white, tb);
        dl->AddText(f, 17.0f, ImVec2(tl.x+28, tl.y+138), wsoft, CC.cond);
        dl->AddText(f, 15.0f, ImVec2(tl.x+28, tl.y+162), wsoft, hl);
        DrawIcon(dl, CC.ic, ImVec2(tl.x+cw-80, tl.y+88), 78.0f, white, 2.2f);
        ImGui::Dummy(ImVec2(w, h+14));
    }
    {
        float h = 116.0f; ImVec2 tl(colX(), ImGui::GetCursorScreenPos().y);
        Primitive bg; bg.cx=tl.x+cw*0.5f; bg.cy=tl.y+h*0.5f; bg.hw=cw*0.5f; bg.hh=h*0.5f;
        bg.corner_radius=28.0f; bg.fade=1.0f; bg.material=Material::Thin; g->Submit(bg);
        dl->AddText(f, 12.0f, ImVec2(tl.x+18, tl.y+12), kInkSoft, "HOURLY FORECAST");
        dl->AddLine(ImVec2(tl.x+16, tl.y+30), ImVec2(tl.x+cw-16, tl.y+30), SA(kInkSoft,0.3f), 1.0f);
        const char* hrs[] = {"Now","1PM","2PM","3PM","4PM","5PM","6PM","7PM"};
        Icon ic[] = {Icon::Sun,Icon::Sun,Icon::CloudSun,Icon::CloudSun,Icon::Cloud,Icon::CloudRain,Icon::CloudRain,Icon::CloudMoon};
        int tp[] = {68,69,70,70,67,64,61,59};
        int n=8; float inner=cw-28, colw=inner/n;
        for (int i=0;i<n;i++){
            float cx = tl.x+14+colw*(i+0.5f);
            ImVec2 ts=f->CalcTextSizeA(13.0f,99999.0f,0,hrs[i]); dl->AddText(f,13.0f,ImVec2(cx-ts.x*0.5f,tl.y+40),kInk,hrs[i]);
            DrawIcon(dl, ic[i], ImVec2(cx, tl.y+72), 22.0f, kInk, 2.0f);
            char tb[8]; snprintf(tb,sizeof(tb),"%d\xC2\xB0",tp[i]+delta);
            ImVec2 vs=f->CalcTextSizeA(15.0f,99999.0f,0,tb); dl->AddText(f,15.0f,ImVec2(cx-vs.x*0.5f,tl.y+92),kInk,tb);
        }
        ImGui::Dummy(ImVec2(w, h+14));
    }
    {
        const char* days[] = {"Today","Tue","Wed","Thu","Fri","Sat","Sun"};
        Icon ic[] = {Icon::CloudSun,Icon::Sun,Icon::CloudRain,Icon::CloudShowers,Icon::CloudSun,Icon::Sun,Icon::Cloud};
        int lo[]={59,57,54,53,58,60,57}, hi[]={74,73,66,64,72,77,70};
        int nd=7; float rowh=38.0f, h=34+nd*rowh+8;
        ImVec2 tl(colX(), ImGui::GetCursorScreenPos().y);
        Primitive bg; bg.cx=tl.x+cw*0.5f; bg.cy=tl.y+h*0.5f; bg.hw=cw*0.5f; bg.hh=h*0.5f;
        bg.corner_radius=28.0f; bg.fade=1.0f; bg.material=Material::Thin; g->Submit(bg);
        dl->AddText(f,12.0f, ImVec2(tl.x+18, tl.y+12), kInkSoft, "10-DAY FORECAST");
        int wlo=50, whi=82;
        for (int i=0;i<nd;i++){
            float ry = tl.y+34 + i*rowh + rowh*0.5f;
            { bool hov=false; if(SurfaceHit(days[i], ImVec2(tl.x+8, ry-rowh*0.5f), ImVec2(cw-16,rowh), &hov)) wSelDay=i;
              if(i==wSelDay) dl->AddRectFilled(ImVec2(tl.x+8,ry-rowh*0.5f+2),ImVec2(tl.x+cw-8,ry+rowh*0.5f-2), SA(Accent(),0.16f), 10.0f);
              else if(hov)   dl->AddRectFilled(ImVec2(tl.x+8,ry-rowh*0.5f+2),ImVec2(tl.x+cw-8,ry+rowh*0.5f-2), SA(kInkSoft,0.08f), 10.0f); }
            dl->AddText(f,16.0f, ImVec2(tl.x+18, ry-9), kInk, days[i]);
            DrawIcon(dl, ic[i], ImVec2(tl.x+132, ry), 20.0f, kInk, 2.0f);
            char lob[8],hib[8]; snprintf(lob,8,"%d\xC2\xB0",lo[i]+delta); snprintf(hib,8,"%d\xC2\xB0",hi[i]+delta);
            dl->AddText(f,16.0f, ImVec2(tl.x+170, ry-9), kInkSoft, lob);
            float bx0=tl.x+212, bx1=tl.x+cw-58, bw=bx1-bx0;
            dl->AddRectFilled(ImVec2(bx0,ry-3), ImVec2(bx1,ry+3), SA(kInkSoft,0.22f), 3.0f);
            float a=(lo[i]-wlo)/(float)(whi-wlo), b=(hi[i]-wlo)/(float)(whi-wlo);
            ImU32 c0=IM_COL32(90,170,255,255), c1=IM_COL32(255,200,80,255);
            dl->AddRectFilledMultiColor(ImVec2(bx0+a*bw,ry-3), ImVec2(bx0+b*bw,ry+3), c0,c1,c1,c0);
            dl->AddText(f,16.0f, ImVec2(tl.x+cw-48, ry-9), kInk, hib);
            if (i<nd-1) dl->AddLine(ImVec2(tl.x+18, ry+rowh*0.5f), ImVec2(tl.x+cw-16, ry+rowh*0.5f), SA(kInkSoft,0.16f),1.0f);
        }
        ImGui::Dummy(ImVec2(w, h+14));
    }
    {
        struct D{ Icon ic; const char* cap; const char* val; const char* sub; };
        D ds[] = {
            {Icon::Sun,"UV INDEX","3","Moderate"}, {Icon::Wind,"WIND","6 mph","WSW gusts"},
            {Icon::Droplet,"HUMIDITY","58%","Dew pt 52\xC2\xB0"}, {Icon::Thermometer,"FEELS LIKE","70\xC2\xB0","Feels warmer"},
            {Icon::CloudRain,"RAINFALL","0 in","None expected"}, {Icon::Eye,"VISIBILITY","10 mi","Perfectly clear"},
        };
        int nt=6; float gap=14, tw=(cw-gap)/2, th=104;
        float sy=ImGui::GetCursorScreenPos().y, bx=colX();
        for (int i=0;i<nt;i++){
            int r=i/2,c=i%2; float x=bx+c*(tw+gap), y=sy+r*(th+gap);
            Primitive bg; bg.cx=x+tw*0.5f; bg.cy=y+th*0.5f; bg.hw=tw*0.5f; bg.hh=th*0.5f;
            bg.corner_radius=26.0f; bg.fade=1.0f; bg.material=Material::Thin; g->Submit(bg);
            DrawIcon(dl, ds[i].ic, ImVec2(x+24, y+25), 15.0f, kInkSoft, 2.0f);
            dl->AddText(f,12.0f, ImVec2(x+38, y+18), kInkSoft, ds[i].cap);
            dl->AddText(f,36.0f, ImVec2(x+22, y+38), kInk, ds[i].val);
            dl->AddText(f,13.0f, ImVec2(x+24, y+80), kInkSoft, ds[i].sub);
        }
        int rows=(nt+1)/2; ImGui::Dummy(ImVec2(w, rows*(th+gap)+4));
    }
}
static void Draw_Music(float w){
    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImFont* f = ImGui::GetFont();
    static float pos = 0.32f, vol = 0.62f; static bool playing = true, shuf=false, rep=false;
    const ImU32 white = IM_COL32(255,255,255,255);
    float t = (float)ImGui::GetTime();
    float cw = w < 460.0f ? w : 460.0f;
    float x0 = ImGui::GetCursorScreenPos().x + (w - cw)*0.5f;
    float y  = ImGui::GetCursorScreenPos().y + 6.0f;
    float art = cw*0.66f; if (art>300.0f) art=300.0f;
    float ax = x0 + (cw-art)*0.5f;
    { Primitive bg; bg.cx=ax+art*0.5f; bg.cy=y+art*0.5f; bg.hw=art*0.5f; bg.hh=art*0.5f;
      bg.corner_radius=30.0f; bg.fade=1.0f; bg.material=Material::Accent; bg.elevate=1.5f; g->Submit(bg);
      DrawIcon(dl, Icon::Music, ImVec2(ax+art*0.5f, y+art*0.5f), art*0.34f, SA(white,0.92f), 2.4f); }
    float yy = y + art + 24.0f;
    dl->AddText(f, 25.0f, ImVec2(x0, yy),      kInk,     "Coastline");
    dl->AddText(f, 16.0f, ImVec2(x0, yy+32.0f), kInkSoft, "June Park \xE2\x80\x94 Afterglow");
    { int nb=14; float bw=4.0f, gp=3.0f, bx=x0+cw-(nb*(bw+gp)); float by=yy+24.0f;
      for (int i=0;i<nb;i++){ float a = playing ? (0.35f+0.65f*fabsf(sinf(t*4.0f + i*0.5f))) : 0.2f;
        float hh=4.0f+a*22.0f; dl->AddRectFilled(ImVec2(bx+i*(bw+gp), by-hh), ImVec2(bx+i*(bw+gp)+bw, by), SA(Accent(),0.9f), 1.5f); } }
    yy += 60.0f;
    ImGui::SetCursorScreenPos(ImVec2(x0, yy));
    Glass::Scrubber("m_scrub", &pos, 0.0f, 1.0f, cw);
    float ty = yy + 14.0f;
    int total=214, cur=(int)(pos*total), rem=total-cur;
    char eb[16],rb[16]; snprintf(eb,16,"%d:%02d",cur/60,cur%60); snprintf(rb,16,"-%d:%02d",rem/60,rem%60);
    dl->AddText(f,12.0f, ImVec2(x0, yy+30.0f), kInkSoft, eb);
    ImVec2 rs=f->CalcTextSizeA(12.0f,99999.0f,0,rb); dl->AddText(f,12.0f, ImVec2(x0+cw-rs.x, yy+30.0f), kInkSoft, rb);
    float by = ty + 58.0f, bcx = x0+cw*0.5f, u=66.0f;
    ImGui::SetCursorScreenPos(ImVec2(bcx-2*u-16, by-16)); if (Glass::IconButton("m_shuf", Icon::Shuffle, 32.0f)) shuf=!shuf;
    ImGui::SetCursorScreenPos(ImVec2(bcx-u-18, by-18));   if (Glass::IconButton("m_prev", Icon::Prev, 36.0f)) pos=0.0f;
    ImGui::SetCursorScreenPos(ImVec2(bcx-28, by-28));      Glass::MorphButton("m_pp", MorphIcon::PlayPause, &playing, 56.0f);
    ImGui::SetCursorScreenPos(ImVec2(bcx+u-18, by-18));   if (Glass::IconButton("m_next", Icon::Next, 36.0f)) pos=0.0f;
    ImGui::SetCursorScreenPos(ImVec2(bcx+2*u-16, by-16)); if (Glass::IconButton("m_rep", Icon::Repeat, 32.0f)) rep=!rep;
    float vy = by + 46.0f;
    DrawIcon(dl, Icon::VolumeLow, ImVec2(x0+12, vy), 14.0f, kInkSoft, 2.0f);
    DrawIcon(dl, Icon::Speaker,   ImVec2(x0+cw-12, vy), 15.0f, kInkSoft, 2.0f);
    float vx0=x0+30, vx1=x0+cw-30, vw=vx1-vx0;
    ImGui::SetCursorScreenPos(ImVec2(vx0, vy-14.0f));
    Glass::Scrubber("m_vol", &vol, 0.0f, 1.0f, vw);
    ImGui::SetCursorScreenPos(ImVec2(x0, vy+22.0f));
    ImGui::Dummy(ImVec2(w, 10.0f));
}
static bool SettRow(float x, float w, float y, float rowh, Icon ic, const char* label,
                    const char* value, int kind, bool* tog){
    ImDrawList* dl = ImGui::GetWindowDrawList(); ImFont* f = ImGui::GetFont();
    ImGui::PushID(label);
    ImGui::SetCursorScreenPos(ImVec2(x, y));
    ImGui::InvisibleButton("r", ImVec2(w, rowh));
    bool hov = ImGui::IsItemHovered(), clicked = ImGui::IsItemDeactivated() && hov;
    if (hov) dl->AddRectFilled(ImVec2(x,y), ImVec2(x+w,y+rowh), SA(kInk,0.06f), 0.0f);
    float cy = y+rowh*0.5f;
    DrawIcon(dl, ic, ImVec2(x+24, cy), 17.0f, kInk, 2.0f);
    dl->AddText(f, 16.0f, ImVec2(x+46, cy-9), kInk, label);
    if (kind==1 && tog){
        float tw=44, th=26, tx=x+w-tw-16;
        Spring& s = g->springs().Get((uint32_t)ImGui::GetID("r"), 3, SpringStyle::Critical, *tog?1.0f:0.0f);
        s.target=*tog?1.0f:0.0f; s.Tick(Dt()); float kc=Clampf(s.x,0,1);
        Primitive tr; tr.cx=tx+tw*0.5f; tr.cy=cy; tr.hw=tw*0.5f; tr.hh=th*0.5f; tr.corner_radius=th*0.5f; tr.fade=1; tr.material=Material::Thin; g->Submit(tr);
        if (kc>0.01f){ Primitive on=tr; on.fade=kc; on.material=Material::Accent; g->Submit(on); }
        float kr=th*0.5f-3.0f, spd=std::min(0.45f, std::fabs(s.v)*0.06f);
        Primitive kn; kn.cx=tx+th*0.5f+kc*(tw-th); kn.cy=cy; kn.hw=kr*(1.0f+spd); kn.hh=kr*(1.0f-spd*0.5f); kn.corner_radius=kr; kn.fade=1; kn.material=Material::Knob; g->Submit(kn);
        if (clicked) *tog=!*tog;
    } else {
        if (value){ ImVec2 vs=f->CalcTextSizeA(15.0f,99999.0f,0,value); dl->AddText(f,15.0f,ImVec2(x+w-vs.x-32,cy-8),kInkSoft,value); }
        DrawIcon(dl, Icon::ChevronR, ImVec2(x+w-16, cy), 13.0f, SA(kInkSoft,0.8f), 2.0f);
    }
    ImGui::PopID();
    return clicked;
}
static void Draw_Settings(float w){
    ImDrawList* dl=ImGui::GetWindowDrawList(); ImFont* f=ImGui::GetFont();
    static bool airplane=false, vpn=false, lowpower=false;
    float cw = w<560.0f?w:560.0f;
    float x0 = ImGui::GetCursorScreenPos().x + (w-cw)*0.5f;
    {
        float h=86.0f; ImVec2 tl(x0, ImGui::GetCursorScreenPos().y);
        Primitive bg; bg.cx=tl.x+cw*0.5f; bg.cy=tl.y+h*0.5f; bg.hw=cw*0.5f; bg.hh=h*0.5f; bg.corner_radius=26.0f; bg.fade=1; bg.material=Material::Thin; g->Submit(bg);
        Primitive av; av.cx=tl.x+46; av.cy=tl.y+h*0.5f; av.hw=26; av.hh=26; av.corner_radius=30; av.fade=1; av.material=Material::Accent; g->Submit(av);
        ImVec2 is=f->CalcTextSizeA(18.0f,99999.0f,0,"PO"); dl->AddText(f,18.0f,ImVec2(tl.x+46-is.x*0.5f, tl.y+h*0.5f-is.y*0.5f), IM_COL32(255,255,255,255), "PO");
        dl->AddText(f,19.0f, ImVec2(tl.x+84, tl.y+22), kInk, "poncipp");
        dl->AddText(f,13.0f, ImVec2(tl.x+84, tl.y+48), kInkSoft, "Glass Account, Cloud, and more");
        DrawIcon(dl, Icon::ChevronR, ImVec2(tl.x+cw-18, tl.y+h*0.5f), 14.0f, SA(kInkSoft,0.8f), 2.0f);
        ImGui::Dummy(ImVec2(w, h+16));
    }
    struct Row{ Icon ic; const char* label; const char* val; int kind; bool* tog; };
    auto section = [&](Row* rows, int n){
        float rowh=48.0f, h=n*rowh; ImVec2 tl(x0, ImGui::GetCursorScreenPos().y);
        Primitive bg; bg.cx=tl.x+cw*0.5f; bg.cy=tl.y+h*0.5f; bg.hw=cw*0.5f; bg.hh=h*0.5f; bg.corner_radius=24.0f; bg.fade=1; bg.material=Material::Thin; g->Submit(bg);
        for (int i=0;i<n;i++){
            SettRow(tl.x, cw, tl.y+i*rowh, rowh, rows[i].ic, rows[i].label, rows[i].val, rows[i].kind, rows[i].tog);
            if (i<n-1) dl->AddLine(ImVec2(tl.x+46, tl.y+(i+1)*rowh), ImVec2(tl.x+cw-12, tl.y+(i+1)*rowh), SA(kInkSoft,0.15f), 1.0f);
        }
        ImGui::Dummy(ImVec2(w, h+16));
    };
    Row s1[]={ {Icon::Airplane,"Airplane Mode",nullptr,1,&airplane}, {Icon::Wifi,"Wi-Fi","GlassNet",0,nullptr},
               {Icon::Bluetooth,"Bluetooth","On",0,nullptr}, {Icon::Signal,"Cellular",nullptr,2,nullptr},
               {Icon::Globe,"VPN",nullptr,1,&vpn} };
    section(s1,5);
    Row s2[]={ {Icon::Bell,"Notifications",nullptr,2,nullptr}, {Icon::Speaker,"Sounds & Haptics",nullptr,2,nullptr},
               {Icon::Moon,"Focus",nullptr,2,nullptr}, {Icon::Hourglass,"Screen Time",nullptr,2,nullptr} };
    section(s2,4);
    Row s3[]={ {Icon::Gear,"General",nullptr,2,nullptr}, {Icon::Display,"Display & Brightness",nullptr,2,nullptr},
               {Icon::Palette,"Wallpaper",nullptr,2,nullptr}, {Icon::Bolt,"Low Power Mode",nullptr,1,&lowpower},
               {Icon::Lock,"Privacy & Security",nullptr,2,nullptr}, {Icon::BatteryFull,"Battery","87%",0,nullptr} };
    section(s3,6);
}
static void DrawAnalog(ImDrawList* dl, ImVec2 c, float r, float hrs, float mins, float secs, ImU32 face, ImU32 hand){
    dl->AddCircleFilled(c, r, SA(face,0.5f), 48);
    dl->AddCircle(c, r, SA(kInk,0.25f), 48, 1.5f);
    for (int i=0;i<12;i++){ float a=i*(6.2831853f/12.0f)-1.5708f; float r0=r*0.82f, r1=r*0.94f;
        dl->AddLine(ImVec2(c.x+cosf(a)*r0,c.y+sinf(a)*r0), ImVec2(c.x+cosf(a)*r1,c.y+sinf(a)*r1), SA(kInkSoft,0.7f), (i%3==0)?2.0f:1.0f); }
    float ah=(hrs/12.0f)*6.2831853f-1.5708f, am=(mins/60.0f)*6.2831853f-1.5708f, as=(secs/60.0f)*6.2831853f-1.5708f;
    dl->AddLine(c, ImVec2(c.x+cosf(ah)*r*0.5f, c.y+sinf(ah)*r*0.5f), hand, 3.5f);
    dl->AddLine(c, ImVec2(c.x+cosf(am)*r*0.72f, c.y+sinf(am)*r*0.72f), hand, 2.5f);
    dl->AddLine(c, ImVec2(c.x+cosf(as)*r*0.8f, c.y+sinf(as)*r*0.8f), Accent(), 1.5f);
    dl->AddCircleFilled(c, 3.0f, Accent());
}
static void Draw_Clock(float w){
    ImDrawList* dl=ImGui::GetWindowDrawList(); ImFont* f=ImGui::GetFont();
    time_t tt=time(nullptr); struct tm lt; localtime_s(&lt,&tt);
    float secs=lt.tm_sec+(float)fmod(ImGui::GetTime(),1.0)*0;
    float cw=w<560.0f?w:560.0f; float x0=ImGui::GetCursorScreenPos().x+(w-cw)*0.5f;
    {
        float h=180.0f; ImVec2 tl(x0, ImGui::GetCursorScreenPos().y);
        Primitive bg; bg.cx=tl.x+cw*0.5f; bg.cy=tl.y+h*0.5f; bg.hw=cw*0.5f; bg.hh=h*0.5f; bg.corner_radius=29.0f; bg.fade=1; bg.material=Material::Thin; g->Submit(bg);
        DrawAnalog(dl, ImVec2(tl.x+96, tl.y+h*0.5f), 64.0f, lt.tm_hour%12 + lt.tm_min/60.0f, (float)lt.tm_min, (float)lt.tm_sec, Accent(), kInk);
        char buf[16]; FmtClock(buf,16,lt.tm_hour,lt.tm_min);
        dl->AddText(f,52.0f, ImVec2(tl.x+196, tl.y+44), kInk, buf);
        const char* ap = lt.tm_hour>=12?"PM":"AM"; dl->AddText(f,20.0f, ImVec2(tl.x+196, tl.y+104), kInkSoft, ap);
        static const char* wd[]={"Sunday","Monday","Tuesday","Wednesday","Thursday","Friday","Saturday"};
        static const char* mo[]={"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
        char db[48]; snprintf(db,48,"%s, %s %d", wd[lt.tm_wday%7], mo[lt.tm_mon%12], lt.tm_mday);
        dl->AddText(f,15.0f, ImVec2(tl.x+198, tl.y+136), kInkSoft, db);
        ImGui::Dummy(ImVec2(w, h+16));
    }
    {
        dl->AddText(f,12.0f, ImVec2(x0+4, ImGui::GetCursorScreenPos().y), kInkSoft, "WORLD CLOCK");
        ImGui::Dummy(ImVec2(w, 22.0f));
        struct C{ const char* city; int off; }; C cs[]={{"Cupertino",0},{"New York",3},{"London",8},{"Tokyo",17}};
        int n=4; float gap=14, tw=(cw-3*gap)/4, th=130; float sy=ImGui::GetCursorScreenPos().y;
        for (int i=0;i<n;i++){
            float x=x0+i*(tw+gap);
            Primitive bg; bg.cx=x+tw*0.5f; bg.cy=sy+th*0.5f; bg.hw=tw*0.5f; bg.hh=th*0.5f; bg.corner_radius=24.0f; bg.fade=1; bg.material=Material::Thin; g->Submit(bg);
            int hh=(lt.tm_hour+cs[i].off)%24; if(hh<0)hh+=24;
            DrawAnalog(dl, ImVec2(x+tw*0.5f, sy+44), 30.0f, hh%12 + lt.tm_min/60.0f, (float)lt.tm_min, (float)lt.tm_sec, Accent(), kInk);
            ImVec2 cs2=f->CalcTextSizeA(13.0f,99999.0f,0,cs[i].city); dl->AddText(f,13.0f,ImVec2(x+tw*0.5f-cs2.x*0.5f, sy+84), kInk, cs[i].city);
            char tb[16]; FmtClock(tb,16,hh,lt.tm_min);
            ImVec2 ts=f->CalcTextSizeA(16.0f,99999.0f,0,tb); dl->AddText(f,16.0f,ImVec2(x+tw*0.5f-ts.x*0.5f, sy+102), kInkSoft, tb);
        }
        ImGui::Dummy(ImVec2(w, th+8));
    }
    (void)secs;
}
static void Draw_Photos(float w){
    ImDrawList* dl=ImGui::GetWindowDrawList(); ImFont* f=ImGui::GetFont();
    float cw=w<720.0f?w:720.0f; float x0=ImGui::GetCursorScreenPos().x+(w-cw)*0.5f;
    static int sel=-1; if(sel>=20) sel=-1;
    auto tileColor=[&](int i)->ImU32{ float hue=fmodf(i*0.137f,1.0f); float hh=hue*6.0f; int hi=(int)hh; float fr=hh-hi; float s=0.45f,v=0.78f;
        float p=v*(1-s), q=v*(1-s*fr), tcol=v*(1-s*(1-fr)); float rr,gg,bb;
        switch(hi%6){case 0:rr=v;gg=tcol;bb=p;break;case 1:rr=q;gg=v;bb=p;break;case 2:rr=p;gg=v;bb=tcol;break;
                    case 3:rr=p;gg=q;bb=v;break;case 4:rr=tcol;gg=p;bb=v;break;default:rr=v;gg=p;bb=q;break;}
        return IM_COL32((int)(rr*255),(int)(gg*255),(int)(bb*255),255); };
    if(sel>=0){
        float bh=44.0f; ImVec2 tb(x0, ImGui::GetCursorScreenPos().y);
        bool bhv=false; if(SurfaceHit("phback", tb, ImVec2(140,bh), &bhv, Sfx::NavPop)) sel=-1;
        DrawIcon(dl, Icon::ChevronL, ImVec2(tb.x+10, tb.y+bh*0.5f), 15.0f, Accent(), 2.4f);
        dl->AddText(f,16.0f, ImVec2(tb.x+24, tb.y+bh*0.5f-9), Accent(), "Library");
        ImGui::Dummy(ImVec2(w, bh));
        float h=400.0f; ImVec2 tl(x0, ImGui::GetCursorScreenPos().y);
        ImU32 col=tileColor(sel); ImgOrGrad(dl, ImVec2(tl.x,tl.y), ImVec2(tl.x+cw,tl.y+h), sel, col, col, 18.0f);
        dl->AddRect(ImVec2(tl.x,tl.y),ImVec2(tl.x+cw,tl.y+h), SA(IM_COL32(255,255,255,255),0.18f),18.0f,0,1.5f);
        char cap[32]; snprintf(cap,32,"Photo %d of 20",sel+1); ImVec2 cs=f->CalcTextSizeA(15.0f,99999.0f,0,cap);
        dl->AddText(f,15.0f,ImVec2(tl.x+cw*0.5f-cs.x*0.5f,tl.y+h+10),kInkSoft,cap);
        ImGui::Dummy(ImVec2(w, h+36));
        return;
    }
    dl->AddText(f,30.0f, ImVec2(x0+2, ImGui::GetCursorScreenPos().y), kInk, "Photos");
    dl->AddText(f,14.0f, ImVec2(x0+2, ImGui::GetCursorScreenPos().y+38), kInkSoft, "12,840 photos \xC2\xB7 312 videos");
    ImGui::Dummy(ImVec2(w, 70.0f));
    {
        float h=150.0f; ImVec2 tl(x0, ImGui::GetCursorScreenPos().y);
        { void* hp = PhotoTex(9);
          if (hp) dl->AddImageRounded((ImTextureID)(intptr_t)hp, ImVec2(tl.x,tl.y), ImVec2(tl.x+cw,tl.y+h), ImVec2(0,0), ImVec2(1,1), IM_COL32(255,255,255,255), 18.0f);
          else dl->AddRectFilledMultiColor(ImVec2(tl.x,tl.y), ImVec2(tl.x+cw,tl.y+h), IM_COL32(120,90,200,255), IM_COL32(220,110,150,255), IM_COL32(255,170,90,255), IM_COL32(150,120,210,255)); }
        dl->AddRectFilledMultiColor(ImVec2(tl.x,tl.y+h*0.45f), ImVec2(tl.x+cw,tl.y+h), IM_COL32(0,0,0,0),IM_COL32(0,0,0,0),IM_COL32(0,0,0,150),IM_COL32(0,0,0,150));
        dl->AddRect(ImVec2(tl.x,tl.y), ImVec2(tl.x+cw,tl.y+h), SA(IM_COL32(255,255,255,255),0.18f), 18.0f, 0, 1.5f);
        dl->AddText(f,13.0f, ImVec2(tl.x+18, tl.y+16), IM_COL32(255,255,255,220), "FOR YOU");
        dl->AddText(f,26.0f, ImVec2(tl.x+18, tl.y+h-52), IM_COL32(255,255,255,255), "Weekend in Big Sur");
        dl->AddText(f,14.0f, ImVec2(tl.x+18, tl.y+h-22), IM_COL32(255,255,255,205), "32 photos \xC2\xB7 Memory");
        ImGui::Dummy(ImVec2(w, h+16));
    }
    {
        int cols=5, n=20; float gap=8.0f; float tile=(cw-(cols-1)*gap)/cols; float sy=ImGui::GetCursorScreenPos().y;
        for (int i=0;i<n;i++){
            int r=i/cols, c=i%cols; float x=x0+c*(tile+gap), y=sy+r*(tile+gap);
            ImU32 col=tileColor(i); bool hov=false; char id[8]; snprintf(id,8,"ph%d",i);
            if(SurfaceHit(id, ImVec2(x,y), ImVec2(tile,tile), &hov, Sfx::NavPush)) sel=i;
            ImgOrGrad(dl, ImVec2(x,y), ImVec2(x+tile,y+tile), i, col, col, 12.0f);
            if(hov) dl->AddRect(ImVec2(x,y),ImVec2(x+tile,y+tile), SA(IM_COL32(255,255,255,255),0.7f), 12.0f, 0, 2.0f);
        }
        int rows=(n+cols-1)/cols; ImGui::Dummy(ImVec2(w, rows*(tile+gap)+4));
    }
}
static void Draw_NotifCenter(float w){
    ImDrawList* dl=ImGui::GetWindowDrawList(); ImFont* f=ImGui::GetFont();
    float cw=w<520.0f?w:520.0f; float x0=ImGui::GetCursorScreenPos().x+(w-cw)*0.5f;
    time_t tt=time(nullptr); struct tm lt; localtime_s(&lt,&tt);
    { char tb[16]; FmtClock(tb,16,lt.tm_hour,lt.tm_min);
      ImVec2 ts=f->CalcTextSizeA(64.0f,99999.0f,0,tb);
      dl->AddText(f,64.0f, ImVec2(x0+cw*0.5f-ts.x*0.5f, ImGui::GetCursorScreenPos().y), kInk, tb);
      static const char* wd[]={"Sunday","Monday","Tuesday","Wednesday","Thursday","Friday","Saturday"};
      static const char* mo[]={"January","February","March","April","May","June","July","August","September","October","November","December"};
      char db[48]; snprintf(db,48,"%s, %s %d", wd[lt.tm_wday%7], mo[lt.tm_mon%12], lt.tm_mday);
      ImVec2 ds=f->CalcTextSizeA(16.0f,99999.0f,0,db); dl->AddText(f,16.0f, ImVec2(x0+cw*0.5f-ds.x*0.5f, ImGui::GetCursorScreenPos().y+72), kInkSoft, db);
      ImGui::Dummy(ImVec2(w, 110.0f)); }
    struct N{ Icon ic; const char* app; const char* time; const char* title; const char* body; };
    N ns[]={
        {Icon::Envelope,"MAIL","now","Olivia Bennett","Lunch on Friday? Let me know what works \xF0\x9F\x99\x82"},
        {Icon::Comment,"MESSAGES","2m ago","Emma","Are we still on for dinner tonight? \xF0\x9F\x98\x84"},
        {Icon::CalendarDays,"CALENDAR","8m ago","Design Review","In 15 minutes \xC2\xB7 Conference Room B"},
        {Icon::CloudSun,"WEATHER","1h ago","Cupertino","Sunny skies through the afternoon, 74\xC2\xB0."},
    };
    int n=4;
    for (int i=0;i<n;i++){
        float h=78.0f; ImVec2 tl(x0, ImGui::GetCursorScreenPos().y);
        Primitive bg; bg.cx=tl.x+cw*0.5f; bg.cy=tl.y+h*0.5f; bg.hw=cw*0.5f; bg.hh=h*0.5f; bg.corner_radius=24.0f; bg.fade=1; bg.material=Material::Thin; g->Submit(bg);
        Primitive tile; tile.cx=tl.x+34; tile.cy=tl.y+30; tile.hw=15; tile.hh=15; tile.corner_radius=8; tile.fade=1; tile.material=Material::Accent; g->Submit(tile);
        DrawIcon(dl, ns[i].ic, ImVec2(tl.x+34, tl.y+30), 16.0f, IM_COL32(255,255,255,255), 2.0f);
        dl->AddText(f,12.0f, ImVec2(tl.x+60, tl.y+14), kInkSoft, ns[i].app);
        ImVec2 ts=f->CalcTextSizeA(12.0f,99999.0f,0,ns[i].time); dl->AddText(f,12.0f, ImVec2(tl.x+cw-ts.x-16, tl.y+14), kInkSoft, ns[i].time);
        dl->AddText(f,15.0f, ImVec2(tl.x+18, tl.y+34), kInk, ns[i].title);
        dl->AddText(f,14.0f, ImVec2(tl.x+18, tl.y+54), kInkSoft, ns[i].body);
        ImGui::Dummy(ImVec2(w, h+12));
    }
}
static void Sparkline(ImDrawList* dl, ImVec2 tl, float w, float h, const float* v, int n, ImU32 col){
    if (n<2) return; float mn=v[0],mx=v[0]; for(int i=1;i<n;i++){mn=fminf(mn,v[i]);mx=fmaxf(mx,v[i]);}
    float rng=mx-mn; if(rng<1e-6f)rng=1.0f;
    for(int i=0;i<n-1;i++){
        float x0=tl.x+w*i/(n-1), x1=tl.x+w*(i+1)/(n-1);
        float y0=tl.y+h-(v[i]-mn)/rng*h, y1=tl.y+h-(v[i+1]-mn)/rng*h;
        dl->AddLine(ImVec2(x0,y0),ImVec2(x1,y1),col,1.8f);
    }
}
static void AreaChart(ImDrawList* dl, ImVec2 tl, float w, float h, const float* v, int n, ImU32 line){
    if (n<2) return; float mn=v[0],mx=v[0]; for(int i=1;i<n;i++){mn=fminf(mn,v[i]);mx=fmaxf(mx,v[i]);}
    float rng=(mx-mn)*1.2f; if(rng<1e-6f)rng=1.0f; mn-=rng*0.08f;
    auto px=[&](int i){return tl.x+w*i/(float)(n-1);}; auto py=[&](int i){return tl.y+h-(v[i]-mn)/rng*h;};
    for(int i=0;i<n-1;i++){ ImVec2 a(px(i),py(i)),b(px(i+1),py(i+1));
        dl->AddQuadFilled(ImVec2(a.x,tl.y+h),a,b,ImVec2(b.x,tl.y+h), SA(line,0.16f)); }
    for(int i=0;i<n-1;i++) dl->AddLine(ImVec2(px(i),py(i)),ImVec2(px(i+1),py(i+1)),line,2.4f);
}
