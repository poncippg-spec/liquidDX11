namespace {
static const char* MS_general[] = { "Background App Refresh","Automatic Updates","Cross-Device Resume","Nearby Share to Everyone","Picture in Picture","Auto-Lock","Set Time Zone Automatically","Auto-Join Hotspots","Suggest New Networks","Low Data Mode","Limit Ad Tracking","Offload Unused Apps","Slide to Type","Character Preview","Shake to Undo","Content & Privacy","Continuity Camera","Shared Clipboard" };
static const char* MS_display[] = { "True Tone","Night Shift","Auto-Brightness","Raise to Wake","Tap to Wake","Attention Aware","Bold Text","Larger Text","Button Shapes","On/Off Labels","Reduce White Point","Auto-Rotate","Always-On Display","Show Wallpaper Depth","Dim Lock Screen","Color Filters","Smart Invert","Increase Contrast" };
static const char* MS_sounds[] = { "Ringtone Vibration","Silent Vibration","Change with Buttons","Keyboard Clicks","Lock Sound","System Haptics","Headphone Safety","Reduce Loud Sounds","Mono Audio","Noise Cancellation","Spatial Audio","Dialogue Boost","Background Sounds","Announce Calls","Announce Notifications","Live Captions" };
static const char* MS_privacy[] = { "Location Services","Precise Location","Share Analytics","App Privacy Report","Allow Tracking","Personalized Ads","Crash Reports","Significant Locations","Microphone Access","Camera Access","Contacts Access","Photos Access","Bluetooth Access","Local Network","Biometric Unlock for Apps","Lockdown Mode","Hide IP Address","Sensitive Content Warning" };
static const char* MS_network[] = { "Wi-Fi","Bluetooth","Cellular Data","Data Roaming","Wi-Fi Assist","Private Wi-Fi Address","Limit IP Tracking","VPN On Demand","Personal Hotspot","Maximize Compatibility","5G Standalone","Wi-Fi Calling","Block Unknown Callers","Silence Junk Callers","DNS Override","Cloud Privacy Relay" };
static const char* MS_battery[] = { "Low Power Mode","Battery Percentage","Optimized Charging","Clean Energy Charging","Battery Health Management","Background Usage","Show Activity","Auto Low Power","Charge Limit 80%","Charging Sounds","Power-Saving Graphics","Reduce Background Activity" };
static const char* MS_keyboard[] = { "Auto-Capitalization","Auto-Correction","Check Spelling","Predictive Text","Smart Punctuation","Enable Caps Lock","Text Replacement","Slide to Type","Delete Slide Path","\".\" Shortcut","Dictation","Auto-Dictation","One-Handed","Emoji Search","Key Flicks","Memoji Stickers" };
static const char* MS_notify[] = { "Allow Notifications","Time Sensitive","Lock Screen","Notification Center","Banners","Badges","Sounds","Show Previews","Group by App","Scheduled Summary","Announce Notifications","Critical Alerts","Repeat Alerts","Assistant Suggestions","Reminders","Haptic Alerts" };
static const char* MS_access[] = { "VoiceOver","Zoom","Display Accommodations","Spoken Content","Audio Descriptions","Voice Control","Switch Control","AssistiveTouch","Reachability","Back Tap","Reduce Motion","Prefer Cross-Fade","Auto-Play Video Previews","Dim Flashing Lights","Reduce Transparency","Sticky Keys","Slow Keys","Mouse Keys" };
static const char* MS_camera[] = { "Grid","Level","Mirror Front Camera","View Outside Frame","Scene Detection","Prioritize Faster Shooting","Lens Correction","Macro Control","HDR Video","Smart HDR","Preserve Settings","Volume Up for Burst","Scan QR Codes","Record Stereo Sound","Live Photo","ProRAW" };
static const char* MS_game[] = { "Game Mode","Reduce Input Latency","Limit Background Apps","Controller Vibration","Performance Overlay","Cooler Performance","Dim Display Off","Silence Notifications","Auto Screen Recording","Allow Friend Requests","Voice Chat","Spatial Audio in Games" };
static const char* MS_focus[] = { "Do Not Disturb","Work","Personal","Sleep","Driving","Fitness","Gaming","Mindfulness","Reading","Share Across Devices","Auto-Reply","Time Sensitive Breakthrough","Dim Lock Screen","Hide Notification Badges" };
static const char* MS_wallet[] = { "Express Transit","Double-Click to Open","Allow Payments","Add Money Automatically","Default Card","Order Tracking","Card Notifications","Background Updates","Rewards Reminders","Tap to Pay" };
static const char* MS_dev[] = { "Show FPS Counter","GPU Frame Capture","Slow Animations","Color Blended Layers","Color Copied Images","Flash Updated Regions","Network Link Conditioner","Verbose Logging","Reset Cache on Launch","Show Touches","Debug Grid","Safe Area Guides","Layout Bounds","Overdraw Heatmap","Disable Vsync","Wireframe Mode" };
static const char* MS_safari[] = { "Block Pop-ups","Prevent Cross-Site Tracking","Hide IP Address","Fraudulent Site Warning","Privacy-Preserving Ads","Check for Payments","AutoFill Passwords","AutoFill Contact Info","Show Favorites Bar","Open Links in New Tab","Color in Tab Bar","Auto Reader Mode","Block All Cookies","Enable JavaScript","Web Inspector","Auto-Save Downloads" };
static const char* MS_mail[] = { "Load Remote Images","Organize by Thread","Collapse Read Messages","Most Recent on Top","Complete Threads","Ask Before Deleting","Privacy Protection","Hide IP Address","Swipe to Archive","Show To/Cc Labels","Flag Style","Auto Bcc Myself","Signature on Replies","Send Read Receipts","Smart Replies","Block Sender" };
static const char* MS_messages[] = { "Chat Sync","Send as SMS","Send Read Receipts","Subject Field","Character Count","Filter Unknown Senders","Keep Messages Forever","Audio Auto-Expire","Video Auto-Expire","Low Quality Image Mode","Notify Mentions","Hide Alerts","Sticker Suggestions","Inline Replies","Reaction Suggestions","Auto-Send Audio" };
static const char* MS_photos[] = { "Cloud Photos","Optimize Storage","Shared Albums","Cellular Upload","Memories Auto-Play","Featured Photos","Show Holiday Events","Auto HDR","Motion Photo Auto","Map Locations","Duplicate Detection","Hidden Album","Recently Deleted Lock","Burst Best Shot","Import Originals","Show Seconds in EXIF" };
static const char* MS_maps[] = { "Show Traffic","Air Quality Index","Weather Overlay","3D Buildings","Show Speed Limit","Avoid Tolls","Avoid Highways","Walking Compass","Voice Guidance","Pause Spoken Audio","Show Compass","Night Mode Map","Nearby Suggestions","Shared ETA","Favorites Sync","Offline Maps" };
static const char* MS_music[] = { "Show Music","Lossless Audio","Dolby Atmos","Sound Check","Late Night EQ","Crossfade","Auto-Play Similar","Show Star Ratings","Cellular Streaming","High Quality Cellular","Download Lossless","Animated Cover Art","Lyrics Sync","Handoff Playback","Use Listening History","Auto Add to Library" };
static const char* MS_siri[] = { "Listen for Assistant","Press Side Button","Allow When Locked","Voice Feedback","Show in Search","Show Suggestions","Learn from this App","Announce Messages","Type to Assistant","Assistant in Car Mode","Personalized Requests","Announce Caller ID","Hands-Free Calls","Automatic Shortcuts","Suggest Shortcuts","Speak Corrections" };
static const char* MS_health[] = { "Track Steps","Track Sleep","Heart Rate Alerts","High HR Notify","Low HR Notify","Irregular Rhythm","Fall Detection","Crash Detection","Walking Steadiness","Headphone Levels","Environmental Sound","Cycle Tracking","Medication Reminders","Share with Doctor","Mobility Metrics","Vitals Summary" };
struct MegaCat { const char* name; Glass::Icon icon; const char* const* items; int n; };
static const MegaCat g_mega[] = {
    { "GENERAL",       Glass::Icon::Gear,        MS_general,  IM_ARRAYSIZE(MS_general)  },
    { "DISPLAY",       Glass::Icon::Display,     MS_display,  IM_ARRAYSIZE(MS_display)  },
    { "SOUNDS",        Glass::Icon::Speaker,     MS_sounds,   IM_ARRAYSIZE(MS_sounds)   },
    { "NETWORK",       Glass::Icon::Wifi,        MS_network,  IM_ARRAYSIZE(MS_network)  },
    { "NOTIFICATIONS", Glass::Icon::Bell,        MS_notify,   IM_ARRAYSIZE(MS_notify)   },
    { "FOCUS",         Glass::Icon::Moon,        MS_focus,    IM_ARRAYSIZE(MS_focus)    },
    { "PRIVACY & SECURITY", Glass::Icon::ShieldHalf, MS_privacy, IM_ARRAYSIZE(MS_privacy) },
    { "BATTERY",       Glass::Icon::BatteryFull, MS_battery,  IM_ARRAYSIZE(MS_battery)  },
    { "KEYBOARD",      Glass::Icon::Keyboard,    MS_keyboard, IM_ARRAYSIZE(MS_keyboard) },
    { "ACCESSIBILITY", Glass::Icon::Person,      MS_access,   IM_ARRAYSIZE(MS_access)   },
    { "CAMERA",        Glass::Icon::Camera,      MS_camera,   IM_ARRAYSIZE(MS_camera)   },
    { "GAME MODE",     Glass::Icon::Gamepad,     MS_game,     IM_ARRAYSIZE(MS_game)     },
    { "CARDS",         Glass::Icon::CreditCard,  MS_wallet,   IM_ARRAYSIZE(MS_wallet)   },
    { "BROWSER",       Glass::Icon::Compass,     MS_safari,   IM_ARRAYSIZE(MS_safari)   },
    { "MAIL",          Glass::Icon::Envelope,    MS_mail,     IM_ARRAYSIZE(MS_mail)     },
    { "MESSAGES",      Glass::Icon::Comment,     MS_messages, IM_ARRAYSIZE(MS_messages) },
    { "PHOTOS",        Glass::Icon::Images,      MS_photos,   IM_ARRAYSIZE(MS_photos)   },
    { "MAPS",          Glass::Icon::Map,         MS_maps,     IM_ARRAYSIZE(MS_maps)     },
    { "MUSIC",         Glass::Icon::Music,       MS_music,    IM_ARRAYSIZE(MS_music)    },
    { "ASSISTANT & SEARCH", Glass::Icon::Microphone, MS_siri, IM_ARRAYSIZE(MS_siri)     },
    { "HEALTH",        Glass::Icon::Heart,       MS_health,   IM_ARRAYSIZE(MS_health)   },
    { "DEVELOPER",     Glass::Icon::Code,        MS_dev,      IM_ARRAYSIZE(MS_dev)      },
};
static const char* MS_trackpad[]  = { "Tap to Click","Secondary Click","Look Up & Data Detectors","Force Click","Tracking Speed","Silent Clicking","Scroll Direction Natural","Smart Zoom","Rotate","Swipe Between Pages","Swipe Between Apps","Notification Center Swipe","Mission Control Swipe","App Exposé","Launchpad Pinch","Show Desktop Spread","Three-Finger Drag","Click Pressure","Haptic Feedback","Pointer Acceleration" };
static const char* MS_mouse[]     = { "Natural Scrolling","Tracking Speed","Scroll Speed","Double-Click Speed","Secondary Click","Smart Zoom","Mouse Acceleration","Swipe Between Pages","Mission Control Button","Auto-Hide Pointer","Pointer Size","Shake to Locate","Scroll Momentum","Primary Button Left","Wireless Mouse Battery","Connect on Startup" };
static const char* MS_displays[]  = { "Automatic Brightness","Adaptive Tone","Night Shift Schedule","High Refresh Rate","Resolution Scaled","Rotation","Mirror Displays","Extend Display","Wireless Display","Reference Mode","HDR","Color Profile","Overscan Correction","Underscan","Show Mirroring in Menu Bar","Detect Displays","Arrange Displays","Presenter Overlay" };
static const char* MS_wallpaper[] = { "Dynamic Desktop","Auto Light/Dark","Still Image","Photo Shuffle","Featured Albums","Change Every Hour","Change on Wake","Change on Login","Add to All Spaces","Translucent Menu Bar","Depth Effect Lock Screen","Perspective Zoom","Live Wallpaper","Color Tint","Blur Behind Windows","Show on All Displays" };
static const char* MS_screentime[]= { "App & Website Activity","Downtime","App Limits","Communication Limits","Communication Safety","Always Allowed","Content Restrictions","Privacy Restrictions","Share Across Devices","Lock Screen Time Settings","Block at Downtime","One More Minute","Pickups Tracking","Notifications Tracking","Web Content Filter","Screen Distance","Family Activity Reports","Hide App Usage" };
static const char* MS_datetime[]  = { "Set Automatically","24-Hour Time","Time Zone Automatic","Show Date in Menu Bar","Show Day of Week","Show Seconds","Flash Time Separators","Use Analog Clock","Announce Time","Network Time Sync","Show AM/PM","International Calendar","Week Starts Monday","Show Week Numbers","Lunar Calendar","Holiday Calendar" };
static const char* MS_language[]  = { "Preferred Language","Region Format","Temperature Unit","Measurement System","First Day of Week","Number Format","Currency Format","Translation Languages","On-Device Translation","Keyboard Languages","Text Direction RTL","Live Text Languages","Region-Specific Apps","Calendar Type","Date Format","List Sort Order" };
static const char* MS_users[]     = { "Automatic Login","Guest User","Fast User Switching","Show Login Window List","Show Password Hints","Show Sleep/Restart Buttons","Allow Screen Reader at Login","Wearable Unlock","Biometric Unlock","Require Password After Sleep","Login Items","Allow Admin Reset","Network Account Server","Hide Fast Switch Menu","Picture Password","Auto-Lock on Idle" };
static const char* MS_sharing[]   = { "Screen Sharing","File Sharing","Media Sharing","Printer Sharing","Remote Login","Remote Management","Remote Events","Internet Sharing","Bluetooth Sharing","Content Caching","Wireless Display Receiver","Cross-Device Control","Resume Sharing","Nearby Share","Public Folder Access","Allow Guest Connections" };
static const char* MS_spotlight[] = { "Applications","Search Suggestions","Bookmarks & History","Calculator","Contacts","Conversion","Definition","Documents","Events & Reminders","Folders","Fonts","Images","Mail & Messages","Movies","Music","PDF Documents","Presentations","Assistant Suggestions","System Preferences","Websites" };
static const char* MS_dock[]      = { "Magnification","Auto-Hide Dock","Position on Screen","Minimize Effect Scale","Double-Click Title Bar","Minimize to App Icon","Animate Opening Apps","Show Indicators","Show Recent Apps","Translucent Dock","Menu Bar Auto-Hide","Show Battery Percent","Show Bluetooth","Show Wi-Fi","Show Sound","Show Search","Show Assistant","Show Clock","Show Fast User Switch","Show Now Playing" };
static const char* MS_ccenter[]   = { "Wi-Fi Module","Bluetooth Module","Nearby Share Module","Focus Module","Screen Mirroring","Display Brightness","Sound Volume","Now Playing","Flashlight","Timer","Calculator","Camera","Screen Recording","Voice Memos","Home Controls","Accessibility Shortcuts","Hearing","Low Power Mode","Dark Mode Toggle","Stopwatch" };
static const char* MS_homekit[]   = { "Allow Home Access","Home Hub","Cameras & Doorbells","Notify on Motion","Notify on Door/Window","Allow Speaker & TV","Personal Requests","Listen for Assistant","Sound Recognition","Activity Notifications","Guest Access","Speaker Resume","Adaptive Lighting","Auto Night Mode","Secure Video","Energy Mode" };
static const char* MS_carplay[]   = { "Allow Car Mode","Wireless Car Mode","Dark Appearance","Show Album Art","Driving Focus Auto","Suggest Destinations","Speak Notifications","Show Calendar","Nearby Charging","Wallpaper","App Layout","Use Portrait","Announce Messages","Now Playing Lock","Steering Wheel Controls","Compact Assistant UI" };
static const char* MS_continuity[]= { "Resume on Device","Shared Clipboard","Auto Unlock","Instant Hotspot","Continuity Camera","Continuity Sketch","Continuity Markup","Nearby Share","Phone Call Relay","SMS Forwarding","Mobile Calls","Second Display","Cross-Device Control","Shared Clipboard History","Wearable Auto-Unlock","Resume Now Playing" };
static const char* MS_icloud[]    = { "Cloud Drive","Cloud Photos","Cloud Backup","Optimize Local Storage","Mail","Contacts","Calendars","Reminders","Notes","Browser","Password Vault","Passwords","Device Finder","Messages in Cloud","Cards","Health","Home","News","Stocks","Hide My Email","Cloud Privacy Relay","Custom Email Domain","Shared Albums","Desktop & Documents" };
static const char* MS_appstore[]  = { "Automatic App Updates","App Downloads over Cellular","Auto-Play Video Previews","In-App Ratings & Reviews","Offload Unused Apps","Personalized Recommendations","Video Autoplay Wi-Fi Only","App Privacy Details","Family Purchase Sharing","Ask to Buy","Password for Purchases","Free Downloads Password","Beta Updates","App Library Auto-Sort","Show on Home Screen","Notify of Updates" };
static const char* MS_facetime[]  = { "Video Calls Enabled","Calls from Mobile","Eye Contact","Portrait Mode Blur","Studio Light","Auto Framing","Mic Mode Voice Isolation","Mic Mode Wide Spectrum","Reactions","Live Captions","Shared Playback","Spatial Audio","Announce Caller","Ringtone","Blocked Contacts","Use Account Email" };
static const char* MS_passwords[] = { "AutoFill Passwords","Detect Compromised Passwords","Password Sync Cloud Vault","Security Recommendations","Verification Codes","Setup Authenticator","Password Sharing Groups","Clean Up Automatically","Suggest Strong Passwords","Allow Filling from Other Apps","Save Passwords","Delete After Use","Show Last Used","Require Biometric Unlock","Block Reused Passwords","Import/Export" };
static const char* MS_fonts[]     = { "Smart Quotes","Smart Dashes","Auto Ligatures","Hinting","Anti-Aliasing","Subpixel Smoothing","Font Smoothing Threshold","Embed Fonts","Install for All Users","Validate on Install","Auto-Activate","Resolve Duplicates","Show Font Preview","Disable System Fonts","Custom Font Sync","Variable Font Axes" };
static const char* MS_storage[]   = { "Optimize Storage","Empty Trash Automatically","Reduce Clutter","Store in Cloud","Offload Unused Apps","Auto Delete Old Messages","Large Attachments Review","Downloads Cleanup","Cache Management","System Data Cleanup","OS Update Storage","Photo Library Optimize","Music Downloads Limit","Podcast Episode Limit","Mail Attachment Limit","Show Storage in Menu" };
static const char* MS_software[]  = { "Automatic Updates","Download New Updates","Install OS Updates","Install App Updates","Install Security Responses","Beta Updates","Rapid Security Response","Notify Before Install","Install Overnight","Download over Cellular","Backup Before Update","Show Update Badge","Auto-Restart","Verify Update Signature","Defer Major Updates","Update While Charging" };
static const MegaCat g_mega2[] = {
    { "TRACKPAD",         Glass::Icon::Sliders,     MS_trackpad,  IM_ARRAYSIZE(MS_trackpad)  },
    { "MOUSE",            Glass::Icon::Crosshairs,  MS_mouse,     IM_ARRAYSIZE(MS_mouse)     },
    { "DISPLAYS",         Glass::Icon::Display,     MS_displays,  IM_ARRAYSIZE(MS_displays)  },
    { "WALLPAPER",        Glass::Icon::Image,       MS_wallpaper, IM_ARRAYSIZE(MS_wallpaper) },
    { "SCREEN TIME",      Glass::Icon::Clock,       MS_screentime,IM_ARRAYSIZE(MS_screentime)},
    { "DATE & TIME",      Glass::Icon::Clock,       MS_datetime,  IM_ARRAYSIZE(MS_datetime)  },
    { "LANGUAGE & REGION",Glass::Icon::Globe,       MS_language,  IM_ARRAYSIZE(MS_language)  },
    { "USERS & GROUPS",   Glass::Icon::Person,      MS_users,     IM_ARRAYSIZE(MS_users)     },
    { "SHARING",          Glass::Icon::ShareNodes,  MS_sharing,   IM_ARRAYSIZE(MS_sharing)   },
    { "SPOTLIGHT",        Glass::Icon::Search,      MS_spotlight, IM_ARRAYSIZE(MS_spotlight) },
    { "DOCK & MENU BAR",  Glass::Icon::Grid,        MS_dock,      IM_ARRAYSIZE(MS_dock)      },
    { "CONTROL CENTER",   Glass::Icon::Sliders,     MS_ccenter,   IM_ARRAYSIZE(MS_ccenter)   },
    { "HOME",             Glass::Icon::House,       MS_homekit,   IM_ARRAYSIZE(MS_homekit)   },
    { "CAR MODE",         Glass::Icon::Map,         MS_carplay,   IM_ARRAYSIZE(MS_carplay)   },
    { "CONTINUITY",       Glass::Icon::Refresh,     MS_continuity,IM_ARRAYSIZE(MS_continuity)},
    { "CLOUD",            Glass::Icon::Cloud,       MS_icloud,    IM_ARRAYSIZE(MS_icloud)    },
    { "APP STORE",        Glass::Icon::Bag,         MS_appstore,  IM_ARRAYSIZE(MS_appstore)  },
    { "VIDEO CALLS",      Glass::Icon::Video,       MS_facetime,  IM_ARRAYSIZE(MS_facetime)  },
    { "PASSWORDS",        Glass::Icon::Lock,        MS_passwords, IM_ARRAYSIZE(MS_passwords) },
    { "FONTS",            Glass::Icon::FileLines,   MS_fonts,     IM_ARRAYSIZE(MS_fonts)     },
    { "STORAGE",          Glass::Icon::Database,    MS_storage,   IM_ARRAYSIZE(MS_storage)   },
    { "SOFTWARE UPDATE",  Glass::Icon::Download,    MS_software,  IM_ARRAYSIZE(MS_software)  },
};
static const char* MS_app[] = {
    "Allow Notifications","Time Sensitive Notifications","Immediate Delivery","Scheduled Summary",
    "Lock Screen Alerts","Notification Center Alerts","Banners","Persistent Banner Style",
    "Badges","Notification Sounds","Custom Alert Sound","Vibration Pattern","Show Previews",
    "Preview When Unlocked","Notification Grouping","Group Automatically","Critical Alerts",
    "Repeat Alerts","Announce with Assistant","Haptic for Alerts","Badge App Icon","Mute Notifications",
    "Location Access","Precise Location","Location While Using","Location Always","Contacts Access",
    "Calendars Access","Reminders Access","Photos Access","Add Photos Only","Full Photo Library",
    "Camera Access","Microphone Access","Speech Recognition","Motion & Fitness","Bluetooth Access",
    "Local Network","Nearby Interactions","Media & Music","Home Data","Health Data",
    "Focus Status","Allow Tracking","Paste from Other Apps","Screen Recording Access","Files & Folders",
    "Learn from this App","Show in App Library","Show on Home Screen","Show in Search",
    "Show Content in Search","Assistant Suggestions","Suggest App","Suggest Notifications","Suggest Shortcuts",
    "Cellular Data","Wi-Fi Assist","Documents & Data","Low Data Mode","Allow on Roaming",
    "Background Downloads","Stream over Cellular","Download over Cellular",
    "Background App Refresh","Background Audio","Picture in Picture","Allow Full Access",
    "Use Face ID to Open","Require Passcode","Screen Time Limit","Always Allowed","App Updates",
    "Offload When Unused","Delete App Data on Remove",
    "Dark Mode Override","Larger Text Override","Bold Text Override","Reduce Motion Override",
    "Sound Effects","Haptics","Autoplay Media","Sync with Cloud","Open Links in App",
    "Clear Cache on Exit","Confirm Before Closing","Show Unread Badge","Default App for Type",
};
static int          CuratedCatCount(){ return (int)IM_ARRAYSIZE(g_mega) + (int)IM_ARRAYSIZE(g_mega2); }
static const MegaCat& CuratedCat(int i){ int a=(int)IM_ARRAYSIZE(g_mega); return i<a ? g_mega[i] : g_mega2[i-a]; }
static int          CuratedTotal(){ int n=0; for (int i=0;i<CuratedCatCount();i++) n += CuratedCat(i).n; return n; }
static const int    MS_APPN = (int)IM_ARRAYSIZE(MS_app);
static bool g_msTog[16384];
static int  MSGrandTotal(){ return CuratedTotal() + Glass::SurfaceCount()*MS_APPN; }
static bool MSContains(const char* hay, const char* ndl){
    if (!ndl||!*ndl) return true; if (!hay) return false;
    for (const char* h=hay; *h; ++h){ const char* a=h; const char* b=ndl;
        while (*a&&*b){ char ca=(*a>='A'&&*a<='Z')?(char)(*a+32):*a, cb=(*b>='A'&&*b<='Z')?(char)(*b+32):*b; if(ca!=cb)break; ++a;++b; }
        if (!*b) return true; }
    return false;
}
static int MSChipRow(const char* (*nameOf)(int), int count, int sel){
    ImGuiStyle& st = ImGui::GetStyle();
    float rightX = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;
    int clicked = -1;
    for (int i=0;i<count;i++){
        if (Glass::Chip(nameOf(i), i==sel)) clicked = i;
        if (i+1<count){
            float lastX2 = ImGui::GetItemRectMax().x;
            ImVec2 nsz = ImGui::CalcTextSize(nameOf(i+1));
            if (lastX2 + st.ItemSpacing.x + nsz.x + 34.0f < rightX) ImGui::SameLine();
        }
    }
    return clicked;
}
static const char* MSCuratedName(int i){ return CuratedCat(i).name; }
static const char* MSAppName(int i){ return Glass::SurfaceName(i); }
static bool MSCompactToggle(const char* label, bool* v, float cellW){
    ImGui::PushID(v);
    const float rowh = 24.0f, sw = 30.0f, sh = 17.0f, pad = 2.5f;
    ImVec2 cur = ImGui::GetCursorScreenPos();
    ImGui::InvisibleButton("##c", ImVec2(cellW, rowh));
    ImGuiID id = ImGui::GetID("##c");
    bool hov = ImGui::IsItemHovered();
    bool clicked = ImGui::IsItemDeactivated() && hov;
    if (clicked) *v = !*v;
    ImDrawList* dl = ImGui::GetWindowDrawList();
    if (hov) dl->AddRectFilled(cur, ImVec2(cur.x + cellW, cur.y + rowh), IM_COL32(255,255,255,14), 6.0f);
    float labMaxX = cur.x + cellW - sw - 10.0f;
    ImVec2 lts = ImGui::CalcTextSize(label);
    dl->PushClipRect(ImVec2(cur.x + 4, cur.y), ImVec2(labMaxX, cur.y + rowh), true);
    dl->AddText(ImVec2(cur.x + 6, cur.y + (rowh - lts.y) * 0.5f), Glass::InkColor(), label);
    dl->PopClipRect();
    ImVec2 sp(cur.x + cellW - sw, cur.y + (rowh - sh) * 0.5f);
    Glass::Spring& k = Glass::g->springs().Get((uint32_t)id, 3, Glass::SpringStyle::Critical, *v ? 1.0f : 0.0f);
    k.target = *v ? 1.0f : 0.0f; k.Tick(ImGui::GetIO().DeltaTime);
    float kc = k.x < 0 ? 0 : (k.x > 1 ? 1 : k.x);
    Glass::Primitive tr; tr.cx = sp.x + sw*0.5f; tr.cy = sp.y + sh*0.5f; tr.hw = sw*0.5f; tr.hh = sh*0.5f;
    tr.corner_radius = sh*0.5f; tr.fade = 1.0f; tr.material = Glass::Material::Thin; Glass::g->Submit(tr);
    Glass::Primitive on = tr; on.material = Glass::Material::Accent; on.fade = kc; Glass::g->Submit(on);
    float kr = sh*0.5f - pad;
    Glass::Primitive kn; kn.cx = sp.x + sh*0.5f + (sw - sh)*kc; kn.cy = sp.y + sh*0.5f;
    kn.hw = kr; kn.hh = kr; kn.corner_radius = kr; kn.fade = 1.0f; kn.material = Glass::Material::Knob; Glass::g->Submit(kn);
    ImGui::PopID();
    return clicked;
}
static int  MSCols(float w){ return w > 600.0f ? 3 : (w > 380.0f ? 2 : 1); }
static const float MS_COLGAP = 12.0f;
static void DrawMegaSettings() {
    static bool init = false;
    IM_ASSERT(MSGrandTotal() <= 16384);
    if (!init) { for (int i=0;i<16384;i++) g_msTog[i] = ((i*13+5) % 5) < 3; init = true; }
    static int  tab = 0;
    static int  selSys = 0;
    static int  selApp = 0;
    static char q[64] = "";
    char hdr[80]; snprintf(hdr, sizeof(hdr), "ALL SETTINGS  \xC2\xB7  %d controls", MSGrandTotal());
    Glass::SeparatorText(hdr);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(1,1,1,0.06f));
    ImGui::PushStyleColor(ImGuiCol_Text,    ImVec4(1,1,1,0.92f));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 12.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12,9));
    ImGui::SetNextItemWidth(-1.0f);
    ImGui::InputTextWithHint("##mssearch", "Search all settings, apps & permissions...", q, sizeof(q));
    ImGui::PopStyleVar(2); ImGui::PopStyleColor(2);
    ImGui::Dummy(ImVec2(0,6));
    if (q[0]) {
        const int CAP = 300; int shown = 0, matches = 0, gi = 0;
        Glass::BeginPanel("SEARCH RESULTS");
        float gw = ImGui::GetContentRegionAvail().x; int nc = MSCols(gw);
        float cellW = (gw - (nc-1)*MS_COLGAP) / nc;
        auto cell = [&](const char* lab, bool* st){
            if (gi % nc != 0) ImGui::SameLine(0, MS_COLGAP);
            MSCompactToggle(lab, st, cellW); ++gi; };
        int base = 0;
        for (int ci=0; ci<CuratedCatCount(); ci++){ const MegaCat& c = CuratedCat(ci);
            for (int i=0;i<c.n;i++){ if (MSContains(c.items[i], q) || MSContains(c.name, q)){
                ++matches; if (shown<CAP){ cell(c.items[i], &g_msTog[(base+i)&16383]); ++shown; } } }
            base += c.n;
        }
        int pbase = CuratedTotal();
        for (int a=0; a<Glass::SurfaceCount(); a++){ const char* an = Glass::SurfaceName(a);
            bool appMatch = MSContains(an, q);
            for (int i=0;i<MS_APPN;i++){ if (appMatch || MSContains(MS_app[i], q)){
                ++matches; if (shown<CAP){ char lbl[112]; snprintf(lbl,sizeof(lbl),"%s \xC2\xB7 %s", an, MS_app[i]);
                    cell(lbl, &g_msTog[(pbase + a*MS_APPN + i)&16383]); ++shown; } } }
        }
        Glass::EndPanel();
        char ft[80]; if (matches==0) snprintf(ft,sizeof(ft),"No settings match \"%s\"", q);
        else if (shown<matches) snprintf(ft,sizeof(ft),"Showing %d of %d matches \xE2\x80\x94 refine to see more", shown, matches);
        else snprintf(ft,sizeof(ft),"%d match%s", matches, matches==1?"":"es");
        Glass::SeparatorText(ft);
        return;
    }
    { const char* tabs[2] = { "System", "Per-App" }; tab = Glass::SegmentedControl("mstab", tabs, 2, tab); }
    ImGui::Dummy(ImVec2(0,6));
    if (tab == 0) {
        int c = MSChipRow(MSCuratedName, CuratedCatCount(), selSys); if (c>=0) selSys=c;
        if (selSys<0||selSys>=CuratedCatCount()) selSys=0;
        ImGui::Dummy(ImVec2(0,6));
        const MegaCat& cat = CuratedCat(selSys);
        int base = 0; for (int k=0;k<selSys;k++) base += CuratedCat(k).n;
        Glass::BeginPanel(cat.name);
        float gw = ImGui::GetContentRegionAvail().x; int nc = MSCols(gw);
        float cellW = (gw - (nc-1)*MS_COLGAP) / nc;
        for (int i=0;i<cat.n;i++){ if (i % nc != 0) ImGui::SameLine(0, MS_COLGAP);
            MSCompactToggle(cat.items[i], &g_msTog[(base+i)&16383], cellW); }
        Glass::EndPanel();
    } else {
        int c = MSChipRow(MSAppName, Glass::SurfaceCount(), selApp); if (c>=0) selApp=c;
        if (selApp<0||selApp>=Glass::SurfaceCount()) selApp=0;
        ImGui::Dummy(ImVec2(0,6));
        int pbase = CuratedTotal() + selApp*MS_APPN;
        char pn[64]; snprintf(pn,sizeof(pn), "%s SETTINGS", Glass::SurfaceName(selApp));
        Glass::BeginPanel(pn);
        float gw = ImGui::GetContentRegionAvail().x; int nc = MSCols(gw);
        float cellW = (gw - (nc-1)*MS_COLGAP) / nc;
        for (int i=0;i<MS_APPN;i++){ if (i % nc != 0) ImGui::SameLine(0, MS_COLGAP);
            MSCompactToggle(MS_app[i], &g_msTog[(pbase+i)&16383], cellW); }
        Glass::EndPanel();
    }
}
}
