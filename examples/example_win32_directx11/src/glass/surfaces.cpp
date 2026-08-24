#define _CRT_SECURE_NO_WARNINGS
#include "surfaces.h"
#include "imgui.h"
#include <cmath>
#include <cstdio>
#include <cstring>
#include <ctime>

namespace Glass {

static inline float Clampf(float x, float a, float b){ return x<a?a:(x>b?b:x); }
static inline float Dt(){ return ImGui::GetIO().DeltaTime; }

#define kInk     (InkColor())
#define kInkSoft (InkSoftColor())
static ImU32 Accent(){ const MaterialParams& p = Params(Material::Accent);
    return IM_COL32((int)(p.tint_rgb[0]*255), (int)(p.tint_rgb[1]*255), (int)(p.tint_rgb[2]*255), 255); }
static inline ImU32 SA(ImU32 c, float a){ a=Clampf(a,0,1); ImU32 al=(ImU32)(((c>>24)&0xFF)*a); return (c&0x00FFFFFFu)|(al<<24); }

static void TextC(ImDrawList* dl, ImFont* f, float sz, ImVec2 center, ImU32 col, const char* s){
    sz *= Glass::WidgetScale();
    ImVec2 ts = f->CalcTextSizeA(sz, 99999.0f, 0.0f, s);
    dl->AddText(f, sz, ImVec2(center.x - ts.x*0.5f, center.y - ts.y*0.5f), col, s);
}

static void FmtClock(char* b, int n, int h24, int mn){
    if (Use24Hour()) snprintf(b, n, "%d:%02d", h24, mn);
    else             snprintf(b, n, "%d:%02d", (h24%12==0)?12:h24%12, mn);
}

static ImU32 Bilerp4(ImU32 ctl, ImU32 ctr, ImU32 cbr, ImU32 cbl, float u, float v){
    auto C=[&](ImU32 c,int s)->float{ return (float)((c>>s)&0xFFu); };
    float iu=1.0f-u, iv=1.0f-v;
    float r =(C(ctl,0)*iu + C(ctr,0)*u)*iv + (C(cbl,0)*iu + C(cbr,0)*u)*v;
    float gg=(C(ctl,8)*iu + C(ctr,8)*u)*iv + (C(cbl,8)*iu + C(cbr,8)*u)*v;
    float bb=(C(ctl,16)*iu+ C(ctr,16)*u)*iv+ (C(cbl,16)*iu+ C(cbr,16)*u)*v;
    float aa=(C(ctl,24)*iu+ C(ctr,24)*u)*iv+ (C(cbl,24)*iu+ C(cbr,24)*u)*v;
    return IM_COL32((int)(r+0.5f),(int)(gg+0.5f),(int)(bb+0.5f),(int)(aa+0.5f));
}

static void RoundedGrad(ImDrawList* dl, ImVec2 a, ImVec2 b, ImU32 ctl, ImU32 ctr, ImU32 cbr, ImU32 cbl, float rnd, ImDrawFlags flags=ImDrawFlags_RoundCornersAll){
    if (b.x-a.x < 1.0f || b.y-a.y < 1.0f) return;
    dl->PathRect(a, b, rnd, flags);
    int n = dl->_Path.Size;
    if (n < 3){ dl->_Path.Size = 0; dl->AddRectFilledMultiColor(a, b, ctl, ctr, cbr, cbl); return; }
    float w = b.x-a.x, h = b.y-a.y;
    ImVec2 ctrp((a.x+b.x)*0.5f,(a.y+b.y)*0.5f);
    ImVec2 uv = ImGui::GetIO().Fonts->TexUvWhitePixel;
    bool aa = (dl->Flags & ImDrawListFlags_AntiAliasedFill) != 0;
    if (!aa){
        dl->PrimReserve(n*3, n+1);
        unsigned int base = dl->_VtxCurrentIdx;
        dl->PrimWriteVtx(ctrp, uv, Bilerp4(ctl,ctr,cbr,cbl,0.5f,0.5f));
        for (int i=0;i<n;i++){ ImVec2 p=dl->_Path[i]; dl->PrimWriteVtx(p, uv, Bilerp4(ctl,ctr,cbr,cbl,(p.x-a.x)/w,(p.y-a.y)/h)); }
        for (int i=0;i<n;i++){ dl->PrimWriteIdx((ImDrawIdx)base); dl->PrimWriteIdx((ImDrawIdx)(base+1+i)); dl->PrimWriteIdx((ImDrawIdx)(base+1+((i+1)%n))); }
        dl->_Path.Size = 0; return;
    }
    const float AA = 1.30f;
    dl->PrimReserve(n*9, 2*n+1);
    unsigned int base = dl->_VtxCurrentIdx, innerB = base+1, outerB = base+1+(unsigned)n;
    dl->PrimWriteVtx(ctrp, uv, Bilerp4(ctl,ctr,cbr,cbl,0.5f,0.5f));
    for (int i=0;i<n;i++){ ImVec2 p=dl->_Path[i]; dl->PrimWriteVtx(p, uv, Bilerp4(ctl,ctr,cbr,cbl,(p.x-a.x)/w,(p.y-a.y)/h)); }
    for (int i=0;i<n;i++){ ImVec2 p=dl->_Path[i]; float dx=p.x-ctrp.x, dy=p.y-ctrp.y; float l=sqrtf(dx*dx+dy*dy); if(l<1e-4f)l=1e-4f;
        dl->PrimWriteVtx(ImVec2(p.x+dx/l*AA, p.y+dy/l*AA), uv, Bilerp4(ctl,ctr,cbr,cbl,(p.x-a.x)/w,(p.y-a.y)/h) & 0x00FFFFFFu); }
    for (int i=0;i<n;i++){ int j=(i+1)%n; dl->PrimWriteIdx((ImDrawIdx)base); dl->PrimWriteIdx((ImDrawIdx)(innerB+i)); dl->PrimWriteIdx((ImDrawIdx)(innerB+j)); }
    for (int i=0;i<n;i++){ int j=(i+1)%n;
        dl->PrimWriteIdx((ImDrawIdx)(innerB+i)); dl->PrimWriteIdx((ImDrawIdx)(innerB+j)); dl->PrimWriteIdx((ImDrawIdx)(outerB+j));
        dl->PrimWriteIdx((ImDrawIdx)(innerB+i)); dl->PrimWriteIdx((ImDrawIdx)(outerB+j)); dl->PrimWriteIdx((ImDrawIdx)(outerB+i)); }
    dl->_Path.Size = 0;
}

static void ImgOrGrad(ImDrawList* dl, ImVec2 a, ImVec2 b, int idx, ImU32 c1, ImU32 c2, float rnd){
    void* t = PhotoTex(idx);
    if (t) dl->AddImageRounded((ImTextureID)(intptr_t)t, a, b, ImVec2(0,0), ImVec2(1,1), IM_COL32(255,255,255,255), rnd);
    else   RoundedGrad(dl, a, b, c1, c2, c2, c1, rnd);
}

static inline char LowerC(char c){ return (c>='A'&&c<='Z') ? (char)(c+32) : c; }
static bool ContainsCI(const char* hay, const char* needle){
    if (!needle || !*needle) return true; if (!hay) return false;
    for (const char* h=hay; *h; ++h){ const char* a=h; const char* b=needle;
        while (*a && *b && LowerC(*a)==LowerC(*b)) { ++a; ++b; }
        if (!*b) return true; }
    return false;
}

static bool CalcKey(const char* id, ImVec2 c, float bw, float bh, const char* label,
                    Material m, ImU32 labelCol, float fontSz){
    ImGui::PushID(id);
    ImGui::SetCursorScreenPos(ImVec2(c.x - bw*0.5f, c.y - bh*0.5f));
    ImGui::InvisibleButton("k", ImVec2(bw, bh));
    bool hov = ImGui::IsItemHovered();
    bool clicked = ImGui::IsItemDeactivated() && hov;
    if (ImGui::IsItemActivated()) PlaySfx(Sfx::CalcKey);
    Spring& s = g->springs().Get((uint32_t)ImGui::GetID("k"), 1, SpringStyle::Bouncy, 1.0f);
    s.target = ImGui::IsItemActive() ? 0.90f : (hov ? 1.05f : 1.0f);
    s.Tick(Dt());
    float sc = Clampf(s.x, 0.8f, 1.2f);
    Primitive b; b.cx=c.x; b.cy=c.y; b.hw=bw*0.5f*sc; b.hh=bh*0.5f*sc;
    b.corner_radius = bh*0.5f*sc; b.fade=1.0f; b.material=m; b.elevate = hov?1.12f:1.0f;
    g->Submit(b);
    TextC(ImGui::GetWindowDrawList(), ImGui::GetFont(), fontSz, c, labelCol, label);
    ImGui::PopID();
    return clicked;
}

static float TileOn(const char* slotid, bool on){
    Spring& s=g->springs().Get((uint32_t)ImGui::GetID(slotid),9,SpringStyle::Critical,on?1.0f:0.0f);
    s.target=on?1.0f:0.0f; s.Tick(Dt()); return Clampf(s.x,0.0f,1.0f);
}

static bool SurfaceHit(const char* id, ImVec2 tl, ImVec2 sz, bool* hovOut=nullptr, Sfx clickSfx=Sfx::Tap){
    ImVec2 keep = ImGui::GetCursorScreenPos();
    ImGui::PushID(id);
    ImGui::SetCursorScreenPos(tl);
    ImGui::InvisibleButton("##hit", ImVec2(sz.x<1.0f?1.0f:sz.x, sz.y<1.0f?1.0f:sz.y), ImGuiButtonFlags_AllowOverlap);
    bool hov = ImGui::IsItemHovered();
    bool clk = ImGui::IsItemDeactivated() && hov;
    if (ImGui::IsItemActivated()) PlaySfx(clickSfx);
    ImGui::PopID();
    ImGui::SetCursorScreenPos(keep);
    if (hovOut) *hovOut = hov;
    return clk;
}

static void RowWash(ImDrawList* dl, ImVec2 tl, ImVec2 br, float rnd, bool sel, bool hov){
    if (sel)      dl->AddRectFilled(tl, br, SA(Accent(), 0.20f), rnd);
    else if (hov) dl->AddRectFilled(tl, br, SA(kInkSoft, 0.10f), rnd);
}
#include "surfaces/part1.inc"
#include "surfaces/part2.inc"
#include "surfaces/part3.inc"
#include "surfaces/part4.inc"
#include "surfaces/part5.inc"
#include "surfaces/part6.inc"
struct SurfaceDef { const char* name; const char* sub; Icon icon; ImU32 tint; void(*draw)(float); };
static SurfaceDef g_surfaces[] = {
    { "Weather",    "Today & 10-day",  Icon::CloudSun,   IM_COL32(64,156,255,255), Draw_Weather     },
    { "Music",      "Now Playing",     Icon::Music,      IM_COL32(255,55,95,255),  Draw_Music       },
    { "Clock",      "World & local",   Icon::Clock,      IM_COL32(255,159,10,255), Draw_Clock       },
    { "Settings",   "System settings", Icon::Gear,       IM_COL32(142,142,147,255),Draw_Settings    },
    { "Photos",     "Library & memories", Icon::Images,  IM_COL32(255,90,140,255), Draw_Photos      },
    { "Notifications","Center & widgets", Icon::Bell,    IM_COL32(255,69,58,255),  Draw_NotifCenter },
    { "Stocks",     "Markets & charts", Icon::Bolt,      IM_COL32(48,209,88,255),  Draw_Stocks      },
    { "Files",      "File browser",    Icon::Folder,     IM_COL32(64,156,255,255), Draw_Files       },
    { "Maps",       "Find your way",   Icon::Map,        IM_COL32(76,200,120,255), Draw_Maps        },
    { "Messages",   "Conversations",   Icon::Comment,    IM_COL32(48,209,88,255),  Draw_Messages    },
    { "Mail",       "Inbox",           Icon::Envelope,   IM_COL32(64,156,255,255), Draw_Mail        },
    { "Home Screen","Apps & dock",     Icon::House,      IM_COL32(94,92,230,255),  Draw_Home        },
    { "Reminders",  "To-do & lists",   Icon::CheckCircle,IM_COL32(255,149,0,255),  Draw_Reminders   },
    { "Health",     "Activity rings",  Icon::Heart,      IM_COL32(255,55,95,255),  Draw_Health      },
    { "Cards",      "Cards & passes",  Icon::CreditCard, IM_COL32(60,60,64,255),   Draw_Wallet      },
    { "Lock Screen","Clock & glances", Icon::Lock,       IM_COL32(120,120,128,255),Draw_Lock        },
    { "Calendar",   "Month & agenda",  Icon::CalendarDays,IM_COL32(255,69,58,255), Draw_Calendar    },
    { "Camera",     "Viewfinder",      Icon::Camera,     IM_COL32(120,120,128,255),Draw_Camera      },
    { "Spotlight",  "Search anything", Icon::Search,     IM_COL32(94,92,230,255),  Draw_Spotlight   },
    { "App Store",  "Discover apps",   Icon::Bag,        IM_COL32(64,156,255,255), Draw_AppStore    },
    { "Notes",      "Jot it down",     Icon::PenEdit,    IM_COL32(255,214,90,255), Draw_Notes       },
    { "Podcasts",   "Listen",          Icon::Microphone, IM_COL32(146,82,232,255), Draw_Podcasts    },
    { "Voice Memos","Record",          Icon::Microphone, IM_COL32(255,69,58,255),  Draw_VoiceMemos  },
    { "Keyboard",   "Type it out",     Icon::Keyboard,   IM_COL32(120,120,128,255),Draw_Keyboard    },
    { "Translate",  "Any language",    Icon::Globe,      IM_COL32(64,156,255,255), Draw_Translate   },
    { "Control Ctr","Toggles & sliders",Icon::Sliders,   IM_COL32(94,92,230,255),  Draw_CControl    },
    { "Fitness",    "Move & workouts", Icon::Dumbbell,   IM_COL32(126,255,40,255), Draw_Fitness     },
    { "News",       "Top stories",     Icon::Newspaper,  IM_COL32(255,69,58,255),  Draw_News        },
    { "Books",      "Reading list",    Icon::Book,       IM_COL32(255,149,0,255),  Draw_Books       },
    { "Video Calls","Group call",      Icon::Video,      IM_COL32(48,209,88,255),  Draw_FaceTime    },
    { "Device Finder","People & devices",Icon::LocationArrow,IM_COL32(48,209,88,255), Draw_FindMy      },
    { "Shortcuts",  "Automations",     Icon::Bolt,       IM_COL32(146,82,232,255), Draw_Shortcuts   },
    { "Freeform",   "Whiteboard",      Icon::PenEdit,    IM_COL32(64,156,255,255), Draw_Freeform    },
    { "Game Center","Achievements",    Icon::Trophy,     IM_COL32(255,159,10,255), Draw_GameCenter  },
    { "Components", "Glass toolkit",   Icon::Cube,       IM_COL32(94,92,230,255),  Draw_Components  },
    { "Compass",    "Find north",      Icon::Compass,    IM_COL32(255,69,58,255),  Draw_Compass     },
    { "Mindfulness","Breathe",         Icon::Leaf,       IM_COL32(48,209,88,255),  Draw_Breathe     },
    { "Timer",      "Countdown",       Icon::Stopwatch,  IM_COL32(255,159,10,255), Draw_Timer       },
    { "TV",         "Movies & shows",  Icon::Tv,         IM_COL32(40,44,70,255),   Draw_TV          },
    { "Sports",     "Live scores",     Icon::Trophy,     IM_COL32(48,209,88,255),  Draw_Sports      },
    { "Widgets",    "Today gallery",   Icon::Grid,       IM_COL32(64,156,255,255), Draw_Widgets     },
    { "Alarms",     "Wake & sleep",    Icon::Bell,       IM_COL32(255,69,58,255),  Draw_Alarms      },
    { "Browser",    "Browse the web",  Icon::Compass,    IM_COL32(64,156,255,255), Draw_Safari      },
    { "Phone",      "Keypad",          Icon::Phone,      IM_COL32(48,209,88,255),  Draw_Phone       },
    { "Contacts",   "People",          Icon::Person,     IM_COL32(120,120,128,255),Draw_Contacts    },
    { "Home",       "Smart home",      Icon::House,      IM_COL32(255,159,10,255), Draw_HomeKit     },
    { "Battery",    "Usage graph",     Icon::BatteryFull,IM_COL32(48,209,88,255),  Draw_Battery     },
    { "Studio",     "Make music",      Icon::Guitar,     IM_COL32(255,159,10,255), Draw_GarageBand  },
    { "Watch",      "Faces",           Icon::Clock,      IM_COL32(255,69,58,255),  Draw_Watch       },
    { "Headphones", "Battery",         Icon::Headphones, IM_COL32(120,120,128,255),Draw_AirPods     },
    { "Journal",    "Reflect",         Icon::Book,       IM_COL32(255,159,10,255), Draw_Journal     },
    { "Numbers",    "Spreadsheet",     Icon::Grid,       IM_COL32(48,209,88,255),  Draw_Numbers     },
    { "Measure",    "AR ruler",        Icon::Crosshairs, IM_COL32(255,214,90,255), Draw_Measure     },
    { "Magnifier",  "Zoom in",         Icon::Search,     IM_COL32(120,120,128,255),Draw_Magnifier   },
    { "Shazam",     "Name that tune",  Icon::Music,      IM_COL32(64,156,255,255), Draw_Shazam      },
    { "Sleep",      "Schedule",        Icon::Bed,        IM_COL32(94,92,230,255),  Draw_Sleep       },
    { "Docs",       "Documents",       Icon::FileLines,  IM_COL32(255,159,10,255), Draw_Pages       },
    { "Slides",     "Presentations",   Icon::Display,    IM_COL32(64,156,255,255), Draw_Keynote     },
    { "Video Editor","Edit video",     Icon::Film,       IM_COL32(146,82,232,255), Draw_iMovie      },
    { "Tips",       "Learn more",      Icon::Lightbulb,  IM_COL32(255,214,90,255), Draw_Tips        },
    { "Wallpaper",  "Personalize",     Icon::Image,      IM_COL32(146,82,232,255), Draw_Wallpaper   },
    { "Focus",      "Modes",           Icon::Moon,       IM_COL32(146,82,232,255), Draw_Focus       },
    { "Storage",    "Manage space",    Icon::Database,   IM_COL32(64,156,255,255), Draw_Storage     },
    { "About",      "Device info",     Icon::InfoCircle, IM_COL32(120,120,128,255),Draw_About       },
    { "Access.",    "Accessibility",   Icon::Person,     IM_COL32(64,156,255,255), Draw_Accessibility},
    { "Transit",    "Get around",      Icon::Map,        IM_COL32(64,156,255,255), Draw_Transit     },
    { "Assistant",  "Ask anything",    Icon::Microphone, IM_COL32(146,82,232,255), Draw_Siri        },
    { "Payments",   "Tap to pay",      Icon::CreditCard, IM_COL32(40,40,44,255),   Draw_GlassPay    },
    { "Emoji",      "Express",         Icon::Heart,      IM_COL32(255,214,90,255), Draw_Emoji       },
    { "Remote",     "Streaming",        Icon::Tv,         IM_COL32(120,120,128,255),Draw_Remote      },
    { "Update",     "Software Update", Icon::Download,   IM_COL32(64,156,255,255), Draw_SoftwareUpdate},
    { "Dictionary", "Look it up",      Icon::Book,       IM_COL32(120,120,128,255),Draw_Dictionary  },
    { "Stopwatch",  "Lap timer",       Icon::Stopwatch,  IM_COL32(255,159,10,255), Draw_Stopwatch   },
    { "Nearby",     "Share nearby",    Icon::ShareNodes, IM_COL32(64,156,255,255), Draw_AirDrop     },
    { "Markup",     "Draw & annotate", Icon::PenEdit,    IM_COL32(255,69,58,255),  Draw_Markup      },
    { "Scanner",    "Scan documents",  Icon::Camera,     IM_COL32(120,120,128,255),Draw_Scanner     },
    { "Level",      "Is it straight?", Icon::Crosshairs, IM_COL32(48,209,88,255),  Draw_Level       },
    { "Flashlight", "Light it up",     Icon::Bolt,       IM_COL32(255,214,90,255), Draw_Flashlight  },
    { "Library",    "Music albums",    Icon::Music,      IM_COL32(255,55,95,255),  Draw_MusicLib    },
    { "Lists",      "Reminders lists", Icon::List,       IM_COL32(255,159,10,255), Draw_ReminderLists},
    { "Steps",      "Activity detail", Icon::Running,    IM_COL32(255,159,10,255), Draw_StepsDetail },
    { "Shows",      "Podcast library", Icon::Microphone, IM_COL32(146,82,232,255), Draw_PodcastLib  },
    { "Sounds",     "Ringtones",       Icon::Speaker,    IM_COL32(255,55,95,255),  Draw_Sounds      },
    { "Feed",       "News for you",    Icon::Newspaper,  IM_COL32(255,69,58,255),  Draw_NewsFeed    },
    { "Watchlist",  "Track stocks",    Icon::Bolt,       IM_COL32(48,209,88,255),  Draw_Watchlist   },
    { "Book Store", "Discover books",  Icon::Book,       IM_COL32(255,69,58,255),  Draw_BookStore   },
    { "Display",    "Brightness",      Icon::Display,    IM_COL32(64,156,255,255), Draw_Display     },
    { "Game",       "Tic-Tac-Toe",     Icon::Gamepad,    IM_COL32(94,92,230,255),  Draw_TicTacToe   },
    { "App Page",   "App Store detail",Icon::Bag,        IM_COL32(64,156,255,255), Draw_AppDetail   },
    { "Albums",     "Photo albums",    Icon::Images,     IM_COL32(255,90,140,255), Draw_PhotoAlbums },
    { "Portrait",   "Camera mode",     Icon::Camera,     IM_COL32(255,214,90,255), Draw_CameraPortrait},
    { "Compose",    "New mail",        Icon::PenEdit,    IM_COL32(64,156,255,255), Draw_MailCompose },
    { "Chats",      "Conversations",   Icon::Comment,    IM_COL32(48,209,88,255),  Draw_MessagesList},
    { "Week",       "Calendar week",   Icon::CalendarDays,IM_COL32(255,69,58,255), Draw_CalWeek     },
    { "Heart",      "Heart rate ECG",  Icon::Heart,      IM_COL32(255,55,95,255),  Draw_HeartRate   },
    { "Checklist",  "Note with checks",Icon::CheckCircle,IM_COL32(255,214,90,255), Draw_Checklist   },
    { "Boarding",   "Boarding pass",   Icon::Airplane,   IM_COL32(255,90,70,255),  Draw_BoardingPass},
    { "Radio",      "Stations",        Icon::Wifi,       IM_COL32(255,55,95,255),  Draw_Radio       },
    { "Recents",    "Call log",        Icon::Phone,      IM_COL32(48,209,88,255),  Draw_Recents     },
    { "Live Text",  "Translate camera",Icon::Camera,     IM_COL32(64,156,255,255), Draw_TranslateCam},
    { "Counter",    "Tap to count",    Icon::Plus,       IM_COL32(48,209,88,255),  Draw_Counter     },
    { "Color Mixer","RGB mixer",       Icon::Palette,    IM_COL32(146,82,232,255), Draw_ColorMixer  },
    { "Tip Calc",   "Split the bill",  Icon::CreditCard, IM_COL32(48,209,88,255),  Draw_TipCalc     },
    { "Dice",       "Roll \xF0\x9F\x8E\xB2",  Icon::Dice, IM_COL32(255,69,58,255),  Draw_Dice        },
    { "Converter",  "Units",           Icon::Refresh,    IM_COL32(64,156,255,255), Draw_UnitConv    },
    { "Piano",      "Play notes",      Icon::Music,      IM_COL32(40,40,44,255),   Draw_Piano       },
    { "Calculator", "Crunch numbers",  Icon::Calculator, IM_COL32(255,149,0,255),  Draw_Calculator  },

    { "Media",      "Music \xC2\xB7 Podcasts \xC2\xB7 TV", Icon::Music,    IM_COL32(255,55,95,255),  Draw_GroupMedia   },
    { "Web",        "Browser \xC2\xB7 Maps",       Icon::Compass,   IM_COL32(64,156,255,255), Draw_GroupWeb     },
    { "Connect",    "Messages \xC2\xB7 Mail \xC2\xB7 Calls", Icon::Comment, IM_COL32(48,209,88,255), Draw_GroupConnect },
    { "Workspace",  "Files \xC2\xB7 Notes \xC2\xB7 Calendar",Icon::Folder, IM_COL32(255,159,10,255), Draw_GroupWork    },
    { "Photos",     "Library \xC2\xB7 Camera",     Icon::Images,    IM_COL32(255,90,140,255), Draw_GroupPhotos  },
    { "Tools",      "Calculator \xC2\xB7 Units",   Icon::Calculator,IM_COL32(255,149,0,255),  Draw_GroupTools   },
    { "System",     "Settings \xC2\xB7 Battery",   Icon::Gear,      IM_COL32(142,142,147,255),Draw_GroupSystem  },
    { "Health",     "Activity \xC2\xB7 Sleep",     Icon::Heart,     IM_COL32(255,55,95,255),  Draw_GroupHealth  },
    { "Cards",      "Cards \xC2\xB7 Passes",       Icon::CreditCard,IM_COL32(60,60,64,255),   Draw_GroupWallet  },
    { "Today",      "Weather \xC2\xB7 Clock \xC2\xB7 News", Icon::CloudSun, IM_COL32(64,156,255,255), Draw_GroupToday },
    { "Create",     "Music \xC2\xB7 Video \xC2\xB7 Docs", Icon::Guitar,  IM_COL32(255,159,10,255), Draw_GroupCreate },
};

static constexpr int kSurfaceTotal = (int)(sizeof(g_surfaces)/sizeof(g_surfaces[0]));
static constexpr int kGroupCount   = 11;
static constexpr int kGroupBase    = kSurfaceTotal - kGroupCount;
static_assert(kSurfaceTotal > kGroupCount, "g_surfaces must hold the trailing GROUP shells plus the app rows");
static_assert(kGroupBase >= 0, "kGroupCount exceeds the surface table size");

int          SurfaceCount(){ return (int)(sizeof(g_surfaces)/sizeof(g_surfaces[0])); }
const char*  SurfaceName(int i){ return (i>=0&&i<SurfaceCount())?g_surfaces[i].name:""; }
const char*  SurfaceSubtitle(int i){ return (i>=0&&i<SurfaceCount())?g_surfaces[i].sub:""; }
Icon         SurfaceIcon(int i){ return (i>=0&&i<SurfaceCount())?g_surfaces[i].icon:Icon::None; }
ImU32        SurfaceTint(int i){ return (i>=0&&i<SurfaceCount())?g_surfaces[i].tint:Accent(); }
void         SurfaceDraw(int i, float w){ if(i>=0&&i<SurfaceCount()&&g_surfaces[i].draw) g_surfaces[i].draw(w); }

int SurfaceLaunchpad(float w, const char* query){
    int clicked = -1;
    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImGui::Dummy(ImVec2(0,2));
    ImVec2 tp = ImGui::GetCursorScreenPos();
    dl->AddText(ImGui::GetFont(), 30.0f, ImVec2(tp.x+2, tp.y), kInk, "Apps");
    bool hasQ = query && query[0];
    int total = SurfaceCount();
    int filt[256]; int fn = 0;
    for (int i=0;i<total && fn<256;i++) if (!hasQ || ContainsCI(SurfaceName(i),query) || ContainsCI(SurfaceSubtitle(i),query)) filt[fn++] = i;
    char sub[80];
    if (hasQ) snprintf(sub, sizeof(sub), "%d result%s for \"%s\"", fn, fn==1?"":"s", query);
    dl->AddText(ImGui::GetFont(), 14.0f, ImVec2(tp.x+2, tp.y+38), kInkSoft, hasQ?sub:"A gallery of glass surfaces");
    ImGui::Dummy(ImVec2(w, 70.0f));
    const float tile = 84.0f, gap = 16.0f, lblH = 24.0f;
    int cols = (int)((w + gap) / (tile + gap)); if (cols < 1) cols = 1;
    float gridW = cols*tile + (cols-1)*gap;
    float ox = ImGui::GetCursorScreenPos().x + (w - gridW)*0.5f;
    float oy = ImGui::GetCursorScreenPos().y;
    for (int k=0;k<fn;k++){
        int si = filt[k];
        int r=k/cols, c=k%cols;
        float x = ox + c*(tile+gap), y = oy + r*(tile+lblH+gap);
        ImGui::PushID(si);
        ImGui::SetCursorScreenPos(ImVec2(x, y));
        ImGui::InvisibleButton("app", ImVec2(tile, tile+lblH));
        bool hov = ImGui::IsItemHovered();
        if (ImGui::IsItemDeactivated() && hov) clicked = si;
        Spring& s = g->springs().Get((uint32_t)ImGui::GetID("app"), 1, SpringStyle::Bouncy, 1.0f);
        s.target = ImGui::IsItemActive()?0.92f:(hov?1.06f:1.0f); s.Tick(Dt());
        float sc = Clampf(s.x,0.8f,1.2f);
        float gel = 1.0f - sc;
        float ic = tile*0.5f*sc;
        float ihw = ic*(1.0f + gel*0.5f), ihh = ic*(1.0f - gel*0.4f);
        ImVec2 cen(x+tile*0.5f, y+tile*0.5f);
        Primitive b; b.cx=cen.x; b.cy=cen.y; b.hw=ihw; b.hh=ihh; b.corner_radius=std::min(ihw,ihh)*0.62f;
        b.fade=1.0f; b.material = hov?Material::Accent:Material::Thin; b.elevate = hov?1.18f:1.0f;
        g->Submit(b);
        DrawIcon(dl, SurfaceIcon(si), cen, tile*0.42f*sc, hov?IM_COL32(255,255,255,255):kInk, 2.2f);
        const char* nm = SurfaceName(si);
        ImVec2 ts = ImGui::GetFont()->CalcTextSizeA(13.0f, 99999.0f, 0.0f, nm);
        dl->AddText(ImGui::GetFont(), 13.0f, ImVec2(cen.x - ts.x*0.5f, y+tile+4.0f), kInk, nm);
        ImGui::PopID();
    }
    if (fn == 0) { const char* msg = "No apps match your search"; ImVec2 ms=ImGui::GetFont()->CalcTextSizeA(16.0f,99999.0f,0,msg);
        dl->AddText(ImGui::GetFont(), 16.0f, ImVec2(ox+gridW*0.5f-ms.x*0.5f, oy+40.0f), kInkSoft, msg); }
    int rows = (fn+cols-1)/cols; if (rows<1) rows=1;
    ImGui::SetCursorScreenPos(ImVec2(ox, oy));
    ImGui::Dummy(ImVec2(w, rows*(tile+lblH+gap) + 8.0f));
    return clicked;
}

int SurfaceDock(float cx, float bottomY, float maxW, int activeSurface){
    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImGuiIO& io = ImGui::GetIO();
    int dk[kGroupCount];
    for (int i = 0; i < kGroupCount; ++i) dk[i] = kGroupBase + i;
    const int n = kGroupCount;
    const float base = 42.0f, gap = 8.0f, mag = 26.0f, sigma = 74.0f;
    float restTotal = n*base + (n-1)*gap;
    float barW = maxW * 0.95f;
    float barGrow = barW * 0.03f;
    barW += barGrow;
    float midX = cx + barGrow * 0.5f;
    float startX = midX - restTotal*0.5f;
    float mx = io.MousePos.x, my = io.MousePos.y;
    float dt = Dt();
    bool inRange = my > bottomY - 150.0f && my < bottomY + 26.0f && mx > startX - 60.0f && mx < startX + restTotal + 60.0f;
    static float dockAct = 0.0f;
    dockAct += ((inRange ? 1.0f : 0.0f) - dockAct) * Clampf(12.0f*dt, 0.0f, 1.0f);
    static float scSm[32]; static bool scInit = false;
    if (!scInit){ for (int k=0;k<32;k++) scSm[k] = base; scInit = true; }
    float sc[32], scaledTotal = 0.0f;
    for (int i=0;i<n;i++){
        float rc = startX + i*(base+gap) + base*0.5f; float d = mx - rc;
        float gmag = expf(-(d*d)/(2.0f*sigma*sigma)) * dockAct;
        float target = base + mag*gmag;
        scSm[i] += (target - scSm[i]) * Clampf(22.0f*dt, 0.0f, 1.0f);
        sc[i] = scSm[i]; scaledTotal += sc[i];
    }
    scaledTotal += (n-1)*gap;
    float barH = base + 14.0f;
    Spring& barWake = g->springs().Get((uint32_t)ImGui::GetID("dockbar"), 6, SpringStyle::Bouncy, 0.0f);
    barWake.target = dockAct; barWake.Tick(dt);
    float wake = Clampf(barWake.x, 0.0f, 1.2f);
    Primitive bar; bar.cx = midX; bar.cy = bottomY - barH*0.5f; bar.hw = barW*0.5f; bar.hh = barH*0.5f;
    bar.corner_radius = 22.0f + 3.0f*wake; bar.fade = 1.0f; bar.material = Material::Regular;
    bar.elevate = 1.0f + 0.12f*wake; g->SubmitOverlay(bar);
    float x = midX - scaledTotal*0.5f; int clicked = -1; int hov = -1; float hovCx = midX;
    for (int i=0;i<n;i++){
        float s = sc[i]; float ix = x, iy = bottomY - 6.0f - s;
        ImGui::PushID(2000+i); ImGui::SetCursorScreenPos(ImVec2(ix, iy)); ImGui::InvisibleButton("dk", ImVec2(s, s));
        bool h = ImGui::IsItemHovered(); if (h) hov = i;
        bool act = ImGui::IsItemActive();
        uint32_t did = (uint32_t)ImGui::GetID("dk");
        Spring& ps = g->springs().Get(did, 3, SpringStyle::Bouncy, 1.0f);
        ps.target = act ? 0.84f : 1.0f; ps.Tick(Dt());
        Spring& bnc = g->springs().Get(did, 4, SpringStyle::Bouncy, 0.0f);
        bnc.target = 0.0f;
        if (ImGui::IsItemDeactivated() && h) { clicked = dk[i]; bnc.v += 5.5f; }
        bnc.Tick(Dt());
        Spring& dotS = g->springs().Get(did, 5, SpringStyle::Bouncy, (dk[i]==activeSurface)?1.0f:0.0f);
        dotS.target = (dk[i]==activeSurface) ? 1.0f : 0.0f; dotS.Tick(Dt());
        ImGui::PopID();
        float pv = Clampf(ps.x, 0.7f, 1.1f);
        float bub = Clampf(bnc.x, -0.20f, 0.30f);
        float dotk = Clampf(dotS.x, 0.0f, 1.3f);
        float gel = 1.0f - pv;
        float vis = s * pv * (1.0f + bub);
        float jw  = bub * 0.5f;
        ImVec2 igc(ix+s*0.5f, iy + s*0.5f);
        if (h) hovCx = igc.x;
        Primitive ic; ic.cx = igc.x; ic.cy = igc.y;
        ic.hw = vis*0.5f*(1.0f + gel*0.6f + jw); ic.hh = vis*0.5f*(1.0f - gel*0.5f - jw);
        ic.corner_radius = std::min(ic.hw, ic.hh)*0.86f; ic.fade = 1.0f; ic.material = h ? Material::Accent : Material::Regular;
        ic.elevate = h ? 1.16f : 1.0f;
        g->SubmitOverlay(ic);
        DrawIcon(dl, SurfaceIcon(dk[i]), ImVec2(igc.x+1.0f, igc.y+1.0f), vis*0.50f, SA(IM_COL32(0,0,0,255),0.30f), 2.2f);
        DrawIcon(dl, SurfaceIcon(dk[i]), igc, vis*0.50f, IM_COL32(255,255,255,255), 2.2f);
        if (dotk > 0.01f)
            dl->AddCircleFilled(ImVec2(ix+s*0.5f, bottomY + 4.0f), 2.4f*dotk, SA(InkColor(), 0.85f*Clampf(dotk,0.0f,1.0f)));
        x += s + gap;
    }
    static int sndHov = -1;
    if (hov != sndHov) { if (hov >= 0) PlaySfx(Sfx::TapSoft); sndHov = hov; }
    static float labA = 0.0f, labX = 0.0f; static int labI = -1; static bool labInit = false;
    if (!labInit) { labX = midX; labInit = true; }
    labA += (((hov >= 0) ? 1.0f : 0.0f) - labA) * Clampf(16.0f*dt, 0.0f, 1.0f);
    if (hov >= 0) { labI = hov; labX += (hovCx - labX) * Clampf(20.0f*dt, 0.0f, 1.0f); }
    if (labA > 0.01f && labI >= 0 && labI < n) {
        const char* nm = SurfaceName(dk[labI]);
        ImVec2 ts = ImGui::GetFont()->CalcTextSizeA(14.0f, 99999.0f, 0, nm);
        float rise = (1.0f - labA) * 8.0f;
        float lx = labX, ly = bottomY - 6.0f - sc[labI] - 30.0f + rise;
        int bgA = (int)(0.9f * labA * 255.0f), txA = (int)(labA * 255.0f);
        dl->AddRectFilled(ImVec2(lx-ts.x*0.5f-10, ly-4), ImVec2(lx+ts.x*0.5f+10, ly+ts.y+4), IM_COL32(20,20,26,bgA), 8.0f);
        dl->AddText(ImGui::GetFont(), 14.0f, ImVec2(lx-ts.x*0.5f, ly), IM_COL32(255,255,255,txA), nm);
    }
    return clicked;
}

}
