static void Draw_About(float w){
    ImDrawList* dl=ImGui::GetWindowDrawList(); ImFont* f=ImGui::GetFont();
    float cw=w<560.0f?w:560.0f; float x0=ImGui::GetCursorScreenPos().x+(w-cw)*0.5f;
    dl->AddText(f,30.0f,ImVec2(x0+2,ImGui::GetCursorScreenPos().y),kInk,"About"); ImGui::Dummy(ImVec2(w,50.0f));
    struct Kv{ const char* k; const char* v; }; Kv kv[]={ {"Name","poncipp's Glass"},{"Model","Glass Pro 16"},{"OS Version","Liquid 1.0 (55A)"},{"Serial","GLAS2026XYZ"},{"Capacity","512 GB"},{"Available","196 GB"},{"Wi-Fi Address","9C:3G:LA:55:00:01"},{"Bluetooth","9C:3G:LA:55:00:02"},{"Created by","poncipp (@pondot)"},{"GitHub","github.com/poncippg-spec"} };
    int n=10; float rowh=48.0f, h=n*rowh; ImVec2 tl(x0, ImGui::GetCursorScreenPos().y);
    Primitive bg; bg.cx=tl.x+cw*0.5f; bg.cy=tl.y+h*0.5f; bg.hw=cw*0.5f; bg.hh=h*0.5f; bg.corner_radius=24; bg.fade=1; bg.material=Material::Thin; g->Submit(bg);
    for(int i=0;i<n;i++){ float ry=tl.y+i*rowh, cy=ry+rowh*0.5f;
        dl->AddText(f,16.0f,ImVec2(tl.x+18,cy-9),kInk,kv[i].k); ImVec2 vs=f->CalcTextSizeA(16.0f,99999.0f,0,kv[i].v); dl->AddText(f,16.0f,ImVec2(tl.x+cw-vs.x-18,cy-9),kInkSoft,kv[i].v);
        if(i<n-1) dl->AddLine(ImVec2(tl.x+18,ry+rowh),ImVec2(tl.x+cw-14,ry+rowh),SA(kInkSoft,0.14f),1.0f); }
    ImGui::Dummy(ImVec2(w, h+12));
}
static void Draw_Accessibility(float w){
    float cw=w<560.0f?w:560.0f; float x0=ImGui::GetCursorScreenPos().x+(w-cw)*0.5f;
    ImDrawList* dl=ImGui::GetWindowDrawList(); ImFont* f=ImGui::GetFont();
    dl->AddText(f,30.0f,ImVec2(x0+2,ImGui::GetCursorScreenPos().y),kInk,"Accessibility"); ImGui::Dummy(ImVec2(w,50.0f));
    struct R{ Icon ic; const char* l; const char* v; int kind; bool* t; };
    static bool vo=false, zoom=false, bold=true, reduce=false, contrast=false;
    R s1[]={ {Icon::Eye,"VoiceOver",nullptr,1,&vo},{Icon::Search,"Zoom",nullptr,1,&zoom},{Icon::Display,"Display & Text Size",nullptr,2,nullptr},{Icon::Font,"Bold Text",nullptr,1,&bold} };
    R s2[]={ {Icon::Bolt,"Reduce Motion",nullptr,1,&reduce},{Icon::Palette,"Increase Contrast",nullptr,1,&contrast},{Icon::Speaker,"Audio & Visual",nullptr,2,nullptr} };
    auto section=[&](R* rows,int n){ float rowh=48.0f,h=n*rowh; ImVec2 tl(x0, ImGui::GetCursorScreenPos().y);
        Primitive bg; bg.cx=tl.x+cw*0.5f; bg.cy=tl.y+h*0.5f; bg.hw=cw*0.5f; bg.hh=h*0.5f; bg.corner_radius=24; bg.fade=1; bg.material=Material::Thin; g->Submit(bg);
        for(int i=0;i<n;i++){ SettRow(tl.x,cw,tl.y+i*rowh,rowh,rows[i].ic,rows[i].l,rows[i].v,rows[i].kind,rows[i].t); if(i<n-1) dl->AddLine(ImVec2(tl.x+46,tl.y+(i+1)*rowh),ImVec2(tl.x+cw-12,tl.y+(i+1)*rowh),SA(kInkSoft,0.15f),1.0f); }
        ImGui::Dummy(ImVec2(w, h+16)); };
    section(s1,4); section(s2,3);
}
static void Draw_Transit(float w){
    ImDrawList* dl=ImGui::GetWindowDrawList(); ImFont* f=ImGui::GetFont();
    float cw=w<640.0f?w:640.0f; float x0=ImGui::GetCursorScreenPos().x+(w-cw)*0.5f;
    { float h=280.0f; ImVec2 tl(x0, ImGui::GetCursorScreenPos().y);
      Primitive bg; bg.cx=tl.x+cw*0.5f; bg.cy=tl.y+h*0.5f; bg.hw=cw*0.5f; bg.hh=h*0.5f; bg.corner_radius=28; bg.fade=1; bg.material=Material::Thin; g->Submit(bg);
      dl->PushClipRect(ImVec2(tl.x,tl.y),ImVec2(tl.x+cw,tl.y+h),true);
      ImU32 lc[3]={IM_COL32(255,69,58,255),IM_COL32(64,156,255,255),IM_COL32(48,209,88,255)};
      ImVec2 L1[4]={{tl.x+40,tl.y+60},{tl.x+200,tl.y+60},{tl.x+200,tl.y+220},{tl.x+cw-60,tl.y+220}};
      ImVec2 L2[4]={{tl.x+80,tl.y+30},{tl.x+80,tl.y+180},{tl.x+340,tl.y+180},{tl.x+340,tl.y+40}};
      ImVec2 L3[3]={{tl.x+30,tl.y+140},{tl.x+cw-100,tl.y+140},{tl.x+cw-100,tl.y+h-30}};
      for(int i=0;i<3;i++) dl->AddLine(L1[i],L1[i+1],lc[0],6.0f);
      for(int i=0;i<3;i++) dl->AddLine(L2[i],L2[i+1],lc[1],6.0f);
      for(int i=0;i<2;i++) dl->AddLine(L3[i],L3[i+1],lc[2],6.0f);
      ImVec2 st[6]={{tl.x+200,tl.y+60},{tl.x+200,tl.y+220},{tl.x+80,tl.y+180},{tl.x+340,tl.y+180},{tl.x+cw-100,tl.y+140},{tl.x+340,tl.y+40}};
      for(int i=0;i<6;i++){ dl->AddCircleFilled(st[i],6.0f,IM_COL32(255,255,255,255)); dl->AddCircle(st[i],6.0f,kInkSoft,0,1.5f); }
      dl->PopClipRect(); ImGui::Dummy(ImVec2(w, h+14)); }
    { float h=80.0f; ImVec2 tl(x0, ImGui::GetCursorScreenPos().y);
      Primitive bg; bg.cx=tl.x+cw*0.5f; bg.cy=tl.y+h*0.5f; bg.hw=cw*0.5f; bg.hh=h*0.5f; bg.corner_radius=24; bg.fade=1; bg.material=Material::Thin; g->Submit(bg);
      dl->AddText(f,22.0f,ImVec2(tl.x+20,tl.y+16),kInk,"24 min");
      dl->AddText(f,14.0f,ImVec2(tl.x+20,tl.y+48),kInkSoft,"Red Line -> Blue Line -> 5 min walk");
      Primitive go; go.cx=tl.x+cw-50; go.cy=tl.y+h*0.5f; go.hw=30; go.hh=18; go.corner_radius=24; go.fade=1; go.material=Material::Accent; g->Submit(go);
      DrawIcon(dl,Icon::LocationArrow,ImVec2(tl.x+cw-50,tl.y+h*0.5f),16.0f,IM_COL32(255,255,255,255),2.2f);
      ImGui::Dummy(ImVec2(w, h+12)); }
}
static void Draw_Siri(float w){
    ImDrawList* dl=ImGui::GetWindowDrawList(); ImFont* f=ImGui::GetFont(); float t=(float)ImGui::GetTime();
    float cw=w<560.0f?w:560.0f; float x0=ImGui::GetCursorScreenPos().x+(w-cw)*0.5f;
    ImVec2 c(x0+cw*0.5f, ImGui::GetCursorScreenPos().y+140.0f);
    ImU32 cols[4]={IM_COL32(64,156,255,255),IM_COL32(146,82,232,255),IM_COL32(255,55,95,255),IM_COL32(48,209,88,255)};
    for(int i=0;i<4;i++){ float a=t*(0.6f+i*0.2f)+i*1.57f; float rx=70.0f+sinf(a)*16.0f, ry=70.0f+cosf(a*1.1f)*16.0f; ImVec2 oc(c.x+cosf(a)*10.0f,c.y+sinf(a)*10.0f);
        dl->AddCircleFilled(oc, (rx+ry)*0.5f, SA(cols[i],0.30f), 48); }
    dl->AddCircleFilled(c, 50.0f, SA(IM_COL32(255,255,255,255),0.12f), 48);
    ImVec2 ts=f->CalcTextSizeA(20.0f,99999.0f,0,"Go ahead, I'm listening"); dl->AddText(f,20.0f,ImVec2(c.x-ts.x*0.5f,c.y+98),kInk,"Go ahead, I'm listening");
    ImGui::Dummy(ImVec2(w, 280.0f));
    { const char* sg[3]={"Set a timer","Play music","What's the weather?"};
      for(int i=0;i<3;i++){ float h=44.0f; ImVec2 tl(x0+cw*0.1f, ImGui::GetCursorScreenPos().y); float ww=cw*0.8f;
        Primitive bg; bg.cx=tl.x+ww*0.5f; bg.cy=tl.y+h*0.5f; bg.hw=ww*0.5f; bg.hh=h*0.5f; bg.corner_radius=h*0.5f; bg.fade=1; bg.material=Material::Thin; g->Submit(bg);
        ImVec2 ss=f->CalcTextSizeA(15.0f,99999.0f,0,sg[i]); dl->AddText(f,15.0f,ImVec2(tl.x+ww*0.5f-ss.x*0.5f,tl.y+h*0.5f-9),kInk,sg[i]); ImGui::Dummy(ImVec2(w,h+10)); } }
}
static void Draw_GlassPay(float w){
    ImDrawList* dl=ImGui::GetWindowDrawList(); ImFont* f=ImGui::GetFont(); float t=(float)ImGui::GetTime();
    float cw=w<460.0f?w:460.0f; float x0=ImGui::GetCursorScreenPos().x+(w-cw)*0.5f;
    { float h=150.0f; ImVec2 tl(x0, ImGui::GetCursorScreenPos().y);
      RoundedGrad(dl, ImVec2(tl.x,tl.y),ImVec2(tl.x+cw,tl.y+h), IM_COL32(40,40,55,255),IM_COL32(90,80,150,255),IM_COL32(150,90,120,255),IM_COL32(50,50,70,255), 18.0f);
      dl->AddRect(ImVec2(tl.x,tl.y),ImVec2(tl.x+cw,tl.y+h),SA(IM_COL32(255,255,255,255),0.18f),18.0f,0,1.5f);
      DrawIcon(dl,Icon::Spark,ImVec2(tl.x+30,tl.y+28),18.0f,IM_COL32(255,255,255,255),2.2f); dl->AddText(f,16.0f,ImVec2(tl.x+52,tl.y+19),IM_COL32(255,255,255,255),"Pay");
      dl->AddRectFilled(ImVec2(tl.x+24,tl.y+h-58),ImVec2(tl.x+58,tl.y+h-34),SA(IM_COL32(255,215,120,255),0.9f),5.0f);
      dl->AddText(f,17.0f,ImVec2(tl.x+24,tl.y+h-26),IM_COL32(255,255,255,255),"\xE2\x80\xA2\xE2\x80\xA2\xE2\x80\xA2\xE2\x80\xA2 2847");
      ImGui::Dummy(ImVec2(w, h+18)); }
    { dl->AddText(f,14.0f,ImVec2(x0+4,ImGui::GetCursorScreenPos().y),kInkSoft,"PAY \xC2\xB7 BLUE BOTTLE COFFEE");
      dl->AddText(f,40.0f,ImVec2(x0+2,ImGui::GetCursorScreenPos().y+18),kInk,"$129.00"); ImGui::Dummy(ImVec2(w,76.0f)); }
    { ImVec2 c(x0+cw*0.5f, ImGui::GetCursorScreenPos().y+60.0f); float prog=fmodf(t*0.5f,1.0f);
      dl->AddCircle(c,40.0f,SA(kInkSoft,0.25f),48,3.0f); dl->PathArcTo(c,40.0f,-1.5708f,-1.5708f+6.2831853f*prog,48); dl->PathStroke(Accent(),0,3.0f);
      DrawIcon(dl,Icon::Person,c,30.0f,kInk,2.2f);
      ImVec2 ts=f->CalcTextSizeA(16.0f,99999.0f,0,"Double-Click to Pay"); dl->AddText(f,16.0f,ImVec2(c.x-ts.x*0.5f,c.y+56),kInk,"Double-Click to Pay");
      ImGui::Dummy(ImVec2(w, 150.0f)); }
}
static void Draw_Emoji(float w){
    ImDrawList* dl=ImGui::GetWindowDrawList();
    float cw=w<600.0f?w:600.0f; float x0=ImGui::GetCursorScreenPos().x+(w-cw)*0.5f;
    { Icon ic[6]={Icon::Clock,Icon::Heart,Icon::Star,Icon::Leaf,Icon::MugHot,Icon::Flag};
      float h=40.0f; ImVec2 tl(x0, ImGui::GetCursorScreenPos().y);
      Primitive bg; bg.cx=tl.x+cw*0.5f; bg.cy=tl.y+h*0.5f; bg.hw=cw*0.5f; bg.hh=h*0.5f; bg.corner_radius=12; bg.fade=1; bg.material=Material::Thin; g->Submit(bg);
      for(int i=0;i<6;i++) DrawIcon(dl,ic[i],ImVec2(tl.x+cw*(i+0.5f)/6.0f,tl.y+h*0.5f),16.0f,i==0?Accent():kInkSoft,2.0f);
      ImGui::Dummy(ImVec2(w, h+12)); }
    { int n=40, cols=8; float gap=10, cellw=(cw-(cols-1)*gap)/cols; float r=cellw*0.42f; float sy=ImGui::GetCursorScreenPos().y;
      for(int i=0;i<n;i++){ int rr=i/cols,c=i%cols; ImVec2 fc(x0+c*(cellw+gap)+cellw*0.5f, sy+rr*(cellw+gap)+cellw*0.5f);
        dl->AddCircleFilled(fc,r,IM_COL32(255,205,70,255),32); dl->AddCircle(fc,r,SA(IM_COL32(0,0,0,255),0.15f),32,1.0f);
        int kind=i%5; ImU32 ink=IM_COL32(60,45,10,255);
        if(kind==2){ dl->AddLine(ImVec2(fc.x-r*0.4f,fc.y-r*0.2f),ImVec2(fc.x-r*0.15f,fc.y-r*0.3f),ink,2.0f); dl->AddLine(ImVec2(fc.x+r*0.15f,fc.y-r*0.3f),ImVec2(fc.x+r*0.4f,fc.y-r*0.2f),ink,2.0f); }
        else { dl->AddCircleFilled(ImVec2(fc.x-r*0.32f,fc.y-r*0.18f),r*0.1f,ink); dl->AddCircleFilled(ImVec2(fc.x+r*0.32f,fc.y-r*0.18f),r*0.1f,ink); }
        if(kind==1){ dl->PathArcTo(ImVec2(fc.x,fc.y-r*0.05f),r*0.45f,0.4f,2.74f,16); dl->PathStroke(ink,0,2.2f); }
        if(kind==0||kind==4){ dl->PathArcTo(ImVec2(fc.x,fc.y+r*0.05f),r*0.45f,0.4f,2.74f,16); dl->PathStroke(ink,0,2.2f); }
        else if(kind==3){ dl->AddCircleFilled(ImVec2(fc.x,fc.y+r*0.25f),r*0.16f,ink); }
        else if(kind==2){ dl->PathArcTo(ImVec2(fc.x,fc.y+r*0.4f),r*0.45f,3.54f,5.88f,16); dl->PathStroke(ink,0,2.2f); }
        else { dl->AddLine(ImVec2(fc.x-r*0.25f,fc.y+r*0.28f),ImVec2(fc.x+r*0.25f,fc.y+r*0.28f),ink,2.2f); }
      }
      int rows=(n+cols-1)/cols; ImGui::Dummy(ImVec2(w, rows*(cellw+gap)+4)); }
}
static void Draw_Remote(float w){
    ImDrawList* dl=ImGui::GetWindowDrawList();
    float cw=w<360.0f?w:360.0f; float x0=ImGui::GetCursorScreenPos().x+(w-cw)*0.5f;
    float h=520.0f; ImVec2 tl(x0, ImGui::GetCursorScreenPos().y);
    Primitive body; body.cx=tl.x+cw*0.5f; body.cy=tl.y+h*0.5f; body.hw=cw*0.34f; body.hh=h*0.5f; body.corner_radius=cw*0.18f; body.fade=1; body.material=Material::Thin; g->Submit(body);
    float bx=tl.x+cw*0.5f;
    ImVec2 tc(bx, tl.y+120.0f); float tr=cw*0.24f;
    dl->AddCircleFilled(tc,tr,SA(kInk,0.06f),48); dl->AddCircle(tc,tr,SA(kInkSoft,0.4f),48,2.0f);
    DrawIcon(dl,Icon::ChevronU,ImVec2(tc.x,tc.y-tr*0.62f),14.0f,kInkSoft,2.0f);
    DrawIcon(dl,Icon::ChevronD,ImVec2(tc.x,tc.y+tr*0.62f),14.0f,kInkSoft,2.0f);
    DrawIcon(dl,Icon::ChevronL,ImVec2(tc.x-tr*0.62f,tc.y),14.0f,kInkSoft,2.0f);
    DrawIcon(dl,Icon::ChevronR,ImVec2(tc.x+tr*0.62f,tc.y),14.0f,kInkSoft,2.0f);
    auto rb=[&](float cx,float cy,Icon ic,bool accent){ Primitive b; b.cx=cx;b.cy=cy;b.hw=34;b.hh=22;b.corner_radius=24;b.fade=1;b.material=accent?Material::Accent:Material::Thin; g->Submit(b); DrawIcon(dl,ic,ImVec2(cx,cy),16.0f,accent?IM_COL32(255,255,255,255):kInk,2.0f); };
    float ry=tl.y+260; rb(bx-cw*0.16f,ry,Icon::ChevronL,false); rb(bx+cw*0.16f,ry,Icon::Tv,false);
    ry+=64; rb(bx-cw*0.16f,ry,Icon::Microphone,true); rb(bx+cw*0.16f,ry,Icon::Play,false);
    ry+=72; Primitive v; v.cx=bx; v.cy=ry+24; v.hw=28; v.hh=54; v.corner_radius=29; v.fade=1; v.material=Material::Thin; g->Submit(v);
    DrawIcon(dl,Icon::Plus,ImVec2(bx,ry+4),16.0f,kInk,2.0f); DrawIcon(dl,Icon::Minus,ImVec2(bx,ry+44),16.0f,kInk,2.0f);
    ImGui::Dummy(ImVec2(w, h+12));
}
static void Draw_SoftwareUpdate(float w){
    ImDrawList* dl=ImGui::GetWindowDrawList(); ImFont* f=ImGui::GetFont(); float t=(float)ImGui::GetTime();
    float cw=w<560.0f?w:560.0f; float x0=ImGui::GetCursorScreenPos().x+(w-cw)*0.5f;
    { float h=160.0f; ImVec2 tl(x0, ImGui::GetCursorScreenPos().y); ImVec2 c(tl.x+80,tl.y+h*0.5f);
      Primitive bg; bg.cx=tl.x+cw*0.5f; bg.cy=tl.y+h*0.5f; bg.hw=cw*0.5f; bg.hh=h*0.5f; bg.corner_radius=28; bg.fade=1; bg.material=Material::Thin; g->Submit(bg);
      float prog=0.4f+0.3f*sinf(t*0.5f)*0.0f+0.42f;
      prog=0.62f; dl->AddCircle(c,44.0f,SA(kInkSoft,0.2f),48,7.0f); dl->PathArcTo(c,44.0f,-1.5708f,-1.5708f+6.2831853f*prog,48); dl->PathStroke(Accent(),0,7.0f);
      char pb[8]; snprintf(pb,8,"%d%%",(int)(prog*100)); ImVec2 ps=f->CalcTextSizeA(18.0f,99999.0f,0,pb); dl->AddText(f,18.0f,ImVec2(c.x-ps.x*0.5f,c.y-ps.y*0.5f),kInk,pb);
      dl->AddText(f,20.0f,ImVec2(tl.x+150,tl.y+38),kInk,"Liquid 1.1");
      dl->AddText(f,14.0f,ImVec2(tl.x+150,tl.y+66),kInkSoft,"Downloading\xE2\x80\xA6 2.1 GB of 3.4 GB");
      dl->AddText(f,13.0f,ImVec2(tl.x+150,tl.y+92),kInkSoft,"Prepared overnight, ready to install");
      ImGui::Dummy(ImVec2(w, h+14)); }
    { float h=180.0f; ImVec2 tl(x0, ImGui::GetCursorScreenPos().y);
      Primitive bg; bg.cx=tl.x+cw*0.5f; bg.cy=tl.y+h*0.5f; bg.hw=cw*0.5f; bg.hh=h*0.5f; bg.corner_radius=26; bg.fade=1; bg.material=Material::Thin; g->Submit(bg);
      dl->AddText(f,12.0f,ImVec2(tl.x+20,tl.y+14),kInkSoft,"WHAT'S NEW");
      const char* notes[4]={"A refreshed design across the system","Smarter, faster on-device search","Improvements to battery life and stability","New customization options for the Lock Screen"};
      for(int i=0;i<4;i++){ float ny=tl.y+40+i*32; dl->AddCircleFilled(ImVec2(tl.x+24,ny+8),3.0f,Accent()); dl->AddText(f,14.0f,ImVec2(tl.x+38,ny),kInk,notes[i],NULL,cw-56); }
      ImGui::Dummy(ImVec2(w, h+14)); }
    { ImGui::SetCursorScreenPos(ImVec2(x0+cw*0.5f-90, ImGui::GetCursorScreenPos().y)); Glass::Button("Install Tonight", true, ImVec2(180,44)); ImGui::Dummy(ImVec2(w,10.0f)); }
}
static void Draw_Dictionary(float w){
    ImDrawList* dl=ImGui::GetWindowDrawList(); ImFont* f=ImGui::GetFont();
    float cw=w<600.0f?w:600.0f; float x0=ImGui::GetCursorScreenPos().x+(w-cw)*0.5f;
    { float h=44.0f; ImVec2 tl(x0, ImGui::GetCursorScreenPos().y);
      Primitive bg; bg.cx=tl.x+cw*0.5f; bg.cy=tl.y+h*0.5f; bg.hw=cw*0.5f; bg.hh=h*0.5f; bg.corner_radius=h*0.5f; bg.fade=1; bg.material=Material::Thin; g->Submit(bg);
      DrawIcon(dl,Icon::Search,ImVec2(tl.x+24,tl.y+h*0.5f),14.0f,kInkSoft,2.0f); dl->AddText(f,15.0f,ImVec2(tl.x+44,tl.y+h*0.5f-9),kInk,"glass");
      ImGui::Dummy(ImVec2(w, h+14)); }
    { float h=300.0f; ImVec2 tl(x0, ImGui::GetCursorScreenPos().y);
      Primitive bg; bg.cx=tl.x+cw*0.5f; bg.cy=tl.y+h*0.5f; bg.hw=cw*0.5f; bg.hh=h*0.5f; bg.corner_radius=24; bg.fade=1; bg.material=Material::Thin; g->Submit(bg);
      dl->AddText(f,32.0f,ImVec2(tl.x+24,tl.y+20),kInk,"glass");
      dl->AddText(f,15.0f,ImVec2(tl.x+24,tl.y+60),kInkSoft,"| glas | noun");
      const char* defs[3]={"1. a hard, brittle, transparent substance, typically made by fusing sand with soda and lime.","2. a container made from glass, used for drinking.","3. (glasses) a pair of lenses set in a frame, worn to correct eyesight."};
      float dy=tl.y+96; for(int i=0;i<3;i++){ dl->AddText(f,15.0f,ImVec2(tl.x+24,dy),kInk,defs[i],NULL,cw-48); dy+=58; }
      dl->AddText(f,13.0f,ImVec2(tl.x+24,tl.y+h-30),Accent(),"ORIGIN  Old English glaes, of Germanic origin.");
      ImGui::Dummy(ImVec2(w, h+12)); }
}
static void Draw_Stopwatch(float w){
    ImDrawList* dl=ImGui::GetWindowDrawList(); ImFont* f=ImGui::GetFont();
    float cw=w<520.0f?w:520.0f; float x0=ImGui::GetCursorScreenPos().x+(w-cw)*0.5f;
    static bool running=false; static double elapsed=0.0, lapBase=0.0;
    static double laps[64]; static int nlaps=0;
    if (running) elapsed += (double)Dt();
    double el=elapsed; int m=(int)(el/60.0)%60, s=((int)el)%60, cs=(int)(el*100.0)%100;
    { ImVec2 c(x0+cw*0.5f, ImGui::GetCursorScreenPos().y+120.0f); float R=100.0f;
      dl->AddCircle(c,R,SA(kInkSoft,0.2f),64,5.0f);
      float frac=(float)(fmod(el,60.0)/60.0); dl->PathArcTo(c,R,-1.5708f,-1.5708f+6.2831853f*frac,64); dl->PathStroke(Accent(),0,5.0f);
      char tb[24]; snprintf(tb,24,"%02d:%02d.%02d",m,s,cs); ImVec2 ts=f->CalcTextSizeA(40.0f,99999.0f,0,tb); dl->AddText(f,40.0f,ImVec2(c.x-ts.x*0.5f,c.y-ts.y*0.5f),kInk,tb);
      ImGui::Dummy(ImVec2(w, 250.0f)); }
    { float by=ImGui::GetCursorScreenPos().y;
      ImGui::SetCursorScreenPos(ImVec2(x0+cw*0.5f-100, by));
      if (running){ if (Glass::Button("Lap", false, ImVec2(90,44)) && nlaps<64){ laps[nlaps++]=elapsed-lapBase; lapBase=elapsed; } }
      else        { if (Glass::Button("Reset", false, ImVec2(90,44))){ elapsed=0; lapBase=0; nlaps=0; } }
      ImGui::SetCursorScreenPos(ImVec2(x0+cw*0.5f+10, by));
      if (Glass::Button(running?"Stop":"Start", true, ImVec2(90,44))) running=!running;
      ImGui::Dummy(ImVec2(w,54.0f)); }
    { int show = nlaps<6?nlaps:6; float rowh=44.0f, h=(show>0?show:1)*rowh; ImVec2 tl(x0, ImGui::GetCursorScreenPos().y);
      Primitive bg; bg.cx=tl.x+cw*0.5f; bg.cy=tl.y+h*0.5f; bg.hw=cw*0.5f; bg.hh=h*0.5f; bg.corner_radius=22; bg.fade=1; bg.material=Material::Thin; g->Submit(bg);
      if (show==0){ const char* none="No laps yet"; ImVec2 ns=f->CalcTextSizeA(15.0f,99999.0f,0,none); dl->AddText(f,15.0f,ImVec2(tl.x+cw*0.5f-ns.x*0.5f, tl.y+rowh*0.5f-8.0f), kInkSoft, none); }
      for(int i=0;i<show;i++){ int li=nlaps-1-i; double lv=laps[li]; float ry=tl.y+i*rowh, cy=ry+rowh*0.5f;
        char lb[16]; snprintf(lb,16,"Lap %d",li+1); dl->AddText(f,15.0f,ImVec2(tl.x+18,cy-8),kInkSoft,lb);
        char vb[24]; snprintf(vb,24,"%02d:%05.2f",(int)(lv/60.0),fmod(lv,60.0)); ImVec2 vs=f->CalcTextSizeA(15.0f,99999.0f,0,vb); dl->AddText(f,15.0f,ImVec2(tl.x+cw-vs.x-18,cy-8),kInk,vb);
        if(i<show-1) dl->AddLine(ImVec2(tl.x+18,ry+rowh),ImVec2(tl.x+cw-14,ry+rowh),SA(kInkSoft,0.14f),1.0f); }
      ImGui::Dummy(ImVec2(w, h+12)); }
}
static void Draw_AirDrop(float w){
    ImDrawList* dl=ImGui::GetWindowDrawList(); ImFont* f=ImGui::GetFont(); float t=(float)ImGui::GetTime();
    float cw=w<560.0f?w:560.0f; float x0=ImGui::GetCursorScreenPos().x+(w-cw)*0.5f;
    dl->AddText(f,24.0f,ImVec2(x0+2,ImGui::GetCursorScreenPos().y),kInk,"Nearby Share"); dl->AddText(f,14.0f,ImVec2(x0+2,ImGui::GetCursorScreenPos().y+34),kInkSoft,"Share with people nearby"); ImGui::Dummy(ImVec2(w,64.0f));
    ImVec2 c(x0+cw*0.5f, ImGui::GetCursorScreenPos().y+170.0f);
    for(int i=0;i<3;i++){ float ph=fmodf(t*0.4f+i*0.33f,1.0f); dl->AddCircle(c,40.0f+ph*150.0f,SA(Accent(),(1.0f-ph)*0.35f),64,2.0f); }
    dl->AddCircleFilled(c,34.0f,SA(Accent(),0.9f),48); DrawIcon(dl,Icon::Person,c,28.0f,IM_COL32(255,255,255,255),2.2f);
    struct P{ float ang; const char* i; ImU32 col; }; P ps[4]={ {0.6f,"CF",IM_COL32(48,209,88,255)},{2.0f,"TC",IM_COL32(255,159,10,255)},{3.6f,"SK",IM_COL32(146,82,232,255)},{5.0f,"EC",IM_COL32(255,69,58,255)} };
    for(int i=0;i<4;i++){ ImVec2 pc(c.x+cosf(ps[i].ang)*150.0f, c.y+sinf(ps[i].ang)*150.0f); dl->AddCircleFilled(pc,24.0f,ps[i].col); ImVec2 is=f->CalcTextSizeA(14.0f,99999.0f,0,ps[i].i); dl->AddText(f,14.0f,ImVec2(pc.x-is.x*0.5f,pc.y-is.y*0.5f),IM_COL32(255,255,255,255),ps[i].i); }
    ImGui::Dummy(ImVec2(w, 360.0f));
}
static void Draw_Markup(float w){
    ImDrawList* dl=ImGui::GetWindowDrawList(); ImFont* f=ImGui::GetFont();
    float cw=w<640.0f?w:640.0f; float x0=ImGui::GetCursorScreenPos().x+(w-cw)*0.5f;
    float h=380.0f; ImVec2 tl(x0, ImGui::GetCursorScreenPos().y);
    Primitive bg; bg.cx=tl.x+cw*0.5f; bg.cy=tl.y+h*0.5f; bg.hw=cw*0.5f; bg.hh=h*0.5f; bg.corner_radius=24; bg.fade=1; bg.material=Material::Thin; g->Submit(bg);
    dl->PushClipRect(ImVec2(tl.x,tl.y),ImVec2(tl.x+cw,tl.y+h),true);
    dl->AddBezierCubic(ImVec2(tl.x+60,tl.y+120),ImVec2(tl.x+160,tl.y+40),ImVec2(tl.x+260,tl.y+200),ImVec2(tl.x+360,tl.y+90), IM_COL32(255,69,58,255), 4.0f);
    dl->AddBezierCubic(ImVec2(tl.x+80,tl.y+240),ImVec2(tl.x+200,tl.y+300),ImVec2(tl.x+300,tl.y+200),ImVec2(tl.x+cw-60,tl.y+260), IM_COL32(64,156,255,255), 5.0f);
    dl->AddCircle(ImVec2(tl.x+cw*0.5f,tl.y+h*0.5f),60.0f,IM_COL32(48,209,88,255),0,4.0f);
    dl->AddText(f,28.0f,ImVec2(tl.x+cw-220,tl.y+40),IM_COL32(255,214,90,255),"Glass!");
    dl->PopClipRect();
    float py=tl.y+h-2; ImU32 pens[5]={IM_COL32(20,20,24,255),IM_COL32(255,69,58,255),IM_COL32(64,156,255,255),IM_COL32(48,209,88,255),IM_COL32(255,214,90,255)};
    Primitive pal; pal.cx=tl.x+cw*0.5f; pal.cy=py+22; pal.hw=cw*0.42f; pal.hh=24; pal.corner_radius=28; pal.fade=1; pal.material=Material::Thick; g->Submit(pal);
    Icon tools[3]={Icon::PenEdit,Icon::Font,Icon::Crosshairs}; float bx=tl.x+cw*0.5f-cw*0.42f+24;
    for(int i=0;i<3;i++){ DrawIcon(dl,tools[i],ImVec2(bx+i*40,py+22),16.0f,i==0?Accent():kInk,2.0f); }
    for(int i=0;i<5;i++){ dl->AddCircleFilled(ImVec2(bx+130+i*36,py+22),10.0f,pens[i]); if(i==1) dl->AddCircle(ImVec2(bx+130+i*36,py+22),13.0f,kInk,0,2.0f); }
    ImGui::Dummy(ImVec2(w, h+60.0f));
}
static void Draw_Scanner(float w){
    ImDrawList* dl=ImGui::GetWindowDrawList(); float t=(float)ImGui::GetTime();
    float cw=w<520.0f?w:520.0f; float x0=ImGui::GetCursorScreenPos().x+(w-cw)*0.5f;
    float h=420.0f; ImVec2 tl(x0, ImGui::GetCursorScreenPos().y);
    dl->AddRectFilled(ImVec2(tl.x,tl.y),ImVec2(tl.x+cw,tl.y+h), IM_COL32(30,32,38,255), 20.0f);
    dl->PushClipRect(ImVec2(tl.x,tl.y),ImVec2(tl.x+cw,tl.y+h),true);
    ImVec2 q0(tl.x+70,tl.y+90),q1(tl.x+cw-50,tl.y+70),q2(tl.x+cw-70,tl.y+h-90),q3(tl.x+90,tl.y+h-70);
    dl->AddQuadFilled(q0,q1,q2,q3, SA(IM_COL32(255,255,255,255),0.9f));
    for(int i=0;i<10;i++){ float fy=tl.y+120+i*26; float lw=(i%3==2)?cw*0.3f:cw*0.5f; dl->AddRectFilled(ImVec2(tl.x+110,fy),ImVec2(tl.x+110+lw,fy+6),SA(IM_COL32(40,40,50,255),0.6f),2.0f); }
    ImU32 yel=IM_COL32(255,214,90,255); ImVec2 cn[4]={q0,q1,q2,q3};
    for(int i=0;i<4;i++){ dl->AddCircleFilled(cn[i],6.0f,yel); }
    dl->AddQuad(q0,q1,q2,q3,yel,2.5f);
    float sy=tl.y+90+fmodf(t*120.0f,h-180.0f); dl->AddLine(ImVec2(tl.x+70,sy),ImVec2(tl.x+cw-50,sy),SA(yel,0.7f),2.0f);
    dl->PopClipRect();
    dl->AddCircleFilled(ImVec2(tl.x+cw*0.5f,tl.y+h-2),24.0f,IM_COL32(255,255,255,255)); dl->AddCircle(ImVec2(tl.x+cw*0.5f,tl.y+h-2),30.0f,IM_COL32(255,255,255,255),0,3.0f);
    ImGui::Dummy(ImVec2(w, h+34.0f));
}
static void Draw_Level(float w){
    ImDrawList* dl=ImGui::GetWindowDrawList(); ImFont* f=ImGui::GetFont(); float t=(float)ImGui::GetTime();
    float cw=w<520.0f?w:520.0f; float x0=ImGui::GetCursorScreenPos().x+(w-cw)*0.5f;
    float ang=sinf(t*0.4f)*8.0f; bool flat=fabsf(ang)<1.0f;
    ImVec2 c(x0+cw*0.5f, ImGui::GetCursorScreenPos().y+200.0f); float R=140.0f;
    ImU32 col = flat?IM_COL32(48,209,88,255):Accent();
    dl->AddCircleFilled(c,R,SA(col,0.12f),64); dl->AddCircle(c,R,SA(col,0.5f),64,3.0f);
    dl->AddCircle(c,40.0f,SA(kInkSoft,0.4f),48,2.0f);
    ImVec2 b(c.x+ang*9.0f, c.y); dl->AddCircleFilled(b,40.0f,SA(col,0.85f),48);
    char ab[12]; snprintf(ab,12,"%.0f\xC2\xB0",fabsf(ang)); ImVec2 ts=f->CalcTextSizeA(40.0f,99999.0f,0,ab); dl->AddText(f,40.0f,ImVec2(c.x-ts.x*0.5f,c.y-ts.y*0.5f),IM_COL32(255,255,255,255),ab);
    dl->AddLine(ImVec2(c.x-R-20,c.y),ImVec2(c.x-R+10,c.y),SA(kInkSoft,0.5f),2.0f); dl->AddLine(ImVec2(c.x+R-10,c.y),ImVec2(c.x+R+20,c.y),SA(kInkSoft,0.5f),2.0f);
    ImGui::Dummy(ImVec2(w, 420.0f));
}
static void Draw_Flashlight(float w){
    ImDrawList* dl=ImGui::GetWindowDrawList(); ImFont* f=ImGui::GetFont();
    static bool on=true; static int lvl=2;
    float cw=w<460.0f?w:460.0f; float x0=ImGui::GetCursorScreenPos().x+(w-cw)*0.5f;
    ImVec2 c(x0+cw*0.5f, ImGui::GetCursorScreenPos().y+170.0f);
    if(on){ for(int i=0;i<4;i++) dl->AddCircleFilled(c, 90.0f+i*22.0f, SA(IM_COL32(255,235,150,255),0.06f*(4-i)/4.0f*lvl/3.0f),48); }
    ImGui::SetCursorScreenPos(ImVec2(c.x-80,c.y-80)); ImGui::PushID("fl"); ImGui::InvisibleButton("b",ImVec2(160,160));
    bool flh=ImGui::IsItemHovered(), fla=ImGui::IsItemActive(); if(ImGui::IsItemDeactivated()&&flh) on=!on;
    Spring& flsp=g->springs().Get((uint32_t)ImGui::GetID("b"),1,SpringStyle::Bouncy,1.0f); flsp.target=fla?0.92f:(flh?1.04f:1.0f); flsp.Tick(Dt()); float fls=Clampf(flsp.x,0.8f,1.15f);
    float flok=TileOn("b",on); ImGui::PopID();
    float fr2=80.0f*fls;
    Primitive b; b.cx=c.x; b.cy=c.y; b.hw=fr2; b.hh=fr2; b.corner_radius=fr2; b.fade=1; b.material=Material::Thin; b.elevate=flh?1.12f:1.0f; g->Submit(b);
    if(flok>0.01f){ Primitive o=b; o.fade=flok; o.material=Material::Accent; g->Submit(o); }
    DrawIcon(dl,Icon::Bolt,c,70.0f*fls,flok>0.5f?IM_COL32(255,255,255,255):kInkSoft,2.6f);
    ImVec2 ts=f->CalcTextSizeA(16.0f,99999.0f,0, on?"On":"Off"); dl->AddText(f,16.0f,ImVec2(c.x-ts.x*0.5f,c.y+96),kInkSoft, on?"Tap to turn off":"Tap to turn on");
    ImGui::Dummy(ImVec2(w, 300.0f));
    { ImGui::SetCursorScreenPos(ImVec2(x0+cw*0.5f-120, ImGui::GetCursorScreenPos().y));
      for(int i=0;i<3;i++){ ImGui::PushID(i); ImGui::SetCursorScreenPos(ImVec2(x0+cw*0.5f-120+i*84, ImGui::GetCursorScreenPos().y)); if(Glass::Button(i==0?"Low":(i==1?"Med":"High"), lvl==i+1, ImVec2(76,40))) lvl=i+1; ImGui::PopID(); }
      ImGui::Dummy(ImVec2(w, 50.0f)); }
}
static void CoverGrid(float x0, float cw, const char* const* titles, const char* const* subs, const ImU32* a, const ImU32* b, int n, int cols){
    ImDrawList* dl=ImGui::GetWindowDrawList(); ImFont* f=ImGui::GetFont();
    float gap=14, cellw=(cw-(cols-1)*gap)/cols, lblH=40.0f; float sy=ImGui::GetCursorScreenPos().y;
    for(int i=0;i<n;i++){ int r=i/cols,c=i%cols; float x=x0+c*(cellw+gap), y=sy+r*(cellw+lblH+gap);
        ImgOrGrad(dl, ImVec2(x,y), ImVec2(x+cellw,y+cellw), i, a[i], b[i], 12.0f);
        dl->AddRect(ImVec2(x,y),ImVec2(x+cellw,y+cellw),SA(IM_COL32(255,255,255,255),0.12f),12.0f,0,1.0f);
        dl->AddText(f,14.0f,ImVec2(x+2,y+cellw+6),kInk,titles[i],NULL,cellw);
        if(subs) dl->AddText(f,12.0f,ImVec2(x+2,y+cellw+24),kInkSoft,subs[i],NULL,cellw); }
    int rows=(n+cols-1)/cols; ImGui::Dummy(ImVec2(x0+cw - x0, 0)); ImGui::Dummy(ImVec2(cw, rows*(cellw+lblH+gap)+4));
}
static void Draw_MusicLib(float w){
    ImDrawList* dl=ImGui::GetWindowDrawList(); ImFont* f=ImGui::GetFont();
    float cw=w<660.0f?w:660.0f; float x0=ImGui::GetCursorScreenPos().x+(w-cw)*0.5f;
    dl->AddText(f,30.0f,ImVec2(x0+2,ImGui::GetCursorScreenPos().y),kInk,"Library"); ImGui::Dummy(ImVec2(w,50.0f));
    const char* t[6]={"Afterglow","Golden Days","Open Road","Night Drive","Slow Mornings","Paper Planes"};
    const char* s[6]={"June Park","The Hours","Marigold","Atlas","Coastlines","The Wanderers"};
    ImU32 a[6]={IM_COL32(200,50,80,255),IM_COL32(40,90,160,255),IM_COL32(40,140,120,255),IM_COL32(150,90,40,255),IM_COL32(90,50,140,255),IM_COL32(60,110,150,255)};
    ImU32 b[6]={IM_COL32(255,110,130,255),IM_COL32(90,150,220,255),IM_COL32(90,210,180,255),IM_COL32(220,160,90,255),IM_COL32(160,110,210,255),IM_COL32(110,180,220,255)};
    ImGui::SetCursorScreenPos(ImVec2(x0, ImGui::GetCursorScreenPos().y)); CoverGrid(x0,cw,t,s,a,b,6,3);
}
static void Draw_ReminderLists(float w){
    ImDrawList* dl=ImGui::GetWindowDrawList(); ImFont* f=ImGui::GetFont();
    float cw=w<600.0f?w:600.0f; float x0=ImGui::GetCursorScreenPos().x+(w-cw)*0.5f;
    dl->AddText(f,30.0f,ImVec2(x0+2,ImGui::GetCursorScreenPos().y),kInk,"My Lists"); ImGui::Dummy(ImVec2(w,50.0f));
    struct L{ Icon ic; const char* n; int cnt; ImU32 c; }; L ls[]={ {Icon::List,"Today",4,IM_COL32(64,156,255,255)},{Icon::Bolt,"Work",9,IM_COL32(255,159,10,255)},{Icon::House,"Home",6,IM_COL32(146,82,232,255)},{Icon::Cart,"Groceries",12,IM_COL32(48,209,88,255)},{Icon::Heart,"Wishlist",3,IM_COL32(255,69,58,255)},{Icon::Rocket,"Someday",21,IM_COL32(120,120,128,255)} };
    int n=6, cols=2; float gap=14, tw=(cw-gap)/2, th=80; float sy=ImGui::GetCursorScreenPos().y;
    for(int i=0;i<n;i++){ int r=i/cols,c=i%cols; float x=x0+c*(tw+gap), y=sy+r*(th+gap);
        Primitive bg; bg.cx=x+tw*0.5f; bg.cy=y+th*0.5f; bg.hw=tw*0.5f; bg.hh=th*0.5f; bg.corner_radius=24; bg.fade=1; bg.material=Material::Thin; g->Submit(bg);
        dl->AddCircleFilled(ImVec2(x+30,y+28),16.0f,ls[i].c); DrawIcon(dl,ls[i].ic,ImVec2(x+30,y+28),14.0f,IM_COL32(255,255,255,255),2.0f);
        char cb[8]; snprintf(cb,8,"%d",ls[i].cnt); ImVec2 cs=f->CalcTextSizeA(22.0f,99999.0f,0,cb); dl->AddText(f,22.0f,ImVec2(x+tw-cs.x-18,y+16),kInk,cb);
        dl->AddText(f,16.0f,ImVec2(x+16,y+50),kInk,ls[i].n); }
    int rows=(n+cols-1)/cols; ImGui::Dummy(ImVec2(w, rows*(th+gap)+4));
}
static void Draw_StepsDetail(float w){
    ImDrawList* dl=ImGui::GetWindowDrawList(); ImFont* f=ImGui::GetFont();
    float cw=w<620.0f?w:620.0f; float x0=ImGui::GetCursorScreenPos().x+(w-cw)*0.5f;
    { float h=240.0f; ImVec2 tl(x0, ImGui::GetCursorScreenPos().y);
      Primitive bg; bg.cx=tl.x+cw*0.5f; bg.cy=tl.y+h*0.5f; bg.hw=cw*0.5f; bg.hh=h*0.5f; bg.corner_radius=29; bg.fade=1; bg.material=Material::Thin; g->Submit(bg);
      dl->AddText(f,13.0f,ImVec2(tl.x+22,tl.y+16),IM_COL32(255,159,10,255),"STEPS");
      dl->AddText(f,48.0f,ImVec2(tl.x+22,tl.y+32),kInk,"8,421");
      dl->AddText(f,14.0f,ImVec2(tl.x+22,tl.y+86),kInkSoft,"Today \xC2\xB7 goal 10,000");
      const char* d[7]={"M","T","W","T","F","S","S"}; float v[7]={0.6f,0.8f,0.5f,0.9f,0.7f,0.4f,0.84f};
      float gx=tl.x+22, gw=cw-44, by=tl.y+h-30, bh=90; float bw=gw/7.0f;
      for(int i=0;i<7;i++){ float cx=gx+bw*(i+0.5f); float hh=bh*v[i]; dl->AddRectFilled(ImVec2(cx-14,by-bh),ImVec2(cx+14,by),SA(kInkSoft,0.16f),5.0f); dl->AddRectFilled(ImVec2(cx-14,by-hh),ImVec2(cx+14,by),i==6?IM_COL32(255,159,10,255):SA(IM_COL32(255,159,10,255),0.6f),5.0f); ImVec2 ds=f->CalcTextSizeA(12.0f,99999.0f,0,d[i]); dl->AddText(f,12.0f,ImVec2(cx-ds.x*0.5f,by+6),kInkSoft,d[i]); }
      ImGui::Dummy(ImVec2(w, h+14)); }
    { struct Kv{ const char* k; const char* v; }; Kv kv[3]={ {"Distance","3.8 mi"},{"Flights climbed","12"},{"Avg this week","7,940 steps"} };
      float rowh=52.0f,h=3*rowh; ImVec2 tl(x0, ImGui::GetCursorScreenPos().y);
      Primitive bg; bg.cx=tl.x+cw*0.5f; bg.cy=tl.y+h*0.5f; bg.hw=cw*0.5f; bg.hh=h*0.5f; bg.corner_radius=24; bg.fade=1; bg.material=Material::Thin; g->Submit(bg);
      for(int i=0;i<3;i++){ float ry=tl.y+i*rowh,cy=ry+rowh*0.5f; dl->AddText(f,16.0f,ImVec2(tl.x+18,cy-9),kInk,kv[i].k); ImVec2 vs=f->CalcTextSizeA(16.0f,99999.0f,0,kv[i].v); dl->AddText(f,16.0f,ImVec2(tl.x+cw-vs.x-18,cy-9),kInkSoft,kv[i].v); if(i<2) dl->AddLine(ImVec2(tl.x+18,ry+rowh),ImVec2(tl.x+cw-14,ry+rowh),SA(kInkSoft,0.14f),1.0f); }
      ImGui::Dummy(ImVec2(w, h+12)); }
}
static void Draw_PodcastLib(float w){
    ImDrawList* dl=ImGui::GetWindowDrawList(); ImFont* f=ImGui::GetFont();
    float cw=w<660.0f?w:660.0f; float x0=ImGui::GetCursorScreenPos().x+(w-cw)*0.5f;
    dl->AddText(f,30.0f,ImVec2(x0+2,ImGui::GetCursorScreenPos().y),kInk,"Shows"); ImGui::Dummy(ImVec2(w,50.0f));
    const char* t[6]={"The Daily Brief","Tech Today","Mindful Minutes","The Long Story","Money Talks","On the Record"};
    const char* s[6]={"Weekly \xC2\xB7 142 eps","Biweekly","Daily","Monthly","Weekly","Weekly"};
    ImU32 a[6]={IM_COL32(60,40,120,255),IM_COL32(40,120,130,255),IM_COL32(150,60,70,255),IM_COL32(40,110,70,255),IM_COL32(120,90,40,255),IM_COL32(90,60,130,255)};
    ImU32 b[6]={IM_COL32(120,90,200,255),IM_COL32(90,200,210,255),IM_COL32(220,110,120,255),IM_COL32(90,190,130,255),IM_COL32(210,170,90,255),IM_COL32(160,120,210,255)};
    ImGui::SetCursorScreenPos(ImVec2(x0, ImGui::GetCursorScreenPos().y)); CoverGrid(x0,cw,t,s,a,b,6,3);
}
static void Draw_Sounds(float w){
    ImDrawList* dl=ImGui::GetWindowDrawList(); ImFont* f=ImGui::GetFont();
    float cw=w<560.0f?w:560.0f; float x0=ImGui::GetCursorScreenPos().x+(w-cw)*0.5f;
    dl->AddText(f,30.0f,ImVec2(x0+2,ImGui::GetCursorScreenPos().y),kInk,"Ringtone"); ImGui::Dummy(ImVec2(w,50.0f));
    const char* rs[7]={"Reflection","Glass","Crystal","Chime","Ripple","Aurora","Pulse"};
    static int sel=1; int n=7; float rowh=52.0f,h=n*rowh; ImVec2 tl(x0, ImGui::GetCursorScreenPos().y);
    Primitive bg; bg.cx=tl.x+cw*0.5f; bg.cy=tl.y+h*0.5f; bg.hw=cw*0.5f; bg.hh=h*0.5f; bg.corner_radius=24; bg.fade=1; bg.material=Material::Thin; g->Submit(bg);
    for(int i=0;i<n;i++){ float ry=tl.y+i*rowh,cy=ry+rowh*0.5f; ImGui::PushID(i); ImGui::SetCursorScreenPos(ImVec2(tl.x,ry)); ImGui::InvisibleButton("r",ImVec2(cw,rowh)); if(ImGui::IsItemDeactivated()&&ImGui::IsItemHovered()) sel=i; ImGui::PopID();
        if(i==sel) DrawIcon(dl,Icon::Check,ImVec2(tl.x+26,cy),16.0f,Accent(),2.4f);
        dl->AddText(f,16.0f,ImVec2(tl.x+52,cy-9),kInk,rs[i]);
        DrawIcon(dl,Icon::Play,ImVec2(tl.x+cw-22,cy),14.0f,kInkSoft,2.0f);
        if(i<n-1) dl->AddLine(ImVec2(tl.x+52,ry+rowh),ImVec2(tl.x+cw-14,ry+rowh),SA(kInkSoft,0.14f),1.0f); }
    ImGui::Dummy(ImVec2(w, h+12));
}
static void Draw_NewsFeed(float w){
    ImDrawList* dl=ImGui::GetWindowDrawList(); ImFont* f=ImGui::GetFont();
    float cw=w<640.0f?w:640.0f; float x0=ImGui::GetCursorScreenPos().x+(w-cw)*0.5f;
    dl->AddText(f,30.0f,ImVec2(x0+2,ImGui::GetCursorScreenPos().y),kInk,"For You"); ImGui::Dummy(ImVec2(w,50.0f));
    struct A{ const char* tag; const char* head; const char* src; ImU32 c; }; A as[]={
        {"DESIGN","The quiet return of texture and depth","Cult of Mac",IM_COL32(64,156,255,255)},
        {"TECH","On-device AI is quietly changing your phone","The Verge",IM_COL32(48,209,88,255)},
        {"TECH","What to expect from this year's lineup","Tech Daily",IM_COL32(255,69,58,255)},
        {"SCIENCE","The telescope's latest images, explained","NatGeo",IM_COL32(255,159,10,255)},
        {"OPINION","Calm software in a noisy world","The Verge",IM_COL32(146,82,232,255)} };
    for(int i=0;i<5;i++){ float h=110.0f; ImVec2 tl(x0, ImGui::GetCursorScreenPos().y);
        Primitive bg; bg.cx=tl.x+cw*0.5f; bg.cy=tl.y+h*0.5f; bg.hw=cw*0.5f; bg.hh=h*0.5f; bg.corner_radius=24; bg.fade=1; bg.material=Material::Thin; g->Submit(bg);
        ImVec2 tgs=f->CalcTextSizeA(11.0f,99999.0f,0,as[i].tag); dl->AddRectFilled(ImVec2(tl.x+18,tl.y+16),ImVec2(tl.x+18+tgs.x+16,tl.y+34),SA(as[i].c,0.2f),6.0f); dl->AddText(f,11.0f,ImVec2(tl.x+26,tl.y+19),as[i].c,as[i].tag);
        dl->AddText(f,18.0f,ImVec2(tl.x+18,tl.y+42),kInk,as[i].head,NULL,cw-140);
        dl->AddText(f,12.0f,ImVec2(tl.x+18,tl.y+h-24),kInkSoft,as[i].src);
        dl->AddRectFilled(ImVec2(tl.x+cw-104,tl.y+16),ImVec2(tl.x+cw-16,tl.y+h-16),SA(as[i].c,0.5f),10.0f);
        ImGui::Dummy(ImVec2(w, h+12)); }
}
static void Draw_Watchlist(float w){
    ImDrawList* dl=ImGui::GetWindowDrawList(); ImFont* f=ImGui::GetFont();
    const ImU32 up=IM_COL32(48,209,88,255), down=IM_COL32(255,69,58,255);
    float cw=w<600.0f?w:600.0f; float x0=ImGui::GetCursorScreenPos().x+(w-cw)*0.5f;
    dl->AddText(f,30.0f,ImVec2(x0+2,ImGui::GetCursorScreenPos().y),kInk,"Watchlist"); ImGui::Dummy(ImVec2(w,50.0f));
    struct S{ const char* s; float p; float ch; float seed; }; S st[]={ {"AAPL",214.29f,1.24f,0.0f},{"NVDA",128.44f,3.18f,1.3f},{"TSLA",178.08f,-2.04f,2.1f},{"MSFT",441.58f,0.62f,3.4f},{"AMZN",183.10f,0.95f,4.0f},{"META",502.30f,-1.12f,4.7f},{"GLAS",68.40f,5.11f,5.3f} };
    int n=7; static float ser[32]; float rowh=56.0f,h=n*rowh; ImVec2 tl(x0, ImGui::GetCursorScreenPos().y);
    Primitive bg; bg.cx=tl.x+cw*0.5f; bg.cy=tl.y+h*0.5f; bg.hw=cw*0.5f; bg.hh=h*0.5f; bg.corner_radius=24; bg.fade=1; bg.material=Material::Thin; g->Submit(bg);
    for(int i=0;i<n;i++){ float ry=tl.y+i*rowh,cy=ry+rowh*0.5f;
        dl->AddText(f,17.0f,ImVec2(tl.x+18,cy-12),kInk,st[i].s);
        for(int k=0;k<32;k++){ float tt=k*0.35f; ser[k]=10+sinf(tt+st[i].seed)*2+(st[i].ch>0?k*0.05f:-k*0.04f); }
        Sparkline(dl,ImVec2(tl.x+cw*0.42f,cy-12),96,24,ser,32,st[i].ch>=0?up:down);
        char pb[16]; snprintf(pb,16,"%.2f",st[i].p); ImVec2 ps=f->CalcTextSizeA(16.0f,99999.0f,0,pb); dl->AddText(f,16.0f,ImVec2(tl.x+cw-ps.x-92,cy-9),kInk,pb);
        char cb[16]; snprintf(cb,16,"%+.2f%%",st[i].ch); float pw=64,px=tl.x+cw-pw-14; Primitive pl; pl.cx=px+pw*0.5f; pl.cy=cy; pl.hw=pw*0.5f; pl.hh=13; pl.corner_radius=7; pl.fade=1; pl.material=st[i].ch>=0?Material::Accent:Material::Thin; g->Submit(pl);
        ImVec2 cs=f->CalcTextSizeA(12.0f,99999.0f,0,cb); dl->AddText(f,12.0f,ImVec2(px+pw*0.5f-cs.x*0.5f,cy-7),st[i].ch>=0?IM_COL32(255,255,255,255):down,cb);
        if(i<n-1) dl->AddLine(ImVec2(tl.x+18,ry+rowh),ImVec2(tl.x+cw-14,ry+rowh),SA(kInkSoft,0.14f),1.0f); }
    ImGui::Dummy(ImVec2(w, h+12));
}
static void Draw_BookStore(float w){
    ImDrawList* dl=ImGui::GetWindowDrawList(); ImFont* f=ImGui::GetFont();
    float cw=w<660.0f?w:660.0f; float x0=ImGui::GetCursorScreenPos().x+(w-cw)*0.5f;
    { float h=150.0f; ImVec2 tl(x0, ImGui::GetCursorScreenPos().y);
      dl->AddRectFilledMultiColor(ImVec2(tl.x,tl.y),ImVec2(tl.x+cw,tl.y+h), IM_COL32(140,40,60,255),IM_COL32(200,90,70,255),IM_COL32(230,150,90,255),IM_COL32(160,60,70,255));
      dl->AddRect(ImVec2(tl.x,tl.y),ImVec2(tl.x+cw,tl.y+h),SA(IM_COL32(255,255,255,255),0.15f),18.0f,0,1.5f);
      dl->AddText(f,12.0f,ImVec2(tl.x+20,tl.y+16),SA(IM_COL32(255,255,255,255),0.85f),"NEW & TRENDING");
      dl->AddText(f,26.0f,ImVec2(tl.x+20,tl.y+h-58),IM_COL32(255,255,255,255),"The Midnight Library");
      dl->AddText(f,14.0f,ImVec2(tl.x+20,tl.y+h-28),SA(IM_COL32(255,255,255,255),0.85f),"by Matt Haig \xC2\xB7 $14.99");
      ImGui::Dummy(ImVec2(w, h+16)); }
    dl->AddText(f,18.0f,ImVec2(x0+4,ImGui::GetCursorScreenPos().y),kInk,"Top Charts"); ImGui::Dummy(ImVec2(w,30.0f));
    const char* t[6]={"Project Hail Mary","Atomic Habits","Circe","Dune","Educated","Sapiens"};
    const char* s[6]={"$12.99","$9.99","$14.99","$19.99","$11.99","$8.99"};
    ImU32 a[6]={IM_COL32(60,90,160,255),IM_COL32(40,140,90,255),IM_COL32(150,60,120,255),IM_COL32(180,120,40,255),IM_COL32(80,80,90,255),IM_COL32(120,60,160,255)};
    ImU32 b[6]={IM_COL32(90,140,210,255),IM_COL32(90,200,140,255),IM_COL32(210,110,170,255),IM_COL32(230,180,90,255),IM_COL32(130,130,150,255),IM_COL32(170,110,210,255)};
    ImGui::SetCursorScreenPos(ImVec2(x0, ImGui::GetCursorScreenPos().y)); CoverGrid(x0,cw,t,s,a,b,6,3);
}
static void Draw_Display(float w){
    ImDrawList* dl=ImGui::GetWindowDrawList(); ImFont* f=ImGui::GetFont();
    static float bright=0.8f, textsz=0.5f; static int appear=0; static bool trueTone=true, nightShift=false;
    float cw=w<560.0f?w:560.0f; float x0=ImGui::GetCursorScreenPos().x+(w-cw)*0.5f;
    dl->AddText(f,30.0f,ImVec2(x0+2,ImGui::GetCursorScreenPos().y),kInk,"Display"); ImGui::Dummy(ImVec2(w,52.0f));
    { float h=120.0f; ImVec2 tl(x0, ImGui::GetCursorScreenPos().y);
      Primitive bg; bg.cx=tl.x+cw*0.5f; bg.cy=tl.y+h*0.5f; bg.hw=cw*0.5f; bg.hh=h*0.5f; bg.corner_radius=24; bg.fade=1; bg.material=Material::Thin; g->Submit(bg);
      const char* an[2]={"Light","Dark"}; ImU32 ac[2]={IM_COL32(235,235,240,255),IM_COL32(20,20,26,255)};
      for(int i=0;i<2;i++){ float pw=(cw-60)/2, px=tl.x+20+i*(pw+20), py=tl.y+16;
        ImGui::PushID(i); ImGui::SetCursorScreenPos(ImVec2(px,py)); ImGui::InvisibleButton("ap",ImVec2(pw,64)); if(ImGui::IsItemDeactivated()&&ImGui::IsItemHovered()) appear=i; ImGui::PopID();
        dl->AddRectFilled(ImVec2(px,py),ImVec2(px+pw,py+64),ac[i],10.0f); dl->AddRectFilled(ImVec2(px+12,py+14),ImVec2(px+pw-12,py+22),i==0?IM_COL32(180,180,190,255):IM_COL32(80,80,90,255),3.0f); dl->AddRectFilled(ImVec2(px+12,py+30),ImVec2(px+pw-40,py+38),i==0?IM_COL32(200,200,210,255):IM_COL32(60,60,70,255),3.0f);
        if(appear==i) dl->AddRect(ImVec2(px-2,py-2),ImVec2(px+pw+2,py+66),Accent(),12.0f,0,2.5f);
        ImVec2 ns=f->CalcTextSizeA(13.0f,99999.0f,0,an[i]); dl->AddText(f,13.0f,ImVec2(px+pw*0.5f-ns.x*0.5f,py+70),kInk,an[i]); }
      ImGui::Dummy(ImVec2(w, h+16)); }
    ImGui::Indent((w-cw)*0.5f); ImGui::PushItemWidth(cw);
    Glass::BeginPanel("BRIGHTNESS"); Glass::Slider("Brightness", &bright, 0.0f, 1.0f, "%.0f%%"); Glass::RowToggle(Glass::Icon::Sun, 0, "True Tone", &trueTone); Glass::RowToggle(Glass::Icon::Moon, 0, "Night Shift", &nightShift); Glass::EndPanel();
    Glass::BeginPanel("TEXT SIZE"); Glass::Slider("Size", &textsz, 0.0f, 1.0f, "%.0f%%"); Glass::EndPanel();
    ImGui::PopItemWidth(); ImGui::Unindent((w-cw)*0.5f);
}
static void Draw_TicTacToe(float w){
    ImDrawList* dl=ImGui::GetWindowDrawList(); ImFont* f=ImGui::GetFont();
    static int board[9]={0}; static int turn=1;
    float cw=w<420.0f?w:420.0f; float x0=ImGui::GetCursorScreenPos().x+(w-cw)*0.5f;
    int xw=0;
    const int wl[8][3]={{0,1,2},{3,4,5},{6,7,8},{0,3,6},{1,4,7},{2,5,8},{0,4,8},{2,4,6}};
    for(int i=0;i<8;i++){ if(board[wl[i][0]]&&board[wl[i][0]]==board[wl[i][1]]&&board[wl[i][1]]==board[wl[i][2]]) xw=board[wl[i][0]]; }
    char st[24]; if(xw) snprintf(st,24,"%s wins!",xw==1?"X":"O"); else snprintf(st,24,"%s to move",turn==1?"X":"O");
    ImVec2 ss=f->CalcTextSizeA(20.0f,99999.0f,0,st); dl->AddText(f,20.0f,ImVec2(x0+cw*0.5f-ss.x*0.5f,ImGui::GetCursorScreenPos().y),kInk,st);
    ImGui::Dummy(ImVec2(w,40.0f));
    float gx=x0, gw=cw, cell=gw/3.0f; float top=ImGui::GetCursorScreenPos().y;
    Primitive bg; bg.cx=gx+gw*0.5f; bg.cy=top+gw*0.5f; bg.hw=gw*0.5f; bg.hh=gw*0.5f; bg.corner_radius=28; bg.fade=1; bg.material=Material::Thin; g->Submit(bg);
    for(int i=0;i<9;i++){ int r=i/3,c=i%3; float cx=gx+c*cell+cell*0.5f, cy=top+r*cell+cell*0.5f;
        ImGui::PushID(i); ImGui::SetCursorScreenPos(ImVec2(gx+c*cell,top+r*cell)); ImGui::InvisibleButton("t",ImVec2(cell,cell));
        bool ch=ImGui::IsItemHovered(); if(!xw&&board[i]==0&&ImGui::IsItemDeactivated()&&ch){ board[i]=turn; turn=turn==1?2:1; }
        Spring& mp=g->springs().Get((uint32_t)ImGui::GetID("t"),3,SpringStyle::Bouncy,board[i]?1.0f:0.0f); mp.target=board[i]?1.0f:0.0f; mp.Tick(Dt()); float mk=Clampf(mp.x,0,1.25f); ImGui::PopID();
        if(board[i]==0&&ch&&!xw) dl->AddRectFilled(ImVec2(gx+c*cell+6,top+r*cell+6),ImVec2(gx+(c+1)*cell-6,top+(r+1)*cell-6),SA(kInkSoft,0.10f),10.0f);
        float m=cell*0.26f*mk; if(board[i]==1){ dl->AddLine(ImVec2(cx-m,cy-m),ImVec2(cx+m,cy+m),Accent(),5.0f); dl->AddLine(ImVec2(cx-m,cy+m),ImVec2(cx+m,cy-m),Accent(),5.0f); }
        else if(board[i]==2){ dl->AddCircle(ImVec2(cx,cy),m,IM_COL32(255,159,10,255),32,5.0f); } }
    for(int i=1;i<3;i++){ dl->AddLine(ImVec2(gx+i*cell,top+10),ImVec2(gx+i*cell,top+gw-10),SA(kInkSoft,0.4f),2.0f); dl->AddLine(ImVec2(gx+10,top+i*cell),ImVec2(gx+gw-10,top+i*cell),SA(kInkSoft,0.4f),2.0f); }
    ImGui::SetCursorScreenPos(ImVec2(x0, top+gw+12)); if(Glass::Button("New Game", true, ImVec2(cw,44))){ for(int i=0;i<9;i++)board[i]=0; turn=1; }
    ImGui::Dummy(ImVec2(w, 10.0f));
}
static void Draw_AppDetail(float w){
    ImDrawList* dl=ImGui::GetWindowDrawList(); ImFont* f=ImGui::GetFont();
    float cw=w<660.0f?w:660.0f; float x0=ImGui::GetCursorScreenPos().x+(w-cw)*0.5f;
    { float h=120.0f; ImVec2 tl(x0, ImGui::GetCursorScreenPos().y);
      dl->AddRectFilledMultiColor(ImVec2(tl.x,tl.y),ImVec2(tl.x+100,tl.y+100), IM_COL32(60,40,120,255),IM_COL32(150,80,180,255),IM_COL32(150,80,180,255),IM_COL32(60,40,120,255));
      dl->AddText(f,22.0f,ImVec2(tl.x+116,tl.y+10),kInk,"Glass Studio");
      dl->AddText(f,14.0f,ImVec2(tl.x+116,tl.y+40),kInkSoft,"Design liquid interfaces");
      Primitive gp; gp.cx=tl.x+116+34; gp.cy=tl.y+82; gp.hw=44; gp.hh=16; gp.corner_radius=22; gp.fade=1; gp.material=Material::Accent; g->Submit(gp); ImVec2 gs=f->CalcTextSizeA(14.0f,99999.0f,0,"GET"); dl->AddText(f,14.0f,ImVec2(tl.x+116+34-gs.x*0.5f,tl.y+74),IM_COL32(255,255,255,255),"GET");
      ImGui::Dummy(ImVec2(w, h+14)); }
    { float h=70.0f; ImVec2 tl(x0, ImGui::GetCursorScreenPos().y);
      Primitive bg; bg.cx=tl.x+cw*0.5f; bg.cy=tl.y+h*0.5f; bg.hw=cw*0.5f; bg.hh=h*0.5f; bg.corner_radius=22; bg.fade=1; bg.material=Material::Thin; g->Submit(bg);
      const char* k[4]={"4.8","#1","12+","142 MB"}; const char* v[4]={"24K Ratings","Graphics","Age","Size"};
      for(int i=0;i<4;i++){ float cx=tl.x+cw*(i+0.5f)/4.0f; ImVec2 ks=f->CalcTextSizeA(20.0f,99999.0f,0,k[i]); dl->AddText(f,20.0f,ImVec2(cx-ks.x*0.5f,tl.y+12),kInk,k[i]); ImVec2 vs=f->CalcTextSizeA(11.0f,99999.0f,0,v[i]); dl->AddText(f,11.0f,ImVec2(cx-vs.x*0.5f,tl.y+42),kInkSoft,v[i]); if(i<3) dl->AddLine(ImVec2(tl.x+cw*(i+1)/4.0f,tl.y+14),ImVec2(tl.x+cw*(i+1)/4.0f,tl.y+h-14),SA(kInkSoft,0.2f),1.0f); }
      ImGui::Dummy(ImVec2(w, h+14)); }
    dl->AddText(f,16.0f,ImVec2(x0+2,ImGui::GetCursorScreenPos().y),kInk,"Preview"); ImGui::Dummy(ImVec2(w,26.0f));
    { float h=220.0f; ImVec2 tl(x0, ImGui::GetCursorScreenPos().y); float sw=(cw-24)/3.0f;
      ImU32 cs[3]={IM_COL32(40,50,90,255),IM_COL32(90,50,80,255),IM_COL32(40,90,80,255)};
      for(int i=0;i<3;i++){ float sx=tl.x+i*(sw+12); dl->AddRectFilledMultiColor(ImVec2(sx,tl.y),ImVec2(sx+sw,tl.y+h), cs[i],SA(cs[i],0.6f),SA(cs[i],0.6f),cs[i]); dl->AddRect(ImVec2(sx,tl.y),ImVec2(sx+sw,tl.y+h),SA(IM_COL32(255,255,255,255),0.12f),12.0f,0,1.0f); }
      ImGui::Dummy(ImVec2(w, h+12)); }
}
static void Draw_PhotoAlbums(float w){
    ImDrawList* dl=ImGui::GetWindowDrawList(); ImFont* f=ImGui::GetFont();
    float cw=w<660.0f?w:660.0f; float x0=ImGui::GetCursorScreenPos().x+(w-cw)*0.5f;
    dl->AddText(f,30.0f,ImVec2(x0+2,ImGui::GetCursorScreenPos().y),kInk,"Albums"); ImGui::Dummy(ImVec2(w,50.0f));
    struct Al{ const char* n; int cnt; ImU32 a; ImU32 b; }; Al al[6]={ {"Recents",1240,IM_COL32(60,90,160,255),IM_COL32(110,150,220,255)},{"Favorites",86,IM_COL32(200,60,90,255),IM_COL32(250,120,150,255)},{"Big Sur",32,IM_COL32(180,120,40,255),IM_COL32(240,180,90,255)},{"Family",214,IM_COL32(40,140,90,255),IM_COL32(100,210,150,255)},{"Screenshots",512,IM_COL32(120,120,128,255),IM_COL32(170,170,180,255)},{"Selfies",128,IM_COL32(146,82,232,255),IM_COL32(190,150,240,255)} };
    int n=6, cols=3; float gap=14, cellw=(cw-(cols-1)*gap)/cols, lblH=40.0f; float sy=ImGui::GetCursorScreenPos().y;
    for(int i=0;i<n;i++){ int r=i/cols,c=i%cols; float x=x0+c*(cellw+gap), y=sy+r*(cellw+lblH+gap);
        ImgOrGrad(dl, ImVec2(x,y), ImVec2(x+cellw,y+cellw), i+3, al[i].a, al[i].b, 12.0f); dl->AddRect(ImVec2(x,y),ImVec2(x+cellw,y+cellw),SA(IM_COL32(255,255,255,255),0.12f),12.0f,0,1.0f);
        dl->AddText(f,15.0f,ImVec2(x+2,y+cellw+5),kInk,al[i].n); char cb[12]; snprintf(cb,12,"%d",al[i].cnt); dl->AddText(f,12.0f,ImVec2(x+2,y+cellw+24),kInkSoft,cb); }
    int rows=(n+cols-1)/cols; ImGui::Dummy(ImVec2(w, rows*(cellw+lblH+gap)+4));
}
static void Draw_CameraPortrait(float w){
    ImDrawList* dl=ImGui::GetWindowDrawList(); ImFont* f=ImGui::GetFont();
    float cw=w<480.0f?w:480.0f; float x0=ImGui::GetCursorScreenPos().x+(w-cw)*0.5f;
    float top=ImGui::GetCursorScreenPos().y, vh=cw*1.15f; if(vh>540)vh=540;
    dl->AddRectFilledMultiColor(ImVec2(x0,top),ImVec2(x0+cw,top+vh), IM_COL32(60,50,55,255),IM_COL32(90,70,80,255),IM_COL32(40,35,45,255),IM_COL32(70,55,65,255));
    dl->AddRect(ImVec2(x0,top),ImVec2(x0+cw,top+vh),SA(IM_COL32(255,255,255,255),0.12f),20.0f,0,1.5f);
    dl->PushClipRect(ImVec2(x0,top),ImVec2(x0+cw,top+vh),true);
    ImVec2 c(x0+cw*0.5f,top+vh*0.42f); dl->AddCircleFilled(c,70.0f,SA(IM_COL32(220,190,170,255),0.5f),48);
    dl->AddRectFilled(ImVec2(x0+cw*0.5f-46,top+20),ImVec2(x0+cw*0.5f+46,top+44),SA(IM_COL32(0,0,0,255),0.35f),12.0f); dl->AddText(f,13.0f,ImVec2(x0+cw*0.5f-30,top+24),IM_COL32(255,214,90,255),"PORTRAIT");
    dl->AddRectFilled(ImVec2(x0+20,top+vh-44),ImVec2(x0+90,top+vh-18),SA(IM_COL32(0,0,0,255),0.35f),12.0f); dl->AddText(f,14.0f,ImVec2(x0+30,top+vh-40),IM_COL32(255,214,90,255),"f1.4");
    dl->PopClipRect();
    ImGui::Dummy(ImVec2(w, vh+14));
    { float sy=ImGui::GetCursorScreenPos().y; const char* modes[3]={"PHOTO","PORTRAIT","VIDEO"}; for(int i=0;i<3;i++){ float mx=x0+cw*0.5f+(i-1)*100; ImVec2 ms=f->CalcTextSizeA(13.0f,99999.0f,0,modes[i]); dl->AddText(f,13.0f,ImVec2(mx-ms.x*0.5f,sy),i==1?IM_COL32(255,214,90,255):kInkSoft,modes[i]); }
      dl->AddCircleFilled(ImVec2(x0+cw*0.5f,sy+50),32.0f,IM_COL32(255,255,255,255)); dl->AddCircle(ImVec2(x0+cw*0.5f,sy+50),38.0f,IM_COL32(255,255,255,255),0,3.0f);
      ImGui::Dummy(ImVec2(w, 100.0f)); }
}
static void Draw_MailCompose(float w){
    ImDrawList* dl=ImGui::GetWindowDrawList(); ImFont* f=ImGui::GetFont();
    float cw=w<620.0f?w:620.0f; float x0=ImGui::GetCursorScreenPos().x+(w-cw)*0.5f;
    { float h=44.0f; ImVec2 tl(x0, ImGui::GetCursorScreenPos().y); dl->AddText(f,16.0f,ImVec2(tl.x,tl.y+12),kInkSoft,"Cancel"); ImVec2 ts=f->CalcTextSizeA(18.0f,99999.0f,0,"New Message"); dl->AddText(f,18.0f,ImVec2(tl.x+cw*0.5f-ts.x*0.5f,tl.y+10),kInk,"New Message");
      Primitive sd; sd.cx=tl.x+cw-30; sd.cy=tl.y+22; sd.hw=22; sd.hh=16; sd.corner_radius=22; sd.fade=1; sd.material=Material::Accent; g->Submit(sd); DrawIcon(dl,Icon::ArrowUp,ImVec2(tl.x+cw-30,tl.y+22),14.0f,IM_COL32(255,255,255,255),2.2f);
      ImGui::Dummy(ImVec2(w, h+12)); }
    { const char* lab[3]={"To:","Cc/Bcc:","Subject:"}; const char* val[3]={"pondot@icloud.com","","Lunch on Friday?"};
      float h=3*48.0f; ImVec2 tl(x0, ImGui::GetCursorScreenPos().y);
      Primitive bg; bg.cx=tl.x+cw*0.5f; bg.cy=tl.y+h*0.5f; bg.hw=cw*0.5f; bg.hh=h*0.5f; bg.corner_radius=18; bg.fade=1; bg.material=Material::Thin; g->Submit(bg);
      for(int i=0;i<3;i++){ float ry=tl.y+i*48.0f, cy=ry+24; dl->AddText(f,15.0f,ImVec2(tl.x+18,cy-8),kInkSoft,lab[i]); dl->AddText(f,15.0f,ImVec2(tl.x+110,cy-8),val[i][0]?kInk:kInkSoft,val[i][0]?val[i]:"");
        if(i<2) dl->AddLine(ImVec2(tl.x+14,ry+48),ImVec2(tl.x+cw-14,ry+48),SA(kInkSoft,0.14f),1.0f); }
      ImGui::Dummy(ImVec2(w, h+12)); }
    { float h=200.0f; ImVec2 tl(x0, ImGui::GetCursorScreenPos().y);
      Primitive bg; bg.cx=tl.x+cw*0.5f; bg.cy=tl.y+h*0.5f; bg.hw=cw*0.5f; bg.hh=h*0.5f; bg.corner_radius=18; bg.fade=1; bg.material=Material::Thin; g->Submit(bg);
      dl->AddText(f,15.0f,ImVec2(tl.x+18,tl.y+16),kInk,"Hi Olivia,",NULL,cw-36);
      float ly=tl.y+50; for(int i=0;i<5;i++){ float lw=(i==4)?cw*0.4f:cw-60; dl->AddRectFilled(ImVec2(tl.x+18,ly),ImVec2(tl.x+18+lw,ly+8),SA(kInkSoft,0.35f),3.0f); ly+=22; }
      ImGui::Dummy(ImVec2(w, h+12)); }
}
static void Draw_MessagesList(float w){
    ImDrawList* dl=ImGui::GetWindowDrawList(); ImFont* f=ImGui::GetFont();
    float cw=w<600.0f?w:600.0f; float x0=ImGui::GetCursorScreenPos().x+(w-cw)*0.5f;
    dl->AddText(f,30.0f,ImVec2(x0+2,ImGui::GetCursorScreenPos().y),kInk,"Messages"); ImGui::Dummy(ImVec2(w,50.0f));
    struct C{ const char* n; const char* p; const char* t; bool unread; ImU32 col; const char* i; }; C cs[]={
        {"Emma","Are we still on for dinner tonight?","now",true,IM_COL32(48,209,88,255),"EW"},
        {"Olivia Bennett","Lunch on Friday? \xF0\x9F\x99\x82","9:32",true,IM_COL32(255,159,10,255),"OB"},
        {"Work","James: standup moved to 10:30","8:10",false,IM_COL32(146,82,232,255),"W"},
        {"Mom","Call me when you wake up \xE2\x9D\xA4\xEF\xB8\x8F","Yesterday",false,IM_COL32(255,69,58,255),"M"},
        {"David Park","Sounds good, thanks!","Yesterday",false,IM_COL32(64,156,255,255),"DP"},
        {"Delivery","Your package was delivered","Tue",false,IM_COL32(120,120,128,255),"D"} };
    int n=6; float rowh=74.0f,h=n*rowh; ImVec2 tl(x0, ImGui::GetCursorScreenPos().y);
    Primitive bg; bg.cx=tl.x+cw*0.5f; bg.cy=tl.y+h*0.5f; bg.hw=cw*0.5f; bg.hh=h*0.5f; bg.corner_radius=24; bg.fade=1; bg.material=Material::Thin; g->Submit(bg);
    for(int i=0;i<n;i++){ float ry=tl.y+i*rowh,cy=ry+rowh*0.5f;
        if(cs[i].unread) dl->AddCircleFilled(ImVec2(tl.x+16,cy),4.0f,Accent());
        dl->AddCircleFilled(ImVec2(tl.x+48,cy),22.0f,cs[i].col); ImVec2 is=f->CalcTextSizeA(15.0f,99999.0f,0,cs[i].i); dl->AddText(f,15.0f,ImVec2(tl.x+48-is.x*0.5f,cy-is.y*0.5f),IM_COL32(255,255,255,255),cs[i].i);
        dl->AddText(f,16.0f,ImVec2(tl.x+80,cy-18),kInk,cs[i].n); ImVec2 tts=f->CalcTextSizeA(12.0f,99999.0f,0,cs[i].t); dl->AddText(f,12.0f,ImVec2(tl.x+cw-tts.x-16,cy-16),kInkSoft,cs[i].t);
        dl->AddText(f,13.0f,ImVec2(tl.x+80,cy+4),kInkSoft,cs[i].p,NULL,cw-180);
        if(i<n-1) dl->AddLine(ImVec2(tl.x+80,ry+rowh),ImVec2(tl.x+cw-14,ry+rowh),SA(kInkSoft,0.14f),1.0f); }
    ImGui::Dummy(ImVec2(w, h+12));
}
static void Draw_CalWeek(float w){
    ImDrawList* dl=ImGui::GetWindowDrawList(); ImFont* f=ImGui::GetFont();
    float cw=w<700.0f?w:700.0f; float x0=ImGui::GetCursorScreenPos().x+(w-cw)*0.5f;
    dl->AddText(f,26.0f,ImVec2(x0+2,ImGui::GetCursorScreenPos().y),kInk,"This Week"); ImGui::Dummy(ImVec2(w,42.0f));
    float h=420.0f; ImVec2 tl(x0, ImGui::GetCursorScreenPos().y);
    Primitive bg; bg.cx=tl.x+cw*0.5f; bg.cy=tl.y+h*0.5f; bg.hw=cw*0.5f; bg.hh=h*0.5f; bg.corner_radius=24; bg.fade=1; bg.material=Material::Thin; g->Submit(bg);
    dl->PushClipRect(ImVec2(tl.x,tl.y),ImVec2(tl.x+cw,tl.y+h),true);
    const char* days[7]={"MON","TUE","WED","THU","FRI","SAT","SUN"}; float colw=(cw-50)/7.0f; float gx=tl.x+50;
    for(int i=0;i<7;i++){ float cx=gx+colw*(i+0.5f); ImVec2 ds=f->CalcTextSizeA(12.0f,99999.0f,0,days[i]); dl->AddText(f,12.0f,ImVec2(cx-ds.x*0.5f,tl.y+8),i==2?Accent():kInkSoft,days[i]); }
    for(int hh=0;hh<8;hh++){ float hy=tl.y+34+hh*46.0f; char hb[8]; snprintf(hb,8,"%d%s",((hh+8)>12?(hh+8-12):(hh+8)),(hh+8>=12)?"PM":"AM"); dl->AddText(f,10.0f,ImVec2(tl.x+8,hy-6),kInkSoft,hb); dl->AddLine(ImVec2(gx,hy),ImVec2(tl.x+cw-6,hy),SA(kInkSoft,0.1f),1.0f); }
    struct Ev{ int day; float start; float dur; ImU32 c; const char* t; }; Ev ev[]={ {0,1.0f,1.5f,IM_COL32(64,156,255,255),"Standup"},{2,0.5f,2.0f,IM_COL32(255,69,58,255),"Review"},{2,3.0f,1.0f,IM_COL32(48,209,88,255),"Lunch"},{4,2.0f,2.5f,IM_COL32(146,82,232,255),"Ship"},{1,4.0f,1.0f,IM_COL32(255,159,10,255),"1:1"} };
    for(int i=0;i<5;i++){ float ex=gx+colw*ev[i].day+3, ey=tl.y+34+ev[i].start*46.0f, eh=ev[i].dur*46.0f-4;
        dl->AddRectFilled(ImVec2(ex,ey),ImVec2(ex+colw-6,ey+eh), SA(ev[i].c,0.85f), 6.0f);
        dl->AddText(f,11.0f,ImVec2(ex+6,ey+5),IM_COL32(255,255,255,255),ev[i].t,NULL,colw-12); }
    dl->PopClipRect(); ImGui::Dummy(ImVec2(w, h+12));
}
static void Draw_HeartRate(float w){
    ImDrawList* dl=ImGui::GetWindowDrawList(); ImFont* f=ImGui::GetFont(); float t=(float)ImGui::GetTime();
    float cw=w<600.0f?w:600.0f; float x0=ImGui::GetCursorScreenPos().x+(w-cw)*0.5f;
    { float h=200.0f; ImVec2 tl(x0, ImGui::GetCursorScreenPos().y);
      Primitive bg; bg.cx=tl.x+cw*0.5f; bg.cy=tl.y+h*0.5f; bg.hw=cw*0.5f; bg.hh=h*0.5f; bg.corner_radius=29; bg.fade=1; bg.material=Material::Thin; g->Submit(bg);
      DrawIcon(dl,Icon::Heart,ImVec2(tl.x+34,tl.y+30),18.0f,IM_COL32(255,55,95,255),2.0f);
      dl->AddText(f,13.0f,ImVec2(tl.x+56,tl.y+22),IM_COL32(255,55,95,255),"HEART RATE");
      dl->AddText(f,52.0f,ImVec2(tl.x+22,tl.y+44),kInk,"68"); dl->AddText(f,18.0f,ImVec2(tl.x+110,tl.y+70),kInkSoft,"BPM");
      float gy=tl.y+150, gx0=tl.x+20, gw=cw-40; ImVec2 prev(gx0,gy);
      for(float px=0; px<gw; px+=3){ float ph=fmodf(px*0.02f + t*2.0f, 6.2831853f); float spike=0; float seg=fmodf((px+ t*120.0f),120.0f);
        if(seg>54&&seg<58) spike=-34.0f; else if(seg>=58&&seg<62) spike=24.0f; else if(seg>=62&&seg<66) spike=-10.0f; else spike=sinf(px*0.3f)*2.0f;
        ImVec2 cur(gx0+px, gy+spike); dl->AddLine(prev,cur,IM_COL32(255,55,95,255),2.0f); prev=cur; (void)ph; }
      ImGui::Dummy(ImVec2(w, h+14)); }
    { struct Z{ const char* n; const char* r; ImU32 c; float v; }; Z zs[]={ {"Resting","58-72 BPM",IM_COL32(64,156,255,255),0.6f},{"Walking","90-110",IM_COL32(48,209,88,255),0.3f},{"Workout","130-160",IM_COL32(255,159,10,255),0.5f},{"Peak","170+",IM_COL32(255,69,58,255),0.15f} };
      float rowh=54.0f,h=4*rowh; ImVec2 tl(x0, ImGui::GetCursorScreenPos().y);
      Primitive bg; bg.cx=tl.x+cw*0.5f; bg.cy=tl.y+h*0.5f; bg.hw=cw*0.5f; bg.hh=h*0.5f; bg.corner_radius=24; bg.fade=1; bg.material=Material::Thin; g->Submit(bg);
      for(int i=0;i<4;i++){ float ry=tl.y+i*rowh,cy=ry+rowh*0.5f; dl->AddCircleFilled(ImVec2(tl.x+24,cy),6.0f,zs[i].c); dl->AddText(f,16.0f,ImVec2(tl.x+40,cy-16),kInk,zs[i].n); dl->AddText(f,12.0f,ImVec2(tl.x+40,cy+3),kInkSoft,zs[i].r);
        float bx=tl.x+cw*0.5f, bw=cw*0.4f; dl->AddRectFilled(ImVec2(bx,cy-3),ImVec2(bx+bw,cy+3),SA(kInkSoft,0.2f),3.0f); dl->AddRectFilled(ImVec2(bx,cy-3),ImVec2(bx+bw*zs[i].v,cy+3),zs[i].c,3.0f);
        if(i<3) dl->AddLine(ImVec2(tl.x+40,ry+rowh),ImVec2(tl.x+cw-14,ry+rowh),SA(kInkSoft,0.14f),1.0f); }
      ImGui::Dummy(ImVec2(w, h+12)); }
}
static void Draw_Checklist(float w){
    ImDrawList* dl=ImGui::GetWindowDrawList(); ImFont* f=ImGui::GetFont();
    float cw=w<600.0f?w:600.0f; float x0=ImGui::GetCursorScreenPos().x+(w-cw)*0.5f;
    dl->AddText(f,30.0f,ImVec2(x0+2,ImGui::GetCursorScreenPos().y),kInk,"Packing List"); dl->AddText(f,13.0f,ImVec2(x0+2,ImGui::GetCursorScreenPos().y+38),kInkSoft,"Edited just now"); ImGui::Dummy(ImVec2(w,66.0f));
    static bool d[8]={true,true,false,false,true,false,false,false};
    const char* it[8]={"Passport","Charger & cables","Headphones","Sunglasses","Toothbrush","Swimsuit","Book to read","Snacks for the flight"};
    float rowh=50.0f,h=8*rowh; ImVec2 tl(x0, ImGui::GetCursorScreenPos().y);
    Primitive bg; bg.cx=tl.x+cw*0.5f; bg.cy=tl.y+h*0.5f; bg.hw=cw*0.5f; bg.hh=h*0.5f; bg.corner_radius=24; bg.fade=1; bg.material=Material::Thin; g->Submit(bg);
    for(int i=0;i<8;i++){ float ry=tl.y+i*rowh,cy=ry+rowh*0.5f; ReminderCheck((const char*)it[i],ImVec2(tl.x+30,cy),11.0f,&d[i]);
        dl->AddText(f,16.0f,ImVec2(tl.x+54,cy-9),d[i]?SA(kInkSoft,0.7f):kInk,it[i]);
        if(i<7) dl->AddLine(ImVec2(tl.x+54,ry+rowh),ImVec2(tl.x+cw-14,ry+rowh),SA(kInkSoft,0.12f),1.0f); }
    ImGui::Dummy(ImVec2(w, h+12));
}
static void Draw_BoardingPass(float w){
    ImDrawList* dl=ImGui::GetWindowDrawList(); ImFont* f=ImGui::GetFont();
    float cw=w<460.0f?w:460.0f; float x0=ImGui::GetCursorScreenPos().x+(w-cw)*0.5f;
    float h=460.0f; ImVec2 tl(x0, ImGui::GetCursorScreenPos().y);
    RoundedGrad(dl, ImVec2(tl.x,tl.y),ImVec2(tl.x+cw,tl.y+h*0.6f), IM_COL32(180,70,60,255),IM_COL32(220,120,80,255),IM_COL32(230,150,90,255),IM_COL32(200,90,70,255), 20.0f, ImDrawFlags_RoundCornersTop);
    dl->AddRectFilled(ImVec2(tl.x,tl.y+h*0.6f),ImVec2(tl.x+cw,tl.y+h), IM_COL32(245,245,248,255), 20.0f, ImDrawFlags_RoundCornersBottom);
    ImU32 w1=IM_COL32(255,255,255,255), dk=IM_COL32(40,40,50,255);
    dl->AddText(f,13.0f,ImVec2(tl.x+24,tl.y+20),SA(w1,0.85f),"BOARDING PASS");
    DrawIcon(dl,Icon::Airplane,ImVec2(tl.x+cw-30,tl.y+28),18.0f,w1,2.0f);
    dl->AddText(f,52.0f,ImVec2(tl.x+24,tl.y+50),w1,"SFO"); dl->AddText(f,52.0f,ImVec2(tl.x+cw-130,tl.y+50),w1,"JFK");
    DrawIcon(dl,Icon::Airplane,ImVec2(tl.x+cw*0.5f,tl.y+76),20.0f,w1,2.0f);
    dl->AddText(f,13.0f,ImVec2(tl.x+24,tl.y+118),SA(w1,0.85f),"San Francisco"); dl->AddText(f,13.0f,ImVec2(tl.x+cw-110,tl.y+118),SA(w1,0.85f),"New York");
    struct Kv{ const char* k; const char* v; }; Kv kv[6]={ {"FLIGHT","GL 350"},{"GATE","D12"},{"SEAT","2A"},{"BOARDS","9:15 AM"},{"CLASS","First"},{"ZONE","1"} };
    for(int i=0;i<6;i++){ int r=i/3,c=i%3; float kx=tl.x+24+c*((cw-48)/3.0f), ky=tl.y+h*0.6f+20+r*54.0f; dl->AddText(f,11.0f,ImVec2(kx,ky),SA(dk,0.5f),kv[i].k); dl->AddText(f,20.0f,ImVec2(kx,ky+16),dk,kv[i].v); }
    float by=tl.y+h-50; for(float bx=tl.x+24; bx<tl.x+cw-24; bx+=4){ float bw=(((int)bx)%3==0)?3.0f:1.5f; dl->AddRectFilled(ImVec2(bx,by),ImVec2(bx+bw,by+34),dk,0); }
    ImGui::Dummy(ImVec2(w, h+14));
}
static void Draw_Radio(float w){
    ImDrawList* dl=ImGui::GetWindowDrawList(); ImFont* f=ImGui::GetFont();
    float cw=w<660.0f?w:660.0f; float x0=ImGui::GetCursorScreenPos().x+(w-cw)*0.5f;
    dl->AddText(f,30.0f,ImVec2(x0+2,ImGui::GetCursorScreenPos().y),kInk,"Radio"); ImGui::Dummy(ImVec2(w,50.0f));
    const char* t[6]={"Today's Hits","Chill","Pure Focus","Synthwave","Classical","Jazz Cafe"};
    const char* s[6]={"Top 40 now","Ambient beats","Deep work","Retro nights","Calm strings","Smooth evening"};
    ImU32 a[6]={IM_COL32(200,50,80,255),IM_COL32(40,120,150,255),IM_COL32(90,60,160,255),IM_COL32(180,60,120,255),IM_COL32(60,90,160,255),IM_COL32(150,100,40,255)};
    ImU32 b[6]={IM_COL32(255,110,130,255),IM_COL32(90,200,210,255),IM_COL32(150,110,220,255),IM_COL32(230,110,180,255),IM_COL32(110,150,220,255),IM_COL32(210,170,90,255)};
    static int sel=0; static bool playing=true; if(sel<0||sel>5) sel=0;
    { int cols=3, n=6; float gap=14, cellw=(cw-(cols-1)*gap)/cols, ch=cellw*0.72f; float sy=ImGui::GetCursorScreenPos().y;
      for(int i=0;i<n;i++){ int r=i/cols,c=i%cols; float x=x0+c*(cellw+gap), y=sy+r*(ch+gap); bool hov=false;
        if(SurfaceHit(t[i], ImVec2(x,y), ImVec2(cellw,ch), &hov)){ sel=i; playing=true; }
        dl->AddRectFilledMultiColor(ImVec2(x,y),ImVec2(x+cellw,y+ch), a[i],b[i],b[i],a[i]);
        dl->AddRectFilledMultiColor(ImVec2(x,y+ch*0.5f),ImVec2(x+cellw,y+ch), IM_COL32(0,0,0,0),IM_COL32(0,0,0,0),IM_COL32(0,0,0,120),IM_COL32(0,0,0,120));
        if(i==sel)   dl->AddRect(ImVec2(x,y),ImVec2(x+cellw,y+ch), Accent(), 12.0f, 0, 3.0f);
        else if(hov) dl->AddRect(ImVec2(x,y),ImVec2(x+cellw,y+ch), SA(IM_COL32(255,255,255,255),0.6f), 12.0f, 0, 2.0f);
        else         dl->AddRect(ImVec2(x,y),ImVec2(x+cellw,y+ch), SA(IM_COL32(255,255,255,255),0.12f), 12.0f, 0, 1.0f);
        if(i==sel&&playing) DrawIcon(dl,Icon::Music,ImVec2(x+cellw-20,y+18),13.0f,IM_COL32(255,255,255,255),2.0f);
        dl->AddText(f,15.0f,ImVec2(x+12,y+ch-42),IM_COL32(255,255,255,255),t[i],NULL,cellw-20);
        dl->AddText(f,12.0f,ImVec2(x+12,y+ch-22),SA(IM_COL32(255,255,255,255),0.85f),s[i],NULL,cellw-20); }
      int rows=(n+cols-1)/cols; ImGui::Dummy(ImVec2(w, rows*(ch+gap)+4)); }
    { float h=64.0f; ImVec2 tl(x0, ImGui::GetCursorScreenPos().y);
      Primitive bg; bg.cx=tl.x+cw*0.5f; bg.cy=tl.y+h*0.5f; bg.hw=cw*0.5f; bg.hh=h*0.5f; bg.corner_radius=22; bg.fade=1; bg.material=Material::Thick; g->Submit(bg);
      Primitive ar; ar.cx=tl.x+36; ar.cy=tl.y+h*0.5f; ar.hw=20; ar.hh=20; ar.corner_radius=10; ar.fade=1; ar.material=Material::Accent; g->Submit(ar); DrawIcon(dl,Icon::Music,ImVec2(tl.x+36,tl.y+h*0.5f),18.0f,IM_COL32(255,255,255,255),2.0f);
      char np[64]; snprintf(np,64,"%s \xC2\xB7 %s",t[sel],s[sel]);
      dl->AddText(f,15.0f,ImVec2(tl.x+68,tl.y+14),kInk,np); dl->AddText(f,12.0f,ImVec2(tl.x+68,tl.y+36),kInkSoft,playing?"On air":"Paused");
      bool phv=false; if(SurfaceHit("radplay", ImVec2(tl.x+cw-50,tl.y+h*0.5f-22), ImVec2(44,44), &phv, playing?Sfx::Pause:Sfx::Play)) playing=!playing;
      DrawIcon(dl,playing?Icon::Pause:Icon::Play,ImVec2(tl.x+cw-28,tl.y+h*0.5f),18.0f,phv?Accent():kInk,2.0f);
      ImGui::Dummy(ImVec2(w, h+12)); }
}
static void Draw_Recents(float w){
    ImDrawList* dl=ImGui::GetWindowDrawList(); ImFont* f=ImGui::GetFont();
    float cw=w<600.0f?w:600.0f; float x0=ImGui::GetCursorScreenPos().x+(w-cw)*0.5f;
    dl->AddText(f,30.0f,ImVec2(x0+2,ImGui::GetCursorScreenPos().y),kInk,"Recents"); ImGui::Dummy(ImVec2(w,50.0f));
    struct R{ const char* n; const char* t; int kind; }; R rs[]={ {"Emma Wilson","9:41 AM",0},{"James Carter","9:12 AM",2},{"Mom","Yesterday",1},{"David Park","Yesterday",0},{"Video call: Sofia","Tuesday",3},{"Unknown","Monday",2},{"Dr. Reyes","Monday",0} };
    int n=7; float rowh=58.0f,h=n*rowh; ImVec2 tl(x0, ImGui::GetCursorScreenPos().y);
    Primitive bg; bg.cx=tl.x+cw*0.5f; bg.cy=tl.y+h*0.5f; bg.hw=cw*0.5f; bg.hh=h*0.5f; bg.corner_radius=24; bg.fade=1; bg.material=Material::Thin; g->Submit(bg);
    for(int i=0;i<n;i++){ float ry=tl.y+i*rowh,cy=ry+rowh*0.5f; bool missed=(rs[i].kind==2);
        Icon ic = rs[i].kind==3?Icon::Video:(rs[i].kind==1?Icon::Phone:(rs[i].kind==2?Icon::Phone:Icon::Phone));
        DrawIcon(dl, rs[i].kind==0?Icon::ArrowUp:(rs[i].kind==1?Icon::ArrowDown:(rs[i].kind==3?Icon::Video:Icon::Phone)), ImVec2(tl.x+24,cy),13.0f, missed?IM_COL32(255,69,58,255):kInkSoft,2.0f);
        dl->AddText(f,16.0f,ImVec2(tl.x+48,cy-9),missed?IM_COL32(255,69,58,255):kInk,rs[i].n);
        ImVec2 ts=f->CalcTextSizeA(13.0f,99999.0f,0,rs[i].t); dl->AddText(f,13.0f,ImVec2(tl.x+cw-ts.x-44,cy-8),kInkSoft,rs[i].t);
        DrawIcon(dl,Icon::InfoCircle,ImVec2(tl.x+cw-20,cy),15.0f,Accent(),2.0f); (void)ic;
        if(i<n-1) dl->AddLine(ImVec2(tl.x+48,ry+rowh),ImVec2(tl.x+cw-14,ry+rowh),SA(kInkSoft,0.14f),1.0f); }
    ImGui::Dummy(ImVec2(w, h+12));
}
static void Draw_TranslateCam(float w){
    ImDrawList* dl=ImGui::GetWindowDrawList(); ImFont* f=ImGui::GetFont();
    float cw=w<520.0f?w:520.0f; float x0=ImGui::GetCursorScreenPos().x+(w-cw)*0.5f;
    float h=400.0f; ImVec2 tl(x0, ImGui::GetCursorScreenPos().y);
    dl->AddRectFilledMultiColor(ImVec2(tl.x,tl.y),ImVec2(tl.x+cw,tl.y+h), IM_COL32(50,52,58,255),IM_COL32(70,72,82,255),IM_COL32(40,42,50,255),IM_COL32(60,58,66,255));
    dl->AddRect(ImVec2(tl.x,tl.y),ImVec2(tl.x+cw,tl.y+h),SA(IM_COL32(255,255,255,255),0.12f),20.0f,0,1.5f);
    dl->PushClipRect(ImVec2(tl.x,tl.y),ImVec2(tl.x+cw,tl.y+h),true);
    dl->AddRectFilled(ImVec2(tl.x+50,tl.y+70),ImVec2(tl.x+cw-50,tl.y+150),SA(IM_COL32(230,225,210,255),0.85f),8.0f);
    dl->AddText(f,14.0f,ImVec2(tl.x+64,tl.y+84),SA(IM_COL32(60,55,45,255),0.5f),"Salida de emergencia");
    dl->AddRectFilled(ImVec2(tl.x+60,tl.y+108),ImVec2(tl.x+cw-60,tl.y+140),Accent(),6.0f); ImVec2 ts=f->CalcTextSizeA(18.0f,99999.0f,0,"Emergency Exit"); dl->AddText(f,18.0f,ImVec2(tl.x+cw*0.5f-ts.x*0.5f,tl.y+114),IM_COL32(255,255,255,255),"Emergency Exit");
    dl->AddRectFilled(ImVec2(tl.x+90,tl.y+220),ImVec2(tl.x+cw-90,tl.y+270),SA(IM_COL32(230,225,210,255),0.85f),8.0f);
    dl->AddRectFilled(ImVec2(tl.x+100,tl.y+232),ImVec2(tl.x+cw-100,tl.y+258),Accent(),6.0f); ImVec2 t2=f->CalcTextSizeA(16.0f,99999.0f,0,"Push to open"); dl->AddText(f,16.0f,ImVec2(tl.x+cw*0.5f-t2.x*0.5f,tl.y+237),IM_COL32(255,255,255,255),"Push to open");
    dl->AddRect(ImVec2(tl.x+cw*0.5f-40,tl.y+h-80),ImVec2(tl.x+cw*0.5f+40,tl.y+h-40),IM_COL32(255,214,90,255),6.0f,0,2.0f);
    dl->PopClipRect();
    dl->AddText(f,13.0f,ImVec2(tl.x+20,tl.y+20),SA(IM_COL32(255,255,255,255),0.85f),"Spanish \xE2\x86\x92 English");
    ImGui::Dummy(ImVec2(w, h+14));
}
static void Draw_Counter(float w){
    ImDrawList* dl=ImGui::GetWindowDrawList(); ImFont* f=ImGui::GetFont();
    static int count=0; float cw=w<460.0f?w:460.0f; float x0=ImGui::GetCursorScreenPos().x+(w-cw)*0.5f;
    char nb[16]; snprintf(nb,16,"%d",count); ImVec2 ns=f->CalcTextSizeA(96.0f,99999.0f,0,nb);
    dl->AddText(f,96.0f, ImVec2(x0+cw*0.5f-ns.x*0.5f, ImGui::GetCursorScreenPos().y+30), count<0?IM_COL32(255,69,58,255):kInk, nb);
    ImGui::Dummy(ImVec2(w,170.0f));
    float bw=(cw-2*12)/3.0f; ImGui::SetCursorScreenPos(ImVec2(x0, ImGui::GetCursorScreenPos().y));
    if(Glass::Button("\xE2\x88\x92", false, ImVec2(bw,54))) count--; ImGui::SameLine(0,12);
    if(Glass::Button("Reset", false, ImVec2(bw,54))) count=0; ImGui::SameLine(0,12);
    if(Glass::Button("+", true, ImVec2(bw,54))) count++;
    ImGui::Dummy(ImVec2(w,10.0f));
}
static void Draw_ColorMixer(float w){
    ImDrawList* dl=ImGui::GetWindowDrawList(); ImFont* f=ImGui::GetFont();
    static float r=0.13f,g2=0.55f,b=0.95f; float cw=w<480.0f?w:480.0f; float x0=ImGui::GetCursorScreenPos().x+(w-cw)*0.5f;
    ImU32 col=IM_COL32((int)(r*255),(int)(g2*255),(int)(b*255),255);
    { ImVec2 tl(x0, ImGui::GetCursorScreenPos().y); float h=140.0f;
      dl->AddRectFilled(ImVec2(tl.x,tl.y),ImVec2(tl.x+cw,tl.y+h), col, 20.0f);
      dl->AddRect(ImVec2(tl.x,tl.y),ImVec2(tl.x+cw,tl.y+h), SA(IM_COL32(255,255,255,255),0.2f),20.0f,0,1.5f);
      char hx[16]; snprintf(hx,16,"#%02X%02X%02X",(int)(r*255),(int)(g2*255),(int)(b*255));
      float lum = r*0.299f+g2*0.587f+b*0.114f; ImU32 tc = lum>0.6f?IM_COL32(0,0,0,255):IM_COL32(255,255,255,255);
      ImVec2 hs=f->CalcTextSizeA(28.0f,99999.0f,0,hx); dl->AddText(f,28.0f,ImVec2(tl.x+cw*0.5f-hs.x*0.5f,tl.y+h*0.5f-14),tc,hx);
      ImGui::Dummy(ImVec2(w,h+18)); }
    ImGui::Indent((w-cw)*0.5f); ImGui::PushItemWidth(cw);
    Glass::Slider("Red",   &r,  0.0f,1.0f,"%.2f");
    Glass::Slider("Green", &g2, 0.0f,1.0f,"%.2f");
    Glass::Slider("Blue",  &b,  0.0f,1.0f,"%.2f");
    ImGui::PopItemWidth(); ImGui::Unindent((w-cw)*0.5f);
}
static void Draw_TipCalc(float w){
    ImDrawList* dl=ImGui::GetWindowDrawList(); ImFont* f=ImGui::GetFont();
    static float bill=42.50f; static int tipIdx=2; static int split=2; float cw=w<480.0f?w:480.0f; float x0=ImGui::GetCursorScreenPos().x+(w-cw)*0.5f;
    float pcts[4]={0.15f,0.18f,0.20f,0.25f}; float tip=bill*pcts[tipIdx]; float total=bill+tip; float per=total/(split<1?1:split);
    { ImVec2 tl(x0, ImGui::GetCursorScreenPos().y); float h=120.0f;
      Primitive bg; bg.cx=tl.x+cw*0.5f; bg.cy=tl.y+h*0.5f; bg.hw=cw*0.5f; bg.hh=h*0.5f; bg.corner_radius=26; bg.fade=1; bg.material=Material::Accent; g->Submit(bg);
      char tb[24]; snprintf(tb,24,"$%.2f",per); ImVec2 ts=f->CalcTextSizeA(46.0f,99999.0f,0,tb); dl->AddText(f,46.0f,ImVec2(tl.x+cw*0.5f-ts.x*0.5f,tl.y+24),IM_COL32(255,255,255,255),tb);
      dl->AddText(f,14.0f,ImVec2(tl.x+18,tl.y+88),SA(IM_COL32(255,255,255,255),0.85f),"per person");
      char tt[32]; snprintf(tt,32,"Total $%.2f",total); ImVec2 tts=f->CalcTextSizeA(14.0f,99999.0f,0,tt); dl->AddText(f,14.0f,ImVec2(tl.x+cw-tts.x-18,tl.y+88),SA(IM_COL32(255,255,255,255),0.85f),tt);
      ImGui::Dummy(ImVec2(w,h+16)); }
    ImGui::Indent((w-cw)*0.5f); ImGui::PushItemWidth(cw);
    Glass::Slider("Bill", &bill, 0.0f, 300.0f, "$%.2f");
    { const char* tp[4]={"15%","18%","20%","25%"}; tipIdx=Glass::SegmentedControl("tip",tp,4,tipIdx); }
    Glass::Stepper("Split", &split, 1, 1, 20);
    ImGui::PopItemWidth(); ImGui::Unindent((w-cw)*0.5f);
}
static void Draw_Dice(float w){
    ImDrawList* dl=ImGui::GetWindowDrawList();
    static int d1=3,d2=5; static unsigned seed=2463534242u; float cw=w<460.0f?w:460.0f; float x0=ImGui::GetCursorScreenPos().x+(w-cw)*0.5f;
    float dsz=110.0f, gap=30.0f; float dy=ImGui::GetCursorScreenPos().y+30.0f;
    auto die=[&](float dx,int v){ dl->AddRectFilled(ImVec2(dx,dy),ImVec2(dx+dsz,dy+dsz), kInk, 18.0f);
        ImU32 pip=IM_COL32(20,20,24,255); float u=dsz*0.26f,c=dsz*0.5f,e=dsz*0.74f;
        auto P=[&](float px,float py){ dl->AddCircleFilled(ImVec2(dx+px,dy+py),dsz*0.072f,pip); };
        if(v==1||v==3||v==5) P(c,c);
        if(v>=2){ P(u,u); P(e,e); }
        if(v>=4){ P(e,u); P(u,e); }
        if(v==6){ P(u,c); P(e,c); } };
    die(x0+cw*0.5f-dsz-gap*0.5f, d1); die(x0+cw*0.5f+gap*0.5f, d2);
    ImGui::Dummy(ImVec2(w,dsz+50.0f));
    ImGui::SetCursorScreenPos(ImVec2(x0+cw*0.5f-90, ImGui::GetCursorScreenPos().y));
    if(Glass::Button("Roll", true, ImVec2(180,50))){ seed^=seed<<13; seed^=seed>>17; seed^=seed<<5; d1=(seed%6)+1; seed^=seed<<13; seed^=seed>>17; seed^=seed<<5; d2=(seed%6)+1; }
    char tb[16]; snprintf(tb,16,"Total: %d",d1+d2); ImFont* f=ImGui::GetFont(); ImVec2 ts=f->CalcTextSizeA(16.0f,99999.0f,0,tb);
    dl->AddText(f,16.0f,ImVec2(x0+cw*0.5f-ts.x*0.5f, ImGui::GetCursorScreenPos().y+58),kInkSoft,tb);
    ImGui::Dummy(ImVec2(w,80.0f));
}
static void Draw_UnitConv(float w){
    ImDrawList* dl=ImGui::GetWindowDrawList(); ImFont* f=ImGui::GetFont();
    static int cat=0; static float val=100.0f; float cw=w<520.0f?w:520.0f; float x0=ImGui::GetCursorScreenPos().x+(w-cw)*0.5f;
    const char* fromU[3]={"km","kg","\xC2\xB0""C"}; const char* toU[3]={"mi","lb","\xC2\xB0""F"};
    float out = cat==0 ? val*0.621371f : (cat==1 ? val*2.20462f : val*9.0f/5.0f+32.0f);
    { ImVec2 tl(x0, ImGui::GetCursorScreenPos().y); float h=130.0f;
      Primitive bg; bg.cx=tl.x+cw*0.5f; bg.cy=tl.y+h*0.5f; bg.hw=cw*0.5f; bg.hh=h*0.5f; bg.corner_radius=26; bg.fade=1; bg.material=Material::Thin; g->Submit(bg);
      char ib[32],ob[32]; snprintf(ib,32,"%.2f %s",val,fromU[cat]); snprintf(ob,32,"%.2f %s",out,toU[cat]);
      dl->AddText(f,30.0f,ImVec2(tl.x+20,tl.y+18),kInk,ib);
      DrawIcon(dl,Icon::ArrowDown,ImVec2(tl.x+34,tl.y+72),16.0f,Accent(),2.0f);
      dl->AddText(f,30.0f,ImVec2(tl.x+20,tl.y+82),Accent(),ob);
      ImGui::Dummy(ImVec2(w,h+16)); }
    ImGui::Indent((w-cw)*0.5f); ImGui::PushItemWidth(cw);
    { const char* ct[3]={"Length","Weight","Temp"}; cat=Glass::SegmentedControl("uc",ct,3,cat); }
    Glass::Slider("Value", &val, cat==2?-40.0f:0.0f, cat==2?120.0f:500.0f, "%.1f");
    ImGui::PopItemWidth(); ImGui::Unindent((w-cw)*0.5f);
}
static void Draw_Piano(float w){
    ImDrawList* dl=ImGui::GetWindowDrawList(); ImFont* f=ImGui::GetFont();
    static int lastKey=-1; float cw=w<660.0f?w:660.0f; float x0=ImGui::GetCursorScreenPos().x+(w-cw)*0.5f;
    const char* names[7]={"C","D","E","F","G","A","B"};
    float top=ImGui::GetCursorScreenPos().y+10.0f, wkW=cw/7.0f, wkH=220.0f;
    for(int i=0;i<7;i++){ float kx=x0+i*wkW; ImGui::PushID(i); ImGui::SetCursorScreenPos(ImVec2(kx,top)); ImGui::InvisibleButton("wk",ImVec2(wkW-2,wkH));
        bool act=ImGui::IsItemActive(); if(ImGui::IsItemActivated()) lastKey=i; ImGui::PopID();
        dl->AddRectFilled(ImVec2(kx+1,top),ImVec2(kx+wkW-1,top+wkH), act?SA(Accent(),0.85f):IM_COL32(248,248,250,255), 6.0f);
        dl->AddText(f,15.0f,ImVec2(kx+wkW*0.5f-5,top+wkH-28),act?IM_COL32(255,255,255,255):IM_COL32(40,40,46,255),names[i]); }
    int bk[5]={0,1,3,4,5}; float bw=wkW*0.6f, bh=wkH*0.6f;
    for(int j=0;j<5;j++){ float kx=x0+(bk[j]+1)*wkW-bw*0.5f; ImGui::PushID(100+j); ImGui::SetCursorScreenPos(ImVec2(kx,top)); ImGui::InvisibleButton("bk",ImVec2(bw,bh));
        bool act=ImGui::IsItemActive(); if(ImGui::IsItemActivated()) lastKey=100+j; ImGui::PopID();
        dl->AddRectFilled(ImVec2(kx,top),ImVec2(kx+bw,top+bh), act?Accent():IM_COL32(28,28,32,255), 4.0f); }
    ImGui::Dummy(ImVec2(w,wkH+24.0f));
    const char* note = lastKey<0?"Tap a key":(lastKey<7?names[lastKey]:"sharp");
    char nb[24]; if(lastKey>=0&&lastKey<7) snprintf(nb,24,"Note: %s",names[lastKey]); else if(lastKey>=100) snprintf(nb,24,"Note: %s#",names[bk[lastKey-100]]); else snprintf(nb,24,"Tap a key");
    ImVec2 ts=f->CalcTextSizeA(18.0f,99999.0f,0,nb); dl->AddText(f,18.0f,ImVec2(x0+cw*0.5f-ts.x*0.5f,ImGui::GetCursorScreenPos().y),kInk,nb); (void)note;
    ImGui::Dummy(ImVec2(w,30.0f));
}
