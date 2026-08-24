static void Draw_Phone(float w){
    ImDrawList* dl=ImGui::GetWindowDrawList(); ImFont* f=ImGui::GetFont();
    static char num[24]=""; static int nl=0;
    auto app=[&](char c){ if(nl<23){ num[nl++]=c; num[nl]=0; } };
    float cw=w<400.0f?w:400.0f; float x0=ImGui::GetCursorScreenPos().x+(w-cw)*0.5f;
    { const char* show = nl? num : "Enter number"; ImVec2 ts=f->CalcTextSizeA(nl?40.0f:18.0f,99999.0f,0,show);
      dl->AddText(f,nl?40.0f:18.0f, ImVec2(x0+cw*0.5f-ts.x*0.5f, ImGui::GetCursorScreenPos().y+10), nl?kInk:kInkSoft, show);
      ImGui::Dummy(ImVec2(w, 70.0f)); }
    struct K{ const char* d; const char* l; }; K ks[12]={ {"1",""},{"2","ABC"},{"3","DEF"},{"4","GHI"},{"5","JKL"},{"6","MNO"},{"7","PQRS"},{"8","TUV"},{"9","WXYZ"},{"*",""},{"0","+"},{"#",""} };
    float gap=18, d=(cw-2*gap)/3.0f; if(d>84) d=84; float gx0=x0+(cw-(3*d+2*gap))*0.5f;
    float top=ImGui::GetCursorScreenPos().y+d*0.5f;
    for(int i=0;i<12;i++){ int r=i/3,c=i%3; float cx=gx0+d*0.5f+c*(d+gap), cy=top+r*(d+gap);
        char id[6]; snprintf(id,6,"pk%d",i);
        ImGui::PushID(id); ImGui::SetCursorScreenPos(ImVec2(cx-d*0.5f,cy-d*0.5f)); ImGui::InvisibleButton("k",ImVec2(d,d));
        bool hov=ImGui::IsItemHovered(), clk=ImGui::IsItemDeactivated()&&hov, act=ImGui::IsItemActive();
        if(ImGui::IsItemActivated()) PlaySfx(Sfx::KeyTap);
        Spring& ksp=g->springs().Get((uint32_t)ImGui::GetID("k"),1,SpringStyle::Bouncy,1.0f); ksp.target=act?0.86f:(hov?1.05f:1.0f); ksp.Tick(Dt()); float ksc=Clampf(ksp.x,0.8f,1.2f);
        Primitive b; b.cx=cx; b.cy=cy; b.hw=d*0.5f*ksc; b.hh=d*0.5f*ksc; b.corner_radius=d*0.5f*ksc; b.fade=1; b.material=act?Material::Accent:(hov?Material::Knob:Material::Thin); b.elevate=hov?1.1f:1.0f; g->Submit(b);
        ImVec2 ds=f->CalcTextSizeA(30.0f,99999.0f,0,ks[i].d); dl->AddText(f,30.0f,ImVec2(cx-ds.x*0.5f,cy-(ks[i].l[0]?20.0f:15.0f)),kInk,ks[i].d);
        if(ks[i].l[0]){ ImVec2 ls=f->CalcTextSizeA(10.0f,99999.0f,0,ks[i].l); dl->AddText(f,10.0f,ImVec2(cx-ls.x*0.5f,cy+10),kInkSoft,ks[i].l); }
        ImGui::PopID(); if(clk) app(ks[i].d[0]); }
    float kb=top+4*(d+gap);
    ImGui::SetCursorScreenPos(ImVec2(gx0+d+gap, kb-d*0.5f)); ImGui::PushID("call"); ImGui::InvisibleButton("c",ImVec2(d,d)); bool ch=ImGui::IsItemDeactivated()&&ImGui::IsItemHovered(); ImGui::PopID();
    dl->AddCircleFilled(ImVec2(gx0+d+gap+d*0.5f,kb),d*0.5f,IM_COL32(48,209,88,255)); DrawIcon(dl,Icon::Phone,ImVec2(gx0+d+gap+d*0.5f,kb),d*0.42f,IM_COL32(255,255,255,255),2.4f);
    if(ch){}
    ImGui::SetCursorScreenPos(ImVec2(gx0+2*(d+gap), kb-d*0.5f)); ImGui::PushID("del"); ImGui::InvisibleButton("x",ImVec2(d,d)); if(ImGui::IsItemDeactivated()&&ImGui::IsItemHovered()){ if(nl>0) num[--nl]=0; } ImGui::PopID();
    if(nl>0) DrawIcon(dl,Icon::ChevronL,ImVec2(gx0+2*(d+gap)+d*0.5f,kb),20.0f,kInkSoft,2.0f);
    ImGui::SetCursorScreenPos(ImVec2(x0, kb+d*0.5f+10)); ImGui::Dummy(ImVec2(w, 8.0f));
}
static void Draw_Contacts(float w){
    ImDrawList* dl=ImGui::GetWindowDrawList(); ImFont* f=ImGui::GetFont();
    float cw=w<560.0f?w:560.0f; float x0=ImGui::GetCursorScreenPos().x+(w-cw)*0.5f;
    struct C{ const char* n; const char* s; ImU32 col; }; static C cs[]={
        {"Ava Martinez","Marketing",IM_COL32(48,209,88,255)},{"David Park","Finance",IM_COL32(255,159,10,255)},
        {"Emma Wilson","Design",IM_COL32(64,156,255,255)},{"James Carter","Engineering",IM_COL32(255,69,58,255)},
        {"Liam Patel","Sales",IM_COL32(146,82,232,255)},{"Noah Kim","Support",IM_COL32(255,90,140,255)},
        {"Olivia Bennett","Product",IM_COL32(94,92,230,255)},{"Sofia Rodriguez","Operations",IM_COL32(255,214,90,255)} };
    int n=8;
    static int sel=-1; if(sel>=n) sel=-1;
    auto initials=[&](const char* nm, char* out){ out[0]=nm[0]; out[1]=0; out[2]=0; const char* sp=strchr(nm,' '); if(sp) out[1]=sp[1]; };
    if(sel>=0){
        C& cc=cs[sel];
        float bh=44.0f; ImVec2 tb(x0, ImGui::GetCursorScreenPos().y);
        bool bhv=false; if(SurfaceHit("ctback", tb, ImVec2(140,bh), &bhv, Sfx::NavPop)) sel=-1;
        DrawIcon(dl, Icon::ChevronL, ImVec2(tb.x+10, tb.y+bh*0.5f), 15.0f, Accent(), 2.4f);
        dl->AddText(f,16.0f, ImVec2(tb.x+24, tb.y+bh*0.5f-9), Accent(), "Contacts");
        ImGui::Dummy(ImVec2(w, bh));
        float h=300.0f; ImVec2 tl(x0, ImGui::GetCursorScreenPos().y);
        Primitive bg; bg.cx=tl.x+cw*0.5f; bg.cy=tl.y+h*0.5f; bg.hw=cw*0.5f; bg.hh=h*0.5f; bg.corner_radius=24; bg.fade=1; bg.material=Material::Thin; g->Submit(bg);
        dl->AddCircleFilled(ImVec2(tl.x+cw*0.5f,tl.y+58),36.0f,cc.col);
        char ini[3]; initials(cc.n,ini); ImVec2 is=f->CalcTextSizeA(28.0f,99999.0f,0,ini); dl->AddText(f,28.0f,ImVec2(tl.x+cw*0.5f-is.x*0.5f,tl.y+58-is.y*0.5f),IM_COL32(255,255,255,255),ini);
        ImVec2 ns=f->CalcTextSizeA(24.0f,99999.0f,0,cc.n); dl->AddText(f,24.0f,ImVec2(tl.x+cw*0.5f-ns.x*0.5f,tl.y+104),kInk,cc.n);
        ImVec2 os=f->CalcTextSizeA(14.0f,99999.0f,0,cc.s); dl->AddText(f,14.0f,ImVec2(tl.x+cw*0.5f-os.x*0.5f,tl.y+134),kInkSoft,cc.s);
        dl->AddLine(ImVec2(tl.x+20,tl.y+168),ImVec2(tl.x+cw-20,tl.y+168),SA(kInkSoft,0.16f),1.0f);
        char ph[24]; snprintf(ph,24,"+1 (408) 555-%04d",1000+sel*111);
        dl->AddText(f,12.0f,ImVec2(tl.x+24,tl.y+178),kInkSoft,"mobile"); dl->AddText(f,16.0f,ImVec2(tl.x+24,tl.y+194),Accent(),ph);
        char em[48]; snprintf(em,48,"pondot@icloud.com");
        dl->AddText(f,12.0f,ImVec2(tl.x+24,tl.y+222),kInkSoft,"email"); dl->AddText(f,16.0f,ImVec2(tl.x+24,tl.y+238),Accent(),em);
        ImGui::SetCursorScreenPos(ImVec2(tl.x+20, tl.y+h-52)); Glass::Button("Message", true, ImVec2((cw-52)*0.5f,38.0f));
        ImGui::SetCursorScreenPos(ImVec2(tl.x+20+(cw-52)*0.5f+12, tl.y+h-52)); Glass::Button("Call", false, ImVec2((cw-52)*0.5f,38.0f));
        ImGui::SetCursorScreenPos(ImVec2(x0,tl.y)); ImGui::Dummy(ImVec2(w, h+12));
        return;
    }
    dl->AddText(f,30.0f, ImVec2(x0+2, ImGui::GetCursorScreenPos().y), kInk, "Contacts");
    ImGui::Dummy(ImVec2(w, 50.0f));
    float rowh=60.0f, h=n*rowh; ImVec2 tl(x0, ImGui::GetCursorScreenPos().y);
    Primitive bg; bg.cx=tl.x+cw*0.5f; bg.cy=tl.y+h*0.5f; bg.hw=cw*0.5f; bg.hh=h*0.5f; bg.corner_radius=24; bg.fade=1; bg.material=Material::Thin; g->Submit(bg);
    for(int i=0;i<n;i++){ float ry=tl.y+i*rowh, cy=ry+rowh*0.5f; bool hov=false;
        if(SurfaceHit(cs[i].n, ImVec2(tl.x,ry), ImVec2(cw-22,rowh), &hov, Sfx::NavPush)) sel=i;
        RowWash(dl, ImVec2(tl.x+6,ry+3), ImVec2(tl.x+cw-22,ry+rowh-3), 12.0f, false, hov);
        dl->AddCircleFilled(ImVec2(tl.x+34,cy),18.0f,cs[i].col);
        char ini[3]; initials(cs[i].n,ini);
        ImVec2 is=f->CalcTextSizeA(14.0f,99999.0f,0,ini); dl->AddText(f,14.0f,ImVec2(tl.x+34-is.x*0.5f,cy-is.y*0.5f),IM_COL32(255,255,255,255),ini);
        dl->AddText(f,16.0f,ImVec2(tl.x+62,cy-16),kInk,cs[i].n);
        dl->AddText(f,12.0f,ImVec2(tl.x+62,cy+3),kInkSoft,cs[i].s);
        if(i<n-1) dl->AddLine(ImVec2(tl.x+62,ry+rowh),ImVec2(tl.x+cw-30,ry+rowh),SA(kInkSoft,0.14f),1.0f); }
    const char* az="ABCDEFGHIJKLMNOPQRSTUVWXYZ"; int m=26; float iy0=tl.y+8, ih=(h-16)/m;
    for(int i=0;i<m;i++){ char ch[2]={az[i],0}; ImVec2 cs2=f->CalcTextSizeA(9.0f,99999.0f,0,ch); dl->AddText(f,9.0f,ImVec2(tl.x+cw-14-cs2.x*0.5f, iy0+i*ih), Accent(), ch); }
    ImGui::Dummy(ImVec2(w, h+12));
}
static void Draw_HomeKit(float w){
    ImDrawList* dl=ImGui::GetWindowDrawList(); ImFont* f=ImGui::GetFont();
    float cw=w<600.0f?w:600.0f; float x0=ImGui::GetCursorScreenPos().x+(w-cw)*0.5f;
    dl->AddText(f,30.0f, ImVec2(x0+2, ImGui::GetCursorScreenPos().y), kInk, "Living Room");
    dl->AddText(f,14.0f, ImVec2(x0+2, ImGui::GetCursorScreenPos().y+38), kInkSoft, "6 accessories \xC2\xB7 72\xC2\xB0 \xC2\xB7 All locked");
    ImGui::Dummy(ImVec2(w, 70.0f));
    struct Acc{ Icon ic; const char* n; const char* st; bool on; }; static Acc ac[6]={
        {Icon::Lightbulb,"Ceiling Lights","On \xC2\xB7 80%",true},{Icon::Thermometer,"Thermostat","Heat to 72\xC2\xB0",true},
        {Icon::Lock,"Front Door","Locked",false},{Icon::Wind,"Ceiling Fan","Off",false},
        {Icon::Tv,"Streaming Box","Playing",true},{Icon::Speaker,"HomePod","Grouped",true} };
    int n=6, cols=2; float gap=14, tw=(cw-gap)/2, th=92; float sy=ImGui::GetCursorScreenPos().y;
    for(int i=0;i<n;i++){ int r=i/cols,c=i%cols; float x=x0+c*(tw+gap), y=sy+r*(th+gap);
        ImGui::PushID(i); ImGui::SetCursorScreenPos(ImVec2(x,y)); ImGui::InvisibleButton("a",ImVec2(tw,th));
        bool hov=ImGui::IsItemHovered(); if(ImGui::IsItemDeactivated()&&hov) ac[i].on=!ac[i].on;
        float ok=TileOn("a",ac[i].on); ImGui::PopID();
        Primitive bg; bg.cx=x+tw*0.5f; bg.cy=y+th*0.5f; bg.hw=tw*0.5f; bg.hh=th*0.5f; bg.corner_radius=24; bg.fade=1; bg.material=Material::Thin; bg.elevate=hov?1.06f:1.0f; g->Submit(bg);
        if(ok>0.01f){ Primitive o=bg; o.fade=ok; o.material=Material::Accent; g->Submit(o); }
        ImU32 fg = ac[i].on?IM_COL32(255,255,255,255):kInk, fgs=ac[i].on?SA(IM_COL32(255,255,255,255),0.85f):kInkSoft;
        dl->AddCircleFilled(ImVec2(x+34,y+34),18.0f, ac[i].on?SA(IM_COL32(255,255,255,255),0.25f):SA(kInkSoft,0.18f));
        DrawIcon(dl,ac[i].ic,ImVec2(x+34,y+34),18.0f,fg,2.2f);
        dl->AddText(f,16.0f,ImVec2(x+62,y+22),fg,ac[i].n);
        dl->AddText(f,13.0f,ImVec2(x+62,y+46),fgs,ac[i].st); }
    int rows=(n+cols-1)/cols; ImGui::Dummy(ImVec2(w, rows*(th+gap)+4));
}
static void Draw_Battery(float w){
    ImDrawList* dl=ImGui::GetWindowDrawList(); ImFont* f=ImGui::GetFont();
    float cw=w<600.0f?w:600.0f; float x0=ImGui::GetCursorScreenPos().x+(w-cw)*0.5f;
    { float h=120.0f; ImVec2 tl(x0, ImGui::GetCursorScreenPos().y);
      Primitive bg; bg.cx=tl.x+cw*0.5f; bg.cy=tl.y+h*0.5f; bg.hw=cw*0.5f; bg.hh=h*0.5f; bg.corner_radius=28; bg.fade=1; bg.material=Material::Thin; g->Submit(bg);
      dl->AddText(f,56.0f,ImVec2(tl.x+24,tl.y+28),kInk,"87%");
      dl->AddText(f,14.0f,ImVec2(tl.x+26,tl.y+90),kInkSoft,"Last charged to 100% at 7:30 AM");
      float bx=tl.x+cw-130, by=tl.y+40, bw=92, bh=42;
      dl->AddRect(ImVec2(bx,by),ImVec2(bx+bw,by+bh),kInkSoft,8.0f,0,2.5f); dl->AddRectFilled(ImVec2(bx+bw,by+12),ImVec2(bx+bw+6,by+bh-12),kInkSoft,2.0f);
      dl->AddRectFilled(ImVec2(bx+4,by+4),ImVec2(bx+4+(bw-8)*0.87f,by+bh-4),IM_COL32(48,209,88,255),5.0f);
      ImGui::Dummy(ImVec2(w, h+14)); }
    { float h=180.0f; ImVec2 tl(x0, ImGui::GetCursorScreenPos().y);
      Primitive bg; bg.cx=tl.x+cw*0.5f; bg.cy=tl.y+h*0.5f; bg.hw=cw*0.5f; bg.hh=h*0.5f; bg.corner_radius=28; bg.fade=1; bg.material=Material::Thin; g->Submit(bg);
      dl->AddText(f,12.0f,ImVec2(tl.x+20,tl.y+14),kInkSoft,"LAST 24 HOURS");
      int nb=24; float gx0=tl.x+20, gw=cw-40, bw=gw/nb, by=tl.y+h-34, bh=100;
      for(int i=0;i<nb;i++){ float lvl=0.2f+0.7f*fabsf(sinf(i*0.5f))*((i>8&&i<11)?0.3f:1.0f); float hh=bh*lvl; float cx=gx0+bw*(i+0.5f);
        ImU32 c=(i>=18)?IM_COL32(48,209,88,255):(lvl>0.6f?Accent():SA(Accent(),0.6f));
        dl->AddRectFilled(ImVec2(cx-bw*0.32f,by-hh),ImVec2(cx+bw*0.32f,by),c,2.0f); }
      const char* hl[5]={"12AM","6AM","12PM","6PM","Now"};
      for(int i=0;i<5;i++){ float lx=gx0+gw*i/4.0f; ImVec2 ls=f->CalcTextSizeA(11.0f,99999.0f,0,hl[i]); dl->AddText(f,11.0f,ImVec2(lx-(i==4?ls.x:ls.x*0.5f),by+8),kInkSoft,hl[i]); }
      ImGui::Dummy(ImVec2(w, h+14)); }
    { static bool lp=false; float h=56.0f; ImVec2 tl(x0, ImGui::GetCursorScreenPos().y);
      Primitive bg; bg.cx=tl.x+cw*0.5f; bg.cy=tl.y+h*0.5f; bg.hw=cw*0.5f; bg.hh=h*0.5f; bg.corner_radius=22; bg.fade=1; bg.material=Material::Thin; g->Submit(bg);
      DrawIcon(dl,Icon::Bolt,ImVec2(tl.x+30,tl.y+h*0.5f),17.0f,IM_COL32(255,214,90,255),2.0f);
      dl->AddText(f,16.0f,ImVec2(tl.x+52,tl.y+h*0.5f-9),kInk,"Low Power Mode");
      float tw2=44,th2=26,tx=tl.x+cw-tw2-18; ImGui::PushID("lp"); ImGui::SetCursorScreenPos(ImVec2(tx,tl.y+h*0.5f-th2*0.5f)); ImGui::InvisibleButton("t",ImVec2(tw2,th2));
      if(ImGui::IsItemDeactivated()&&ImGui::IsItemHovered()) lp=!lp; Spring& s=g->springs().Get((uint32_t)ImGui::GetID("t"),5,SpringStyle::Critical,lp?1.0f:0.0f); s.target=lp?1.0f:0.0f; s.Tick(Dt()); float kc=Clampf(s.x,0,1); ImGui::PopID();
      Primitive tr; tr.cx=tx+tw2*0.5f; tr.cy=tl.y+h*0.5f; tr.hw=tw2*0.5f; tr.hh=th2*0.5f; tr.corner_radius=th2*0.5f; tr.fade=1; tr.material=Material::Thin; g->Submit(tr);
      if(kc>0.01f){ Primitive o=tr; o.fade=kc; o.material=Material::Accent; g->Submit(o);} Primitive kn; kn.cx=tx+th2*0.5f+kc*(tw2-th2); kn.cy=tl.y+h*0.5f; kn.hw=th2*0.5f-3; kn.hh=th2*0.5f-3; kn.corner_radius=th2*0.5f; kn.fade=1; kn.material=Material::Knob; g->Submit(kn);
      ImGui::Dummy(ImVec2(w, h+12)); }
}
static void Draw_GarageBand(float w){
    ImDrawList* dl=ImGui::GetWindowDrawList(); ImFont* f=ImGui::GetFont();
    float cw=w<720.0f?w:720.0f; float x0=ImGui::GetCursorScreenPos().x+(w-cw)*0.5f;
    float t=(float)ImGui::GetTime();
    { float h=50.0f; ImVec2 tl(x0, ImGui::GetCursorScreenPos().y);
      Primitive bg; bg.cx=tl.x+cw*0.5f; bg.cy=tl.y+h*0.5f; bg.hw=cw*0.5f; bg.hh=h*0.5f; bg.corner_radius=18; bg.fade=1; bg.material=Material::Thick; g->Submit(bg);
      DrawIcon(dl,Icon::Backward,ImVec2(tl.x+30,tl.y+h*0.5f),16.0f,kInk,2.0f);
      dl->AddCircleFilled(ImVec2(tl.x+70,tl.y+h*0.5f),16.0f,Accent()); DrawIcon(dl,Icon::Play,ImVec2(tl.x+70,tl.y+h*0.5f),14.0f,IM_COL32(255,255,255,255),2.0f);
      DrawIcon(dl,Icon::Forward,ImVec2(tl.x+110,tl.y+h*0.5f),16.0f,kInk,2.0f);
      dl->AddText(f,18.0f,ImVec2(tl.x+cw*0.5f-30,tl.y+h*0.5f-11),kInk,"120 BPM");
      dl->AddText(f,15.0f,ImVec2(tl.x+cw-90,tl.y+h*0.5f-9),kInkSoft,"4 / 4");
      ImGui::Dummy(ImVec2(w, h+12)); }
    { struct Tk{ Icon ic; const char* n; ImU32 c; }; Tk tks[]={ {Icon::Music,"Drums",IM_COL32(64,156,255,255)},{Icon::Guitar,"Bass",IM_COL32(255,159,10,255)},{Icon::Music,"Keys",IM_COL32(146,82,232,255)},{Icon::Microphone,"Vocals",IM_COL32(255,69,58,255)} };
      int n=4; float th=66.0f, hdr=120.0f, h=n*th; ImVec2 tl(x0, ImGui::GetCursorScreenPos().y);
      Primitive bg; bg.cx=tl.x+cw*0.5f; bg.cy=tl.y+h*0.5f; bg.hw=cw*0.5f; bg.hh=h*0.5f; bg.corner_radius=22; bg.fade=1; bg.material=Material::Thin; g->Submit(bg);
      dl->PushClipRect(ImVec2(tl.x,tl.y),ImVec2(tl.x+cw,tl.y+h),true);
      for(int i=0;i<n;i++){ float ty=tl.y+i*th, cy=ty+th*0.5f;
        DrawIcon(dl,tks[i].ic,ImVec2(tl.x+22,cy),16.0f,tks[i].c,2.0f); dl->AddText(f,14.0f,ImVec2(tl.x+40,cy-8),kInk,tks[i].n);
        float lane=tl.x+hdr; int regs=(i%2==0)?3:2;
        for(int r=0;r<regs;r++){ float rx=lane+r*((cw-hdr-20)/3.0f)+8, rw=(cw-hdr-20)/3.0f-12;
          dl->AddRectFilled(ImVec2(rx,ty+10),ImVec2(rx+rw,ty+th-10), SA(tks[i].c,0.55f), 6.0f);
          for(float wx=rx+6; wx<rx+rw-6; wx+=5){ float wy=fabsf(sinf(wx*0.3f+i))*((th-28)*0.4f); dl->AddLine(ImVec2(wx,cy-wy),ImVec2(wx,cy+wy),SA(IM_COL32(255,255,255,255),0.5f),1.0f); } }
        if(i<n-1) dl->AddLine(ImVec2(tl.x,ty+th),ImVec2(tl.x+cw,ty+th),SA(kInkSoft,0.12f),1.0f); }
      float ph=tl.x+hdr+ fmodf(t*40.0f, cw-hdr-20); dl->AddLine(ImVec2(ph,tl.y),ImVec2(ph,tl.y+h),IM_COL32(255,255,255,200),1.5f);
      dl->PopClipRect(); ImGui::Dummy(ImVec2(w, h+12)); }
}
static void Draw_Watch(float w){
    ImDrawList* dl=ImGui::GetWindowDrawList(); ImFont* f=ImGui::GetFont();
    float cw=w<640.0f?w:640.0f; float x0=ImGui::GetCursorScreenPos().x+(w-cw)*0.5f;
    time_t tt=time(nullptr); struct tm lt; localtime_s(&lt,&tt);
    dl->AddText(f,20.0f, ImVec2(x0+2, ImGui::GetCursorScreenPos().y), kInk, "Faces");
    ImGui::Dummy(ImVec2(w, 36.0f));
    int n=3; float gap=18, tw=(cw-2*gap)/3.0f, fr=tw*0.42f; float sy=ImGui::GetCursorScreenPos().y;
    for(int i=0;i<n;i++){ float x=x0+i*(tw+gap); ImVec2 c(x+tw*0.5f, sy+fr+14);
        dl->AddRectFilled(ImVec2(c.x-fr-10,c.y-fr-10),ImVec2(c.x+fr+10,c.y+fr+10), IM_COL32(20,20,24,255), (fr+10)*0.5f);
        dl->AddCircleFilled(c, fr, IM_COL32(8,8,10,255), 48);
        if(i==0){
            for(int k=0;k<12;k++){ float a=k*(6.2831853f/12.0f); dl->AddLine(ImVec2(c.x+cosf(a)*fr*0.82f,c.y+sinf(a)*fr*0.82f),ImVec2(c.x+cosf(a)*fr*0.92f,c.y+sinf(a)*fr*0.92f),SA(IM_COL32(255,255,255,255),0.7f),2.0f); }
            float ah=(lt.tm_hour%12+lt.tm_min/60.0f)/12.0f*6.2831853f-1.5708f, am=lt.tm_min/60.0f*6.2831853f-1.5708f;
            dl->AddLine(c,ImVec2(c.x+cosf(ah)*fr*0.5f,c.y+sinf(ah)*fr*0.5f),IM_COL32(255,255,255,255),3.0f);
            dl->AddLine(c,ImVec2(c.x+cosf(am)*fr*0.72f,c.y+sinf(am)*fr*0.72f),IM_COL32(255,255,255,255),2.0f);
            dl->AddCircleFilled(c,3.0f,Accent());
        } else if(i==1){
            char tb[8]; FmtClock(tb,8,lt.tm_hour,lt.tm_min); ImVec2 ts=f->CalcTextSizeA(34.0f,99999.0f,0,tb);
            dl->AddText(f,34.0f,ImVec2(c.x-ts.x*0.5f,c.y-ts.y*0.5f),Accent(),tb);
            DrawIcon(dl,Icon::Heart,ImVec2(c.x-fr*0.4f,c.y-fr*0.5f),12.0f,IM_COL32(255,55,95,255),2.0f);
            DrawIcon(dl,Icon::CloudSun,ImVec2(c.x+fr*0.4f,c.y-fr*0.5f),12.0f,IM_COL32(64,156,255,255),2.0f);
        } else {
            float rf[3]={0.8f,0.6f,0.95f}; ImU32 cl[3]={IM_COL32(255,55,95,255),IM_COL32(126,255,40,255),IM_COL32(64,220,255,255)}; float rr[3]={fr*0.8f,fr*0.62f,fr*0.44f};
            for(int k=0;k<3;k++){ dl->PathArcTo(c,rr[k],-1.5708f,-1.5708f+6.2831853f,48); dl->PathStroke(SA(cl[k],0.22f),0,6.0f); dl->PathArcTo(c,rr[k],-1.5708f,-1.5708f+6.2831853f*rf[k],48); dl->PathStroke(cl[k],0,6.0f); }
        }
    }
    ImGui::Dummy(ImVec2(w, 2*fr+50.0f));
}
static void Draw_AirPods(float w){
    ImDrawList* dl=ImGui::GetWindowDrawList(); ImFont* f=ImGui::GetFont();
    float cw=w<520.0f?w:520.0f; float x0=ImGui::GetCursorScreenPos().x+(w-cw)*0.5f;
    { float h=200.0f; ImVec2 tl(x0, ImGui::GetCursorScreenPos().y);
      Primitive bg; bg.cx=tl.x+cw*0.5f; bg.cy=tl.y+h*0.5f; bg.hw=cw*0.5f; bg.hh=h*0.5f; bg.corner_radius=29; bg.fade=1; bg.material=Material::Thin; g->Submit(bg);
      dl->AddText(f,22.0f,ImVec2(tl.x+22,tl.y+18),kInk,"AirPods Pro");
      ImU32 white=IM_COL32(245,245,248,255);
      for(int i=0;i<2;i++){ float bx=tl.x+cw*0.5f+(i==0?-40:10); float by=tl.y+90;
        dl->AddCircleFilled(ImVec2(bx,by),16.0f,white); dl->AddRectFilled(ImVec2(bx-5,by+8),ImVec2(bx+5,by+44),white,5.0f); dl->AddCircleFilled(ImVec2(bx,by-2),5.0f,IM_COL32(120,120,130,255)); }
      ImGui::Dummy(ImVec2(w, h+14)); }
    { float h=160.0f; ImVec2 tl(x0, ImGui::GetCursorScreenPos().y);
      Primitive bg; bg.cx=tl.x+cw*0.5f; bg.cy=tl.y+h*0.5f; bg.hw=cw*0.5f; bg.hh=h*0.5f; bg.corner_radius=28; bg.fade=1; bg.material=Material::Thin; g->Submit(bg);
      struct B{ const char* l; float v; }; B bs[3]={ {"Left",0.92f},{"Case",0.68f},{"Right",0.88f} };
      for(int i=0;i<3;i++){ ImVec2 c(tl.x+cw*(i+0.5f)/3.0f, tl.y+h*0.46f); float r=40;
        dl->PathArcTo(c,r,-1.5708f,-1.5708f+6.2831853f,48); dl->PathStroke(SA(kInkSoft,0.2f),0,8.0f);
        ImU32 col=bs[i].v>0.2f?IM_COL32(48,209,88,255):IM_COL32(255,69,58,255);
        dl->PathArcTo(c,r,-1.5708f,-1.5708f+6.2831853f*bs[i].v,48); dl->PathStroke(col,0,8.0f);
        char pb[8]; snprintf(pb,8,"%d%%",(int)(bs[i].v*100)); ImVec2 ps=f->CalcTextSizeA(20.0f,99999.0f,0,pb); dl->AddText(f,20.0f,ImVec2(c.x-ps.x*0.5f,c.y-ps.y*0.5f),kInk,pb);
        ImVec2 ls=f->CalcTextSizeA(14.0f,99999.0f,0,bs[i].l); dl->AddText(f,14.0f,ImVec2(c.x-ls.x*0.5f,c.y+r+10),kInkSoft,bs[i].l); }
      ImGui::Dummy(ImVec2(w, h+12)); }
}
static void Draw_Journal(float w){
    ImDrawList* dl=ImGui::GetWindowDrawList(); ImFont* f=ImGui::GetFont();
    float cw=w<600.0f?w:600.0f; float x0=ImGui::GetCursorScreenPos().x+(w-cw)*0.5f;
    dl->AddText(f,30.0f, ImVec2(x0+2, ImGui::GetCursorScreenPos().y), kInk, "Journal");
    ImGui::Dummy(ImVec2(w, 50.0f));
    struct E{ const char* date; const char* title; const char* body; ImU32 c; };
    E es[]={
        {"TODAY","A good day's work","Wrapped up the project I've been heads-down on for weeks. Relieved, and a little proud.",IM_COL32(64,156,255,255)},
        {"MAY 28","A quiet morning","Coffee by the window before anyone else was up. The light was perfect. I should do this more often.",IM_COL32(255,159,10,255)},
        {"MAY 26","Dinner with friends","Too much pasta, too much laughing. The kind of evening that reminds you what matters.",IM_COL32(146,82,232,255)},
        {"MAY 24","Long run","3.8 miles along the river. Cleared my head completely \xE2\x80\x94 felt great by the end.",IM_COL32(48,209,88,255)},
    };
    for(int i=0;i<4;i++){ float h=110.0f; ImVec2 tl(x0, ImGui::GetCursorScreenPos().y);
        Primitive bg; bg.cx=tl.x+cw*0.5f; bg.cy=tl.y+h*0.5f; bg.hw=cw*0.5f; bg.hh=h*0.5f; bg.corner_radius=24; bg.fade=1; bg.material=Material::Thin; g->Submit(bg);
        dl->AddRectFilled(ImVec2(tl.x,tl.y),ImVec2(tl.x+6,tl.y+h), es[i].c, 18.0f);
        dl->AddText(f,12.0f,ImVec2(tl.x+22,tl.y+14),es[i].c,es[i].date);
        dl->AddText(f,18.0f,ImVec2(tl.x+22,tl.y+32),kInk,es[i].title);
        dl->AddText(f,14.0f,ImVec2(tl.x+22,tl.y+60),kInkSoft,es[i].body,NULL,cw-40);
        ImGui::Dummy(ImVec2(w, h+12)); }
}
static void Draw_Numbers(float w){
    ImDrawList* dl=ImGui::GetWindowDrawList(); ImFont* f=ImGui::GetFont();
    float cw=w<660.0f?w:660.0f; float x0=ImGui::GetCursorScreenPos().x+(w-cw)*0.5f;
    const char* cols[5]={"Item","Q1","Q2","Q3","Total"};
    const char* items[4]={"Phones","Tablets","Laptops","Watches"};
    static int data[4][3]={ {42,55,61},{30,38,44},{18,22,29},{50,48,52} };
    static int selR=-1, selC=-1;
    { int rows=5; float rowh=40.0f, h=rows*rowh; ImVec2 tl(x0, ImGui::GetCursorScreenPos().y);
      Primitive bg; bg.cx=tl.x+cw*0.5f; bg.cy=tl.y+h*0.5f; bg.hw=cw*0.5f; bg.hh=h*0.5f; bg.corner_radius=22; bg.fade=1; bg.material=Material::Thin; g->Submit(bg);
      float cw0=cw*0.3f, colw=(cw-cw0)/4.0f;
      dl->AddRectFilled(ImVec2(tl.x,tl.y),ImVec2(tl.x+cw,tl.y+rowh), SA(Accent(),0.18f), 16.0f);
      for(int c=0;c<5;c++){ float cx = c==0? tl.x+16 : tl.x+cw0+(c-1)*colw+12; dl->AddText(f,14.0f,ImVec2(cx,tl.y+12),kInk,cols[c]); }
      for(int r=0;r<4;r++){ float ry=tl.y+(r+1)*rowh; int tot=data[r][0]+data[r][1]+data[r][2];
        dl->AddText(f,15.0f,ImVec2(tl.x+16,ry+11),kInk,items[r]);
        for(int c=0;c<3;c++){ float cellx=tl.x+cw0+c*colw; bool hov=false; char cid[12]; snprintf(cid,12,"nc%d_%d",r,c);
          if(SurfaceHit(cid, ImVec2(cellx,ry), ImVec2(colw,rowh), &hov)){ selR=r; selC=c; }
          if(r==selR&&c==selC) dl->AddRect(ImVec2(cellx+2,ry+2),ImVec2(cellx+colw-2,ry+rowh-2), Accent(), 6.0f, 0, 2.0f);
          else if(hov) dl->AddRectFilled(ImVec2(cellx+2,ry+2),ImVec2(cellx+colw-2,ry+rowh-2), SA(kInkSoft,0.10f), 6.0f);
          char vb[8]; snprintf(vb,8,"%d",data[r][c]); dl->AddText(f,15.0f,ImVec2(cellx+12,ry+11),kInkSoft,vb); }
        char tb[8]; snprintf(tb,8,"%d",tot); dl->AddText(f,15.0f,ImVec2(tl.x+cw0+3*colw+12,ry+11),Accent(),tb);
        dl->AddLine(ImVec2(tl.x,ry),ImVec2(tl.x+cw,ry),SA(kInkSoft,0.14f),1.0f); }
      dl->AddLine(ImVec2(tl.x+cw0,tl.y),ImVec2(tl.x+cw0,tl.y+h),SA(kInkSoft,0.14f),1.0f);
      for(int c=1;c<4;c++) dl->AddLine(ImVec2(tl.x+cw0+c*colw,tl.y),ImVec2(tl.x+cw0+c*colw,tl.y+h),SA(kInkSoft,0.1f),1.0f);
      ImGui::Dummy(ImVec2(w, h+14)); }
    if(selR>=0&&selC>=0){ ImGui::Indent((w-cw)*0.5f); ImGui::PushItemWidth(cw);
      char lbl[48]; snprintf(lbl,48,"%s \xC2\xB7 %s",items[selR],cols[selC+1]);
      Glass::Stepper(lbl, &data[selR][selC], 1, 0, 999);
      ImGui::PopItemWidth(); ImGui::Unindent((w-cw)*0.5f); ImGui::Dummy(ImVec2(0,8)); }
    { float h=180.0f; ImVec2 tl(x0, ImGui::GetCursorScreenPos().y);
      Primitive bg; bg.cx=tl.x+cw*0.5f; bg.cy=tl.y+h*0.5f; bg.hw=cw*0.5f; bg.hh=h*0.5f; bg.corner_radius=22; bg.fade=1; bg.material=Material::Thin; g->Submit(bg);
      dl->AddText(f,12.0f,ImVec2(tl.x+18,tl.y+14),kInkSoft,"TOTAL BY ITEM");
      int tot[4]; int mx=1; for(int r=0;r<4;r++){ tot[r]=data[r][0]+data[r][1]+data[r][2]; if(tot[r]>mx)mx=tot[r]; }
      float gx0=tl.x+30, gw=cw-60, by=tl.y+h-34, bh=104; float bw=gw/4.0f;
      ImU32 cls[4]={IM_COL32(64,156,255,255),IM_COL32(48,209,88,255),IM_COL32(255,159,10,255),IM_COL32(255,69,58,255)};
      for(int r=0;r<4;r++){ float cx=gx0+bw*(r+0.5f); float hh=bh*tot[r]/mx;
        dl->AddRectFilled(ImVec2(cx-bw*0.3f,by-hh),ImVec2(cx+bw*0.3f,by),cls[r],4.0f);
        ImVec2 ls=f->CalcTextSizeA(12.0f,99999.0f,0,items[r]); dl->AddText(f,12.0f,ImVec2(cx-ls.x*0.5f,by+8),kInkSoft,items[r]);
        char tb[8]; snprintf(tb,8,"%d",tot[r]); ImVec2 ts=f->CalcTextSizeA(13.0f,99999.0f,0,tb); dl->AddText(f,13.0f,ImVec2(cx-ts.x*0.5f,by-hh-18),kInk,tb); }
      ImGui::Dummy(ImVec2(w, h+12)); }
}
static void Draw_Measure(float w){
    ImDrawList* dl=ImGui::GetWindowDrawList(); ImFont* f=ImGui::GetFont();
    float cw=w<560.0f?w:560.0f; float x0=ImGui::GetCursorScreenPos().x+(w-cw)*0.5f;
    float h=380.0f; ImVec2 tl(x0, ImGui::GetCursorScreenPos().y);
    dl->AddRectFilledMultiColor(ImVec2(tl.x,tl.y),ImVec2(tl.x+cw,tl.y+h), IM_COL32(50,52,60,255),IM_COL32(70,72,84,255),IM_COL32(40,42,50,255),IM_COL32(60,58,66,255));
    dl->AddRect(ImVec2(tl.x,tl.y),ImVec2(tl.x+cw,tl.y+h), SA(IM_COL32(255,255,255,255),0.12f),20.0f,0,1.5f);
    dl->PushClipRect(ImVec2(tl.x,tl.y),ImVec2(tl.x+cw,tl.y+h),true);
    dl->AddQuadFilled(ImVec2(tl.x+60,tl.y+260),ImVec2(tl.x+cw-80,tl.y+200),ImVec2(tl.x+cw-40,tl.y+250),ImVec2(tl.x+100,tl.y+320), SA(IM_COL32(160,140,120,255),0.5f));
    ImVec2 a(tl.x+60,tl.y+260), b(tl.x+cw-80,tl.y+200); ImU32 yel=IM_COL32(255,214,90,255);
    dl->AddLine(a,b,yel,2.5f);
    ImVec2 dir=ImVec2(b.x-a.x,b.y-a.y); float len=sqrtf(dir.x*dir.x+dir.y*dir.y); ImVec2 pn=ImVec2(-dir.y/len,dir.x/len);
    dl->AddLine(ImVec2(a.x+pn.x*8,a.y+pn.y*8),ImVec2(a.x-pn.x*8,a.y-pn.y*8),yel,2.5f);
    dl->AddLine(ImVec2(b.x+pn.x*8,b.y+pn.y*8),ImVec2(b.x-pn.x*8,b.y-pn.y*8),yel,2.5f);
    ImVec2 mid((a.x+b.x)*0.5f,(a.y+b.y)*0.5f); const char* dim="1.42 m"; ImVec2 ds=f->CalcTextSizeA(16.0f,99999.0f,0,dim);
    dl->AddRectFilled(ImVec2(mid.x-ds.x*0.5f-10,mid.y-16),ImVec2(mid.x+ds.x*0.5f+10,mid.y+12), IM_COL32(255,214,90,255), 8.0f);
    dl->AddText(f,16.0f,ImVec2(mid.x-ds.x*0.5f,mid.y-12),IM_COL32(30,28,10,255),dim);
    dl->AddCircle(ImVec2(tl.x+cw*0.5f,tl.y+h*0.5f),10.0f,IM_COL32(255,255,255,200),0,2.0f); dl->AddCircleFilled(ImVec2(tl.x+cw*0.5f,tl.y+h*0.5f),2.0f,IM_COL32(255,255,255,255));
    dl->PopClipRect();
    dl->AddCircleFilled(ImVec2(tl.x+cw*0.5f,tl.y+h-2),22.0f,SA(IM_COL32(255,255,255,255),0.16f)); DrawIcon(dl,Icon::Plus,ImVec2(tl.x+cw*0.5f,tl.y+h-2),18.0f,IM_COL32(255,255,255,255),2.4f);
    ImGui::Dummy(ImVec2(w, h+30.0f));
}
static void Draw_Magnifier(float w){
    ImDrawList* dl=ImGui::GetWindowDrawList();
    static float zoom=0.5f; static int filt=0;
    float cw=w<560.0f?w:560.0f; float x0=ImGui::GetCursorScreenPos().x+(w-cw)*0.5f;
    float h=320.0f; ImVec2 tl(x0, ImGui::GetCursorScreenPos().y);
    dl->AddRectFilled(ImVec2(tl.x,tl.y),ImVec2(tl.x+cw,tl.y+h), IM_COL32(28,30,36,255), 20.0f);
    dl->PushClipRect(ImVec2(tl.x,tl.y),ImVec2(tl.x+cw,tl.y+h),true);
    for(float ly=tl.y+30; ly<tl.y+h-30; ly+=16){ float lw=(((int)ly)%3==0)?cw*0.4f:cw*0.7f; dl->AddRectFilled(ImVec2(tl.x+30,ly),ImVec2(tl.x+30+lw,ly+5),SA(kInkSoft,0.4f),2.0f); }
    ImVec2 lc(tl.x+cw*0.5f,tl.y+h*0.45f); float lr=70.0f+zoom*30.0f;
    dl->AddCircleFilled(lc,lr,IM_COL32(18,20,26,255),48);
    dl->PushClipRect(ImVec2(lc.x-lr,lc.y-lr),ImVec2(lc.x+lr,lc.y+lr),true);
    for(float ly=lc.y-lr; ly<lc.y+lr; ly+=16+zoom*16){ dl->AddRectFilled(ImVec2(lc.x-lr+10,ly),ImVec2(lc.x+lr-10,ly+6+zoom*4),SA(kInk,0.7f),2.0f); }
    dl->PopClipRect();
    dl->AddCircle(lc,lr,IM_COL32(255,214,90,255),48,3.0f);
    dl->PopClipRect();
    ImGui::Dummy(ImVec2(w, h+14));
    ImGui::SetCursorScreenPos(ImVec2(x0, ImGui::GetCursorScreenPos().y));
    ImGui::PushItemWidth(cw); Glass::Slider("Zoom", &zoom, 0.0f, 1.0f, "%.0f%%"); ImGui::PopItemWidth();
    ImGui::Dummy(ImVec2(0,4));
    { const char* fl[3]={"None","Invert","Mono"};
      if(Glass::Chip(fl[0], filt==0)) filt=0; ImGui::SameLine(0,8);
      if(Glass::Chip(fl[1], filt==1)) filt=1; ImGui::SameLine(0,8);
      if(Glass::Chip(fl[2], filt==2)) filt=2; }
    ImGui::Dummy(ImVec2(w, 8.0f));
}
static void Draw_Shazam(float w){
    ImDrawList* dl=ImGui::GetWindowDrawList(); ImFont* f=ImGui::GetFont();
    float cw=w<520.0f?w:520.0f; float x0=ImGui::GetCursorScreenPos().x+(w-cw)*0.5f; float t=(float)ImGui::GetTime();
    ImVec2 c(x0+cw*0.5f, ImGui::GetCursorScreenPos().y+150.0f);
    for(int i=0;i<3;i++){ float ph=fmodf(t*0.5f+i*0.33f,1.0f); dl->AddCircle(c, 70.0f+ph*70.0f, SA(Accent(),(1.0f-ph)*0.5f), 64, 2.5f); }
    dl->AddCircleFilled(c, 70.0f, SA(Accent(),0.9f), 64); DrawIcon(dl,Icon::Music,c,56.0f,IM_COL32(255,255,255,255),2.6f);
    ImVec2 ts=f->CalcTextSizeA(16.0f,99999.0f,0,"Tap to Shazam"); dl->AddText(f,16.0f,ImVec2(c.x-ts.x*0.5f,c.y+92),kInkSoft,"Tap to Shazam");
    ImGui::Dummy(ImVec2(w, 300.0f));
    { dl->AddText(f,18.0f,ImVec2(x0+4,ImGui::GetCursorScreenPos().y),kInk,"Recent"); ImGui::Dummy(ImVec2(w,28.0f));
      struct M{ const char* t; const char* a; }; M ms[]={ {"Coastline","June Park"},{"Golden Days","The Hours"},{"Open Road","Marigold"} };
      float rowh=58.0f,h=3*rowh; ImVec2 tl(x0, ImGui::GetCursorScreenPos().y);
      Primitive bg; bg.cx=tl.x+cw*0.5f; bg.cy=tl.y+h*0.5f; bg.hw=cw*0.5f; bg.hh=h*0.5f; bg.corner_radius=24; bg.fade=1; bg.material=Material::Thin; g->Submit(bg);
      for(int i=0;i<3;i++){ float ry=tl.y+i*rowh, cy=ry+rowh*0.5f; Primitive a; a.cx=tl.x+34; a.cy=cy; a.hw=18; a.hh=18; a.corner_radius=10; a.fade=1; a.material=Material::Accent; g->Submit(a); DrawIcon(dl,Icon::Music,ImVec2(tl.x+34,cy),16.0f,IM_COL32(255,255,255,255),2.0f);
        dl->AddText(f,16.0f,ImVec2(tl.x+62,cy-16),kInk,ms[i].t); dl->AddText(f,13.0f,ImVec2(tl.x+62,cy+3),kInkSoft,ms[i].a);
        if(i<2) dl->AddLine(ImVec2(tl.x+62,ry+rowh),ImVec2(tl.x+cw-14,ry+rowh),SA(kInkSoft,0.14f),1.0f); }
      ImGui::Dummy(ImVec2(w, h+12)); }
}
static void Draw_Sleep(float w){
    ImDrawList* dl=ImGui::GetWindowDrawList(); ImFont* f=ImGui::GetFont();
    float cw=w<560.0f?w:560.0f; float x0=ImGui::GetCursorScreenPos().x+(w-cw)*0.5f;
    { float h=220.0f; ImVec2 tl(x0, ImGui::GetCursorScreenPos().y); ImVec2 c(tl.x+cw*0.5f,tl.y+h*0.5f); float R=80.0f;
      Primitive bg; bg.cx=tl.x+cw*0.5f; bg.cy=tl.y+h*0.5f; bg.hw=cw*0.5f; bg.hh=h*0.5f; bg.corner_radius=29; bg.fade=1; bg.material=Material::Thin; g->Submit(bg);
      dl->AddCircle(c,R,SA(kInkSoft,0.2f),64,14.0f);
      float a0=-1.5708f+ (23.0f/12.0f)*6.2831853f*0.5f, a1=a0+ (7.5f/12.0f)*6.2831853f;
      dl->PathArcTo(c,R,a0,a1,64); dl->PathStroke(SA(Accent(),0.85f),0,14.0f);
      DrawIcon(dl,Icon::Bed,ImVec2(c.x+cosf(a0)*R,c.y+sinf(a0)*R),12.0f,IM_COL32(255,255,255,255),2.0f);
      DrawIcon(dl,Icon::Bell,ImVec2(c.x+cosf(a1)*R,c.y+sinf(a1)*R),12.0f,IM_COL32(255,255,255,255),2.0f);
      dl->AddText(f,22.0f,ImVec2(c.x-58,c.y-22),kInk,"7h 30m"); ImVec2 ss=f->CalcTextSizeA(13.0f,99999.0f,0,"11:00 PM - 6:30 AM"); dl->AddText(f,13.0f,ImVec2(c.x-ss.x*0.5f,c.y+6),kInkSoft,"11:00 PM - 6:30 AM");
      ImGui::Dummy(ImVec2(w, h+14)); }
    { float h=120.0f; ImVec2 tl(x0, ImGui::GetCursorScreenPos().y);
      Primitive bg; bg.cx=tl.x+cw*0.5f; bg.cy=tl.y+h*0.5f; bg.hw=cw*0.5f; bg.hh=h*0.5f; bg.corner_radius=26; bg.fade=1; bg.material=Material::Thin; g->Submit(bg);
      dl->AddText(f,12.0f,ImVec2(tl.x+18,tl.y+12),kInkSoft,"LAST NIGHT");
      const char* lvl[4]={"Awake","REM","Core","Deep"}; ImU32 cl[4]={IM_COL32(255,159,10,255),IM_COL32(146,82,232,255),IM_COL32(64,156,255,255),IM_COL32(40,80,180,255)};
      float gx=tl.x+18, gw=cw-36, by=tl.y+h-16, bh=70.0f, lh=bh/4.0f;
      for(int i=0;i<40;i++){ int s=(int)(2.0f+1.5f*sinf(i*0.4f)+0.8f*sinf(i*1.1f)); s=s<0?0:(s>3?3:s); float bx=gx+gw*i/40.0f; dl->AddRectFilled(ImVec2(bx,by-bh+s*lh-lh),ImVec2(bx+gw/40.0f-1,by-bh+s*lh),cl[s],1.0f); }
      ImGui::Dummy(ImVec2(w, h+12)); }
}
static void Draw_Pages(float w){
    ImDrawList* dl=ImGui::GetWindowDrawList();
    float cw=w<640.0f?w:640.0f; float x0=ImGui::GetCursorScreenPos().x+(w-cw)*0.5f;
    { float h=46.0f; ImVec2 tl(x0, ImGui::GetCursorScreenPos().y);
      Primitive bg; bg.cx=tl.x+cw*0.5f; bg.cy=tl.y+h*0.5f; bg.hw=cw*0.5f; bg.hh=h*0.5f; bg.corner_radius=18; bg.fade=1; bg.material=Material::Thick; g->Submit(bg);
      Icon ic[6]={Icon::Font,Icon::List,Icon::Image,Icon::Cube,Icon::PenEdit,Icon::Ellipsis};
      for(int i=0;i<6;i++) DrawIcon(dl,ic[i],ImVec2(tl.x+cw*(i+0.5f)/6.0f,tl.y+h*0.5f),16.0f,kInk,2.0f);
      ImGui::Dummy(ImVec2(w, h+12)); }
    { float h=400.0f; ImVec2 tl(x0, ImGui::GetCursorScreenPos().y);
      Primitive bg; bg.cx=tl.x+cw*0.5f; bg.cy=tl.y+h*0.5f; bg.hw=cw*0.5f; bg.hh=h*0.5f; bg.corner_radius=12; bg.fade=1; bg.material=Material::Thin; g->Submit(bg);
      static char title[96]="Project Proposal";
      static char body[2048]="Overview\n\nThis document outlines the goals, timeline, and budget for the upcoming project. Our aim is to ship a polished first release by the end of the quarter.\n\nNext steps: confirm scope with the team, finalize the schedule, and assign an owner for each milestone.\n\nClick anywhere and type to edit this document.";
      ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0,0,0,0)); ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0,0,0,0));
      ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0,0,0,0)); ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.93f,0.93f,0.96f,1.0f));
      ImGui::SetCursorScreenPos(ImVec2(tl.x+40, tl.y+22)); ImGui::SetNextItemWidth(cw-80); ImGui::InputTextWithHint("##pgtitle","Title", title, sizeof(title));
      dl->AddLine(ImVec2(tl.x+40,tl.y+52),ImVec2(tl.x+cw-40,tl.y+52),SA(kInkSoft,0.18f),1.0f);
      ImGui::SetCursorScreenPos(ImVec2(tl.x+40, tl.y+62)); ImGui::InputTextMultiline("##pgbody", body, sizeof(body), ImVec2(cw-80, h-84));
      ImGui::PopStyleColor(4);
      ImGui::SetCursorScreenPos(ImVec2(x0,tl.y)); ImGui::Dummy(ImVec2(w, h+12)); }
}
static void Draw_Keynote(float w){
    ImDrawList* dl=ImGui::GetWindowDrawList(); ImFont* f=ImGui::GetFont();
    float cw=w<700.0f?w:700.0f; float x0=ImGui::GetCursorScreenPos().x+(w-cw)*0.5f;
    struct Sl{ const char* title; const char* sub; ImU32 a; ImU32 b; };
    static Sl slides[8]={
        {"Q3 Business Review","Prepared by the Planning Team",IM_COL32(20,24,44,255),IM_COL32(90,50,80,255)},
        {"Agenda","Goals, results, and what's next",IM_COL32(40,60,120,255),IM_COL32(80,110,190,255)},
        {"Highlights","A strong quarter across the board",IM_COL32(120,50,80,255),IM_COL32(190,100,130,255)},
        {"Revenue","Up 18% year over year",IM_COL32(40,110,90,255),IM_COL32(90,180,140,255)},
        {"Roadmap","What we're building next",IM_COL32(110,80,40,255),IM_COL32(210,160,80,255)},
        {"The Team","Growing \xE2\x80\x94 and hiring",IM_COL32(90,50,140,255),IM_COL32(160,110,210,255)},
        {"Goals for Q4","Three priorities, one focus",IM_COL32(130,60,60,255),IM_COL32(210,110,110,255)},
        {"Thank You","Questions?",IM_COL32(60,90,60,255),IM_COL32(110,170,110,255)} };
    static int cur=0; if(cur<0||cur>7) cur=0;
    { float h=cw*0.42f; ImVec2 tl(x0, ImGui::GetCursorScreenPos().y); Sl& s=slides[cur];
      dl->AddRectFilledMultiColor(ImVec2(tl.x,tl.y),ImVec2(tl.x+cw,tl.y+h), s.a,s.b,s.a,s.b);
      dl->AddRect(ImVec2(tl.x,tl.y),ImVec2(tl.x+cw,tl.y+h), SA(IM_COL32(255,255,255,255),0.14f),16.0f,0,1.5f);
      dl->AddText(f,40.0f,ImVec2(tl.x+40,tl.y+h*0.5f-44),IM_COL32(255,255,255,255),s.title);
      dl->AddText(f,18.0f,ImVec2(tl.x+40,tl.y+h*0.5f+8),SA(IM_COL32(255,255,255,255),0.85f),s.sub);
      bool lh=false,rh=false;
      if(SurfaceHit("kprev", ImVec2(tl.x,tl.y+h*0.5f-26), ImVec2(50,52), &lh)) cur=(cur+7)%8;
      if(SurfaceHit("knext", ImVec2(tl.x+cw-50,tl.y+h*0.5f-26), ImVec2(50,52), &rh)) cur=(cur+1)%8;
      DrawIcon(dl,Icon::ChevronL,ImVec2(tl.x+20,tl.y+h*0.5f),18.0f,SA(IM_COL32(255,255,255,255),lh?1.0f:0.7f),2.4f);
      DrawIcon(dl,Icon::ChevronR,ImVec2(tl.x+cw-20,tl.y+h*0.5f),18.0f,SA(IM_COL32(255,255,255,255),rh?1.0f:0.7f),2.4f);
      char pg[8]; snprintf(pg,8,"%d / 8",cur+1); ImVec2 pgs=f->CalcTextSizeA(13.0f,99999.0f,0,pg); dl->AddText(f,13.0f,ImVec2(tl.x+cw-pgs.x-16,tl.y+14),SA(IM_COL32(255,255,255,255),0.85f),pg);
      ImGui::Dummy(ImVec2(w, h+16)); }
    { int n=8, cols=4; float gap=12, tw=(cw-(cols-1)*gap)/cols, th=tw*0.56f; float sy=ImGui::GetCursorScreenPos().y;
      for(int i=0;i<n;i++){ int r=i/cols,c=i%cols; float x=x0+c*(tw+gap), y=sy+r*(th+24); bool hov=false; char tid[8]; snprintf(tid,8,"ks%d",i);
        if(SurfaceHit(tid, ImVec2(x,y), ImVec2(tw,th), &hov)) cur=i;
        dl->AddRectFilledMultiColor(ImVec2(x,y),ImVec2(x+tw,y+th), slides[i].a,slides[i].b,slides[i].a,slides[i].b);
        if(i==cur)    dl->AddRect(ImVec2(x-2,y-2),ImVec2(x+tw+2,y+th+2),Accent(),9.0f,0,2.5f);
        else if(hov)  dl->AddRect(ImVec2(x-1,y-1),ImVec2(x+tw+1,y+th+1),SA(IM_COL32(255,255,255,255),0.5f),9.0f,0,1.5f);
        char nb[6]; snprintf(nb,6,"%d",i+1); dl->AddText(f,12.0f,ImVec2(x+2,y+th+4),i==cur?Accent():kInkSoft,nb); }
      int rows=(n+cols-1)/cols; ImGui::Dummy(ImVec2(w, rows*(th+24)+4)); }
}
static void Draw_iMovie(float w){
    ImDrawList* dl=ImGui::GetWindowDrawList(); float t=(float)ImGui::GetTime();
    float cw=w<700.0f?w:700.0f; float x0=ImGui::GetCursorScreenPos().x+(w-cw)*0.5f;
    { float h=cw*0.4f; ImVec2 tl(x0, ImGui::GetCursorScreenPos().y);
      dl->AddRectFilledMultiColor(ImVec2(tl.x,tl.y),ImVec2(tl.x+cw,tl.y+h), IM_COL32(30,40,60,255),IM_COL32(70,90,130,255),IM_COL32(110,80,90,255),IM_COL32(40,50,70,255));
      dl->AddRect(ImVec2(tl.x,tl.y),ImVec2(tl.x+cw,tl.y+h), SA(IM_COL32(255,255,255,255),0.14f),16.0f,0,1.5f);
      dl->AddCircleFilled(ImVec2(tl.x+cw*0.5f,tl.y+h*0.5f),26.0f,SA(IM_COL32(255,255,255,255),0.25f)); DrawIcon(dl,Icon::Play,ImVec2(tl.x+cw*0.5f+2,tl.y+h*0.5f),20.0f,IM_COL32(255,255,255,255),2.2f);
      ImGui::Dummy(ImVec2(w, h+14)); }
    { float h=110.0f; ImVec2 tl(x0, ImGui::GetCursorScreenPos().y);
      Primitive bg; bg.cx=tl.x+cw*0.5f; bg.cy=tl.y+h*0.5f; bg.hw=cw*0.5f; bg.hh=h*0.5f; bg.corner_radius=22; bg.fade=1; bg.material=Material::Thin; g->Submit(bg);
      dl->PushClipRect(ImVec2(tl.x,tl.y),ImVec2(tl.x+cw,tl.y+h),true);
      float cx=tl.x+14; ImU32 cc[5]={IM_COL32(60,90,150,255),IM_COL32(120,70,90,255),IM_COL32(60,120,90,255),IM_COL32(130,100,60,255),IM_COL32(90,70,140,255)};
      float ws[5]={120,90,140,80,110};
      for(int i=0;i<5;i++){ dl->AddRectFilled(ImVec2(cx,tl.y+18),ImVec2(cx+ws[i],tl.y+72), cc[i], 6.0f);
        for(float fx=cx+6; fx<cx+ws[i]-6; fx+=16) dl->AddRectFilled(ImVec2(fx,tl.y+24),ImVec2(fx+12,tl.y+66),SA(IM_COL32(255,255,255,255),0.12f),2.0f);
        cx+=ws[i]+4; }
      dl->AddRectFilled(ImVec2(tl.x+14,tl.y+80),ImVec2(tl.x+cw-14,tl.y+98), SA(IM_COL32(48,209,88,255),0.4f), 4.0f);
      float ph=tl.x+14+fmodf(t*50.0f, cw-28); dl->AddLine(ImVec2(ph,tl.y),ImVec2(ph,tl.y+h),IM_COL32(255,255,255,220),1.5f); dl->AddCircleFilled(ImVec2(ph,tl.y+4),4.0f,IM_COL32(255,255,255,255));
      dl->PopClipRect(); ImGui::Dummy(ImVec2(w, h+12)); }
}
static void Draw_Tips(float w){
    ImDrawList* dl=ImGui::GetWindowDrawList(); ImFont* f=ImGui::GetFont();
    float cw=w<640.0f?w:640.0f; float x0=ImGui::GetCursorScreenPos().x+(w-cw)*0.5f;
    dl->AddText(f,30.0f,ImVec2(x0+2,ImGui::GetCursorScreenPos().y),kInk,"Tips"); ImGui::Dummy(ImVec2(w,50.0f));
    struct T{ Icon ic; const char* t; const char* b; ImU32 a; ImU32 bcol; }; T ts[]={
        {Icon::Bolt,"Quick Actions","Long-press any app icon for a menu of shortcuts.",IM_COL32(60,80,160,255),IM_COL32(110,140,220,255)},
        {Icon::Moon,"Focus Modes","Filter notifications by what you're doing right now.",IM_COL32(90,60,150,255),IM_COL32(150,110,210,255)},
        {Icon::Camera,"Live Text","Select and copy text from any photo, instantly.",IM_COL32(40,140,90,255),IM_COL32(90,200,140,255)},
        {Icon::Sliders,"Control Center","Swipe down from the top-right to toggle anything.",IM_COL32(180,110,40,255),IM_COL32(230,170,80,255)},
    };
    int n=4, cols=2; float gap=14, tw=(cw-gap)/2, th=150; float sy=ImGui::GetCursorScreenPos().y;
    for(int i=0;i<n;i++){ int r=i/cols,c=i%cols; float x=x0+c*(tw+gap), y=sy+r*(th+gap);
        dl->AddRectFilledMultiColor(ImVec2(x,y),ImVec2(x+tw,y+th), (ts[i].a),(ts[i].bcol),(ts[i].bcol),(ts[i].a));
        dl->AddRect(ImVec2(x,y),ImVec2(x+tw,y+th),SA(IM_COL32(255,255,255,255),0.15f),18.0f,0,1.0f);
        DrawIcon(dl,ts[i].ic,ImVec2(x+30,y+30),22.0f,IM_COL32(255,255,255,255),2.2f);
        dl->AddText(f,18.0f,ImVec2(x+18,y+58),IM_COL32(255,255,255,255),ts[i].t);
        dl->AddText(f,14.0f,ImVec2(x+18,y+86),SA(IM_COL32(255,255,255,255),0.88f),ts[i].b,NULL,tw-36); }
    int rows=(n+cols-1)/cols; ImGui::Dummy(ImVec2(w, rows*(th+gap)+4));
}
static void Draw_Wallpaper(float w){
    ImDrawList* dl=ImGui::GetWindowDrawList(); ImFont* f=ImGui::GetFont();
    float cw=w<600.0f?w:600.0f; float x0=ImGui::GetCursorScreenPos().x+(w-cw)*0.5f;
    struct Wp{ ImU32 a,b,c,d; }; Wp wps[8]={
        {IM_COL32(20,24,60,255),IM_COL32(80,40,120,255),IM_COL32(180,60,90,255),IM_COL32(40,30,70,255)},
        {IM_COL32(10,40,60,255),IM_COL32(20,120,150,255),IM_COL32(60,200,180,255),IM_COL32(10,60,90,255)},
        {IM_COL32(60,20,30,255),IM_COL32(180,60,40,255),IM_COL32(255,150,60,255),IM_COL32(90,30,40,255)},
        {IM_COL32(30,30,40,255),IM_COL32(60,60,80,255),IM_COL32(120,120,140,255),IM_COL32(20,20,28,255)},
        {IM_COL32(20,50,30,255),IM_COL32(40,130,80,255),IM_COL32(120,220,150,255),IM_COL32(20,70,45,255)},
        {IM_COL32(50,20,60,255),IM_COL32(120,40,140,255),IM_COL32(220,110,210,255),IM_COL32(70,30,90,255)},
        {IM_COL32(40,40,20,255),IM_COL32(140,120,40,255),IM_COL32(240,210,90,255),IM_COL32(70,60,25,255)},
        {IM_COL32(20,30,70,255),IM_COL32(40,80,180,255),IM_COL32(90,150,255,255),IM_COL32(25,40,100,255)} };
    { float h=200.0f; ImVec2 tl(x0, ImGui::GetCursorScreenPos().y);
      dl->AddRectFilledMultiColor(ImVec2(tl.x,tl.y),ImVec2(tl.x+cw,tl.y+h), wps[0].a,wps[0].b,wps[0].c,wps[0].d);
      dl->AddRect(ImVec2(tl.x,tl.y),ImVec2(tl.x+cw,tl.y+h),SA(IM_COL32(255,255,255,255),0.14f),20.0f,0,1.5f);
      time_t tt=time(nullptr); struct tm lt; localtime_s(&lt,&tt); char tb[8]; FmtClock(tb,8,lt.tm_hour,lt.tm_min);
      ImVec2 ts=f->CalcTextSizeA(54.0f,99999.0f,0,tb); dl->AddText(f,54.0f,ImVec2(tl.x+cw*0.5f-ts.x*0.5f,tl.y+40),IM_COL32(255,255,255,255),tb);
      ImGui::Dummy(ImVec2(w, h+16)); }
    { dl->AddText(f,18.0f,ImVec2(x0+4,ImGui::GetCursorScreenPos().y),kInk,"Choose Wallpaper"); ImGui::Dummy(ImVec2(w,30.0f));
      int n=8, cols=4; float gap=12, cellw=(cw-(cols-1)*gap)/cols, ch=cellw*1.2f; float sy=ImGui::GetCursorScreenPos().y;
      for(int i=0;i<n;i++){ int r=i/cols,c=i%cols; float x=x0+c*(cellw+gap), y=sy+r*(ch+gap);
        ImgOrGrad(dl, ImVec2(x,y), ImVec2(x+cellw,y+ch), i, wps[i].a, wps[i].c, 10.0f);
        if(i==0) dl->AddRect(ImVec2(x-2,y-2),ImVec2(x+cellw+2,y+ch+2),Accent(),12.0f,0,2.5f);
        else dl->AddRect(ImVec2(x,y),ImVec2(x+cellw,y+ch),SA(IM_COL32(255,255,255,255),0.12f),10.0f,0,1.0f); }
      int rows=(n+cols-1)/cols; ImGui::Dummy(ImVec2(w, rows*(ch+gap)+4)); }
}
static void Draw_Focus(float w){
    ImDrawList* dl=ImGui::GetWindowDrawList(); ImFont* f=ImGui::GetFont();
    float cw=w<560.0f?w:560.0f; float x0=ImGui::GetCursorScreenPos().x+(w-cw)*0.5f;
    dl->AddText(f,30.0f,ImVec2(x0+2,ImGui::GetCursorScreenPos().y),kInk,"Focus"); ImGui::Dummy(ImVec2(w,50.0f));
    struct Fm{ Icon ic; const char* n; const char* d; ImU32 c; }; static Fm fm[4]={
        {Icon::Moon,"Do Not Disturb","Silence all",IM_COL32(146,82,232,255)},
        {Icon::Bolt,"Work","Allow work apps",IM_COL32(64,156,255,255)},
        {Icon::Bed,"Sleep","Wind down",IM_COL32(40,60,140,255)},
        {Icon::Heart,"Personal","Family & friends",IM_COL32(48,209,88,255)} };
    static bool on[4]={false,true,false,false};
    int cols=2; float gap=14, tw=(cw-gap)/2, th=120; float sy=ImGui::GetCursorScreenPos().y;
    for(int i=0;i<4;i++){ int r=i/cols,c=i%cols; float x=x0+c*(tw+gap), y=sy+r*(th+gap);
        ImGui::PushID(i); ImGui::SetCursorScreenPos(ImVec2(x,y)); ImGui::InvisibleButton("fm",ImVec2(tw,th)); bool hov=ImGui::IsItemHovered(); if(ImGui::IsItemDeactivated()&&hov) on[i]=!on[i];
        float ok=TileOn("fm",on[i]); ImGui::PopID();
        Primitive bg; bg.cx=x+tw*0.5f; bg.cy=y+th*0.5f; bg.hw=tw*0.5f; bg.hh=th*0.5f; bg.corner_radius=26; bg.fade=1; bg.material=Material::Thin; bg.elevate=hov?1.06f:1.0f; g->Submit(bg);
        if(ok>0.01f){ Primitive o=bg; o.fade=ok; o.material=Material::Accent; g->Submit(o); }
        ImU32 fg=on[i]?IM_COL32(255,255,255,255):kInk;
        dl->AddCircleFilled(ImVec2(x+34,y+34),18.0f, on[i]?SA(IM_COL32(255,255,255,255),0.25f):SA(fm[i].c,0.22f)); DrawIcon(dl,fm[i].ic,ImVec2(x+34,y+34),18.0f, on[i]?IM_COL32(255,255,255,255):fm[i].c,2.2f);
        dl->AddText(f,17.0f,ImVec2(x+18,y+62),fg,fm[i].n);
        dl->AddText(f,13.0f,ImVec2(x+18,y+86),on[i]?SA(IM_COL32(255,255,255,255),0.85f):kInkSoft,fm[i].d); }
    int rows=2; ImGui::Dummy(ImVec2(w, rows*(th+gap)+4));
}
static void Draw_Storage(float w){
    ImDrawList* dl=ImGui::GetWindowDrawList(); ImFont* f=ImGui::GetFont();
    float cw=w<600.0f?w:600.0f; float x0=ImGui::GetCursorScreenPos().x+(w-cw)*0.5f;
    struct Sg{ const char* n; float gb; ImU32 c; }; Sg sg[]={ {"Apps",84.0f,IM_COL32(64,156,255,255)},{"Photos",128.0f,IM_COL32(255,159,10,255)},{"Media",62.0f,IM_COL32(255,55,95,255)},{"System",24.0f,IM_COL32(142,142,147,255)},{"Other",18.0f,IM_COL32(146,82,232,255)} };
    int n=5; float used=0; for(int i=0;i<n;i++) used+=sg[i].gb; float total=512.0f;
    { float h=140.0f; ImVec2 tl(x0, ImGui::GetCursorScreenPos().y);
      Primitive bg; bg.cx=tl.x+cw*0.5f; bg.cy=tl.y+h*0.5f; bg.hw=cw*0.5f; bg.hh=h*0.5f; bg.corner_radius=28; bg.fade=1; bg.material=Material::Thin; g->Submit(bg);
      char hb[48]; snprintf(hb,48,"%.0f GB of %.0f GB used",used,total); dl->AddText(f,15.0f,ImVec2(tl.x+22,tl.y+18),kInkSoft,hb);
      float bx=tl.x+22, bw=cw-44, by=tl.y+56, bh=26; float cx=bx;
      for(int i=0;i<n;i++){ float sw=bw*sg[i].gb/total; bool first=(i==0); dl->AddRectFilled(ImVec2(cx,by),ImVec2(cx+sw,by+bh), sg[i].c, first?6.0f:0.0f); cx+=sw; }
      dl->AddRectFilled(ImVec2(cx,by),ImVec2(bx+bw,by+bh), SA(kInkSoft,0.2f), 0.0f);
      float ly=tl.y+98; for(int i=0;i<n;i++){ float lx=bx+(cw-44)*i/n; dl->AddCircleFilled(ImVec2(lx+6,ly+6),5.0f,sg[i].c); char lb[24]; snprintf(lb,24,"%s",sg[i].n); dl->AddText(f,12.0f,ImVec2(lx+16,ly),kInk,lb); char gb[16]; snprintf(gb,16,"%.0f GB",sg[i].gb); dl->AddText(f,11.0f,ImVec2(lx+16,ly+16),kInkSoft,gb); }
      ImGui::Dummy(ImVec2(w, h+14)); }
    { float rowh=56.0f, h=n*rowh; ImVec2 tl(x0, ImGui::GetCursorScreenPos().y);
      Primitive bg; bg.cx=tl.x+cw*0.5f; bg.cy=tl.y+h*0.5f; bg.hw=cw*0.5f; bg.hh=h*0.5f; bg.corner_radius=24; bg.fade=1; bg.material=Material::Thin; g->Submit(bg);
      for(int i=0;i<n;i++){ float ry=tl.y+i*rowh, cy=ry+rowh*0.5f; dl->AddRectFilled(ImVec2(tl.x+18,cy-9),ImVec2(tl.x+36,cy+9),sg[i].c,5.0f);
        dl->AddText(f,16.0f,ImVec2(tl.x+48,cy-9),kInk,sg[i].n); char gb[16]; snprintf(gb,16,"%.0f GB",sg[i].gb); ImVec2 gs=f->CalcTextSizeA(15.0f,99999.0f,0,gb); dl->AddText(f,15.0f,ImVec2(tl.x+cw-gs.x-18,cy-8),kInkSoft,gb);
        if(i<n-1) dl->AddLine(ImVec2(tl.x+48,ry+rowh),ImVec2(tl.x+cw-14,ry+rowh),SA(kInkSoft,0.14f),1.0f); }
      ImGui::Dummy(ImVec2(w, h+12)); }
}
