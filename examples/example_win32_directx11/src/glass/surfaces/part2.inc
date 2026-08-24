static void Draw_Stocks(float w){
    ImDrawList* dl=ImGui::GetWindowDrawList(); ImFont* f=ImGui::GetFont();
    const ImU32 up=IM_COL32(48,209,88,255), down=IM_COL32(255,69,58,255);
    float cw=w<640.0f?w:640.0f; float x0=ImGui::GetCursorScreenPos().x+(w-cw)*0.5f;
    struct Stk{ const char* sym; const char* name; float price; float chg; float seed; };
    Stk st[]={ {"GLSS","Glass Labs",214.29f,1.24f,0.0f}, {"NVDA","NVIDIA",128.44f,3.18f,1.3f},
               {"TSLA","Tesla",178.08f,-2.04f,2.1f}, {"MSFT","Microsoft",441.58f,0.62f,3.4f},
               {"AMZN","Amazon.com",178.22f,1.42f,4.2f}, {"GOOG","Alphabet",182.10f,-0.88f,5.0f} };
    int nst=6;
    static int sel=0; if(sel<0||sel>=nst) sel=0;
    static float ser[40];
    auto build=[&](float seed, float chg){ for(int i=0;i<40;i++){ float t=i*0.32f; ser[i]=10.0f+sinf(t+seed)*2.0f+sinf(t*2.3f+seed)*0.9f+ (chg>0?i*0.05f:-i*0.04f); } };
    {
        build(st[sel].seed, st[sel].chg);
        float h=200.0f; ImVec2 tl(x0, ImGui::GetCursorScreenPos().y);
        Primitive bg; bg.cx=tl.x+cw*0.5f; bg.cy=tl.y+h*0.5f; bg.hw=cw*0.5f; bg.hh=h*0.5f; bg.corner_radius=29.0f; bg.fade=1; bg.material=Material::Thin; g->Submit(bg);
        dl->AddText(f,26.0f, ImVec2(tl.x+22, tl.y+16), kInk, st[sel].sym);
        dl->AddText(f,14.0f, ImVec2(tl.x+22, tl.y+50), kInkSoft, st[sel].name);
        char pb[24]; snprintf(pb,24,"%.2f",st[sel].price); ImVec2 ps=f->CalcTextSizeA(28.0f,99999.0f,0,pb);
        dl->AddText(f,28.0f, ImVec2(tl.x+cw-ps.x-22, tl.y+16), kInk, pb);
        char cb[24]; snprintf(cb,24,"%+.2f%%",st[sel].chg); ImVec2 cs=f->CalcTextSizeA(15.0f,99999.0f,0,cb);
        dl->AddText(f,15.0f, ImVec2(tl.x+cw-cs.x-22, tl.y+52), st[sel].chg>=0?up:down, cb);
        AreaChart(dl, ImVec2(tl.x+18, tl.y+86), cw-36, h-100, ser, 40, st[sel].chg>=0?up:down);
        ImGui::Dummy(ImVec2(w, h+14));
    }
    {
        float rowh=58.0f, h=nst*rowh; ImVec2 tl(x0, ImGui::GetCursorScreenPos().y);
        Primitive bg; bg.cx=tl.x+cw*0.5f; bg.cy=tl.y+h*0.5f; bg.hw=cw*0.5f; bg.hh=h*0.5f; bg.corner_radius=26.0f; bg.fade=1; bg.material=Material::Thin; g->Submit(bg);
        for(int i=0;i<nst;i++){
            float ry=tl.y+i*rowh, cy=ry+rowh*0.5f;
            bool hov=false; if(SurfaceHit(st[i].sym, ImVec2(tl.x, ry), ImVec2(cw, rowh), &hov)) sel=i;
            RowWash(dl, ImVec2(tl.x+6, ry+3), ImVec2(tl.x+cw-6, ry+rowh-3), 12.0f, i==sel, hov);
            dl->AddText(f,17.0f, ImVec2(tl.x+18, cy-18), kInk, st[i].sym);
            dl->AddText(f,12.0f, ImVec2(tl.x+18, cy+2), kInkSoft, st[i].name);
            build(st[i].seed, st[i].chg);
            Sparkline(dl, ImVec2(tl.x+cw*0.5f-30, cy-14), 110, 28, ser, 40, st[i].chg>=0?up:down);
            char pb[24]; snprintf(pb,24,"%.2f",st[i].price); ImVec2 ps=f->CalcTextSizeA(16.0f,99999.0f,0,pb);
            dl->AddText(f,16.0f, ImVec2(tl.x+cw-ps.x-92, cy-9), kInk, pb);
            char cb[20]; snprintf(cb,20,"%+.2f%%",st[i].chg);
            float pw2=64.0f, px2=tl.x+cw-pw2-16;
            Primitive pill; pill.cx=px2+pw2*0.5f; pill.cy=cy; pill.hw=pw2*0.5f; pill.hh=13.0f; pill.corner_radius=7.0f; pill.fade=1; pill.material=Material::Thin; g->Submit(pill);
            ImVec2 cs2=f->CalcTextSizeA(13.0f,99999.0f,0,cb);
            dl->AddText(f,13.0f, ImVec2(px2+pw2*0.5f-cs2.x*0.5f, cy-7), st[i].chg>=0?up:down, cb);
            if(i<nst-1) dl->AddLine(ImVec2(tl.x+18,ry+rowh),ImVec2(tl.x+cw-16,ry+rowh),SA(kInkSoft,0.14f),1.0f);
        }
        ImGui::Dummy(ImVec2(w, h+12));
    }
}
static void Draw_Files(float w){
    ImDrawList* dl=ImGui::GetWindowDrawList(); ImFont* f=ImGui::GetFont();
    float cw=w<760.0f?w:760.0f; float x0=ImGui::GetCursorScreenPos().x+(w-cw)*0.5f;
    float sideW=176.0f, gap=16.0f, gridX=x0+sideW+gap, gridW=cw-sideW-gap;
    float top=ImGui::GetCursorScreenPos().y, H=420.0f;
    struct F{ Icon ic; const char* l; };
    static F fav[]={{Icon::PaperPlane,"Nearby Share"},{Icon::Clock,"Recents"},{Icon::Rocket,"Applications"},{Icon::Display,"Desktop"},{Icon::FileLines,"Documents"},{Icon::Download,"Downloads"}};
    static F ic2[]={{Icon::Cloud,"Cloud Drive"},{Icon::Heart,"Shared"}};
    static int sideSel=4;
    static char crumb[64]="";
    {
        Primitive bg; bg.cx=x0+sideW*0.5f; bg.cy=top+H*0.5f; bg.hw=sideW*0.5f; bg.hh=H*0.5f; bg.corner_radius=24.0f; bg.fade=1; bg.material=Material::Thin; g->Submit(bg);
        dl->AddText(f,11.0f, ImVec2(x0+16, top+12), kInkSoft, "FAVORITES");
        for(int i=0;i<6;i++){ float ry=top+34+i*32; bool sel=(i==sideSel); bool hov=false;
            if(SurfaceHit(fav[i].l, ImVec2(x0+8, ry), ImVec2(sideW-16, 28), &hov)){ sideSel=i; crumb[0]=0; }
            if(sel){ Primitive s; s.cx=x0+sideW*0.5f; s.cy=ry+14; s.hw=(sideW-16)*0.5f; s.hh=14; s.corner_radius=8; s.fade=1; s.material=Material::Accent; g->Submit(s); }
            else if(hov) dl->AddRectFilled(ImVec2(x0+8,ry), ImVec2(x0+sideW-8,ry+28), SA(kInkSoft,0.10f), 8.0f);
            DrawIcon(dl, fav[i].ic, ImVec2(x0+24, ry+14), 14.0f, sel?IM_COL32(255,255,255,255):kInk, 2.0f);
            dl->AddText(f,14.0f, ImVec2(x0+40, ry+6), sel?IM_COL32(255,255,255,255):kInk, fav[i].l); }
        dl->AddText(f,11.0f, ImVec2(x0+16, top+34+6*32+8), kInkSoft, "ICLOUD");
        for(int i=0;i<2;i++){ float ry=top+34+6*32+28+i*32; bool sel=(i+6==sideSel); bool hov=false;
            if(SurfaceHit(ic2[i].l, ImVec2(x0+8, ry), ImVec2(sideW-16, 28), &hov)){ sideSel=i+6; crumb[0]=0; }
            if(sel){ Primitive s; s.cx=x0+sideW*0.5f; s.cy=ry+14; s.hw=(sideW-16)*0.5f; s.hh=14; s.corner_radius=8; s.fade=1; s.material=Material::Accent; g->Submit(s); }
            else if(hov) dl->AddRectFilled(ImVec2(x0+8,ry), ImVec2(x0+sideW-8,ry+28), SA(kInkSoft,0.10f), 8.0f);
            DrawIcon(dl, ic2[i].ic, ImVec2(x0+24, ry+14), 14.0f, sel?IM_COL32(255,255,255,255):kInk, 2.0f);
            dl->AddText(f,14.0f, ImVec2(x0+40, ry+6), sel?IM_COL32(255,255,255,255):kInk, ic2[i].l); }
    }
    {
        Primitive bg; bg.cx=gridX+gridW*0.5f; bg.cy=top+H*0.5f; bg.hw=gridW*0.5f; bg.hh=H*0.5f; bg.corner_radius=24.0f; bg.fade=1; bg.material=Material::Thin; g->Submit(bg);
        const char* root = sideSel<6 ? fav[sideSel].l : ic2[sideSel-6].l;
        const char* title = crumb[0] ? crumb : root;
        if(crumb[0]){ bool bh=false; if(SurfaceHit("filesback", ImVec2(gridX+8, top+8), ImVec2(150,34), &bh)) crumb[0]=0;
            DrawIcon(dl, Icon::ChevronL, ImVec2(gridX+18, top+24), 13.0f, Accent(), 2.4f);
            dl->AddText(f,18.0f, ImVec2(gridX+38, top+14), kInk, title); }
        else dl->AddText(f,18.0f, ImVec2(gridX+18, top+14), kInk, title);
        struct It{ Icon ic; const char* n; bool folder; };
        static It rootItems[]={ {Icon::Folder,"Projects",true},{Icon::Folder,"Designs",true},{Icon::Folder,"Travel",true},
            {Icon::FileLines,"notes.md",false},{Icon::Image,"hero.png",false},{Icon::Film,"demo.mp4",false},
            {Icon::FileLines,"spec.pdf",false},{Icon::Music,"track.m4a",false},{Icon::Code,"main.cpp",false},
            {Icon::Folder,"Archive",true} };
        static It subItems[]={ {Icon::FileLines,"readme.txt",false},{Icon::Folder,"assets",true},{Icon::Folder,"build",true},
            {Icon::Image,"cover.png",false},{Icon::Code,"index.html",false},{Icon::Music,"song.m4a",false} };
        It* items = crumb[0] ? subItems : rootItems; int n = crumb[0] ? 6 : 10;
        int cols=4; float pad=18.0f, cellw=(gridW-2*pad)/cols, cellh=96.0f;
        for(int i=0;i<n;i++){ int r=i/cols,c=i%cols; float cellX=gridX+pad+cellw*c, cellY=top+58+r*cellh, cx=cellX+cellw*0.5f, cy=cellY+34;
            bool hov=false; bool clk=SurfaceHit(items[i].n, ImVec2(cellX, cellY), ImVec2(cellw, cellh), &hov);
            if(clk && items[i].folder) snprintf(crumb,64,"%s",items[i].n);
            if(hov) dl->AddRectFilled(ImVec2(cellX+8,cellY+6), ImVec2(cellX+cellw-8,cellY+cellh-6), SA(kInkSoft,0.10f), 14.0f);
            DrawIcon(dl, items[i].ic, ImVec2(cx, cy), items[i].folder?44.0f:38.0f, items[i].folder?Accent():kInk, 2.0f);
            ImVec2 ns=f->CalcTextSizeA(13.0f,99999.0f,0,items[i].n);
            dl->AddText(f,13.0f, ImVec2(cx-ns.x*0.5f, cy+30), kInk, items[i].n); }
    }
    ImGui::Dummy(ImVec2(w, H+12));
}
static void Draw_Maps(float w){
    ImDrawList* dl=ImGui::GetWindowDrawList(); ImFont* f=ImGui::GetFont();
    float cw=w<720.0f?w:720.0f; float x0=ImGui::GetCursorScreenPos().x+(w-cw)*0.5f;
    struct P{ Icon ic; const char* n; const char* cat; const char* d; };
    static P ps[]={ {Icon::MugHot,"Caffe Macs","Coffee \xC2\xB7 $$","0.2 mi"},{Icon::Leaf,"Glass Park","Park \xC2\xB7 Loop","0.6 mi"},{Icon::Cart,"The Market","Grocery","1.1 mi"} };
    static int selPlace=0; if(selPlace<0||selPlace>2) selPlace=0;
    { float h=40.0f; ImVec2 tl(x0, ImGui::GetCursorScreenPos().y);
      Primitive bg; bg.cx=tl.x+cw*0.5f; bg.cy=tl.y+h*0.5f; bg.hw=cw*0.5f; bg.hh=h*0.5f; bg.corner_radius=h*0.5f; bg.fade=1; bg.material=Material::Thin; g->Submit(bg);
      DrawIcon(dl, Icon::Search, ImVec2(tl.x+22, tl.y+h*0.5f), 14.0f, kInkSoft, 2.0f);
      dl->AddText(f,15.0f, ImVec2(tl.x+40, tl.y+h*0.5f-9), kInkSoft, "Search Maps");
      ImGui::Dummy(ImVec2(w, h+14)); }
    { float h=300.0f; ImVec2 tl(x0, ImGui::GetCursorScreenPos().y);
      Primitive bg; bg.cx=tl.x+cw*0.5f; bg.cy=tl.y+h*0.5f; bg.hw=cw*0.5f; bg.hh=h*0.5f; bg.corner_radius=28.0f; bg.fade=1; bg.material=Material::Thin; g->Submit(bg);
      dl->PushClipRect(ImVec2(tl.x,tl.y), ImVec2(tl.x+cw,tl.y+h), true);
      for(float gx=tl.x+30; gx<tl.x+cw; gx+=64) dl->AddLine(ImVec2(gx,tl.y),ImVec2(gx,tl.y+h),SA(kInkSoft,0.18f),6.0f);
      for(float gy=tl.y+28; gy<tl.y+h; gy+=58) dl->AddLine(ImVec2(tl.x,gy),ImVec2(tl.x+cw,gy),SA(kInkSoft,0.18f),6.0f);
      dl->AddRectFilled(ImVec2(tl.x+cw*0.55f,tl.y+40),ImVec2(tl.x+cw*0.8f,tl.y+150), SA(IM_COL32(76,200,120,255),0.45f), 10.0f);
      dl->AddRectFilled(ImVec2(tl.x+20,tl.y+h-90),ImVec2(tl.x+cw*0.4f,tl.y+h-10), SA(IM_COL32(64,156,255,255),0.4f), 8.0f);
      ImVec2 rp[5]={ {tl.x+70,tl.y+h-50},{tl.x+200,tl.y+h-50},{tl.x+200,tl.y+120},{tl.x+cw*0.5f,tl.y+120},{tl.x+cw*0.5f,tl.y+44} };
      for(int i=0;i<4;i++){ dl->AddLine(rp[i],rp[i+1], IM_COL32(64,156,255,255), 6.0f); }
      dl->AddCircleFilled(rp[0], 7.0f, IM_COL32(64,156,255,255)); dl->AddCircle(rp[0],7.0f,IM_COL32(255,255,255,255),0,2.0f);
      DrawIcon(dl, Icon::LocationDot, rp[4], 30.0f, IM_COL32(255,69,58,255), 2.0f);
      { const char* nm=ps[selPlace].n; ImVec2 ns=f->CalcTextSizeA(13.0f,99999.0f,0,nm); float lw=ns.x+22.0f, ly=rp[4].y-46.0f;
        Primitive call; call.cx=rp[4].x; call.cy=ly+12.0f; call.hw=lw*0.5f; call.hh=13.0f; call.corner_radius=9.0f; call.fade=1; call.material=Material::Thick; g->Submit(call);
        dl->AddText(f,13.0f, ImVec2(rp[4].x-ns.x*0.5f, ly+5.0f), kInk, nm); }
      dl->PopClipRect();
      ImGui::Dummy(ImVec2(w, h+14)); }
    for(int i=0;i<3;i++){ float h=64.0f; ImVec2 tl(x0, ImGui::GetCursorScreenPos().y);
        Primitive bg; bg.cx=tl.x+cw*0.5f; bg.cy=tl.y+h*0.5f; bg.hw=cw*0.5f; bg.hh=h*0.5f; bg.corner_radius=22.0f; bg.fade=1; bg.material=Material::Thin; g->Submit(bg);
        bool hov=false; if(SurfaceHit(ps[i].n, tl, ImVec2(cw,h), &hov)) selPlace=i;
        RowWash(dl, ImVec2(tl.x+6,tl.y+4), ImVec2(tl.x+cw-6,tl.y+h-4), 20.0f, i==selPlace, hov);
        Primitive t; t.cx=tl.x+34; t.cy=tl.y+h*0.5f; t.hw=18; t.hh=18; t.corner_radius=12; t.fade=1; t.material=Material::Accent; g->Submit(t);
        DrawIcon(dl, ps[i].ic, ImVec2(tl.x+34, tl.y+h*0.5f), 18.0f, IM_COL32(255,255,255,255), 2.0f);
        dl->AddText(f,16.0f, ImVec2(tl.x+64, tl.y+14), kInk, ps[i].n);
        dl->AddText(f,13.0f, ImVec2(tl.x+64, tl.y+36), kInkSoft, ps[i].cat);
        ImVec2 ds=f->CalcTextSizeA(14.0f,99999.0f,0,ps[i].d); dl->AddText(f,14.0f, ImVec2(tl.x+cw-ds.x-18, tl.y+h*0.5f-8), kInkSoft, ps[i].d);
        ImGui::Dummy(ImVec2(w, h+12)); }
}
static void Draw_Messages(float w){
    ImDrawList* dl=ImGui::GetWindowDrawList(); ImFont* f=ImGui::GetFont();
    float cw=w<540.0f?w:540.0f; float x0=ImGui::GetCursorScreenPos().x+(w-cw)*0.5f;
    struct Msg{ bool mine; char t[160]; };
    static const char* people[3]={"Emma","Dad","Team"};
    static Msg store[3][40]; static int counts[3]={0,0,0};
    static bool init=false;
    if(!init){ init=true;
        auto add=[&](int c,bool mine,const char* s){ if(counts[c]>=40)return; Msg&m=store[c][counts[c]++]; m.mine=mine; snprintf(m.t,sizeof(m.t),"%s",s); };
        add(0,false,"Are we still on for dinner tonight?"); add(0,true,"Yes! 7:30 at the usual place.");
        add(0,false,"Perfect, see you then \xF0\x9F\x99\x8C"); add(0,true,"Running about 5 minutes late!");
        add(1,false,"Call me when you get a chance"); add(1,true,"Will do \xF0\x9F\x91\x8D heading home now.");
        add(2,false,"Standup moved to 10:30 today"); add(2,true,"Got it, thanks for the heads up.");
    }
    static int convo=0; if(convo<0||convo>2) convo=0;
    { float h=64.0f; ImVec2 tl(x0, ImGui::GetCursorScreenPos().y); float aw=cw/3.0f;
      for(int i=0;i<3;i++){ float acx=tl.x+aw*(i+0.5f); bool hov=false;
        if(SurfaceHit(people[i], ImVec2(tl.x+aw*i,tl.y), ImVec2(aw,h), &hov)) convo=i;
        Primitive av; av.cx=acx; av.cy=tl.y+16; av.hw=17; av.hh=17; av.corner_radius=22; av.fade=1; av.material=(i==convo)?Material::Accent:Material::Thin; g->Submit(av);
        char in[2]={people[i][0],0}; TextC(dl,f,15.0f,ImVec2(acx,tl.y+16),(i==convo)?IM_COL32(255,255,255,255):kInk,in);
        ImVec2 ns=f->CalcTextSizeA(13.0f,99999.0f,0,people[i]); dl->AddText(f,13.0f,ImVec2(acx-ns.x*0.5f,tl.y+38),(i==convo)?kInk:kInkSoft,people[i]); }
      ImGui::Dummy(ImVec2(w,h)); }
    float maxb=cw*0.72f, pad=14.0f;
    for(int i=0;i<counts[convo];i++){ Msg& m=store[convo][i];
        ImVec2 sz=f->CalcTextSizeA(15.0f, maxb-2*pad, maxb-2*pad, m.t);
        float bw=sz.x+2*pad, bh=sz.y+2*pad-2;
        float bx = m.mine ? (x0+cw-bw) : x0;
        ImVec2 tl(bx, ImGui::GetCursorScreenPos().y);
        Primitive b; b.cx=tl.x+bw*0.5f; b.cy=tl.y+bh*0.5f; b.hw=bw*0.5f; b.hh=bh*0.5f; b.corner_radius=22.0f; b.fade=1; b.material=m.mine?Material::Accent:Material::Thin; g->Submit(b);
        dl->AddText(f,15.0f, ImVec2(tl.x+pad, tl.y+pad-1), m.mine?IM_COL32(255,255,255,255):kInk, m.t, NULL, maxb-2*pad);
        ImGui::Dummy(ImVec2(w, bh+8));
    }
    { float h=44.0f; ImVec2 tl(x0, ImGui::GetCursorScreenPos().y+4);
      static char buf[160]="";
      Primitive bg; bg.cx=tl.x+(cw-52)*0.5f; bg.cy=tl.y+h*0.5f; bg.hw=(cw-52)*0.5f; bg.hh=h*0.5f; bg.corner_radius=h*0.5f; bg.fade=1; bg.material=Material::Thin; g->Submit(bg);
      ImGui::SetCursorScreenPos(ImVec2(tl.x+18, tl.y+(h-ImGui::GetTextLineHeight())*0.5f));
      ImGui::PushStyleColor(ImGuiCol_FrameBg, 0); ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, 0);
      ImGui::PushStyleColor(ImGuiCol_FrameBgActive, 0); ImGui::PushStyleColor(ImGuiCol_Text, kInk);
      ImGui::SetNextItemWidth(cw-52-40);
      bool enter=ImGui::InputTextWithHint("##msgin","iMessage",buf,sizeof(buf),ImGuiInputTextFlags_EnterReturnsTrue);
      ImGui::PopStyleColor(4);
      bool shv=false; bool sclk=SurfaceHit("msgsend", ImVec2(tl.x+cw-46,tl.y), ImVec2(46,h), &shv, Sfx::Send);
      Primitive s; s.cx=tl.x+cw-22; s.cy=tl.y+h*0.5f; s.hw=20; s.hh=20; s.corner_radius=26; s.fade=1; s.material=Material::Accent; g->Submit(s);
      DrawIcon(dl, Icon::ArrowUp, ImVec2(tl.x+cw-22, tl.y+h*0.5f), 16.0f, IM_COL32(255,255,255,255), 2.4f);
      if((enter||sclk) && buf[0] && counts[convo]<40){ Msg& m=store[convo][counts[convo]++]; m.mine=true; snprintf(m.t,sizeof(m.t),"%s",buf); buf[0]=0; }
      ImGui::SetCursorScreenPos(ImVec2(x0, tl.y)); ImGui::Dummy(ImVec2(w, h+10)); }
}
static void Draw_Mail(float w){
    ImDrawList* dl=ImGui::GetWindowDrawList(); ImFont* f=ImGui::GetFont();
    float cw=w<620.0f?w:620.0f; float x0=ImGui::GetCursorScreenPos().x+(w-cw)*0.5f;
    struct E{ const char* from; const char* subj; const char* prev; const char* time; const char* body; };
    static E es[]={
        {"Olivia Bennett","Lunch on Friday?","Hi! Are you free for lunch this Friday around noon? There's a \xE2\x80\xA6","9:41 AM",
         "Hi! Are you free for lunch this Friday around noon? There's a new place near the office I've been wanting to try. Let me know what works for you."},
        {"Sarah Chen","Re: Q3 planning deck","Thanks for sending this over. I left a few comments on \xE2\x80\xA6","8:12 AM",
         "Thanks for sending this over. I left a few comments on slides 4 and 7 \xE2\x80\x94 mostly small wording tweaks. Looks great overall, really nice work pulling it together."},
        {"Glass Store","Your receipt from Glass Store","Thank you for your purchase. Here are the details of \xE2\x80\xA6","Yesterday",
         "Thank you for your purchase. Here are the details of your recent order. You can view or manage your subscriptions any time in Settings."},
        {"GitHub","[planner] All checks have passed","Your pull request #214 is ready to merge. All 12 \xE2\x80\xA6","Yesterday",
         "Your pull request #214, \"Improve onboarding flow,\" is ready to merge. All 12 status checks have passed and your reviewers have approved the changes."},
        {"Dr. Reyes' Office","Appointment reminder","This is a reminder of your appointment on Tuesday at \xE2\x80\xA6","Tuesday",
         "This is a friendly reminder of your appointment on Tuesday at 2:30 PM. Please arrive about 10 minutes early. Reply to this message to confirm or reschedule."},
        {"The Daily Digest","Your morning briefing","The five stories you should know about today, plus the \xE2\x80\xA6","Monday",
         "Good morning. Here are the five stories you should know about today, plus the weather and a long read for your commute. Have a great day."},
    };
    int n=6;
    static bool unread[6]={true,true,true,false,true,false};
    static int openIdx=-1; if(openIdx>=n) openIdx=-1;
    if(openIdx>=0){
        E& e=es[openIdx];
        float bh=44.0f; ImVec2 tb(x0, ImGui::GetCursorScreenPos().y);
        bool bhv=false; if(SurfaceHit("mailback", tb, ImVec2(140,bh), &bhv, Sfx::NavPop)) openIdx=-1;
        DrawIcon(dl, Icon::ChevronL, ImVec2(tb.x+10, tb.y+bh*0.5f), 15.0f, Accent(), 2.4f);
        dl->AddText(f,16.0f, ImVec2(tb.x+24, tb.y+bh*0.5f-9), Accent(), "Inbox");
        ImGui::Dummy(ImVec2(w, bh));
        float h=300.0f; ImVec2 tl(x0, ImGui::GetCursorScreenPos().y);
        Primitive bg; bg.cx=tl.x+cw*0.5f; bg.cy=tl.y+h*0.5f; bg.hw=cw*0.5f; bg.hh=h*0.5f; bg.corner_radius=24.0f; bg.fade=1; bg.material=Material::Thin; g->Submit(bg);
        dl->AddText(f,22.0f, ImVec2(tl.x+20, tl.y+18), kInk, e.subj);
        Primitive av; av.cx=tl.x+36; av.cy=tl.y+64; av.hw=16; av.hh=16; av.corner_radius=20; av.fade=1; av.material=Material::Accent; g->Submit(av);
        TextC(dl,f,14.0f,ImVec2(tl.x+36,tl.y+64),IM_COL32(255,255,255,255), "\xE2\x97\x8F");
        dl->AddText(f,15.0f, ImVec2(tl.x+60, tl.y+50), kInk, e.from);
        dl->AddText(f,12.0f, ImVec2(tl.x+60, tl.y+70), kInkSoft, e.time);
        dl->AddLine(ImVec2(tl.x+18,tl.y+96),ImVec2(tl.x+cw-18,tl.y+96),SA(kInkSoft,0.16f),1.0f);
        dl->AddText(f,15.0f, ImVec2(tl.x+20, tl.y+112), kInk, e.body, NULL, cw-40);
        ImGui::Dummy(ImVec2(w, h+12));
        return;
    }
    dl->AddText(f,30.0f, ImVec2(x0+2, ImGui::GetCursorScreenPos().y), kInk, "Inbox");
    int uc=0; for(int i=0;i<n;i++) if(unread[i]) uc++;
    char ub[24]; snprintf(ub,24,"%d unread",uc);
    dl->AddText(f,14.0f, ImVec2(x0+2, ImGui::GetCursorScreenPos().y+38), kInkSoft, ub);
    ImGui::Dummy(ImVec2(w, 64.0f));
    for(int i=0;i<n;i++){
        float h=82.0f; ImVec2 tl(x0, ImGui::GetCursorScreenPos().y);
        Primitive bg; bg.cx=tl.x+cw*0.5f; bg.cy=tl.y+h*0.5f; bg.hw=cw*0.5f; bg.hh=h*0.5f; bg.corner_radius=22.0f; bg.fade=1; bg.material=Material::Thin; g->Submit(bg);
        bool hov=false; if(SurfaceHit(es[i].subj, tl, ImVec2(cw, h), &hov, Sfx::NavPush)){ openIdx=i; unread[i]=false; }
        RowWash(dl, ImVec2(tl.x+6, tl.y+4), ImVec2(tl.x+cw-6, tl.y+h-4), 18.0f, false, hov);
        if(unread[i]) dl->AddCircleFilled(ImVec2(tl.x+18, tl.y+24), 4.5f, Accent());
        dl->AddText(f,16.0f, ImVec2(tl.x+34, tl.y+14), kInk, es[i].from);
        ImVec2 ts=f->CalcTextSizeA(13.0f,99999.0f,0,es[i].time); dl->AddText(f,13.0f, ImVec2(tl.x+cw-ts.x-16, tl.y+16), kInkSoft, es[i].time);
        dl->AddText(f,15.0f, ImVec2(tl.x+34, tl.y+38), kInk, es[i].subj);
        dl->AddText(f,13.0f, ImVec2(tl.x+34, tl.y+58), kInkSoft, es[i].prev, NULL, cw-50);
        ImGui::Dummy(ImVec2(w, h+10));
    }
}
static void Draw_Home(float w){
    ImDrawList* dl=ImGui::GetWindowDrawList(); ImFont* f=ImGui::GetFont();
    float cw=w<560.0f?w:560.0f; float x0=ImGui::GetCursorScreenPos().x+(w-cw)*0.5f;
    struct A{ Icon ic; const char* l; ImU32 c; };
    A apps[]={
        {Icon::Comment,"Messages",IM_COL32(48,209,88,255)}, {Icon::Map,"Maps",IM_COL32(64,156,255,255)},
        {Icon::CloudSun,"Weather",IM_COL32(90,170,255,255)}, {Icon::Clock,"Clock",IM_COL32(30,30,32,255)},
        {Icon::Camera,"Camera",IM_COL32(120,120,128,255)}, {Icon::Images,"Photos",IM_COL32(255,90,140,255)},
        {Icon::CalendarDays,"Calendar",IM_COL32(255,255,255,255)}, {Icon::Bolt,"Stocks",IM_COL32(30,30,32,255)},
        {Icon::Wallet,"Cards",IM_COL32(40,40,44,255)}, {Icon::Gear,"Settings",IM_COL32(142,142,147,255)},
        {Icon::Music,"Music",IM_COL32(255,55,95,255)}, {Icon::Bag,"App Store",IM_COL32(64,156,255,255)},
        {Icon::Calculator,"Calc",IM_COL32(255,159,10,255)}, {Icon::Fire,"Fitness",IM_COL32(76,217,100,255)},
        {Icon::Envelope,"Mail",IM_COL32(64,156,255,255)}, {Icon::FileLines,"Notes",IM_COL32(255,214,90,255)},
    };
    int n=16, cols=4; float pad=10.0f, cellw=(cw-2*pad)/cols, icon=58.0f;
    float top=ImGui::GetCursorScreenPos().y+8.0f;
    for(int i=0;i<n;i++){ int r=i/cols,c=i%cols; float cx=x0+pad+cellw*(c+0.5f), cy=top+r*(icon+34.0f)+icon*0.5f;
        dl->AddRectFilled(ImVec2(cx-icon*0.5f,cy-icon*0.5f),ImVec2(cx+icon*0.5f,cy+icon*0.5f), apps[i].c, 14.0f);
        ImU32 gl = (apps[i].c==IM_COL32(255,255,255,255))?IM_COL32(30,30,32,255):IM_COL32(255,255,255,255);
        DrawIcon(dl, apps[i].ic, ImVec2(cx,cy), icon*0.5f, gl, 2.2f);
        ImVec2 ls=f->CalcTextSizeA(12.0f,99999.0f,0,apps[i].l); dl->AddText(f,12.0f, ImVec2(cx-ls.x*0.5f, cy+icon*0.5f+6), IM_COL32(255,255,255,235), apps[i].l); }
    int rows=(n+cols-1)/cols; float gridBottom=top+rows*(icon+34.0f);
    for(int d=0;d<3;d++){ float dx=x0+cw*0.5f+(d-1)*16.0f; dl->AddCircleFilled(ImVec2(dx,gridBottom+8.0f), d==0?3.5f:3.0f, d==0?IM_COL32(255,255,255,255):SA(IM_COL32(255,255,255,255),0.4f)); }
    float dockY=gridBottom+28.0f, dockH=84.0f;
    Primitive dock; dock.cx=x0+cw*0.5f; dock.cy=dockY+dockH*0.5f; dock.hw=cw*0.5f; dock.hh=dockH*0.5f; dock.corner_radius=28.0f; dock.fade=1; dock.material=Material::Thick; g->Submit(dock);
    A dk[]={ {Icon::Phone,"Phone",IM_COL32(48,209,88,255)},{Icon::Globe,"Browser",IM_COL32(64,156,255,255)},{Icon::Comment,"Messages",IM_COL32(48,209,88,255)},{Icon::Music,"Music",IM_COL32(255,55,95,255)} };
    float dcellw=cw/4.0f;
    for(int i=0;i<4;i++){ float cx=x0+dcellw*(i+0.5f), cy=dockY+dockH*0.5f;
        dl->AddRectFilled(ImVec2(cx-icon*0.5f,cy-icon*0.5f),ImVec2(cx+icon*0.5f,cy+icon*0.5f), dk[i].c, 14.0f);
        DrawIcon(dl, dk[i].ic, ImVec2(cx,cy), icon*0.5f, IM_COL32(255,255,255,255), 2.2f); }
    ImGui::SetCursorScreenPos(ImVec2(x0, dockY+dockH+8.0f)); ImGui::Dummy(ImVec2(w, 8.0f));
}
static bool ReminderCheck(const char* id, ImVec2 c, float r, bool* done){
    ImGui::PushID(id); ImGui::SetCursorScreenPos(ImVec2(c.x-r,c.y-r)); ImGui::InvisibleButton("c",ImVec2(2*r,2*r));
    bool clk=ImGui::IsItemDeactivated()&&ImGui::IsItemHovered(); if(clk)*done=!*done;
    Spring& s=g->springs().Get((uint32_t)ImGui::GetID("c"),5,SpringStyle::Bouncy,*done?1.0f:0.0f); s.target=*done?1.0f:0.0f; s.Tick(Dt()); float k=Clampf(s.x,0,1);
    ImDrawList* dl=ImGui::GetWindowDrawList();
    dl->AddCircle(c,r,SA(kInkSoft,0.7f),0,2.0f);
    if(k>0.01f){ dl->AddCircleFilled(c,r*k,Accent());
        if(k>0.45f){ ImU32 w=IM_COL32(255,255,255,255); dl->AddLine(ImVec2(c.x-r*0.42f,c.y+r*0.02f),ImVec2(c.x-r*0.08f,c.y+r*0.34f),w,2.0f); dl->AddLine(ImVec2(c.x-r*0.08f,c.y+r*0.34f),ImVec2(c.x+r*0.46f,c.y-r*0.30f),w,2.0f);} }
    ImGui::PopID(); return clk;
}
static void Draw_Reminders(float w){
    ImDrawList* dl=ImGui::GetWindowDrawList(); ImFont* f=ImGui::GetFont();
    float cw=w<560.0f?w:560.0f; float x0=ImGui::GetCursorScreenPos().x+(w-cw)*0.5f;
    { struct T{ Icon ic; const char* l; int n; ImU32 c; }; T ts[]={ {Icon::CalendarDay,"Today",4,IM_COL32(64,156,255,255)},{Icon::CalendarDays,"Scheduled",9,IM_COL32(255,69,58,255)},{Icon::List,"All",23,IM_COL32(60,60,64,255)},{Icon::Flag,"Flagged",2,IM_COL32(255,159,10,255)} };
      float gap=14, tw=(cw-gap)/2, th=70; float sy=ImGui::GetCursorScreenPos().y;
      for(int i=0;i<4;i++){ int r=i/2,c=i%2; float x=x0+c*(tw+gap), y=sy+r*(th+gap);
        Primitive bg; bg.cx=x+tw*0.5f; bg.cy=y+th*0.5f; bg.hw=tw*0.5f; bg.hh=th*0.5f; bg.corner_radius=22; bg.fade=1; bg.material=Material::Thin; g->Submit(bg);
        Primitive t; t.cx=x+30; t.cy=y+26; t.hw=15; t.hh=15; t.corner_radius=20; t.fade=1; t.material=Material::Accent; g->Submit(t);
        DrawIcon(dl,ts[i].ic,ImVec2(x+30,y+26),15.0f,IM_COL32(255,255,255,255),2.0f);
        char nb[8]; snprintf(nb,8,"%d",ts[i].n); ImVec2 ns=f->CalcTextSizeA(28.0f,99999.0f,0,nb); dl->AddText(f,28.0f,ImVec2(x+tw-ns.x-16,y+10),kInk,nb);
        dl->AddText(f,14.0f,ImVec2(x+16,y+44),kInkSoft,ts[i].l); }
      ImGui::Dummy(ImVec2(w, 2*th+gap+16)); }
    { dl->AddText(f,20.0f, ImVec2(x0+4, ImGui::GetCursorScreenPos().y), Accent(), "Today"); ImGui::Dummy(ImVec2(w,30.0f));
      static bool d[6]={false,true,false,false,false,true};
      const char* tl2[6]={"Pick up dry cleaning","Reply to Sarah","Review the budget","Book flights for July","Renew passport","Buy coffee beans"};
      const char* sub[6]={"Today 9:00 AM \xC2\xB7 Work","Today 11:30 AM","this afternoon",nullptr,"tomorrow",nullptr};
      bool prio[6]={true,false,false,false,false,false};
      float rowh=56.0f, h=6*rowh; ImVec2 tl(x0, ImGui::GetCursorScreenPos().y);
      Primitive bg; bg.cx=tl.x+cw*0.5f; bg.cy=tl.y+h*0.5f; bg.hw=cw*0.5f; bg.hh=h*0.5f; bg.corner_radius=24; bg.fade=1; bg.material=Material::Thin; g->Submit(bg);
      for(int i=0;i<6;i++){ float ry=tl.y+i*rowh, cy=ry+rowh*0.5f;
        ReminderCheck((const char*)tl2[i], ImVec2(tl.x+28, cy), 11.0f, &d[i]);
        ImU32 tc = d[i]?SA(kInkSoft,0.7f):kInk;
        float tx=tl.x+52; if(prio[i]){ dl->AddText(f,16.0f,ImVec2(tx,cy- (sub[i]?17:8)),IM_COL32(255,159,10,255),"!"); tx+=14; }
        dl->AddText(f,16.0f, ImVec2(tx, cy-(sub[i]?17.0f:8.0f)), tc, tl2[i]);
        if(sub[i]) dl->AddText(f,13.0f, ImVec2(tl.x+52, cy+3), kInkSoft, sub[i]);
        if(i<5) dl->AddLine(ImVec2(tl.x+52,ry+rowh),ImVec2(tl.x+cw-14,ry+rowh),SA(kInkSoft,0.14f),1.0f); }
      ImGui::Dummy(ImVec2(w, h+12)); }
}
static void Draw_Health(float w){
    ImDrawList* dl=ImGui::GetWindowDrawList(); ImFont* f=ImGui::GetFont();
    float cw=w<640.0f?w:640.0f; float x0=ImGui::GetCursorScreenPos().x+(w-cw)*0.5f;
    { float h=180.0f; ImVec2 tl(x0, ImGui::GetCursorScreenPos().y);
      Primitive bg; bg.cx=tl.x+cw*0.5f; bg.cy=tl.y+h*0.5f; bg.hw=cw*0.5f; bg.hh=h*0.5f; bg.corner_radius=29; bg.fade=1; bg.material=Material::Thin; g->Submit(bg);
      ImVec2 c(tl.x+96, tl.y+h*0.5f); float fr[3]={0.82f,0.61f,0.94f}; ImU32 cl[3]={IM_COL32(255,55,95,255),IM_COL32(126,255,40,255),IM_COL32(64,220,255,255)};
      float rr[3]={60,45,30};
      for(int i=0;i<3;i++){ dl->PathArcTo(c,rr[i],-1.5708f,-1.5708f+6.2831853f,64); dl->PathStroke(SA(cl[i],0.22f),0,11.0f);
        dl->PathArcTo(c,rr[i],-1.5708f,-1.5708f+6.2831853f*fr[i],64); dl->PathStroke(cl[i],0,11.0f); }
      dl->AddText(f,15.0f, ImVec2(tl.x+186, tl.y+24), kInk, "Move");      dl->AddText(f,22.0f, ImVec2(tl.x+186, tl.y+42), IM_COL32(255,55,95,255), "486/600 CAL");
      dl->AddText(f,15.0f, ImVec2(tl.x+186, tl.y+78), kInk, "Exercise");  dl->AddText(f,22.0f, ImVec2(tl.x+186, tl.y+96), IM_COL32(126,255,40,255), "28/45 MIN");
      dl->AddText(f,15.0f, ImVec2(tl.x+186, tl.y+124), kInk, "Stand");    dl->AddText(f,22.0f, ImVec2(tl.x+186, tl.y+142), IM_COL32(64,220,255,255), "10/12 HR");
      ImGui::Dummy(ImVec2(w, h+14)); }
    { struct S{ Icon ic; const char* l; const char* v; const char* u; ImU32 c; }; S ss[]={ {Icon::Fire,"Steps","8,421","steps",IM_COL32(255,149,0,255)},{Icon::Heart,"Heart Rate","68","BPM",IM_COL32(255,55,95,255)},{Icon::LocationArrow,"Distance","3.8","mi",IM_COL32(64,156,255,255)},{Icon::Moon,"Sleep","7h 12m","last night",IM_COL32(94,92,230,255)} };
      float gap=14, tw=(cw-gap)/2, th=110; float sy=ImGui::GetCursorScreenPos().y;
      static float hr[24]; for(int i=0;i<24;i++) hr[i]=66+sinf(i*0.5f)*6+sinf(i*1.7f)*3;
      for(int i=0;i<4;i++){ int r=i/2,c=i%2; float x=x0+c*(tw+gap), y=sy+r*(th+gap);
        Primitive bg; bg.cx=x+tw*0.5f; bg.cy=y+th*0.5f; bg.hw=tw*0.5f; bg.hh=th*0.5f; bg.corner_radius=26; bg.fade=1; bg.material=Material::Thin; g->Submit(bg);
        DrawIcon(dl,ss[i].ic,ImVec2(x+24,y+24),15.0f,ss[i].c,2.0f);
        dl->AddText(f,13.0f,ImVec2(x+38,y+17),ss[i].c,ss[i].l);
        dl->AddText(f,30.0f,ImVec2(x+22,y+38),kInk,ss[i].v);
        dl->AddText(f,12.0f,ImVec2(x+24,y+78),kInkSoft,ss[i].u);
        if(i==1) Sparkline(dl, ImVec2(x+tw-120,y+44), 104, 44, hr, 24, IM_COL32(255,55,95,255)); }
      int rows=2; ImGui::Dummy(ImVec2(w, rows*(th+gap)+4)); }
}
static void Draw_Wallet(float w){
    ImDrawList* dl=ImGui::GetWindowDrawList(); ImFont* f=ImGui::GetFont();
    float cw=w<460.0f?w:460.0f; float x0=ImGui::GetCursorScreenPos().x+(w-cw)*0.5f;
    struct Card{ const char* name; const char* num; ImU32 a; ImU32 b; Icon ic; };
    Card cs[]={
        {"Glass Card","\xE2\x80\xA2\xE2\x80\xA2\xE2\x80\xA2\xE2\x80\xA2 2847", IM_COL32(245,245,247,255),IM_COL32(200,200,210,255), Icon::Spark},
        {"GlassPay Platinum","\xE2\x80\xA2\xE2\x80\xA2\xE2\x80\xA2\xE2\x80\xA2 9021", IM_COL32(40,40,60,255),IM_COL32(90,80,160,255), Icon::CreditCard},
        {"Transit","Balance $42.50", IM_COL32(0,120,160,255),IM_COL32(40,180,200,255), Icon::Map},
        {"Boarding Pass","SFO \xE2\x86\x92 JFK", IM_COL32(200,80,60,255),IM_COL32(255,150,90,255), Icon::Airplane},
    };
    float ch=150.0f, overlap=92.0f; float sy=ImGui::GetCursorScreenPos().y;
    for(int i=0;i<4;i++){ float y=sy+i*overlap;
        dl->AddRectFilled(ImVec2(x0,y),ImVec2(x0+cw,y+ch), cs[i].a, 18.0f);
        RoundedGrad(dl, ImVec2(x0+2,y+2),ImVec2(x0+cw-2,y+ch*0.5f), SA(cs[i].b,0.55f),SA(cs[i].b,0.55f),IM_COL32(0,0,0,0),IM_COL32(0,0,0,0), 16.0f, ImDrawFlags_RoundCornersTop);
        bool light = (cs[i].a==IM_COL32(245,245,247,255));
        ImU32 tx = light?IM_COL32(30,30,32,255):IM_COL32(255,255,255,255);
        DrawIcon(dl, cs[i].ic, ImVec2(x0+cw-34, y+30), 22.0f, tx, 2.2f);
        dl->AddText(f,18.0f, ImVec2(x0+22, y+20), tx, cs[i].name);
        dl->AddRectFilled(ImVec2(x0+24,y+ch-58),ImVec2(x0+58,y+ch-34), SA(IM_COL32(255,215,120,255), light?0.9f:0.8f), 5.0f);
        dl->AddText(f,17.0f, ImVec2(x0+22, y+ch-28), tx, cs[i].num);
    }
    ImGui::Dummy(ImVec2(w, sy+3*overlap+ch - sy + 12));
}
static void Draw_Lock(float w){
    ImDrawList* dl=ImGui::GetWindowDrawList(); ImFont* f=ImGui::GetFont();
    float cw=w<520.0f?w:520.0f; float x0=ImGui::GetCursorScreenPos().x+(w-cw)*0.5f;
    time_t tt=time(nullptr); struct tm lt; localtime_s(&lt,&tt);
    DrawIcon(dl, Icon::Lock, ImVec2(x0+cw*0.5f, ImGui::GetCursorScreenPos().y+14), 18.0f, SA(kInk,0.8f), 2.0f);
    ImGui::Dummy(ImVec2(w, 36.0f));
    { char tb[16]; FmtClock(tb,16,lt.tm_hour,lt.tm_min);
      ImVec2 ts=f->CalcTextSizeA(96.0f,99999.0f,0,tb); dl->AddText(f,96.0f, ImVec2(x0+cw*0.5f-ts.x*0.5f, ImGui::GetCursorScreenPos().y), kInk, tb);
      static const char* wd[]={"Sunday","Monday","Tuesday","Wednesday","Thursday","Friday","Saturday"};
      static const char* mo[]={"January","February","March","April","May","June","July","August","September","October","November","December"};
      char db[48]; snprintf(db,48,"%s, %s %d", wd[lt.tm_wday%7], mo[lt.tm_mon%12], lt.tm_mday);
      ImVec2 ds=f->CalcTextSizeA(17.0f,99999.0f,0,db); dl->AddText(f,17.0f, ImVec2(x0+cw*0.5f-ds.x*0.5f, ImGui::GetCursorScreenPos().y+104), kInkSoft, db);
      ImGui::Dummy(ImVec2(w, 150.0f)); }
    struct N{ Icon ic; const char* app; const char* t; const char* b; };
    N ns[]={ {Icon::Comment,"MESSAGES","Craig","See you at the review \xF0\x9F\x91\x8B"},{Icon::CalendarDays,"CALENDAR","Design Review","In 15 minutes"} };
    for(int i=0;i<2;i++){ float h=70.0f; ImVec2 tl(x0+cw*0.06f, ImGui::GetCursorScreenPos().y); float ww=cw*0.88f;
      Primitive bg; bg.cx=tl.x+ww*0.5f; bg.cy=tl.y+h*0.5f; bg.hw=ww*0.5f; bg.hh=h*0.5f; bg.corner_radius=24; bg.fade=1; bg.material=Material::Thick; g->Submit(bg);
      Primitive t; t.cx=tl.x+30; t.cy=tl.y+h*0.5f; t.hw=15; t.hh=15; t.corner_radius=8; t.fade=1; t.material=Material::Accent; g->Submit(t);
      DrawIcon(dl,ns[i].ic,ImVec2(tl.x+30,tl.y+h*0.5f),15.0f,IM_COL32(255,255,255,255),2.0f);
      dl->AddText(f,12.0f,ImVec2(tl.x+56,tl.y+12),kInkSoft,ns[i].app);
      dl->AddText(f,15.0f,ImVec2(tl.x+56,tl.y+30),kInk,ns[i].t);
      dl->AddText(f,13.0f,ImVec2(tl.x+56,tl.y+48),kInkSoft,ns[i].b);
      ImGui::Dummy(ImVec2(w, h+12)); }
    { float cy=ImGui::GetCursorScreenPos().y+30;
      Primitive a; a.cx=x0+cw*0.18f; a.cy=cy; a.hw=26; a.hh=26; a.corner_radius=30; a.fade=1; a.material=Material::Thick; g->Submit(a);
      DrawIcon(dl,Icon::Bolt,ImVec2(x0+cw*0.18f,cy),20.0f,kInk,2.2f);
      Primitive b; b.cx=x0+cw*0.82f; b.cy=cy; b.hw=26; b.hh=26; b.corner_radius=30; b.fade=1; b.material=Material::Thick; g->Submit(b);
      DrawIcon(dl,Icon::Camera,ImVec2(x0+cw*0.82f,cy),20.0f,kInk,2.2f);
      ImGui::Dummy(ImVec2(w, 70.0f)); }
}
static void Draw_Calendar(float w){
    ImDrawList* dl=ImGui::GetWindowDrawList(); ImFont* f=ImGui::GetFont();
    float cw=w<560.0f?w:560.0f; float x0=ImGui::GetCursorScreenPos().x+(w-cw)*0.5f;
    time_t tt=time(nullptr); struct tm lt; localtime_s(&lt,&tt);
    static int monthOffset=0;
    static int selDay=0;
    struct tm f0; memset(&f0,0,sizeof(f0)); f0.tm_year=lt.tm_year; f0.tm_mon=lt.tm_mon+monthOffset; f0.tm_mday=1; f0.tm_hour=12; mktime(&f0);
    int mon=f0.tm_mon, yr=f0.tm_year+1900, firstDow=f0.tm_wday;
    int today = (monthOffset==0) ? lt.tm_mday : -1;
    int dim[]={31,28,31,30,31,30,31,31,30,31,30,31}; int days=dim[mon]; if(mon==1 && ((yr%4==0&&yr%100!=0)||yr%400==0)) days=29;
    static const char* mo[]={"January","February","March","April","May","June","July","August","September","October","November","December"};
    if(selDay<1||selDay>days) selDay = (today>0?today:1);
    bool evt[32]={false}; evt[4]=evt[12]=evt[21]=true; if(today>0) evt[today]=true;
    { float cellh=42.0f; int rows=(firstDow+days+6)/7; float h=86.0f+rows*cellh;
      ImVec2 tl(x0, ImGui::GetCursorScreenPos().y);
      Primitive bg; bg.cx=tl.x+cw*0.5f; bg.cy=tl.y+h*0.5f; bg.hw=cw*0.5f; bg.hh=h*0.5f; bg.corner_radius=28; bg.fade=1; bg.material=Material::Thin; g->Submit(bg);
      char hb[32]; snprintf(hb,32,"%s %d",mo[mon],yr); dl->AddText(f,22.0f, ImVec2(tl.x+22, tl.y+16), kInk, hb);
      bool lhv=false,rhv=false;
      if(SurfaceHit("calprev", ImVec2(tl.x+cw-74,tl.y+12), ImVec2(30,30), &lhv)){ monthOffset--; selDay=0; }
      if(SurfaceHit("calnext", ImVec2(tl.x+cw-42,tl.y+12), ImVec2(30,30), &rhv)){ monthOffset++; selDay=0; }
      DrawIcon(dl,Icon::ChevronL,ImVec2(tl.x+cw-58,tl.y+27),14.0f,lhv?kInk:Accent(),2.0f);
      DrawIcon(dl,Icon::ChevronR,ImVec2(tl.x+cw-26,tl.y+27),14.0f,rhv?kInk:Accent(),2.0f);
      const char* wd[]={"S","M","T","W","T","F","S"}; float gx0=tl.x+20, gw=cw-40, cellw=gw/7.0f;
      for(int i=0;i<7;i++){ ImVec2 ws=f->CalcTextSizeA(12.0f,99999.0f,0,wd[i]); dl->AddText(f,12.0f, ImVec2(gx0+cellw*(i+0.5f)-ws.x*0.5f, tl.y+54), kInkSoft, wd[i]); }
      for(int d=1;d<=days;d++){ int idx=firstDow+d-1; int r=idx/7,c=idx%7; float cx=gx0+cellw*(c+0.5f), cy=tl.y+86+r*cellh+cellh*0.5f-6;
        char id[8]; snprintf(id,8,"cd%d",d);
        if(SurfaceHit(id, ImVec2(gx0+cellw*c, tl.y+86+r*cellh), ImVec2(cellw,cellh))) selDay=d;
        if(d==today)        dl->AddCircleFilled(ImVec2(cx,cy+4),15.0f,Accent());
        else if(d==selDay)  dl->AddCircle(ImVec2(cx,cy+4),15.0f,Accent(),0,2.0f);
        char db[4]; snprintf(db,4,"%d",d); ImVec2 ds=f->CalcTextSizeA(15.0f,99999.0f,0,db);
        dl->AddText(f,15.0f, ImVec2(cx-ds.x*0.5f, cy-4), d==today?IM_COL32(255,255,255,255):kInk, db);
        if(evt[d]) dl->AddCircleFilled(ImVec2(cx,cy+20), 2.5f, d==today?IM_COL32(255,255,255,255):Accent()); }
      ImGui::Dummy(ImVec2(w, h+14)); }
    { char ah[40]; snprintf(ah,40,"%s %d",mo[mon],selDay);
      dl->AddText(f,18.0f, ImVec2(x0+4, ImGui::GetCursorScreenPos().y), kInk, ah); ImGui::Dummy(ImVec2(w,28.0f));
      struct Ev{ const char* t; const char* time; ImU32 c; }; Ev es[]={ {"Design Review","10:00 \xE2\x80\x93 11:00",IM_COL32(255,69,58,255)},{"Lunch with Sam","12:30 \xE2\x80\x93 13:30",IM_COL32(48,209,88,255)},{"Dentist","3:00 PM",IM_COL32(64,156,255,255)} };
      if(evt[selDay]){
        for(int i=0;i<3;i++){ float h=58.0f; ImVec2 tl(x0, ImGui::GetCursorScreenPos().y);
          Primitive bg; bg.cx=tl.x+cw*0.5f; bg.cy=tl.y+h*0.5f; bg.hw=cw*0.5f; bg.hh=h*0.5f; bg.corner_radius=22; bg.fade=1; bg.material=Material::Thin; g->Submit(bg);
          dl->AddRectFilled(ImVec2(tl.x+14,tl.y+12),ImVec2(tl.x+18,tl.y+h-12), es[i].c, 2.0f);
          dl->AddText(f,16.0f, ImVec2(tl.x+30, tl.y+12), kInk, es[i].t);
          dl->AddText(f,13.0f, ImVec2(tl.x+30, tl.y+34), kInkSoft, es[i].time);
          ImGui::Dummy(ImVec2(w, h+10)); }
      } else {
        float h=58.0f; ImVec2 tl(x0, ImGui::GetCursorScreenPos().y);
        Primitive bg; bg.cx=tl.x+cw*0.5f; bg.cy=tl.y+h*0.5f; bg.hw=cw*0.5f; bg.hh=h*0.5f; bg.corner_radius=22; bg.fade=1; bg.material=Material::Thin; g->Submit(bg);
        TextC(dl,f,15.0f,ImVec2(tl.x+cw*0.5f,tl.y+h*0.5f),kInkSoft,"No events");
        ImGui::Dummy(ImVec2(w, h+10)); } }
}
static void Draw_Camera(float w){
    ImDrawList* dl=ImGui::GetWindowDrawList(); ImFont* f=ImGui::GetFont();
    float cw=w<480.0f?w:480.0f; float x0=ImGui::GetCursorScreenPos().x+(w-cw)*0.5f;
    float top=ImGui::GetCursorScreenPos().y, vh=cw*1.1f; if(vh>520) vh=520;
    dl->AddRectFilledMultiColor(ImVec2(x0,top),ImVec2(x0+cw,top+vh), IM_COL32(40,44,60,255),IM_COL32(60,70,90,255),IM_COL32(30,34,46,255),IM_COL32(50,40,60,255));
    dl->AddRect(ImVec2(x0,top),ImVec2(x0+cw,top+vh), SA(IM_COL32(255,255,255,255),0.12f), 20.0f, 0, 1.5f);
    dl->PushClipRect(ImVec2(x0,top),ImVec2(x0+cw,top+vh),true);
    float fcx=x0+cw*0.5f, fcy=top+vh*0.45f; dl->AddRect(ImVec2(fcx-36,fcy-36),ImVec2(fcx+36,fcy+36), IM_COL32(255,214,90,255), 4.0f, 0, 1.5f);
    for(int i=1;i<3;i++){ dl->AddLine(ImVec2(x0+cw*i/3,top),ImVec2(x0+cw*i/3,top+vh),SA(IM_COL32(255,255,255,255),0.12f),1.0f); dl->AddLine(ImVec2(x0,top+vh*i/3),ImVec2(x0+cw,top+vh*i/3),SA(IM_COL32(255,255,255,255),0.12f),1.0f); }
    Primitive fl; fl.cx=x0+34; fl.cy=top+30; fl.hw=18; fl.hh=14; fl.corner_radius=10; fl.fade=1; fl.material=Material::Thick; g->Submit(fl); DrawIcon(dl,Icon::Bolt,ImVec2(x0+34,top+30),14.0f,IM_COL32(255,214,90,255),2.0f);
    dl->PopClipRect();
    float my=top+vh+22; const char* modes[]={"PHOTO","VIDEO","PORTRAIT","PANO"}; int sel=0;
    float mx=x0+cw*0.5f; float spacing=92.0f;
    for(int i=0;i<4;i++){ float dxp=mx+(i-sel)*spacing; if(dxp<x0+10||dxp>x0+cw-10) continue; ImVec2 ms=f->CalcTextSizeA(13.0f,99999.0f,0,modes[i]); dl->AddText(f,13.0f,ImVec2(dxp-ms.x*0.5f,my),i==sel?IM_COL32(255,214,90,255):kInkSoft,modes[i]); }
    ImGui::Dummy(ImVec2(w, vh+50.0f));
    float sy=ImGui::GetCursorScreenPos().y+20;
    dl->AddRectFilled(ImVec2(x0+24,sy-22),ImVec2(x0+68,sy+22), IM_COL32(120,140,170,255), 8.0f);
    { bool shv=false; SurfaceHit("camshutter", ImVec2(x0+cw*0.5f-42, sy-42), ImVec2(84,84), &shv, Sfx::Shutter); }
    dl->AddCircleFilled(ImVec2(x0+cw*0.5f,sy), 34.0f, IM_COL32(255,255,255,255)); dl->AddCircle(ImVec2(x0+cw*0.5f,sy),40.0f,IM_COL32(255,255,255,255),0,3.0f);
    Primitive flip; flip.cx=x0+cw-46; flip.cy=sy; flip.hw=24; flip.hh=24; flip.corner_radius=29; flip.fade=1; flip.material=Material::Thick; g->Submit(flip); DrawIcon(dl,Icon::Refresh,ImVec2(x0+cw-46,sy),20.0f,kInk,2.2f);
    ImGui::Dummy(ImVec2(w, 76.0f));
}
static void Draw_Spotlight(float w){
    ImDrawList* dl=ImGui::GetWindowDrawList(); ImFont* f=ImGui::GetFont();
    float cw=w<600.0f?w:600.0f; float x0=ImGui::GetCursorScreenPos().x+(w-cw)*0.5f;
    static char spotQ[64] = "wea";
    { float h=54.0f; ImVec2 tl(x0, ImGui::GetCursorScreenPos().y);
      Primitive bg; bg.cx=tl.x+cw*0.5f; bg.cy=tl.y+h*0.5f; bg.hw=cw*0.5f; bg.hh=h*0.5f; bg.corner_radius=22; bg.fade=1; bg.material=Material::Thick; g->Submit(bg);
      DrawIcon(dl,Icon::Search,ImVec2(tl.x+28,tl.y+h*0.5f),18.0f,kInkSoft,2.0f);
      ImGui::SetCursorScreenPos(ImVec2(tl.x+48, tl.y+h*0.5f-13.0f));
      ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0,0,0,0)); ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0,0,0,0)); ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0,0,0,0)); ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.93f,0.93f,0.96f,1.0f));
      ImGui::SetNextItemWidth(cw-70); ImGui::InputTextWithHint("##spot","Spotlight Search", spotQ, sizeof(spotQ));
      ImGui::PopStyleColor(4);
      ImGui::Dummy(ImVec2(w, h+12)); }
    { dl->AddText(f,11.0f,ImVec2(x0+4,ImGui::GetCursorScreenPos().y),kInkSoft,"TOP HIT"); ImGui::Dummy(ImVec2(w,20.0f));
      float h=64.0f; ImVec2 tl(x0, ImGui::GetCursorScreenPos().y);
      Primitive bg; bg.cx=tl.x+cw*0.5f; bg.cy=tl.y+h*0.5f; bg.hw=cw*0.5f; bg.hh=h*0.5f; bg.corner_radius=22; bg.fade=1; bg.material=Material::Accent; g->Submit(bg);
      DrawIcon(dl,Icon::CloudSun,ImVec2(tl.x+34,tl.y+h*0.5f),22.0f,IM_COL32(255,255,255,255),2.2f);
      dl->AddText(f,17.0f,ImVec2(tl.x+62,tl.y+14),IM_COL32(255,255,255,255),"Weather");
      dl->AddText(f,13.0f,ImVec2(tl.x+62,tl.y+36),SA(IM_COL32(255,255,255,255),0.8f),"Application \xC2\xB7 Current conditions");
      ImGui::Dummy(ImVec2(w, h+14)); }
    { dl->AddText(f,11.0f,ImVec2(x0+4,ImGui::GetCursorScreenPos().y),kInkSoft,"RESULTS"); ImGui::Dummy(ImVec2(w,20.0f));
      struct R{ Icon ic; const char* t; const char* s; }; R rs[]={ {Icon::CloudSun,"Weather \xE2\x80\x94 Cupertino","68\xC2\xB0, Mostly Sunny"},{Icon::Music,"Weekend Playlist","Music \xC2\xB7 Playlist"},{Icon::Map,"Blue Bottle Coffee","Maps \xC2\xB7 0.4 mi"},{Icon::Globe,"weather forecast","Search the web"},{Icon::FileLines,"Q3 Report.pages","Documents"} };
      float rowh=52.0f, h=5*rowh; ImVec2 tl(x0, ImGui::GetCursorScreenPos().y);
      Primitive bg; bg.cx=tl.x+cw*0.5f; bg.cy=tl.y+h*0.5f; bg.hw=cw*0.5f; bg.hh=h*0.5f; bg.corner_radius=22; bg.fade=1; bg.material=Material::Thin; g->Submit(bg);
      for(int i=0;i<5;i++){ float ry=tl.y+i*rowh, cy=ry+rowh*0.5f;
        DrawIcon(dl,rs[i].ic,ImVec2(tl.x+28,cy),17.0f,kInk,2.0f);
        dl->AddText(f,15.0f,ImVec2(tl.x+52,cy-16),kInk,rs[i].t);
        dl->AddText(f,12.0f,ImVec2(tl.x+52,cy+2),kInkSoft,rs[i].s);
        DrawIcon(dl,Icon::ChevronR,ImVec2(tl.x+cw-18,cy),12.0f,SA(kInkSoft,0.7f),2.0f);
        if(i<4) dl->AddLine(ImVec2(tl.x+52,ry+rowh),ImVec2(tl.x+cw-14,ry+rowh),SA(kInkSoft,0.14f),1.0f); }
      ImGui::Dummy(ImVec2(w, h+12)); }
}
static void Draw_AppStore(float w){
    ImDrawList* dl=ImGui::GetWindowDrawList(); ImFont* f=ImGui::GetFont();
    float cw=w<640.0f?w:640.0f; float x0=ImGui::GetCursorScreenPos().x+(w-cw)*0.5f;
    dl->AddText(f,13.0f, ImVec2(x0+2, ImGui::GetCursorScreenPos().y), IM_COL32(255,69,58,255), "THURSDAY, MAY 29");
    dl->AddText(f,32.0f, ImVec2(x0+2, ImGui::GetCursorScreenPos().y+18), kInk, "Today");
    ImGui::Dummy(ImVec2(w, 66.0f));
    { float h=240.0f; ImVec2 tl(x0, ImGui::GetCursorScreenPos().y);
      dl->AddRectFilledMultiColor(ImVec2(tl.x,tl.y),ImVec2(tl.x+cw,tl.y+h), IM_COL32(40,40,70,255),IM_COL32(80,60,140,255),IM_COL32(140,80,120,255),IM_COL32(50,50,90,255));
      dl->AddRect(ImVec2(tl.x,tl.y),ImVec2(tl.x+cw,tl.y+h), SA(IM_COL32(255,255,255,255),0.15f),22.0f,0,1.5f);
      dl->AddText(f,13.0f, ImVec2(tl.x+22, tl.y+18), SA(IM_COL32(255,255,255,255),0.85f), "FEATURED");
      dl->AddText(f,30.0f, ImVec2(tl.x+22, tl.y+h-92), IM_COL32(255,255,255,255), "Focus");
      dl->AddText(f,16.0f, ImVec2(tl.x+22, tl.y+h-54), SA(IM_COL32(255,255,255,255),0.85f), "Make time for what matters most.");
      Primitive a; a.cx=tl.x+cw-48; a.cy=tl.y+h-44; a.hw=24; a.hh=24; a.corner_radius=18; a.fade=1; a.material=Material::Thick; g->Submit(a);
      DrawIcon(dl,Icon::Spark,ImVec2(tl.x+cw-48,tl.y+h-44),22.0f,IM_COL32(255,255,255,255),2.2f);
      ImGui::Dummy(ImVec2(w, h+18)); }
    { struct A{ Icon ic; const char* n; const char* s; ImU32 c; }; A as[]={ {Icon::Music,"Lumi Music","Your library, beautifully sorted",IM_COL32(255,55,95,255)},{Icon::CloudSun,"Weather Pro","Hyperlocal forecasts",IM_COL32(64,156,255,255)},{Icon::Gamepad,"Tangle","Puzzle \xC2\xB7 Editors' Choice",IM_COL32(94,92,230,255)},{Icon::Heart,"Vital","Health & activity",IM_COL32(48,209,88,255)} };
      float rowh=72.0f, h=4*rowh; ImVec2 tl(x0, ImGui::GetCursorScreenPos().y);
      Primitive bg; bg.cx=tl.x+cw*0.5f; bg.cy=tl.y+h*0.5f; bg.hw=cw*0.5f; bg.hh=h*0.5f; bg.corner_radius=24; bg.fade=1; bg.material=Material::Thin; g->Submit(bg);
      for(int i=0;i<4;i++){ float ry=tl.y+i*rowh, cy=ry+rowh*0.5f;
        dl->AddRectFilled(ImVec2(tl.x+16,cy-24),ImVec2(tl.x+64,cy+24), as[i].c, 12.0f);
        DrawIcon(dl,as[i].ic,ImVec2(tl.x+40,cy),26.0f,IM_COL32(255,255,255,255),2.2f);
        dl->AddText(f,16.0f,ImVec2(tl.x+78,cy-18),kInk,as[i].n);
        dl->AddText(f,13.0f,ImVec2(tl.x+78,cy+2),kInkSoft,as[i].s);
        float bw=64,bx=tl.x+cw-bw-16; Primitive get; get.cx=bx+bw*0.5f; get.cy=cy; get.hw=bw*0.5f; get.hh=14; get.corner_radius=18; get.fade=1; get.material=Material::Accent; g->Submit(get);
        ImVec2 gs=f->CalcTextSizeA(13.0f,99999.0f,0,"GET"); dl->AddText(f,13.0f,ImVec2(bx+bw*0.5f-gs.x*0.5f,cy-7),IM_COL32(255,255,255,255),"GET");
        if(i<3) dl->AddLine(ImVec2(tl.x+78,ry+rowh),ImVec2(tl.x+cw-14,ry+rowh),SA(kInkSoft,0.14f),1.0f); }
      ImGui::Dummy(ImVec2(w, h+12)); }
}
static void Draw_Notes(float w){
    ImDrawList* dl=ImGui::GetWindowDrawList(); ImFont* f=ImGui::GetFont();
    float cw=w<640.0f?w:640.0f; float x0=ImGui::GetCursorScreenPos().x+(w-cw)*0.5f;
    static char titles[6][64]; static char bodies[6][512]; static int sel=0; static bool init=false;
    if(!init){ const char* dt[6]={"Trip to Portland","Grocery list","Meeting notes","Book recommendations","Workout plan","Gift ideas"};
        const char* db[6]={"Flights booked for the 14th. Look into the riverfront hotel and a coffee tour downtown.","Coffee beans, oat milk, eggs, spinach, pasta, and the good chocolate.","Q3 kickoff: align on goals, owners, and timelines. Follow up with Sarah on the deck.","The Midnight Library, Project Hail Mary, and Educated were all excellent reads.","Mon/Wed/Fri: 30 min run plus core. Tue/Thu: upper body. Rest on the weekend.","Noise-cancelling headphones, a nice notebook, or a plant for the desk."};
        for(int i=0;i<6;i++){ snprintf(titles[i],64,"%s",dt[i]); snprintf(bodies[i],512,"%s",db[i]); } init=true; }
    dl->AddText(f,30.0f, ImVec2(x0+2, ImGui::GetCursorScreenPos().y), kInk, "Notes");
    ImGui::Dummy(ImVec2(w, 46.0f));
    ImGui::Indent((w-cw)*0.5f); ImGui::PushItemWidth(cw);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(1,1,1,0.06f)); ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1,1,1,0.92f));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 16.0f); ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(14,11));
    ImGui::SetNextItemWidth(cw); ImGui::InputTextWithHint("##ntitle", "Title", titles[sel], 64);
    ImGui::Dummy(ImVec2(0,6));
    ImGui::SetNextItemWidth(cw); ImGui::InputTextMultiline("##nbody", bodies[sel], 512, ImVec2(cw, 132.0f));
    ImGui::PopStyleVar(2); ImGui::PopStyleColor(2);
    ImGui::PopItemWidth(); ImGui::Unindent((w-cw)*0.5f);
    ImGui::Dummy(ImVec2(0,12));
    int n=6, cols=2; float gap=14, tw=(cw-gap)/2, th=92; float sy=ImGui::GetCursorScreenPos().y;
    for(int i=0;i<n;i++){ int r=i/cols,c=i%cols; float x=x0+c*(tw+gap), y=sy+r*(th+gap);
        ImGui::PushID(i); ImGui::SetCursorScreenPos(ImVec2(x,y)); ImGui::InvisibleButton("nt",ImVec2(tw,th));
        bool hov=ImGui::IsItemHovered(); if(ImGui::IsItemDeactivated()&&hov) sel=i;
        float ok=TileOn("nt", sel==i); ImGui::PopID();
        Primitive bg; bg.cx=x+tw*0.5f; bg.cy=y+th*0.5f; bg.hw=tw*0.5f; bg.hh=th*0.5f; bg.corner_radius=22; bg.fade=1; bg.material=Material::Thin; bg.elevate=hov?1.06f:1.0f; g->Submit(bg);
        if(ok>0.01f){ Primitive o=bg; o.fade=ok*0.55f; o.material=Material::Accent; g->Submit(o); }
        dl->AddRectFilled(ImVec2(x+14,y+16),ImVec2(x+17,y+th-16), IM_COL32(255,214,90,255), 2.0f);
        dl->AddText(f,15.0f, ImVec2(x+26,y+16), kInk, titles[i][0]?titles[i]:"Untitled");
        dl->AddText(f,12.0f, ImVec2(x+26,y+42), kInkSoft, bodies[i], NULL, tw-40); }
    int rows=(n+cols-1)/cols; ImGui::Dummy(ImVec2(w, rows*(th+gap)+4));
}
static void Draw_Podcasts(float w){
    ImDrawList* dl=ImGui::GetWindowDrawList(); ImFont* f=ImGui::GetFont();
    float cw=w<560.0f?w:560.0f; float x0=ImGui::GetCursorScreenPos().x+(w-cw)*0.5f;
    struct E{ const char* t; const char* s; const char* d; };
    static E es[]={ {"The Future of Work","The Daily Brief \xC2\xB7 Ep. 142","May 28"},{"A Conversation on Cities","The Daily Brief \xC2\xB7 Ep. 141","May 21"},{"Markets This Week","The Daily Brief \xC2\xB7 Ep. 140","May 14"},{"The Science of Sleep","The Daily Brief \xC2\xB7 Ep. 139","May 7"} };
    static int sel=0; if(sel<0||sel>3) sel=0;
    static bool playing=true; static float prog=0.4f;
    if(playing){ prog+=Dt()*0.02f; if(prog>1.0f){ prog=1.0f; playing=false; } }
    { float h=120.0f; ImVec2 tl(x0, ImGui::GetCursorScreenPos().y);
      Primitive bg; bg.cx=tl.x+cw*0.5f; bg.cy=tl.y+h*0.5f; bg.hw=cw*0.5f; bg.hh=h*0.5f; bg.corner_radius=26; bg.fade=1; bg.material=Material::Thin; g->Submit(bg);
      Primitive art; art.cx=tl.x+58; art.cy=tl.y+h*0.5f; art.hw=40; art.hh=40; art.corner_radius=18; art.fade=1; art.material=Material::Accent; g->Submit(art);
      DrawIcon(dl,Icon::Microphone,ImVec2(tl.x+58,tl.y+h*0.5f),32.0f,IM_COL32(255,255,255,255),2.2f);
      dl->AddText(f,12.0f,ImVec2(tl.x+114,tl.y+18),kInkSoft,playing?"NOW PLAYING":"PAUSED");
      dl->AddText(f,18.0f,ImVec2(tl.x+114,tl.y+36),kInk,es[sel].t);
      dl->AddText(f,13.0f,ImVec2(tl.x+114,tl.y+62),kInkSoft,es[sel].s);
      dl->AddRectFilled(ImVec2(tl.x+114,tl.y+92),ImVec2(tl.x+cw-56,tl.y+96),SA(kInkSoft,0.3f),2.0f);
      dl->AddRectFilled(ImVec2(tl.x+114,tl.y+92),ImVec2(tl.x+114+(cw-170)*prog,tl.y+96),Accent(),2.0f);
      float pbx=tl.x+cw-36, pby=tl.y+h*0.5f; bool phv=false;
      if(SurfaceHit("ppplay", ImVec2(pbx-20,pby-20), ImVec2(40,40), &phv, playing?Sfx::Pause:Sfx::Play)) playing=!playing;
      Primitive pb; pb.cx=pbx; pb.cy=pby; pb.hw=18; pb.hh=18; pb.corner_radius=22; pb.fade=1; pb.material=Material::Accent; pb.elevate=phv?1.12f:1.0f; g->Submit(pb);
      DrawIcon(dl, playing?Icon::Pause:Icon::Play, ImVec2(pbx,pby), 16.0f, IM_COL32(255,255,255,255), 2.2f);
      ImGui::Dummy(ImVec2(w, h+14)); }
    { dl->AddText(f,18.0f,ImVec2(x0+4,ImGui::GetCursorScreenPos().y),kInk,"Up Next"); ImGui::Dummy(ImVec2(w,28.0f));
      float rowh=66.0f, h=4*rowh; ImVec2 tl(x0, ImGui::GetCursorScreenPos().y);
      Primitive bg; bg.cx=tl.x+cw*0.5f; bg.cy=tl.y+h*0.5f; bg.hw=cw*0.5f; bg.hh=h*0.5f; bg.corner_radius=24; bg.fade=1; bg.material=Material::Thin; g->Submit(bg);
      for(int i=0;i<4;i++){ float ry=tl.y+i*rowh, cy=ry+rowh*0.5f; bool hov=false;
        if(SurfaceHit(es[i].t, ImVec2(tl.x, ry), ImVec2(cw, rowh), &hov)){ sel=i; prog=0.0f; playing=true; }
        RowWash(dl, ImVec2(tl.x+6,ry+3), ImVec2(tl.x+cw-6,ry+rowh-3), 12.0f, i==sel, hov);
        DrawIcon(dl,(i==sel&&playing)?Icon::Pause:Icon::Play,ImVec2(tl.x+28,cy),16.0f,Accent(),2.0f);
        dl->AddText(f,15.0f,ImVec2(tl.x+52,cy-16),kInk,es[i].t);
        dl->AddText(f,12.0f,ImVec2(tl.x+52,cy+3),kInkSoft,es[i].s);
        ImVec2 ds=f->CalcTextSizeA(12.0f,99999.0f,0,es[i].d); dl->AddText(f,12.0f,ImVec2(tl.x+cw-ds.x-16,cy-6),kInkSoft,es[i].d);
        if(i<3) dl->AddLine(ImVec2(tl.x+52,ry+rowh),ImVec2(tl.x+cw-14,ry+rowh),SA(kInkSoft,0.14f),1.0f); }
      ImGui::Dummy(ImVec2(w, h+12)); }
}
static void Draw_VoiceMemos(float w){
    ImDrawList* dl=ImGui::GetWindowDrawList(); ImFont* f=ImGui::GetFont();
    float cw=w<540.0f?w:540.0f; float x0=ImGui::GetCursorScreenPos().x+(w-cw)*0.5f;
    float t=(float)ImGui::GetTime();
    { float h=160.0f; ImVec2 tl(x0, ImGui::GetCursorScreenPos().y);
      Primitive bg; bg.cx=tl.x+cw*0.5f; bg.cy=tl.y+h*0.5f; bg.hw=cw*0.5f; bg.hh=h*0.5f; bg.corner_radius=28; bg.fade=1; bg.material=Material::Thin; g->Submit(bg);
      float cy=tl.y+62, midx=tl.x+20; int nb=(int)((cw-40)/5);
      for(int i=0;i<nb;i++){ float a=0.2f+0.8f*fabsf(sinf(i*0.45f + t*3.0f))*fabsf(sinf(i*0.13f)); float bh=4+a*40; dl->AddRectFilled(ImVec2(midx+i*5,cy-bh*0.5f),ImVec2(midx+i*5+3,cy+bh*0.5f),SA(Accent(),0.9f),1.5f); }
      char tb[16]; snprintf(tb,16,"%d:%02d.%d",(int)t/60,(int)t%60,((int)(t*10))%10); ImVec2 ts=f->CalcTextSizeA(28.0f,99999.0f,0,tb);
      dl->AddText(f,28.0f,ImVec2(tl.x+cw*0.5f-ts.x*0.5f,tl.y+96),kInk,tb);
      dl->AddCircleFilled(ImVec2(tl.x+cw*0.5f,tl.y+h-26), 16.0f, IM_COL32(255,69,58,255));
      dl->AddCircle(ImVec2(tl.x+cw*0.5f,tl.y+h-26), 22.0f, SA(kInk,0.5f), 0, 2.0f);
      ImGui::Dummy(ImVec2(w, h+14)); }
    { struct M{ const char* t; const char* d; const char* dur; }; M ms[]={ {"New Recording","Today, 9:41 AM","0:42"},{"Design standup","Yesterday","12:08"},{"Song idea","Tuesday","1:30"},{"Meeting notes","May 24","24:51"} };
      float rowh=58.0f, h=4*rowh; ImVec2 tl(x0, ImGui::GetCursorScreenPos().y);
      Primitive bg; bg.cx=tl.x+cw*0.5f; bg.cy=tl.y+h*0.5f; bg.hw=cw*0.5f; bg.hh=h*0.5f; bg.corner_radius=24; bg.fade=1; bg.material=Material::Thin; g->Submit(bg);
      for(int i=0;i<4;i++){ float ry=tl.y+i*rowh, cy=ry+rowh*0.5f;
        dl->AddText(f,16.0f,ImVec2(tl.x+18,cy-16),kInk,ms[i].t);
        dl->AddText(f,12.0f,ImVec2(tl.x+18,cy+3),kInkSoft,ms[i].d);
        ImVec2 us=f->CalcTextSizeA(13.0f,99999.0f,0,ms[i].dur); dl->AddText(f,13.0f,ImVec2(tl.x+cw-us.x-18,cy-6),kInkSoft,ms[i].dur);
        if(i<3) dl->AddLine(ImVec2(tl.x+18,ry+rowh),ImVec2(tl.x+cw-14,ry+rowh),SA(kInkSoft,0.14f),1.0f); }
      ImGui::Dummy(ImVec2(w, h+12)); }
}
static bool KKey(const char* id, float x, float y, float kw, float kh, const char* lbl, Icon ic, bool accent, float fz){
    ImGui::PushID(id); ImGui::SetCursorScreenPos(ImVec2(x,y)); ImGui::InvisibleButton("k",ImVec2(kw,kh));
    bool hov=ImGui::IsItemHovered(), clk=ImGui::IsItemDeactivated()&&hov, act=ImGui::IsItemActive();
    Spring& sp=g->springs().Get((uint32_t)ImGui::GetID("k"),1,SpringStyle::Bouncy,1.0f);
    sp.target=act?0.88f:(hov?1.05f:1.0f); sp.Tick(Dt()); float sc=Clampf(sp.x,0.8f,1.2f);
    Primitive b; b.cx=x+kw*0.5f; b.cy=y+kh*0.5f; b.hw=kw*0.5f*sc; b.hh=kh*0.5f*sc; b.corner_radius=7.0f*sc; b.fade=1;
    b.material=(accent||act)?Material::Accent:(hov?Material::Knob:Material::Thin); b.elevate=hov?1.1f:1.0f; g->Submit(b);
    ImDrawList* dl=ImGui::GetWindowDrawList(); ImU32 col=(accent||act)?IM_COL32(255,255,255,255):kInk;
    if(ic!=Icon::None) DrawIcon(dl,ic,ImVec2(x+kw*0.5f,y+kh*0.5f),fz*sc,col,2.0f);
    else { ImVec2 ts=ImGui::GetFont()->CalcTextSizeA(fz*sc,99999.0f,0,lbl); dl->AddText(ImGui::GetFont(),fz*sc,ImVec2(x+kw*0.5f-ts.x*0.5f,y+kh*0.5f-ts.y*0.5f),col,lbl); }
    ImGui::PopID(); return clk;
}
static void Draw_Keyboard(float w){
    ImDrawList* dl=ImGui::GetWindowDrawList(); ImFont* f=ImGui::GetFont();
    static char buf[256]="Glass feels alive"; static int len=17; static bool shift=false, nums=false;
    auto app=[&](char c){ if(len<255){ buf[len++]=c; buf[len]=0; } };
    float cw=w<600.0f?w:600.0f; float x0=ImGui::GetCursorScreenPos().x+(w-cw)*0.5f;
    { float h=120.0f; ImVec2 tl(x0, ImGui::GetCursorScreenPos().y);
      Primitive bg; bg.cx=tl.x+cw*0.5f; bg.cy=tl.y+h*0.5f; bg.hw=cw*0.5f; bg.hh=h*0.5f; bg.corner_radius=24; bg.fade=1; bg.material=Material::Thin; g->Submit(bg);
      dl->PushClipRect(ImVec2(tl.x+14,tl.y+10),ImVec2(tl.x+cw-14,tl.y+h-10),true);
      dl->AddText(f,20.0f, ImVec2(tl.x+18, tl.y+16), kInk, buf, NULL, cw-36);
      dl->PopClipRect();
      ImGui::Dummy(ImVec2(w, h+14)); }
    float gap=6.0f, kw=(cw-9*gap)/10.0f, kh=46.0f, rg=kh+9.0f;
    float top=ImGui::GetCursorScreenPos().y;
    const char* r1 = nums? "1234567890" : "qwertyuiop";
    const char* r2 = nums? "-/:;()$&@\"" : "asdfghjkl";
    const char* r3 = nums? ".,?!'" : "zxcvbnm";
    for(int i=0;r1[i];i++){ char id[4]={'a',(char)('0'+i),0,0}; char lab[2]={ (shift&&!nums)?(char)(r1[i]-32):r1[i], 0};
        if(KKey(id, x0+i*(kw+gap), top, kw, kh, lab, Icon::None, false, 18.0f)) app(lab[0]); }
    { int n=(int)strlen(r2); float rw=n*kw+(n-1)*gap, rx=x0+(cw-rw)*0.5f, ry=top+rg;
      for(int i=0;i<n;i++){ char id[4]={'b',(char)('0'+i),0,0}; char lab[2]={ (shift&&!nums)?(char)(r2[i]-32):r2[i], 0};
        bool isL = (r2[i]>='a'&&r2[i]<='z');
        if(KKey(id, rx+i*(kw+gap), ry, kw, kh, lab, Icon::None, false, 18.0f)) app(lab[0]); (void)isL; } }
    { int n=(int)strlen(r3); float wkey=kw*1.4f; float rw=wkey*2+gap*2+n*kw+(n-1)*gap; float rx=x0+(cw-rw)*0.5f, ry=top+rg*2;
      if(KKey("shift", rx, ry, wkey, kh, nums?"#+=":"", nums?Icon::None:Icon::ArrowUp, shift&&!nums, 17.0f)){ if(!nums) shift=!shift; }
      float lx=rx+wkey+gap;
      for(int i=0;i<n;i++){ char id[4]={'c',(char)('0'+i),0,0}; char lab[2]={ (shift&&!nums)?(char)(r3[i]-32):r3[i], 0};
        if(KKey(id, lx+i*(kw+gap), ry, kw, kh, lab, Icon::None, false, 18.0f)) app(lab[0]); }
      if(KKey("del", lx+n*(kw+gap)+gap- gap, ry, wkey, kh, "", Icon::ChevronL, false, 16.0f)){ if(len>0) buf[--len]=0; } }
    { float ry=top+rg*3; float wkey=kw*1.5f;
      if(KKey("mode", x0, ry, wkey, kh, nums?"ABC":"123", Icon::None, false, 15.0f)) nums=!nums;
      float spx=x0+wkey+gap, spw=cw-2*(wkey+gap);
      if(KKey("space", spx, ry, spw, kh, "space", Icon::None, false, 16.0f)) app(' ');
      if(KKey("ret", x0+cw-wkey, ry, wkey, kh, "return", Icon::None, true, 14.0f)) app(' '); }
    ImGui::SetCursorScreenPos(ImVec2(x0, top+rg*4)); ImGui::Dummy(ImVec2(w, 8.0f));
}
