static void Draw_Translate(float w){
    ImDrawList* dl=ImGui::GetWindowDrawList(); ImFont* f=ImGui::GetFont();
    float cw=w<560.0f?w:560.0f; float x0=ImGui::GetCursorScreenPos().x+(w-cw)*0.5f;
    { float h=48.0f; ImVec2 tl(x0, ImGui::GetCursorScreenPos().y);
      Primitive bg; bg.cx=tl.x+cw*0.5f; bg.cy=tl.y+h*0.5f; bg.hw=cw*0.5f; bg.hh=h*0.5f; bg.corner_radius=h*0.5f; bg.fade=1; bg.material=Material::Thin; g->Submit(bg);
      dl->AddText(f,16.0f, ImVec2(tl.x+34, tl.y+h*0.5f-9), kInk, "English");
      DrawIcon(dl, Icon::Refresh, ImVec2(tl.x+cw*0.5f, tl.y+h*0.5f), 16.0f, Accent(), 2.0f);
      ImVec2 ss=f->CalcTextSizeA(16.0f,99999.0f,0,"Spanish"); dl->AddText(f,16.0f, ImVec2(tl.x+cw-ss.x-34, tl.y+h*0.5f-9), kInk, "Spanish");
      ImGui::Dummy(ImVec2(w, h+16)); }
    static char xsrc[256] = "Where is the nearest cafe?";
    { float h=120.0f; ImVec2 tl(x0, ImGui::GetCursorScreenPos().y);
      Primitive bg; bg.cx=tl.x+cw*0.5f; bg.cy=tl.y+h*0.5f; bg.hw=cw*0.5f; bg.hh=h*0.5f; bg.corner_radius=26; bg.fade=1; bg.material=Material::Thin; g->Submit(bg);
      dl->AddText(f,12.0f, ImVec2(tl.x+20, tl.y+14), kInkSoft, "ENGLISH");
      ImGui::SetCursorScreenPos(ImVec2(tl.x+18, tl.y+32));
      ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0,0,0,0)); ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0,0,0,0)); ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0,0,0,0)); ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.93f,0.93f,0.96f,1.0f));
      ImGui::InputTextMultiline("##xsrc", xsrc, sizeof(xsrc), ImVec2(cw-36, h-50));
      ImGui::PopStyleColor(4);
      DrawIcon(dl, Icon::Speaker, ImVec2(tl.x+cw-30, tl.y+h-26), 16.0f, kInkSoft, 2.0f);
      ImGui::Dummy(ImVec2(w, h+14)); }
    { float h=120.0f; ImVec2 tl(x0, ImGui::GetCursorScreenPos().y);
      Primitive bg; bg.cx=tl.x+cw*0.5f; bg.cy=tl.y+h*0.5f; bg.hw=cw*0.5f; bg.hh=h*0.5f; bg.corner_radius=26; bg.fade=1; bg.material=Material::Accent; g->Submit(bg);
      dl->AddText(f,12.0f, ImVec2(tl.x+20, tl.y+14), SA(IM_COL32(255,255,255,255),0.8f), "SPANISH");
      dl->AddText(f,22.0f, ImVec2(tl.x+20, tl.y+38), IM_COL32(255,255,255,255),
                  "\xC2\xBF" "D\xC3\xB3nde est\xC3\xA1 la cafeter\xC3\xAD" "a m\xC3\xA1s cercana?", NULL, cw-40);
      DrawIcon(dl, Icon::Speaker, ImVec2(tl.x+cw-30, tl.y+h-26), 16.0f, IM_COL32(255,255,255,220), 2.0f);
      ImGui::Dummy(ImVec2(w, h+14)); }
}
static bool RoundTog(const char* id, ImVec2 c, float r, Icon ic, bool* on){
    ImGui::PushID(id); ImGui::SetCursorScreenPos(ImVec2(c.x-r,c.y-r)); ImGui::InvisibleButton("t",ImVec2(2*r,2*r));
    bool hov=ImGui::IsItemHovered(), act=ImGui::IsItemActive(), clk=ImGui::IsItemDeactivated()&&hov; if(clk)*on=!*on;
    Spring& sp=g->springs().Get((uint32_t)ImGui::GetID("t"),1,SpringStyle::Bouncy,1.0f);
    sp.target=act?0.90f:(hov?1.06f:1.0f); sp.Tick(Dt()); float rr=r*Clampf(sp.x,0.8f,1.2f);
    Spring& so=g->springs().Get((uint32_t)ImGui::GetID("t"),2,SpringStyle::Critical,*on?1.0f:0.0f);
    so.target=*on?1.0f:0.0f; so.Tick(Dt()); float k=Clampf(so.x,0,1);
    Primitive b; b.cx=c.x;b.cy=c.y;b.hw=rr;b.hh=rr;b.corner_radius=rr;b.fade=1;b.material=Material::Thick;b.elevate=hov?1.12f:1.0f; g->Submit(b);
    if(k>0.01f){ Primitive on2=b; on2.fade=k; on2.material=Material::Accent; g->Submit(on2); }
    DrawIcon(ImGui::GetWindowDrawList(),ic,c,rr*0.95f, k>0.5f?IM_COL32(255,255,255,255):kInk,2.0f);
    ImGui::PopID(); return clk;
}
static void Draw_CControl(float w){
    ImDrawList* dl=ImGui::GetWindowDrawList(); ImFont* f=ImGui::GetFont();
    static bool air=false, cell=true, wifi=true, bt=true, focus=false, rot=false;
    static float bright=0.8f, vol=0.6f;
    float cw=w<520.0f?w:520.0f; float x0=ImGui::GetCursorScreenPos().x+(w-cw)*0.5f;
    float gap=14.0f, mw=(cw-gap)/2.0f; float top=ImGui::GetCursorScreenPos().y;
    { float h=150.0f; ImVec2 tl(x0, top);
      Primitive bg; bg.cx=tl.x+mw*0.5f; bg.cy=tl.y+h*0.5f; bg.hw=mw*0.5f; bg.hh=h*0.5f; bg.corner_radius=28; bg.fade=1; bg.material=Material::Thin; g->Submit(bg);
      float r=26.0f; RoundTog("air",ImVec2(tl.x+mw*0.30f,tl.y+h*0.32f),r,Icon::Airplane,&air);
      RoundTog("cell",ImVec2(tl.x+mw*0.70f,tl.y+h*0.32f),r,Icon::Signal,&cell);
      RoundTog("wifi",ImVec2(tl.x+mw*0.30f,tl.y+h*0.70f),r,Icon::Wifi,&wifi);
      RoundTog("bt",ImVec2(tl.x+mw*0.70f,tl.y+h*0.70f),r,Icon::Bluetooth,&bt); }
    { float h=68.0f;
      ImVec2 t1(x0+mw+gap, top);
      ImGui::SetCursorScreenPos(ImVec2(t1.x,t1.y)); ImGui::PushID("focus"); ImGui::InvisibleButton("b",ImVec2(mw,h)); bool fh=ImGui::IsItemHovered(); if(ImGui::IsItemDeactivated()&&fh) focus=!focus; float fok=TileOn("b",focus); ImGui::PopID();
      Primitive b1; b1.cx=t1.x+mw*0.5f; b1.cy=t1.y+h*0.5f; b1.hw=mw*0.5f; b1.hh=h*0.5f; b1.corner_radius=26; b1.fade=1; b1.material=Material::Thin; b1.elevate=fh?1.06f:1.0f; g->Submit(b1);
      if(fok>0.01f){ Primitive o=b1; o.fade=fok; o.material=Material::Accent; g->Submit(o); }
      DrawIcon(dl,Icon::Moon,ImVec2(t1.x+28,t1.y+h*0.5f),18.0f,fok>0.5f?IM_COL32(255,255,255,255):kInk,2.0f);
      dl->AddText(f,16.0f,ImVec2(t1.x+50,t1.y+h*0.5f-9),fok>0.5f?IM_COL32(255,255,255,255):kInk,"Focus");
      ImVec2 t2(x0+mw+gap, top+h+14); float h2b=150-h-14;
      ImGui::SetCursorScreenPos(ImVec2(t2.x,t2.y)); ImGui::PushID("rot"); ImGui::InvisibleButton("b",ImVec2(mw,h2b)); bool rh=ImGui::IsItemHovered(); if(ImGui::IsItemDeactivated()&&rh) rot=!rot; float rok=TileOn("b",rot); ImGui::PopID();
      Primitive b2; b2.cx=t2.x+mw*0.5f; b2.cy=t2.y+h2b*0.5f; b2.hw=mw*0.5f; b2.hh=h2b*0.5f; b2.corner_radius=26; b2.fade=1; b2.material=Material::Thin; b2.elevate=rh?1.06f:1.0f; g->Submit(b2);
      if(rok>0.01f){ Primitive o=b2; o.fade=rok; o.material=Material::Accent; g->Submit(o); }
      DrawIcon(dl,Icon::Refresh,ImVec2(t2.x+28,t2.y+h2b*0.5f),18.0f,rok>0.5f?IM_COL32(255,255,255,255):kInk,2.0f);
      dl->AddText(f,15.0f,ImVec2(t2.x+50,t2.y+h2b*0.5f-9),rok>0.5f?IM_COL32(255,255,255,255):kInk,"Lock"); }
    ImGui::Dummy(ImVec2(w, 150.0f+gap));
    { float h=180.0f, top2=ImGui::GetCursorScreenPos().y;
      struct V{ const char* id; Icon ic; float* v; }; V vs[2]={ {"bri",Icon::Sun,&bright},{"vol",Icon::Speaker,&vol} };
      for(int i=0;i<2;i++){ float x=x0+i*(mw+gap); ImVec2 tl(x, top2);
        Primitive bg; bg.cx=tl.x+mw*0.5f; bg.cy=tl.y+h*0.5f; bg.hw=mw*0.5f; bg.hh=h*0.5f; bg.corner_radius=28; bg.fade=1; bg.material=Material::Thin; g->Submit(bg);
        float fillH=(h-24)* *vs[i].v;
        ImGui::SetCursorScreenPos(ImVec2(tl.x+12,tl.y+12)); ImGui::PushID(vs[i].id); ImGui::InvisibleButton("s",ImVec2(mw-24,h-24));
        if(ImGui::IsItemActive()){ float my=ImGui::GetIO().MousePos.y; *vs[i].v=Clampf(1.0f-(my-(tl.y+12))/(h-24),0,1); } ImGui::PopID();
        Primitive fl; fl.cx=tl.x+mw*0.5f; fl.cy=tl.y+12+(h-24)-fillH*0.5f; fl.hw=(mw-24)*0.5f; fl.hh=fillH*0.5f; fl.corner_radius=22; fl.fade=1; fl.material=Material::Accent; g->Submit(fl);
        DrawIcon(dl,vs[i].ic,ImVec2(tl.x+mw*0.5f,tl.y+h-24),18.0f,IM_COL32(255,255,255,235),2.0f); }
      ImGui::Dummy(ImVec2(w, h+gap)); }
    { float th=64.0f, top3=ImGui::GetCursorScreenPos().y; Icon ic[4]={Icon::Bolt,Icon::Stopwatch,Icon::Calculator,Icon::Camera}; float tw=(cw-3*gap)/4.0f;
      for(int i=0;i<4;i++){ float x=x0+i*(tw+gap); Primitive b; b.cx=x+tw*0.5f; b.cy=top3+th*0.5f; b.hw=tw*0.5f; b.hh=th*0.5f; b.corner_radius=24; b.fade=1; b.material=Material::Thin; g->Submit(b);
        DrawIcon(dl,ic[i],ImVec2(x+tw*0.5f,top3+th*0.5f),20.0f,kInk,2.2f); }
      ImGui::Dummy(ImVec2(w, th+10)); }
}
static void Draw_Fitness(float w){
    ImDrawList* dl=ImGui::GetWindowDrawList(); ImFont* f=ImGui::GetFont();
    float cw=w<600.0f?w:600.0f; float x0=ImGui::GetCursorScreenPos().x+(w-cw)*0.5f;
    { float h=200.0f; ImVec2 tl(x0, ImGui::GetCursorScreenPos().y);
      Primitive bg; bg.cx=tl.x+cw*0.5f; bg.cy=tl.y+h*0.5f; bg.hw=cw*0.5f; bg.hh=h*0.5f; bg.corner_radius=29; bg.fade=1; bg.material=Material::Thin; g->Submit(bg);
      dl->AddText(f,12.0f, ImVec2(tl.x+22, tl.y+16), kInkSoft, "THIS WEEK");
      dl->AddText(f,40.0f, ImVec2(tl.x+22, tl.y+34), kInk, "3,182");
      dl->AddText(f,14.0f, ImVec2(tl.x+22, tl.y+82), kInkSoft, "active calories");
      const char* days[7]={"M","T","W","T","F","S","S"}; float val[7]={0.5f,0.7f,0.45f,0.9f,0.6f,0.3f,0.8f};
      float bx0=tl.x+22, bw2=cw-44, bh=70.0f, by=tl.y+h-30; float cwid=bw2/7.0f;
      for(int i=0;i<7;i++){ float cx=bx0+cwid*(i+0.5f); float hh=bh*val[i];
        dl->AddRectFilled(ImVec2(cx-12,by-bh),ImVec2(cx+12,by), SA(kInkSoft,0.18f), 6.0f);
        dl->AddRectFilled(ImVec2(cx-12,by-hh),ImVec2(cx+12,by), i==3?IM_COL32(126,255,40,255):Accent(), 6.0f);
        ImVec2 ds=f->CalcTextSizeA(12.0f,99999.0f,0,days[i]); dl->AddText(f,12.0f,ImVec2(cx-ds.x*0.5f,by+6),kInkSoft,days[i]); }
      ImGui::Dummy(ImVec2(w, h+14)); }
    { dl->AddText(f,18.0f, ImVec2(x0+4, ImGui::GetCursorScreenPos().y), kInk, "Recent Workouts"); ImGui::Dummy(ImVec2(w,28.0f));
      struct Wk{ Icon ic; const char* n; const char* d; const char* cal; ImU32 c; }; Wk ws[]={ {Icon::Running,"Outdoor Run","32:14 \xC2\xB7 3.8 mi","412 CAL",IM_COL32(126,255,40,255)},{Icon::Bicycle,"Cycling","48:02 \xC2\xB7 12.1 mi","506 CAL",IM_COL32(64,220,255,255)},{Icon::Dumbbell,"Strength","41:30","288 CAL",IM_COL32(255,55,95,255)},{Icon::Walking,"Walk","18:45 \xC2\xB7 1.2 mi","96 CAL",IM_COL32(255,159,10,255)} };
      float rowh=64.0f, h=4*rowh; ImVec2 tl(x0, ImGui::GetCursorScreenPos().y);
      Primitive bg; bg.cx=tl.x+cw*0.5f; bg.cy=tl.y+h*0.5f; bg.hw=cw*0.5f; bg.hh=h*0.5f; bg.corner_radius=24; bg.fade=1; bg.material=Material::Thin; g->Submit(bg);
      for(int i=0;i<4;i++){ float ry=tl.y+i*rowh, cy=ry+rowh*0.5f;
        dl->AddCircleFilled(ImVec2(tl.x+34,cy), 18.0f, SA(ws[i].c,0.22f)); DrawIcon(dl,ws[i].ic,ImVec2(tl.x+34,cy),18.0f,ws[i].c,2.2f);
        dl->AddText(f,16.0f,ImVec2(tl.x+62,cy-17),kInk,ws[i].n);
        dl->AddText(f,13.0f,ImVec2(tl.x+62,cy+3),kInkSoft,ws[i].d);
        ImVec2 cs=f->CalcTextSizeA(15.0f,99999.0f,0,ws[i].cal); dl->AddText(f,15.0f,ImVec2(tl.x+cw-cs.x-16,cy-8),ws[i].c,ws[i].cal);
        if(i<3) dl->AddLine(ImVec2(tl.x+62,ry+rowh),ImVec2(tl.x+cw-14,ry+rowh),SA(kInkSoft,0.14f),1.0f); }
      ImGui::Dummy(ImVec2(w, h+12)); }
}
static void Draw_News(float w){
    ImDrawList* dl=ImGui::GetWindowDrawList(); ImFont* f=ImGui::GetFont();
    float cw=w<640.0f?w:640.0f; float x0=ImGui::GetCursorScreenPos().x+(w-cw)*0.5f;
    struct Ar{ const char* src; const char* head; const char* time; const char* body; ImU32 c; };
    static Ar hero={ "TECHNOLOGY \xC2\xB7 THE VERGE","Nova unveils its most ambitious software update yet","now",
        "At its annual developer conference, the company introduced a sweeping redesign alongside deeper on-device intelligence. We break down what's new, what's changed, and what it means for the apps you use every day.", IM_COL32(60,80,120,255) };
    static Ar ar[]={
        {"WIRED","The quiet rise of on-device machine learning","2h","More of the models that power your phone now run locally. That means faster results, stronger privacy, and features that keep working even when you're offline.",IM_COL32(120,120,128,255)},
        {"The New York Times","Markets steady as technology leads modest gains","4h","Major indexes edged higher on Thursday, lifted by strength in technology shares ahead of next week's earnings reports.",IM_COL32(64,156,255,255)},
        {"The Athletic","A title race that comes down to the final week","6h","Three clubs remain in contention with two matches to play. Here's every scenario, and exactly what each side needs to lift the trophy.",IM_COL32(126,200,40,255)},
        {"National Geographic","How cities are planting their way cooler","Yesterday","Facing hotter summers, urban planners are turning to trees, green roofs, and reflective streets to bring temperatures back down.",IM_COL32(255,90,140,255)},
        {"Bon App\xC3\xA9tit","Five weeknight dinners in under 30 minutes","Yesterday","Fast, forgiving, and built around pantry staples \xE2\x80\x94 these are the recipes our editors keep coming back to.",IM_COL32(255,149,0,255)} };
    static int openIdx=-1;
    if(openIdx>=0){
        Ar& a = (openIdx==5)? hero : ar[openIdx];
        float bh=44.0f; ImVec2 tb(x0, ImGui::GetCursorScreenPos().y);
        bool bhv=false; if(SurfaceHit("newsback", tb, ImVec2(120,bh), &bhv, Sfx::NavPop)) openIdx=-1;
        DrawIcon(dl, Icon::ChevronL, ImVec2(tb.x+10, tb.y+bh*0.5f), 15.0f, Accent(), 2.4f);
        dl->AddText(f,16.0f, ImVec2(tb.x+24, tb.y+bh*0.5f-9), Accent(), "Today");
        ImGui::Dummy(ImVec2(w, bh));
        float h=300.0f; ImVec2 tl(x0, ImGui::GetCursorScreenPos().y);
        Primitive bg; bg.cx=tl.x+cw*0.5f; bg.cy=tl.y+h*0.5f; bg.hw=cw*0.5f; bg.hh=h*0.5f; bg.corner_radius=24; bg.fade=1; bg.material=Material::Thin; g->Submit(bg);
        dl->AddText(f,12.0f, ImVec2(tl.x+20, tl.y+18), kInkSoft, a.src);
        dl->AddText(f,24.0f, ImVec2(tl.x+20, tl.y+38), kInk, a.head, NULL, cw-40);
        dl->AddLine(ImVec2(tl.x+18,tl.y+104),ImVec2(tl.x+cw-18,tl.y+104),SA(kInkSoft,0.16f),1.0f);
        dl->AddText(f,15.0f, ImVec2(tl.x+20, tl.y+120), kInk, a.body, NULL, cw-40);
        ImGui::Dummy(ImVec2(w, h+12));
        return;
    }
    dl->AddText(f,30.0f, ImVec2(x0+2, ImGui::GetCursorScreenPos().y), kInk, "Today");
    ImGui::Dummy(ImVec2(w, 48.0f));
    { float h=200.0f; ImVec2 tl(x0, ImGui::GetCursorScreenPos().y);
      bool hv=false; if(SurfaceHit("newshero", tl, ImVec2(cw,h), &hv, Sfx::NavPush)) openIdx=5;
      dl->AddRectFilledMultiColor(ImVec2(tl.x,tl.y),ImVec2(tl.x+cw,tl.y+h), IM_COL32(30,40,60,255),IM_COL32(60,80,120,255),IM_COL32(90,60,80,255),IM_COL32(40,50,70,255));
      dl->AddRect(ImVec2(tl.x,tl.y),ImVec2(tl.x+cw,tl.y+h), SA(IM_COL32(255,255,255,255),hv?0.30f:0.14f),20.0f,0,1.5f);
      dl->AddText(f,12.0f,ImVec2(tl.x+20,tl.y+18),SA(IM_COL32(255,255,255,255),0.8f),"TECHNOLOGY \xC2\xB7 THE VERGE");
      dl->AddText(f,26.0f,ImVec2(tl.x+20,tl.y+h-78),IM_COL32(255,255,255,255),"Nova unveils its most ambitious update yet",NULL,cw-40);
      dl->AddText(f,13.0f,ImVec2(tl.x+20,tl.y+h-26),SA(IM_COL32(255,255,255,255),0.8f),"A sweeping redesign, plus deeper on-device intelligence");
      ImGui::Dummy(ImVec2(w, h+16)); }
    { float rowh=92.0f, h=5*rowh; ImVec2 tl(x0, ImGui::GetCursorScreenPos().y);
      Primitive bg; bg.cx=tl.x+cw*0.5f; bg.cy=tl.y+h*0.5f; bg.hw=cw*0.5f; bg.hh=h*0.5f; bg.corner_radius=24; bg.fade=1; bg.material=Material::Thin; g->Submit(bg);
      for(int i=0;i<5;i++){ float ry=tl.y+i*rowh; bool hov=false;
        if(SurfaceHit(ar[i].head, ImVec2(tl.x,ry), ImVec2(cw,rowh), &hov, Sfx::NavPush)) openIdx=i;
        RowWash(dl, ImVec2(tl.x+6,ry+4), ImVec2(tl.x+cw-6,ry+rowh-4), 14.0f, false, hov);
        dl->AddText(f,12.0f,ImVec2(tl.x+18,ry+14),kInkSoft,ar[i].src);
        dl->AddText(f,16.0f,ImVec2(tl.x+18,ry+32),kInk,ar[i].head,NULL,cw-150);
        dl->AddText(f,12.0f,ImVec2(tl.x+18,ry+rowh-22),SA(kInkSoft,0.8f),ar[i].time);
        dl->AddRectFilled(ImVec2(tl.x+cw-104,ry+16),ImVec2(tl.x+cw-16,ry+rowh-16), ar[i].c, 10.0f);
        if(i<4) dl->AddLine(ImVec2(tl.x+18,ry+rowh),ImVec2(tl.x+cw-16,ry+rowh),SA(kInkSoft,0.14f),1.0f); }
      ImGui::Dummy(ImVec2(w, h+12)); }
}
static void Draw_Books(float w){
    ImDrawList* dl=ImGui::GetWindowDrawList(); ImFont* f=ImGui::GetFont();
    float cw=w<640.0f?w:640.0f; float x0=ImGui::GetCursorScreenPos().x+(w-cw)*0.5f;
    struct Bk{ const char* t; const char* author; float prog; ImU32 a; ImU32 b; };
    static Bk bks[]={ {"The Midnight Library","by Matt Haig",0.62f,IM_COL32(60,90,160,255),IM_COL32(90,140,210,255)},{"Project Hail Mary","by Andy Weir",0.20f,IM_COL32(40,140,90,255),IM_COL32(90,200,140,255)},{"Circe","by Madeline Miller",0.45f,IM_COL32(150,60,120,255),IM_COL32(210,110,170,255)},{"Atomic Habits","by James Clear",0.10f,IM_COL32(180,120,40,255),IM_COL32(230,180,90,255)},{"Dune","by Frank Herbert",0.78f,IM_COL32(80,80,90,255),IM_COL32(130,130,150,255)},{"Educated","by Tara Westover",0.33f,IM_COL32(120,60,160,255),IM_COL32(170,110,210,255)},{"Sapiens","by Y. N. Harari",0.55f,IM_COL32(160,70,70,255),IM_COL32(220,120,110,255)},{"The Alchemist","by Paulo Coelho",0.05f,IM_COL32(50,120,150,255),IM_COL32(90,180,210,255)} };
    static int reading=0; if(reading<0||reading>7) reading=0;
    { float h=150.0f; ImVec2 tl(x0, ImGui::GetCursorScreenPos().y);
      Primitive bg; bg.cx=tl.x+cw*0.5f; bg.cy=tl.y+h*0.5f; bg.hw=cw*0.5f; bg.hh=h*0.5f; bg.corner_radius=26; bg.fade=1; bg.material=Material::Thin; g->Submit(bg);
      dl->AddRectFilledMultiColor(ImVec2(tl.x+20,tl.y+20),ImVec2(tl.x+96,tl.y+h-20), bks[reading].a,bks[reading].b,bks[reading].b,bks[reading].a);
      dl->AddText(f,12.0f,ImVec2(tl.x+116,tl.y+24),kInkSoft,"READING NOW");
      dl->AddText(f,20.0f,ImVec2(tl.x+116,tl.y+44),kInk,bks[reading].t);
      char sub[80]; snprintf(sub,80,"%s \xC2\xB7 %d%% complete",bks[reading].author,(int)(bks[reading].prog*100.0f));
      dl->AddText(f,14.0f,ImVec2(tl.x+116,tl.y+72),kInkSoft,sub);
      dl->AddRectFilled(ImVec2(tl.x+116,tl.y+104),ImVec2(tl.x+cw-24,tl.y+108),SA(kInkSoft,0.3f),2.0f);
      dl->AddRectFilled(ImVec2(tl.x+116,tl.y+104),ImVec2(tl.x+116+(cw-140)*bks[reading].prog,tl.y+108),Accent(),2.0f);
      ImGui::Dummy(ImVec2(w, h+16)); }
    { dl->AddText(f,18.0f,ImVec2(x0+4,ImGui::GetCursorScreenPos().y),kInk,"Want to Read"); ImGui::Dummy(ImVec2(w,30.0f));
      int n=8, cols=4; float gap=16, cellw=(cw-(cols-1)*gap)/cols, bh=cellw*1.45f; float sy=ImGui::GetCursorScreenPos().y;
      for(int i=0;i<n;i++){ int r=i/cols,c=i%cols; float x=x0+c*(cellw+gap), y=sy+r*(bh+34); bool hov=false;
        if(SurfaceHit(bks[i].t, ImVec2(x,y), ImVec2(cellw,bh), &hov, Sfx::PageTurn)) reading=i;
        ImgOrGrad(dl, ImVec2(x,y), ImVec2(x+cellw,y+bh), i+5, bks[i].a, bks[i].b, 8.0f);
        dl->AddRectFilled(ImVec2(x,y),ImVec2(x+6,y+bh), SA(IM_COL32(0,0,0,255),0.25f), 0);
        dl->AddText(f,13.0f,ImVec2(x+12,y+14),IM_COL32(255,255,255,235),bks[i].t,NULL,cellw-18);
        if(i==reading)   dl->AddRect(ImVec2(x,y),ImVec2(x+cellw,y+bh), Accent(), 8.0f, 0, 3.0f);
        else if(hov)     dl->AddRect(ImVec2(x,y),ImVec2(x+cellw,y+bh), SA(IM_COL32(255,255,255,255),0.5f), 8.0f, 0, 2.0f); }
      int rows=(n+cols-1)/cols; ImGui::Dummy(ImVec2(w, rows*(bh+34)+4)); }
}
static void Draw_FaceTime(float w){
    ImDrawList* dl=ImGui::GetWindowDrawList(); ImFont* f=ImGui::GetFont();
    float cw=w<560.0f?w:560.0f; float x0=ImGui::GetCursorScreenPos().x+(w-cw)*0.5f;
    static int spotlight=0; static bool mic=true, cam=true;
    { struct P{ const char* n; ImU32 a; ImU32 b; const char* initials; }; P ps[]={ {"Tim",IM_COL32(60,80,140,255),IM_COL32(100,130,200,255),"TC"},{"Craig",IM_COL32(140,70,90,255),IM_COL32(200,120,130,255),"CF"},{"Johnny",IM_COL32(50,120,90,255),IM_COL32(90,180,140,255),"JA"},{"Susan",IM_COL32(120,90,150,255),IM_COL32(180,140,210,255),"SK"} };
      int cols=2; float gap=12, tw=(cw-gap)/2, th=150; float sy=ImGui::GetCursorScreenPos().y;
      for(int i=0;i<4;i++){ int r=i/cols,c=i%cols; float x=x0+c*(tw+gap), y=sy+r*(th+gap); bool hov=false;
        if(SurfaceHit(ps[i].n, ImVec2(x,y), ImVec2(tw,th), &hov)) spotlight=i;
        dl->AddRectFilledMultiColor(ImVec2(x,y),ImVec2(x+tw,y+th), (ps[i].a),(ps[i].b),(ps[i].a),(ps[i].b));
        dl->AddRect(ImVec2(x,y),ImVec2(x+tw,y+th), i==spotlight?Accent():SA(IM_COL32(255,255,255,255),hov?0.4f:0.12f), 18.0f, 0, i==spotlight?3.0f:1.5f);
        dl->AddCircleFilled(ImVec2(x+tw*0.5f,y+th*0.42f), 30.0f, SA(IM_COL32(255,255,255,255),0.25f));
        ImVec2 is=f->CalcTextSizeA(22.0f,99999.0f,0,ps[i].initials); dl->AddText(f,22.0f,ImVec2(x+tw*0.5f-is.x*0.5f,y+th*0.42f-is.y*0.5f),IM_COL32(255,255,255,255),ps[i].initials);
        dl->AddText(f,14.0f,ImVec2(x+14,y+th-26),IM_COL32(255,255,255,235),ps[i].n);
        if(i==0 && !mic) DrawIcon(dl,Icon::Microphone,ImVec2(x+tw-22,y+th-22),14.0f,IM_COL32(255,99,97,255),2.2f); }
      ImGui::Dummy(ImVec2(w, 2*th+gap+16)); }
    { float h=72.0f; ImVec2 tl(x0+cw*0.1f, ImGui::GetCursorScreenPos().y); float ww=cw*0.8f;
      Primitive bg; bg.cx=tl.x+ww*0.5f; bg.cy=tl.y+h*0.5f; bg.hw=ww*0.5f; bg.hh=h*0.5f; bg.corner_radius=h*0.5f; bg.fade=1; bg.material=Material::Thick; g->Submit(bg);
      Icon ic[5]={Icon::Microphone,Icon::Video,Icon::Refresh,Icon::Users,Icon::Phone};
      const char* fid[5]={"ftmic","ftcam","ftflip","ftusers","ftend"};
      for(int i=0;i<5;i++){ float cx=tl.x+ww*(i+0.5f)/5.0f, cy=tl.y+h*0.5f; bool hov=false;
        if(SurfaceHit(fid[i], ImVec2(cx-24,cy-24), ImVec2(48,48), &hov)){ if(i==0) mic=!mic; else if(i==1) cam=!cam; }
        bool off = (i==0&&!mic)||(i==1&&!cam);
        ImU32 fill = (i==4||off)?IM_COL32(255,69,58,255):SA(IM_COL32(255,255,255,255),hov?0.28f:0.16f);
        dl->AddCircleFilled(ImVec2(cx,cy), 22.0f, fill);
        DrawIcon(dl,ic[i],ImVec2(cx,cy),18.0f,IM_COL32(255,255,255,255),2.2f); }
      ImGui::Dummy(ImVec2(w, h+12)); }
}
static void Draw_FindMy(float w){
    ImDrawList* dl=ImGui::GetWindowDrawList(); ImFont* f=ImGui::GetFont();
    float cw=w<640.0f?w:640.0f; float x0=ImGui::GetCursorScreenPos().x+(w-cw)*0.5f;
    { float h=200.0f; ImVec2 tl(x0, ImGui::GetCursorScreenPos().y);
      Primitive bg; bg.cx=tl.x+cw*0.5f; bg.cy=tl.y+h*0.5f; bg.hw=cw*0.5f; bg.hh=h*0.5f; bg.corner_radius=28; bg.fade=1; bg.material=Material::Thin; g->Submit(bg);
      dl->PushClipRect(ImVec2(tl.x,tl.y),ImVec2(tl.x+cw,tl.y+h),true);
      for(float gx=tl.x+40; gx<tl.x+cw; gx+=70) dl->AddLine(ImVec2(gx,tl.y),ImVec2(gx,tl.y+h),SA(kInkSoft,0.16f),5.0f);
      for(float gy=tl.y+34; gy<tl.y+h; gy+=56) dl->AddLine(ImVec2(tl.x,gy),ImVec2(tl.x+cw,gy),SA(kInkSoft,0.16f),5.0f);
      struct Mk{ float fx, fy; ImU32 c; const char* i; }; Mk mk[]={ {0.3f,0.4f,IM_COL32(48,209,88,255),"EW"},{0.62f,0.62f,IM_COL32(255,159,10,255),"JC"},{0.8f,0.3f,IM_COL32(146,82,232,255),"SR"} };
      for(int i=0;i<3;i++){ ImVec2 c(tl.x+cw*mk[i].fx, tl.y+h*mk[i].fy);
        dl->AddCircleFilled(c,16.0f,mk[i].c); dl->AddCircle(c,16.0f,IM_COL32(255,255,255,255),0,2.5f);
        ImVec2 is=f->CalcTextSizeA(12.0f,99999.0f,0,mk[i].i); dl->AddText(f,12.0f,ImVec2(c.x-is.x*0.5f,c.y-is.y*0.5f),IM_COL32(255,255,255,255),mk[i].i); }
      dl->PopClipRect(); ImGui::Dummy(ImVec2(w, h+14)); }
    { struct P{ const char* n; const char* loc; const char* d; ImU32 c; const char* i; }; P ps[]={ {"Emma Wilson","Downtown","0.6 mi",IM_COL32(48,209,88,255),"EW"},{"James Carter","Home","1.2 mi",IM_COL32(255,159,10,255),"JC"},{"Sofia Rodriguez","San Francisco","38 mi",IM_COL32(146,82,232,255),"SR"} };
      float rowh=64.0f, h=3*rowh; ImVec2 tl(x0, ImGui::GetCursorScreenPos().y);
      Primitive bg; bg.cx=tl.x+cw*0.5f; bg.cy=tl.y+h*0.5f; bg.hw=cw*0.5f; bg.hh=h*0.5f; bg.corner_radius=24; bg.fade=1; bg.material=Material::Thin; g->Submit(bg);
      for(int i=0;i<3;i++){ float ry=tl.y+i*rowh, cy=ry+rowh*0.5f;
        dl->AddCircleFilled(ImVec2(tl.x+34,cy),18.0f,ps[i].c); ImVec2 is=f->CalcTextSizeA(13.0f,99999.0f,0,ps[i].i); dl->AddText(f,13.0f,ImVec2(tl.x+34-is.x*0.5f,cy-is.y*0.5f),IM_COL32(255,255,255,255),ps[i].i);
        dl->AddText(f,16.0f,ImVec2(tl.x+62,cy-17),kInk,ps[i].n);
        dl->AddText(f,13.0f,ImVec2(tl.x+62,cy+3),kInkSoft,ps[i].loc);
        ImVec2 ds=f->CalcTextSizeA(14.0f,99999.0f,0,ps[i].d); dl->AddText(f,14.0f,ImVec2(tl.x+cw-ds.x-16,cy-8),Accent(),ps[i].d);
        if(i<2) dl->AddLine(ImVec2(tl.x+62,ry+rowh),ImVec2(tl.x+cw-14,ry+rowh),SA(kInkSoft,0.14f),1.0f); }
      ImGui::Dummy(ImVec2(w, h+12)); }
}
static void Draw_Shortcuts(float w){
    ImDrawList* dl=ImGui::GetWindowDrawList(); ImFont* f=ImGui::GetFont();
    float cw=w<660.0f?w:660.0f; float x0=ImGui::GetCursorScreenPos().x+(w-cw)*0.5f;
    dl->AddText(f,30.0f, ImVec2(x0+2, ImGui::GetCursorScreenPos().y), kInk, "Shortcuts");
    ImGui::Dummy(ImVec2(w, 50.0f));
    struct Sc{ Icon ic; const char* n; ImU32 a; ImU32 b; }; Sc sc[]={
        {Icon::House,"Arrive Home",IM_COL32(60,90,180,255),IM_COL32(110,140,220,255)},
        {Icon::MugHot,"Morning",IM_COL32(180,110,40,255),IM_COL32(230,170,80,255)},
        {Icon::Bed,"Wind Down",IM_COL32(90,60,150,255),IM_COL32(150,110,210,255)},
        {Icon::Car,"Drive Mode",IM_COL32(40,140,150,255),IM_COL32(90,200,210,255)},
        {Icon::Camera,"Selfie",IM_COL32(180,60,110,255),IM_COL32(230,120,160,255)},
        {Icon::Music,"Play Playlist",IM_COL32(200,50,80,255),IM_COL32(250,110,130,255)},
        {Icon::PaperPlane,"Text Craig",IM_COL32(40,160,90,255),IM_COL32(90,210,140,255)},
        {Icon::Bolt,"Low Power",IM_COL32(180,150,30,255),IM_COL32(230,200,70,255)},
    };
    int n=8, cols=3; float gap=16, cellw=(cw-(cols-1)*gap)/cols, ch=cellw*0.6f; float sy=ImGui::GetCursorScreenPos().y;
    for(int i=0;i<n;i++){ int r=i/cols,c=i%cols; float x=x0+c*(cellw+gap), y=sy+r*(ch+gap);
        dl->AddRectFilledMultiColor(ImVec2(x,y),ImVec2(x+cellw,y+ch), (sc[i].a),(sc[i].b),(sc[i].b),(sc[i].a));
        dl->AddRect(ImVec2(x,y),ImVec2(x+cellw,y+ch),SA(IM_COL32(255,255,255,255),0.15f),16.0f,0,1.0f);
        DrawIcon(dl,sc[i].ic,ImVec2(x+26,y+24),20.0f,IM_COL32(255,255,255,255),2.2f);
        dl->AddText(f,15.0f,ImVec2(x+14,y+ch-30),IM_COL32(255,255,255,255),sc[i].n,NULL,cellw-24); }
    int rows=(n+cols-1)/cols; ImGui::Dummy(ImVec2(w, rows*(ch+gap)+4));
}
static void Draw_Freeform(float w){
    ImDrawList* dl=ImGui::GetWindowDrawList(); ImFont* f=ImGui::GetFont();
    float cw=w<720.0f?w:720.0f; float x0=ImGui::GetCursorScreenPos().x+(w-cw)*0.5f;
    float h=420.0f; ImVec2 tl(x0, ImGui::GetCursorScreenPos().y);
    Primitive bg; bg.cx=tl.x+cw*0.5f; bg.cy=tl.y+h*0.5f; bg.hw=cw*0.5f; bg.hh=h*0.5f; bg.corner_radius=26; bg.fade=1; bg.material=Material::Thin; g->Submit(bg);
    static ImVec2 pts[6000]; static unsigned char pcol[6000]; static bool pbrk[6000]; static int np=0; static int pen=0;
    ImU32 palette[5]={ Accent(), IM_COL32(255,214,90,255), IM_COL32(130,210,150,255), IM_COL32(255,120,140,255), kInk };
    ImGui::SetCursorScreenPos(tl); ImGui::InvisibleButton("ffcanvas", ImVec2(cw,h));
    bool act=ImGui::IsItemActive(); ImVec2 mp=ImGui::GetIO().MousePos;
    if(ImGui::IsItemActivated() && np<6000){ pts[np]=ImVec2(mp.x-tl.x,mp.y-tl.y); pbrk[np]=true; pcol[np]=(unsigned char)pen; np++; }
    else if(act && np<6000){ ImVec2 r(mp.x-tl.x,mp.y-tl.y);
        if(np==0 || (fabsf(r.x-pts[np-1].x)+fabsf(r.y-pts[np-1].y))>1.6f){ pts[np]=r; pbrk[np]=false; pcol[np]=(unsigned char)pen; np++; } }
    dl->PushClipRect(ImVec2(tl.x,tl.y),ImVec2(tl.x+cw,tl.y+h),true);
    for(float gy=tl.y+24; gy<tl.y+h; gy+=26) for(float gx=tl.x+24; gx<tl.x+cw; gx+=26) dl->AddCircleFilled(ImVec2(gx,gy),1.0f,SA(kInkSoft,0.3f));
    for(int i=1;i<np;i++){ if(pbrk[i]) continue; dl->AddLine(ImVec2(tl.x+pts[i-1].x,tl.y+pts[i-1].y),ImVec2(tl.x+pts[i].x,tl.y+pts[i].y), palette[pcol[i]<5?pcol[i]:0], 3.0f); }
    if(np==0){ const char* hint="Draw here \xE2\x80\x94 pick a color below"; ImVec2 hs=f->CalcTextSizeA(15.0f,99999.0f,0,hint); dl->AddText(f,15.0f,ImVec2(tl.x+cw*0.5f-hs.x*0.5f,tl.y+h*0.5f-8),SA(kInkSoft,0.55f),hint); }
    dl->PopClipRect();
    float bw=46.0f, tbw=6*bw; float bx=tl.x+cw*0.5f-tbw*0.5f, by=tl.y+h+10.0f;
    Primitive tbar; tbar.cx=tl.x+cw*0.5f; tbar.cy=by+22; tbar.hw=tbw*0.5f+6; tbar.hh=24; tbar.corner_radius=28; tbar.fade=1; tbar.material=Material::Thick; g->Submit(tbar);
    for(int i=0;i<5;i++){ float ccx=bx+bw*(i+0.5f), ccy=by+22; bool hv=false; char id[8]; snprintf(id,8,"ffp%d",i);
        if(SurfaceHit(id, ImVec2(ccx-20,by), ImVec2(40,44), &hv)) pen=i;
        dl->AddCircleFilled(ImVec2(ccx,ccy), 11.0f, palette[i]);
        if(i==pen)   dl->AddCircle(ImVec2(ccx,ccy), 15.0f, kInk, 0, 2.5f);
        else if(hv)  dl->AddCircle(ImVec2(ccx,ccy), 15.0f, SA(kInkSoft,0.5f), 0, 2.0f); }
    { float ccx=bx+bw*5.5f, ccy=by+22; bool chv=false;
        if(SurfaceHit("ffclear", ImVec2(ccx-20,by), ImVec2(40,44), &chv)) np=0;
        ImU32 xc=chv?IM_COL32(255,99,97,255):kInk; float s=8.0f;
        dl->AddLine(ImVec2(ccx-s,ccy-s),ImVec2(ccx+s,ccy+s),xc,2.4f); dl->AddLine(ImVec2(ccx-s,ccy+s),ImVec2(ccx+s,ccy-s),xc,2.4f); }
    ImGui::SetCursorScreenPos(tl); ImGui::Dummy(ImVec2(w, h+60.0f));
}
static void Draw_GameCenter(float w){
    ImDrawList* dl=ImGui::GetWindowDrawList(); ImFont* f=ImGui::GetFont();
    float cw=w<600.0f?w:600.0f; float x0=ImGui::GetCursorScreenPos().x+(w-cw)*0.5f;
    { float h=110.0f; ImVec2 tl(x0, ImGui::GetCursorScreenPos().y);
      Primitive bg; bg.cx=tl.x+cw*0.5f; bg.cy=tl.y+h*0.5f; bg.hw=cw*0.5f; bg.hh=h*0.5f; bg.corner_radius=28; bg.fade=1; bg.material=Material::Accent; g->Submit(bg);
      dl->AddCircleFilled(ImVec2(tl.x+56,tl.y+h*0.5f),34.0f,SA(IM_COL32(255,255,255,255),0.25f));
      ImVec2 is=f->CalcTextSizeA(26.0f,99999.0f,0,"AC"); dl->AddText(f,26.0f,ImVec2(tl.x+56-is.x*0.5f,tl.y+h*0.5f-is.y*0.5f),IM_COL32(255,255,255,255),"AC");
      dl->AddText(f,22.0f,ImVec2(tl.x+106,tl.y+26),IM_COL32(255,255,255,255),"alexcarter");
      dl->AddText(f,14.0f,ImVec2(tl.x+106,tl.y+58),SA(IM_COL32(255,255,255,255),0.85f),"Level 24 \xC2\xB7 1,284 friends");
      DrawIcon(dl,Icon::Trophy,ImVec2(tl.x+cw-34,tl.y+h*0.5f),26.0f,IM_COL32(255,214,90,255),2.2f);
      ImGui::Dummy(ImVec2(w, h+16)); }
    { dl->AddText(f,18.0f,ImVec2(x0+4,ImGui::GetCursorScreenPos().y),kInk,"Achievements"); ImGui::Dummy(ImVec2(w,30.0f));
      struct Ac{ Icon ic; const char* n; bool got; ImU32 c; }; Ac ac[]={ {Icon::Crown,"Champion",true,IM_COL32(255,214,90,255)},{Icon::Fire,"On Fire",true,IM_COL32(255,90,60,255)},{Icon::Medal,"Top 10",true,IM_COL32(64,156,255,255)},{Icon::Gem,"Collector",false,IM_COL32(146,82,232,255)},{Icon::Rocket,"Speedrun",true,IM_COL32(48,209,88,255)},{Icon::Star,"5 Stars",false,IM_COL32(255,159,10,255)} };
      int n=6, cols=3; float gap=14, tw=(cw-(cols-1)*gap)/cols, th=96; float sy=ImGui::GetCursorScreenPos().y;
      for(int i=0;i<n;i++){ int r=i/cols,c=i%cols; float x=x0+c*(tw+gap), y=sy+r*(th+gap);
        Primitive b; b.cx=x+tw*0.5f; b.cy=y+th*0.5f; b.hw=tw*0.5f; b.hh=th*0.5f; b.corner_radius=24; b.fade=1; b.material=Material::Thin; g->Submit(b);
        float a=ac[i].got?1.0f:0.3f;
        dl->AddCircleFilled(ImVec2(x+tw*0.5f,y+34),22.0f,SA(ac[i].c,0.22f*a)); DrawIcon(dl,ac[i].ic,ImVec2(x+tw*0.5f,y+34),22.0f,SA(ac[i].c,a),2.2f);
        ImVec2 ns=f->CalcTextSizeA(13.0f,99999.0f,0,ac[i].n); dl->AddText(f,13.0f,ImVec2(x+tw*0.5f-ns.x*0.5f,y+th-26),SA(kInk,a),ac[i].n); }
      int rows=(n+cols-1)/cols; ImGui::Dummy(ImVec2(w, rows*(th+gap)+4)); }
}
static void Draw_Components(float w){
    float cw=w<560.0f?w:560.0f;
    ImGui::Indent((w-cw)*0.5f);
    static bool t1=true,t2=false,cbx=true; static int seg=1,rad=0,step=3,dd=1,rate=4;
    static float sl=0.4f; static char tf[64]="Liquid"; static bool chA=true,chB=false,chC=true;
    Glass::BeginPanel("BUTTONS");
    Glass::Button("Primary", true); ImGui::SameLine(0,8); Glass::Button("Secondary", false); ImGui::SameLine(0,8);
    if(Glass::IconButton("ci",Glass::Icon::Heart,36.0f)){} ImGui::SameLine(0,8); if(Glass::IconButton("cs",Glass::Icon::ShareNodes,36.0f)){}
    Glass::EndPanel();
    Glass::BeginPanel("TOGGLES & CHECKS");
    Glass::Toggle("Wi-Fi", &t1); Glass::Toggle("Bluetooth", &t2);
    Glass::Checkbox("Analytics", &cbx);
    Glass::EndPanel();
    Glass::BeginPanel("SELECTION");
    { const char* sg[]={"Day","Week","Month"}; seg=Glass::SegmentedControl("sg",sg,3,seg); }
    { const char* rg[]={"Low","Medium","High"}; rad=Glass::RadioGroup("rg",rg,3,rad); }
    Glass::Chip("Swift", chA); ImGui::SameLine(0,8); Glass::Chip("Metal", chB); ImGui::SameLine(0,8); Glass::Chip("Glass", chC);
    Glass::EndPanel();
    Glass::BeginPanel("VALUES");
    Glass::Slider("Opacity", &sl, 0.0f, 1.0f, "%.2f");
    Glass::Stepper("Quantity", &step, 1, 0, 20);
    Glass::EndPanel();
    Glass::BeginPanel("INDICATORS");
    Glass::ProgressBar(sl);
    Glass::ProgressRing("pr", sl, 28.0f, "");
    ImGui::SameLine(0,18); Glass::Spinner("sp", 13.0f, 0);
    Glass::EndPanel();
    Glass::BeginPanel("CONTENT");
    Glass::Avatar("JA", 40.0f, IM_COL32(10,132,255,255)); ImGui::SameLine(0,12);
    Glass::Badge("PRO", IM_COL32(255,149,0,255)); ImGui::SameLine(0,8); Glass::StatusPill("Online", IM_COL32(52,209,88,255));
    rate=Glass::Rating("rt", rate);
    Glass::TextField("Name", tf, (int)sizeof(tf), "Type here", Glass::Icon::Person);
    { const char* it[]={"Opus","Sonnet","Haiku"}; Glass::Dropdown("Model", it, 3, &dd); }
    Glass::EndPanel();
    ImGui::Unindent((w-cw)*0.5f);
}
static void Draw_Compass(float w){
    ImDrawList* dl=ImGui::GetWindowDrawList(); ImFont* f=ImGui::GetFont();
    float cw=w<520.0f?w:520.0f; float x0=ImGui::GetCursorScreenPos().x+(w-cw)*0.5f;
    float t=(float)ImGui::GetTime(); float heading=40.0f+sinf(t*0.25f)*35.0f;
    float h=cw; ImVec2 tl(x0, ImGui::GetCursorScreenPos().y); ImVec2 c(tl.x+cw*0.5f, tl.y+h*0.5f); float R=cw*0.4f;
    Primitive bg; bg.cx=c.x; bg.cy=c.y; bg.hw=R+24; bg.hh=R+24; bg.corner_radius=R+24; bg.fade=1; bg.material=Material::Thin; g->Submit(bg);
    for(int i=0;i<72;i++){ float a=i*(6.2831853f/72.0f) - heading*0.01745329f; float r0=R*(i%18==0?0.82f:0.9f), r1=R;
        dl->AddLine(ImVec2(c.x+cosf(a)*r0,c.y+sinf(a)*r0),ImVec2(c.x+cosf(a)*r1,c.y+sinf(a)*r1), SA(kInkSoft, i%18==0?0.9f:0.4f), i%18==0?2.5f:1.0f); }
    const char* card[4]={"N","E","S","W"};
    for(int i=0;i<4;i++){ float a=i*(1.5707963f) - 1.5707963f - heading*0.01745329f; ImVec2 p(c.x+cosf(a)*R*0.68f, c.y+sinf(a)*R*0.68f);
        ImVec2 ts=f->CalcTextSizeA(22.0f,99999.0f,0,card[i]); dl->AddText(f,22.0f,ImVec2(p.x-ts.x*0.5f,p.y-ts.y*0.5f), i==0?IM_COL32(255,69,58,255):kInk, card[i]); }
    float na=-1.5707963f - heading*0.01745329f;
    dl->AddTriangleFilled(ImVec2(c.x+cosf(na)*R*0.55f,c.y+sinf(na)*R*0.55f), ImVec2(c.x+cosf(na+1.5f)*12,c.y+sinf(na+1.5f)*12), ImVec2(c.x+cosf(na-1.5f)*12,c.y+sinf(na-1.5f)*12), IM_COL32(255,69,58,255));
    dl->AddCircleFilled(c, 6.0f, kInk);
    char hb[16]; snprintf(hb,16,"%d\xC2\xB0",(int)heading); ImVec2 hs=f->CalcTextSizeA(40.0f,99999.0f,0,hb);
    dl->AddText(f,40.0f,ImVec2(c.x-hs.x*0.5f,c.y-20),kInk,hb);
    const char* dir = (heading<45||heading>=315)?"North":(heading<135?"East":(heading<225?"South":"West"));
    ImVec2 ds=f->CalcTextSizeA(14.0f,99999.0f,0,dir); dl->AddText(f,14.0f,ImVec2(c.x-ds.x*0.5f,c.y+22),kInkSoft,dir);
    ImGui::Dummy(ImVec2(w, h+12));
}
static void Draw_Breathe(float w){
    ImDrawList* dl=ImGui::GetWindowDrawList(); ImFont* f=ImGui::GetFont();
    float cw=w<520.0f?w:520.0f; float x0=ImGui::GetCursorScreenPos().x+(w-cw)*0.5f;
    float t=(float)ImGui::GetTime(); float ph=sinf(t*0.5f)*0.5f+0.5f;
    float h=cw*0.9f; ImVec2 tl(x0, ImGui::GetCursorScreenPos().y); ImVec2 c(tl.x+cw*0.5f, tl.y+h*0.45f);
    float base=cw*0.16f, R=base*(0.7f+0.7f*ph);
    int petals=6; for(int i=0;i<petals;i++){ float a=i*(6.2831853f/petals)+t*0.15f; float off=R*0.6f;
        dl->AddCircleFilled(ImVec2(c.x+cosf(a)*off,c.y+sinf(a)*off), R, SA(Accent(),0.22f), 48); }
    dl->AddCircleFilled(c, R*0.7f, SA(Accent(),0.5f), 48);
    const char* msg = ph>0.5f? "Inhale" : "Exhale";
    ImVec2 ms=f->CalcTextSizeA(26.0f,99999.0f,0,msg); dl->AddText(f,26.0f,ImVec2(c.x-ms.x*0.5f,c.y-13),IM_COL32(255,255,255,235),msg);
    ImVec2 ts=f->CalcTextSizeA(15.0f,99999.0f,0,"Breathe with the glass"); dl->AddText(f,15.0f,ImVec2(tl.x+cw*0.5f-ts.x*0.5f,tl.y+h-30),kInkSoft,"Breathe with the glass");
    ImGui::Dummy(ImVec2(w, h+12));
}
static void Draw_Timer(float w){
    ImDrawList* dl=ImGui::GetWindowDrawList(); ImFont* f=ImGui::GetFont();
    float cw=w<520.0f?w:520.0f; float x0=ImGui::GetCursorScreenPos().x+(w-cw)*0.5f;
    static float total=300.0f, remaining=183.0f; static bool running=true;
    if(running){ remaining -= Dt(); if(remaining<0){ remaining=0; running=false; } }
    float frac = total>0? remaining/total : 0;
    float h=cw*0.78f; ImVec2 tl(x0, ImGui::GetCursorScreenPos().y); ImVec2 c(tl.x+cw*0.5f, tl.y+h*0.42f); float R=cw*0.32f;
    dl->PathArcTo(c,R,-1.5707963f,-1.5707963f+6.2831853f,96); dl->PathStroke(SA(kInkSoft,0.2f),0,12.0f);
    dl->PathArcTo(c,R,-1.5707963f,-1.5707963f+6.2831853f*frac,96); dl->PathStroke(Accent(),0,12.0f);
    char tb[16]; int rs=(int)remaining; snprintf(tb,16,"%d:%02d",rs/60,rs%60); ImVec2 ts=f->CalcTextSizeA(52.0f,99999.0f,0,tb);
    dl->AddText(f,52.0f,ImVec2(c.x-ts.x*0.5f,c.y-26),kInk,tb);
    float by=tl.y+h-6;
    ImGui::SetCursorScreenPos(ImVec2(x0+cw*0.5f-90, by)); if(Glass::Button(running?"Pause":"Resume", true, ImVec2(84,40))) running=!running;
    ImGui::SetCursorScreenPos(ImVec2(x0+cw*0.5f+6, by)); if(Glass::Button("Reset", false, ImVec2(84,40))){ remaining=total; running=false; }
    ImGui::SetCursorScreenPos(ImVec2(x0, by+50));
    int mins[4]={1,3,5,10}; float pw=(cw-3*10)/4.0f;
    for(int i=0;i<4;i++){ ImGui::SetCursorScreenPos(ImVec2(x0+i*(pw+10), by+50)); char id[8]; snprintf(id,8,"pz%d",i); char lb[8]; snprintf(lb,8,"%dm",mins[i]);
        if(Glass::Button(lb, false, ImVec2(pw,34))){ total=remaining=mins[i]*60.0f; running=false; } }
    ImGui::SetCursorScreenPos(ImVec2(x0, by+94)); ImGui::Dummy(ImVec2(w, 8.0f));
}
static void Draw_TV(float w){
    ImDrawList* dl=ImGui::GetWindowDrawList(); ImFont* f=ImGui::GetFont();
    float cw=w<720.0f?w:720.0f; float x0=ImGui::GetCursorScreenPos().x+(w-cw)*0.5f;
    struct M{ const char* t; ImU32 a; ImU32 b; };
    static M ms[]={ {"Northern Lights",IM_COL32(40,60,120,255),IM_COL32(80,110,190,255)},{"City of Echoes",IM_COL32(120,50,80,255),IM_COL32(190,100,130,255)},{"Tidewater",IM_COL32(40,110,80,255),IM_COL32(90,180,130,255)},{"The Long Road",IM_COL32(110,60,140,255),IM_COL32(170,110,200,255)},{"Afterglow",IM_COL32(150,100,40,255),IM_COL32(210,160,80,255)},{"Silver Lake",IM_COL32(50,110,140,255),IM_COL32(90,170,200,255)} };
    static int sel=-1; static bool playing=false;
    const char* heroTitle = (sel>=0)? ms[sel].t : "The Quiet Hour";
    { float h=240.0f; ImVec2 tl(x0, ImGui::GetCursorScreenPos().y);
      ImU32 ha=(sel>=0)?ms[sel].a:IM_COL32(20,24,40,255), hb=(sel>=0)?ms[sel].b:IM_COL32(120,50,70,255);
      dl->AddRectFilledMultiColor(ImVec2(tl.x,tl.y),ImVec2(tl.x+cw,tl.y+h), ha,IM_COL32(60,40,90,255),hb,IM_COL32(30,30,50,255));
      dl->AddRect(ImVec2(tl.x,tl.y),ImVec2(tl.x+cw,tl.y+h), SA(IM_COL32(255,255,255,255),0.14f),22.0f,0,1.5f);
      dl->AddText(f,12.0f,ImVec2(tl.x+22,tl.y+18),SA(IM_COL32(255,255,255,255),0.8f),playing?"NOW PLAYING":"FEATURED FILM");
      dl->AddText(f,34.0f,ImVec2(tl.x+22,tl.y+h-92),IM_COL32(255,255,255,255),heroTitle);
      dl->AddText(f,14.0f,ImVec2(tl.x+22,tl.y+h-50),SA(IM_COL32(255,255,255,255),0.82f),"2026 \xC2\xB7 Sci-Fi \xC2\xB7 2h 14m");
      float bw=120, bx=tl.x+22, by=tl.y+h-32; bool phv=false;
      if(SurfaceHit("tvplay", ImVec2(bx,by-14), ImVec2(bw,32), &phv, playing?Sfx::Pause:Sfx::Play)) playing=!playing;
      Primitive pb; pb.cx=bx+bw*0.5f; pb.cy=by+2; pb.hw=bw*0.5f; pb.hh=15; pb.corner_radius=20; pb.fade=1; pb.material=Material::Thick; pb.elevate=phv?1.1f:1.0f; g->Submit(pb);
      DrawIcon(dl,playing?Icon::Pause:Icon::Play,ImVec2(bx+18,by+2),12.0f,IM_COL32(255,255,255,255),2.0f); dl->AddText(f,14.0f,ImVec2(bx+32,by-6),IM_COL32(255,255,255,255),playing?"Pause":"Play");
      ImGui::Dummy(ImVec2(w, h+18)); }
    { dl->AddText(f,18.0f,ImVec2(x0+4,ImGui::GetCursorScreenPos().y),kInk,"Continue Watching"); ImGui::Dummy(ImVec2(w,30.0f));
      int n=6, cols=3; float gap=14, cellw=(cw-(cols-1)*gap)/cols, ph=cellw*0.62f; float sy=ImGui::GetCursorScreenPos().y;
      for(int i=0;i<n;i++){ int r=i/cols,c=i%cols; float x=x0+c*(cellw+gap), y=sy+r*(ph+38); bool hov=false;
        if(SurfaceHit(ms[i].t, ImVec2(x,y), ImVec2(cellw,ph), &hov)){ sel=i; playing=true; }
        ImgOrGrad(dl, ImVec2(x,y), ImVec2(x+cellw,y+ph), i+7, ms[i].a, ms[i].b, 12.0f);
        dl->AddRect(ImVec2(x,y),ImVec2(x+cellw,y+ph), i==sel?Accent():SA(IM_COL32(255,255,255,255),hov?0.4f:0.12f),12.0f,0,i==sel?3.0f:1.0f);
        dl->AddText(f,14.0f,ImVec2(x+2,y+ph+8),i==sel?Accent():kInk,ms[i].t); }
      int rows=(n+cols-1)/cols; ImGui::Dummy(ImVec2(w, rows*(ph+38)+4)); }
}
static void Draw_Sports(float w){
    ImDrawList* dl=ImGui::GetWindowDrawList(); ImFont* f=ImGui::GetFont();
    float cw=w<600.0f?w:600.0f; float x0=ImGui::GetCursorScreenPos().x+(w-cw)*0.5f;
    dl->AddText(f,30.0f, ImVec2(x0+2, ImGui::GetCursorScreenPos().y), kInk, "Scores");
    ImGui::Dummy(ImVec2(w, 50.0f));
    struct Gm{ const char* a; const char* b; int sa; int sb; const char* st; ImU32 ca; ImU32 cb; bool live; };
    Gm gs[]={ {"Warriors","Lakers",112,108,"FINAL",IM_COL32(64,156,255,255),IM_COL32(255,159,10,255),false},
              {"Celtics","Heat",58,61,"Q3 4:12",IM_COL32(48,209,88,255),IM_COL32(255,69,58,255),true},
              {"Arsenal","Chelsea",2,2,"78'",IM_COL32(94,92,230,255),IM_COL32(255,90,140,255),true},
              {"Cupertino","Mountain View",7,3,"FINAL",IM_COL32(255,214,90,255),IM_COL32(120,120,128,255),false} };
    for(int i=0;i<4;i++){ float h=86.0f; ImVec2 tl(x0, ImGui::GetCursorScreenPos().y);
        Primitive bg; bg.cx=tl.x+cw*0.5f; bg.cy=tl.y+h*0.5f; bg.hw=cw*0.5f; bg.hh=h*0.5f; bg.corner_radius=24; bg.fade=1; bg.material=Material::Thin; g->Submit(bg);
        dl->AddCircleFilled(ImVec2(tl.x+34,tl.y+30),14.0f,gs[i].ca); dl->AddText(f,16.0f,ImVec2(tl.x+58,tl.y+22),kInk,gs[i].a);
        dl->AddCircleFilled(ImVec2(tl.x+34,tl.y+58),14.0f,gs[i].cb); dl->AddText(f,16.0f,ImVec2(tl.x+58,tl.y+50),kInk,gs[i].b);
        char sa[8],sb[8]; snprintf(sa,8,"%d",gs[i].sa); snprintf(sb,8,"%d",gs[i].sb);
        bool awin=gs[i].sa>=gs[i].sb;
        dl->AddText(f,22.0f,ImVec2(tl.x+cw-120,tl.y+18),awin?kInk:kInkSoft,sa);
        dl->AddText(f,22.0f,ImVec2(tl.x+cw-120,tl.y+46),!awin?kInk:kInkSoft,sb);
        ImU32 stc = gs[i].live?IM_COL32(255,69,58,255):kInkSoft; ImVec2 ss=f->CalcTextSizeA(13.0f,99999.0f,0,gs[i].st);
        if(gs[i].live) dl->AddCircleFilled(ImVec2(tl.x+cw-ss.x-30,tl.y+h*0.5f),3.5f,IM_COL32(255,69,58,255));
        dl->AddText(f,13.0f,ImVec2(tl.x+cw-ss.x-20,tl.y+h*0.5f-7),stc,gs[i].st);
        ImGui::Dummy(ImVec2(w, h+12)); }
}
static void Draw_Widgets(float w){
    ImDrawList* dl=ImGui::GetWindowDrawList(); ImFont* f=ImGui::GetFont();
    float cw=w<560.0f?w:560.0f; float x0=ImGui::GetCursorScreenPos().x+(w-cw)*0.5f;
    float gap=14, tw=(cw-gap)/2, th=150; float sy=ImGui::GetCursorScreenPos().y;
    time_t tt=time(nullptr); struct tm lt; localtime_s(&lt,&tt);
    auto cardAt=[&](int idx)->ImVec2{ int r=idx/2,c=idx%2; float x=x0+c*(tw+gap), y=sy+r*(th+gap);
        Primitive bg; bg.cx=x+tw*0.5f; bg.cy=y+th*0.5f; bg.hw=tw*0.5f; bg.hh=th*0.5f; bg.corner_radius=28; bg.fade=1; bg.material=Material::Thin; g->Submit(bg); return ImVec2(x,y); };
    { ImVec2 p=cardAt(0); DrawIcon(dl,Icon::CloudSun,ImVec2(p.x+30,p.y+30),22.0f,Accent(),2.0f);
      dl->AddText(f,13.0f,ImVec2(p.x+50,p.y+22),kInkSoft,"Cupertino"); dl->AddText(f,46.0f,ImVec2(p.x+18,p.y+50),kInk,"68\xC2\xB0");
      dl->AddText(f,13.0f,ImVec2(p.x+18,p.y+th-30),kInkSoft,"Mostly Sunny  H:74 L:59"); }
    { ImVec2 p=cardAt(1); dl->AddText(f,13.0f,ImVec2(p.x+18,p.y+16),IM_COL32(255,69,58,255),"THURSDAY");
      dl->AddText(f,44.0f,ImVec2(p.x+16,p.y+30),kInk,"29");
      dl->AddRectFilled(ImVec2(p.x+18,p.y+84),ImVec2(p.x+tw-18,p.y+108),SA(Accent(),0.18f),6.0f);
      dl->AddText(f,12.0f,ImVec2(p.x+24,p.y+90),kInk,"10:00  Design Review"); }
    { ImVec2 p=cardAt(2); ImVec2 c(p.x+44,p.y+th*0.5f); float fr[3]={0.8f,0.6f,0.95f}; ImU32 cl[3]={IM_COL32(255,55,95,255),IM_COL32(126,255,40,255),IM_COL32(64,220,255,255)}; float rr[3]={30,22,14};
      for(int i=0;i<3;i++){ dl->PathArcTo(c,rr[i],-1.5708f,-1.5708f+6.2831853f,48); dl->PathStroke(SA(cl[i],0.22f),0,6.0f); dl->PathArcTo(c,rr[i],-1.5708f,-1.5708f+6.2831853f*fr[i],48); dl->PathStroke(cl[i],0,6.0f); }
      dl->AddText(f,14.0f,ImVec2(p.x+86,p.y+40),kInk,"Move"); dl->AddText(f,18.0f,ImVec2(p.x+86,p.y+58),IM_COL32(255,55,95,255),"486");
      dl->AddText(f,12.0f,ImVec2(p.x+86,p.y+86),kInkSoft,"Exercise 28m"); }
    { ImVec2 p=cardAt(3);
      char tb[16]; FmtClock(tb,16,lt.tm_hour,lt.tm_min); dl->AddText(f,38.0f,ImVec2(p.x+18,p.y+18),kInk,tb);
      dl->AddRect(ImVec2(p.x+18,p.y+84),ImVec2(p.x+78,p.y+108),kInkSoft,4.0f,0,2.0f); dl->AddRectFilled(ImVec2(p.x+78,p.y+91),ImVec2(p.x+82,p.y+101),kInkSoft,1.0f);
      dl->AddRectFilled(ImVec2(p.x+21,p.y+87),ImVec2(p.x+21+54*0.87f,p.y+105),IM_COL32(48,209,88,255),3.0f);
      dl->AddText(f,16.0f,ImVec2(p.x+92,p.y+88),kInk,"87%"); }
    ImGui::Dummy(ImVec2(w, 2*th+gap+12));
}
static void Draw_Alarms(float w){
    ImDrawList* dl=ImGui::GetWindowDrawList(); ImFont* f=ImGui::GetFont();
    float cw=w<540.0f?w:540.0f; float x0=ImGui::GetCursorScreenPos().x+(w-cw)*0.5f;
    { float h=110.0f; ImVec2 tl(x0, ImGui::GetCursorScreenPos().y);
      Primitive bg; bg.cx=tl.x+cw*0.5f; bg.cy=tl.y+h*0.5f; bg.hw=cw*0.5f; bg.hh=h*0.5f; bg.corner_radius=28; bg.fade=1; bg.material=Material::Thin; g->Submit(bg);
      DrawIcon(dl,Icon::Bell,ImVec2(tl.x+34,tl.y+h*0.5f),20.0f,Accent(),2.0f);
      dl->AddText(f,12.0f,ImVec2(tl.x+62,tl.y+26),kInkSoft,"NEXT ALARM");
      dl->AddText(f,40.0f,ImVec2(tl.x+60,tl.y+40),kInk,"6:30 AM");
      dl->AddText(f,14.0f,ImVec2(tl.x+62,tl.y+86),kInkSoft,"Weekdays \xC2\xB7 in 8h 12m");
      ImGui::Dummy(ImVec2(w, h+16)); }
    { static bool on[5]={true,true,false,true,false};
      struct Al{ const char* t; const char* l; const char* ampm; }; Al al[]={ {"6:30","Wake up","AM"},{"7:15","Workout","AM"},{"12:30","Lunch","PM"},{"9:00","Wind down","PM"},{"11:00","Sleep","PM"} };
      float rowh=64.0f, h=5*rowh; ImVec2 tl(x0, ImGui::GetCursorScreenPos().y);
      Primitive bg; bg.cx=tl.x+cw*0.5f; bg.cy=tl.y+h*0.5f; bg.hw=cw*0.5f; bg.hh=h*0.5f; bg.corner_radius=24; bg.fade=1; bg.material=Material::Thin; g->Submit(bg);
      for(int i=0;i<5;i++){ float ry=tl.y+i*rowh, cy=ry+rowh*0.5f; float al_a = on[i]?1.0f:0.4f;
        char tb[16]; snprintf(tb,16,"%s",al[i].t); dl->AddText(f,30.0f,ImVec2(tl.x+18,cy-20),SA(kInk,al_a),tb);
        ImVec2 ts=f->CalcTextSizeA(30.0f,99999.0f,0,tb); dl->AddText(f,14.0f,ImVec2(tl.x+18+ts.x+4,cy-2),SA(kInkSoft,al_a),al[i].ampm);
        dl->AddText(f,13.0f,ImVec2(tl.x+18,cy+12),SA(kInkSoft,al_a),al[i].l);
        float tw2=44, th2=26, tx=tl.x+cw-tw2-18; ImGui::PushID(i); ImGui::SetCursorScreenPos(ImVec2(tx,cy-th2*0.5f)); ImGui::InvisibleButton("a",ImVec2(tw2,th2));
        if(ImGui::IsItemDeactivated()&&ImGui::IsItemHovered()) on[i]=!on[i];
        Spring& s=g->springs().Get((uint32_t)ImGui::GetID("a"),5,SpringStyle::Critical,on[i]?1.0f:0.0f); s.target=on[i]?1.0f:0.0f; s.Tick(Dt()); float kc=Clampf(s.x,0,1); ImGui::PopID();
        Primitive tr; tr.cx=tx+tw2*0.5f; tr.cy=cy; tr.hw=tw2*0.5f; tr.hh=th2*0.5f; tr.corner_radius=th2*0.5f; tr.fade=1; tr.material=Material::Thin; g->Submit(tr);
        if(kc>0.01f){ Primitive o=tr; o.fade=kc; o.material=Material::Accent; g->Submit(o); }
        Primitive kn; kn.cx=tx+th2*0.5f+kc*(tw2-th2); kn.cy=cy; kn.hw=th2*0.5f-3; kn.hh=th2*0.5f-3; kn.corner_radius=th2*0.5f; kn.fade=1; kn.material=Material::Knob; g->Submit(kn);
        if(i<4) dl->AddLine(ImVec2(tl.x+18,ry+rowh),ImVec2(tl.x+cw-14,ry+rowh),SA(kInkSoft,0.14f),1.0f); }
      ImGui::Dummy(ImVec2(w, h+12)); }
}
static void Draw_Safari(float w){
    ImDrawList* dl=ImGui::GetWindowDrawList(); ImFont* f=ImGui::GetFont();
    float cw=w<700.0f?w:700.0f; float x0=ImGui::GetCursorScreenPos().x+(w-cw)*0.5f;
    static char url[128] = "github.com/poncippg-spec";
    { float h=44.0f; ImVec2 tl(x0, ImGui::GetCursorScreenPos().y);
      Primitive bg; bg.cx=tl.x+cw*0.5f; bg.cy=tl.y+h*0.5f; bg.hw=cw*0.5f; bg.hh=h*0.5f; bg.corner_radius=14; bg.fade=1; bg.material=Material::Thin; g->Submit(bg);
      DrawIcon(dl,Icon::Lock,ImVec2(tl.x+20,tl.y+h*0.5f),11.0f,kInkSoft,2.0f);
      ImGui::SetCursorScreenPos(ImVec2(tl.x+36, tl.y+h*0.5f-11.0f));
      ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0,0,0,0)); ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0,0,0,0)); ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0,0,0,0)); ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.93f,0.93f,0.96f,1.0f));
      ImGui::SetNextItemWidth(cw-72); ImGui::InputTextWithHint("##url","Search or enter website", url, sizeof(url));
      ImGui::PopStyleColor(4);
      DrawIcon(dl,Icon::Refresh,ImVec2(tl.x+cw-22,tl.y+h*0.5f),14.0f,kInkSoft,2.0f);
      ImGui::Dummy(ImVec2(w, h+12)); }
    { float h=360.0f; ImVec2 tl(x0, ImGui::GetCursorScreenPos().y);
      Primitive bg; bg.cx=tl.x+cw*0.5f; bg.cy=tl.y+h*0.5f; bg.hw=cw*0.5f; bg.hh=h*0.5f; bg.corner_radius=22; bg.fade=1; bg.material=Material::Thin; g->Submit(bg);
      dl->PushClipRect(ImVec2(tl.x,tl.y),ImVec2(tl.x+cw,tl.y+h),true);
      dl->AddRectFilledMultiColor(ImVec2(tl.x+16,tl.y+16),ImVec2(tl.x+cw-16,tl.y+150), IM_COL32(30,40,70,255),IM_COL32(70,90,140,255),IM_COL32(110,70,90,255),IM_COL32(40,50,80,255));
      dl->AddText(f,26.0f,ImVec2(tl.x+30,tl.y+110),IM_COL32(255,255,255,255),"Glass");
      dl->AddText(f,22.0f,ImVec2(tl.x+20,tl.y+166),kInk,"The latest on iPhone, Mac, and more");
      ImU32 line=SA(kInkSoft,0.4f), link=Accent();
      float ly=tl.y+204; for(int i=0;i<7;i++){ float lw=(i%3==2)?cw*0.5f:cw-44; dl->AddRectFilled(ImVec2(tl.x+20,ly),ImVec2(tl.x+20+lw,ly+9), (i==3||i==5)?SA(link,0.5f):line, 3.0f); ly+=20; }
      dl->PopClipRect(); ImGui::Dummy(ImVec2(w, h+12)); }
    { float h=54.0f; ImVec2 tl(x0, ImGui::GetCursorScreenPos().y);
      Primitive bg; bg.cx=tl.x+cw*0.5f; bg.cy=tl.y+h*0.5f; bg.hw=cw*0.5f; bg.hh=h*0.5f; bg.corner_radius=22; bg.fade=1; bg.material=Material::Thick; g->Submit(bg);
      Icon ic[5]={Icon::ChevronL,Icon::ChevronR,Icon::ShareNodes,Icon::Book,Icon::Copy};
      for(int i=0;i<5;i++) DrawIcon(dl,ic[i],ImVec2(tl.x+cw*(i+0.5f)/5.0f,tl.y+h*0.5f),17.0f,(i<2)?Accent():kInk,2.0f);
      dl->AddRect(ImVec2(tl.x+cw*4.5f/5.0f-9,tl.y+h*0.5f-8),ImVec2(tl.x+cw*4.5f/5.0f+9,tl.y+h*0.5f+8),kInk,3.0f,0,1.5f);
      dl->AddText(f,11.0f,ImVec2(tl.x+cw*4.5f/5.0f-3,tl.y+h*0.5f-7),kInk,"4");
      ImGui::Dummy(ImVec2(w, h+10)); }
}
