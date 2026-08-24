static int DrawMiniTabs(const char* id, const char* const* names, const Icon* icons, int n, int sel, float w){
    ImDrawList* dl=ImGui::GetWindowDrawList(); ImFont* f=ImGui::GetFont(); float cw=w<660.0f?w:660.0f, x0=ImGui::GetCursorScreenPos().x+(w-cw)*0.5f, h=46.0f, tabw=cw/(float)n, y=ImGui::GetCursorScreenPos().y;
    Primitive bg; bg.cx=x0+cw*0.5f; bg.cy=y+h*0.5f; bg.hw=cw*0.5f; bg.hh=h*0.5f; bg.corner_radius=h*0.5f; bg.fade=1.0f; bg.material=Material::Thin; g->Submit(bg);
    ImGuiID mtid=ImGui::GetID(id); Spring& sp=g->springs().Get((uint32_t)mtid,11,SpringStyle::Critical,(float)sel); sp.target=(float)sel; sp.Tick(Dt()); float sx=Clampf(sp.x,0.0f,(float)(n-1));
    Spring& tl=g->springs().Get((uint32_t)mtid,12,SpringStyle::Overdamped,(float)sel); tl.target=sx; tl.Tick(Dt()); float tlx=Clampf(tl.x,0.0f,(float)(n-1));
    float pcy=y+h*0.5f, phw=tabw*0.5f-4.0f, phh=h*0.5f-4.0f, pr=(h-8.0f)*0.5f, hX=x0+(sx+0.5f)*tabw, tX=x0+(tlx+0.5f)*tabw;
    if (std::fabs(hX-tX)>1.5f) SubmitBlob(hX,pcy,phw,phh,tX,pcy,phw,phh,pr,16.0f,1.0f,Material::Accent);
    else { Primitive pill; pill.cx=hX; pill.cy=pcy; pill.hw=phw; pill.hh=phh; pill.corner_radius=pr; pill.fade=1.0f; pill.material=Material::Accent; g->Submit(pill); }
    int res=sel;
    for (int i=0;i<n;i++){
        float tx=x0+i*tabw, tcx=tx+tabw*0.5f, tcy=y+h*0.5f;
        ImGui::PushID(i); ImGui::SetCursorScreenPos(ImVec2(tx,y)); ImGui::InvisibleButton("mt",ImVec2(tabw,h)); if (ImGui::IsItemDeactivated()&&ImGui::IsItemHovered()) res=i; ImGui::PopID();
        ImU32 col=(i==sel)?IM_COL32(255,255,255,255):kInk; ImVec2 ns=f->CalcTextSizeA(15.0f,99999.0f,0.0f,names[i]); bool showIcon=tabw>84.0f; float iw=showIcon?23.0f:0.0f, sxs=tcx-(iw+ns.x)*0.5f;
        if (showIcon) DrawIcon(dl,icons[i],ImVec2(sxs+8.0f,tcy),16.0f,col,2.0f); dl->AddText(f,15.0f,ImVec2(sxs+iw,tcy-ns.y*0.5f),col,names[i]);
    }
    ImGui::Dummy(ImVec2(w,h+14.0f)); return res;
}
static int GroupTabs(const char* id, const int* m, const char* const* n, const Icon* ic, int cnt, int t, float w){ if (t<0||t>=cnt) t=0; t=DrawMiniTabs(id,n,ic,cnt,t,w); SurfaceDraw(m[t],w); return t; }

static void Draw_GroupMedia  (float w){ static const int M[]={1,21,38,97,54};   static const char* N[]={"Music","Podcasts","TV","Radio","Shazam"};        static const Icon I[]={Icon::Music,Icon::Microphone,Icon::Tv,Icon::Wifi,Icon::Music};            static int t; t=GroupTabs("gmedia",M,N,I,5,t,w); }
static void Draw_GroupWeb    (float w){ static const int M[]={42,8,65};         static const char* N[]={"Browser","Maps","Transit"};                      static const Icon I[]={Icon::Compass,Icon::Map,Icon::Map};                                       static int t; t=GroupTabs("gweb",M,N,I,3,t,w); }
static void Draw_GroupConnect(float w){ static const int M[]={9,10,29,43,44};   static const char* N[]={"Messages","Mail","Video Calls","Phone","Contacts"}; static const Icon I[]={Icon::Comment,Icon::Envelope,Icon::Video,Icon::Phone,Icon::Person};       static int t; t=GroupTabs("gconn",M,N,I,5,t,w); }
static void Draw_GroupWork   (float w){ static const int M[]={7,20,16,12,32};   static const char* N[]={"Files","Notes","Calendar","Reminders","Freeform"};static const Icon I[]={Icon::Folder,Icon::PenEdit,Icon::CalendarDays,Icon::CheckCircle,Icon::PenEdit}; static int t; t=GroupTabs("gwork",M,N,I,5,t,w); }
static void Draw_GroupPhotos (float w){ static const int M[]={4,89,17};         static const char* N[]={"Photos","Albums","Camera"};                      static const Icon I[]={Icon::Images,Icon::Images,Icon::Camera};                                  static int t; t=GroupTabs("gphoto",M,N,I,3,t,w); }
static void Draw_GroupTools  (float w){ static const int M[]={106,104,102,100,101}; static const char* N[]={"Calc","Units","Tip","Counter","Color"};        static const Icon I[]={Icon::Calculator,Icon::Refresh,Icon::CreditCard,Icon::Plus,Icon::Palette};static int t; t=GroupTabs("gtool",M,N,I,5,t,w); }
static void Draw_GroupSystem (float w){ static const int M[]={3,46,62,86,63};   static const char* N[]={"Settings","Battery","Storage","Display","About"}; static const Icon I[]={Icon::Gear,Icon::BatteryFull,Icon::Database,Icon::Display,Icon::InfoCircle};static int t; t=GroupTabs("gsys",M,N,I,5,t,w); }
static void Draw_GroupHealth (float w){ static const int M[]={13,26,80,94,55};  static const char* N[]={"Health","Fitness","Steps","Heart","Sleep"};      static const Icon I[]={Icon::Heart,Icon::Dumbbell,Icon::Running,Icon::Heart,Icon::Bed};          static int t; t=GroupTabs("ghlth",M,N,I,5,t,w); }
static void Draw_GroupWallet (float w){ static const int M[]={14,67,96};        static const char* N[]={"Cards","Payments","Boarding"};                    static const Icon I[]={Icon::CreditCard,Icon::CreditCard,Icon::Airplane};                        static int t; t=GroupTabs("gwlt",M,N,I,3,t,w); }
static void Draw_GroupToday  (float w){ static const int M[]={0,2,6,27,40};     static const char* N[]={"Weather","Clock","Stocks","News","Widgets"};      static const Icon I[]={Icon::CloudSun,Icon::Clock,Icon::Bolt,Icon::Newspaper,Icon::Grid};        static int t; t=GroupTabs("gtdy",M,N,I,5,t,w); }
static void Draw_GroupCreate (float w){ static const int M[]={47,58,56,57};     static const char* N[]={"GarageBand","iMovie","Pages","Keynote"};         static const Icon I[]={Icon::Guitar,Icon::Film,Icon::FileLines,Icon::Display};                   static int t; t=GroupTabs("gcrt",M,N,I,4,t,w); }
