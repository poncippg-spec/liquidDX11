            else if (dispPage == 6) {
                Glass::BeginPanel("ACCOUNT");
                Glass::TextField("Name", acctName, (int)sizeof(acctName), "Your name", Glass::Icon::Person);
                Glass::TextField("Email", acctEmail, (int)sizeof(acctEmail), "Email address", Glass::Icon::Globe);
                Glass::EndPanel();
                Glass::BeginPanel("PREFERENCES");
                Glass::RowToggle(Glass::Icon::Bell,    IM_COL32(255,69,58,255),  "Notifications", &setNotif);
                if (Glass::RowToggle(Glass::Icon::Speaker, IM_COL32(10,132,255,255), "Sounds",        &setSounds)) Glass::SetSfxEnabled(setSounds);
                Glass::RowToggle(Glass::Icon::Bolt,    IM_COL32(255,149,0,255),  "Haptics",       &setHaptics);
                Glass::RowToggle(Glass::Icon::Cloud,   IM_COL32(52,199,89,255),  "Auto-Update",   &setAutoUpdate);
                Glass::EndPanel();
                Glass::BeginPanel("RECORDING");
                if (Glass::Button(g_recording ? "Stop Recording" : "Record Video", true)) {
                    if (g_recording) RecorderStop();
                    else { screenshotMode = false; ::SetWindowDisplayAffinity(hwnd, 0x11u); RecorderStart(cw, ch); }
                }
                ImGui::SameLine();
                if (g_recording) { int s = (int)(g_rec_time / 10000000LL); char rb[32];
                    std::snprintf(rb, sizeof(rb), "Recording  %d:%02d", s/60, s%60); ImGui::TextUnformatted(rb); }
                else ImGui::TextDisabled("Saves a 1080p MP4 to glass_recording.mp4  (or F9)");
                ImGui::Dummy(ImVec2(0, 6));
                { bool prev = screenshotMode;
                  Glass::RowToggle(Glass::Icon::Eye, IM_COL32(94,92,230,255), "Allow OBS / Snipping Tool", &screenshotMode);
                  if (screenshotMode != prev) ::SetWindowDisplayAffinity(hwnd, screenshotMode ? 0u : 0x11u); }
                ImGui::TextDisabled("Built-in recorder works without OBS (no WDA tricks needed).");
                Glass::EndPanel();
                Glass::BeginPanel("APPEARANCE");
                const char* themes[] = { "Dark", "Light", "Auto" };
                Glass::Dropdown("theme2", themes, 3, &theme);
                ImGui::Dummy(ImVec2(0, 6));
                const char* layouts[] = { "Comfort", "Compact", "Spacious" };
                setLayout = Glass::SegmentedControl("layout", layouts, 3, setLayout);
                Glass::EndPanel();
                Glass::BeginPanel("AUDIO");
                ImGui::SetNextItemWidth(220.0f*scale); Glass::Slider("Volume",     &setVolume, 0.0f, 1.0f, "%.2f");
                ImGui::SetNextItemWidth(220.0f*scale); Glass::Slider("Brightness", &setBright, 0.0f, 1.0f, "%.2f");
                Glass::EndPanel();
                Glass::BeginPanel("PRIVACY");
                Glass::Checkbox("Share analytics", &setChkA);
                Glass::Checkbox("Personalized ads", &setChkB);
                Glass::Checkbox("Crash reports", &setChkC);
                Glass::EndPanel();
                Glass::BeginPanel("PLAN");
                const char* plans[] = { "Free", "Plus", "Pro", "Team" };
                setRadio = Glass::RadioGroup("plan", plans, 4, setRadio);
                Glass::EndPanel();
                Glass::BeginPanel("DEVICES");
                Glass::Stepper("Linked Devices", &setStep, 1, 1, 10);
                ImGui::Dummy(ImVec2(0, 6));
                if (Glass::Chip("Beta",    setTagA)) setTagA = !setTagA; ImGui::SameLine();
                if (Glass::Chip("Insider", setTagB)) setTagB = !setTagB; ImGui::SameLine();
                if (Glass::Chip("Dev",     setTagC)) setTagC = !setTagC;
                Glass::EndPanel();
                Glass::CollapsingTitle("Components", ImGui::GetScrollY());
                ImGui::Dummy(ImVec2(0, 4));
                { const char* menus[] = { "File", "Edit", "View", "Window", "Help" };
                  int mi = Glass::MenuBar("mbar", menus, 5);
                  if (mi >= 0) { static const char* it[] = { "New", "Open\xE2\x80\xA6", "Save", "Close" };
                                 Glass::ContextMenuOpen(io.MousePos.x, io.MousePos.y + 6.0f, it, 4, 3); } }
                Glass::BeginPanel("LIQUID METABALL");
                {
                    float bw = ImGui::GetContentRegionAvail().x; if (bw > 320.0f) bw = 320.0f;
                    const float bhh = 112.0f;
                    ImVec2 bc = ImGui::GetCursorScreenPos();
                    ImGui::InvisibleButton("##blobarea", ImVec2(bw, bhh));
                    bool bhov = ImGui::IsItemHovered();
                    float ax = bc.x + bw*0.34f, ay = bc.y + bhh*0.5f;
                    float mx = bhov ? io.MousePos.x : bc.x + bw*0.66f;
                    float my = bhov ? io.MousePos.y : bc.y + bhh*0.5f;
                    if (mx < bc.x+26) mx = bc.x+26; if (mx > bc.x+bw-26) mx = bc.x+bw-26;
                    if (my < bc.y+26) my = bc.y+26; if (my > bc.y+bhh-26) my = bc.y+bhh-26;
                    Glass::SubmitBlob(ax, ay, 28.0f, 28.0f, mx, my, 22.0f, 22.0f, 26.0f, 28.0f, 1.0f, Glass::Material::Accent);
                    ImGui::GetWindowDrawList()->AddText(ImVec2(bc.x + 4, bc.y + bhh - 18),
                        IM_COL32(151,154,168,200), "hover to merge the glass blobs");
                }
                Glass::EndPanel();
                Glass::BeginPanel("SEGMENTS");
                { Glass::Icon ti[3] = { Glass::Icon::Person, Glass::Icon::Globe, Glass::Icon::Gear };
                  const char* tl[3] = { "Profile", "Network", "System" };
                  scTab = Glass::TabBar("sctab", ti, tl, 3, scTab); }
                Glass::EndPanel();
                Glass::BeginPanel("MEDIA");
                Glass::Scrubber("scsc", &scScrub, 0.0f, 1.0f);
                ImGui::Dummy(ImVec2(0, 8));
                Glass::Spinner("scspin", 14.0f);
                ImGui::SameLine(0, 16); ImGui::AlignTextToFramePadding(); ImGui::TextDisabled("Loading\xE2\x80\xA6");
                Glass::EndPanel();
                Glass::BeginPanel("PICKER");
                { const char* di[6] = { "Low", "Medium", "High", "Ultra", "Max", "Off" };
                  scDrum = Glass::DrumPicker("scdrum", di, 6, scDrum, ImVec2(150.0f, 118.0f)); }
                Glass::EndPanel();
                if (Glass::BeginDisclosure("Advanced", &scDisc)) {
                    Glass::RowToggle(Glass::Icon::Bolt, IM_COL32(255,149,0,255), "Experimental", &scAdvT);
                    ImGui::SetNextItemWidth(220.0f*scale); Glass::Slider("Sensitivity", &scAdvS, 0.0f, 1.0f, "%.2f");
                }
                Glass::EndDisclosure();
                Glass::BeginPanel("DATE");
                Glass::Calendar("cal", &v3Day, &v3Month, &v3Year);
                Glass::EndPanel();
                Glass::BeginPanel("LIBRARY");
                { const char* tl[] = { "Workspace", "Designs", "Glass.fig", "Icons", "Code", "main.cpp" };
                  int dp[] = { 0,1,2,1,0,1 };
                  Glass::Icon ic[] = { Glass::Icon::Box, Glass::Icon::Box, Glass::Icon::Palette, Glass::Icon::Box, Glass::Icon::Cube, Glass::Icon::Eye };
                  v3Tree = Glass::OutlineTree("tree", tl, dp, ic, 6, v3Tree); }
                Glass::EndPanel();
                Glass::BeginPanel("TRANSPORT");
                Glass::MorphButton("mp1", Glass::MorphIcon::PlayPause, &v3Play); ImGui::SameLine();
                { static bool m2 = false; Glass::MorphButton("mp2", Glass::MorphIcon::PlusClose, &m2); ImGui::SameLine(); }
                { static bool m3 = false; Glass::MorphButton("mp3", Glass::MorphIcon::MenuClose, &m3); }
                ImGui::SameLine(0, 20); ImGui::AlignTextToFramePadding(); Glass::Odometer("odo", v3Odo, 1.7f);
                ImGui::SameLine(0, 14); if (Glass::Button("+10", false)) v3Odo += 10;
                Glass::EndPanel();
                Glass::BeginPanel("METRICS");
                { float fr[3] = { 0.7f, 0.45f, 0.9f };
                  ImU32 rc[3] = { IM_COL32(255,69,58,255), IM_COL32(52,209,89,255), IM_COL32(10,132,255,255) };
                  Glass::ActivityRings("rings", fr, rc, 3, 40.0f); }
                ImGui::SameLine(0, 22); Glass::Gauge("gauge", v3Gauge, 0.0f, 100.0f, 44.0f, "%.0f");
                ImGui::Dummy(ImVec2(0, 8)); Glass::LevelIndicator(0.62f, 10, ImVec2(190, 10));
                ImGui::Dummy(ImVec2(0, 8)); v3Page = Glass::PageDots("dots", 5, v3Page);
                Glass::EndPanel();
                Glass::BeginPanel("LOADING");
                Glass::Skeleton("sk1", ImVec2(230, 14)); ImGui::Dummy(ImVec2(0, 6));
                Glass::Skeleton("sk2", ImVec2(170, 14)); ImGui::SameLine(0, 14);
                Glass::Spinner("ld", 11.0f);
                Glass::EndPanel();
                Glass::BeginPanel("TAGS");
                Glass::TokenField("tok", v3Tokens, &v3TokCount, 8, v3TokIn, (int)sizeof(v3TokIn));
                ImGui::Dummy(ImVec2(0, 6));
                { const char* models[] = { "Opus", "Sonnet", "Haiku" }; Glass::ComboBox("Model", v3Combo, (int)sizeof(v3Combo), models, 3); }
                Glass::EndPanel();
                Glass::BeginPanel("SWIPE");
                { const char* acts[] = { "Archive", "Delete" }; ImU32 ac[] = { IM_COL32(10,132,255,255), IM_COL32(255,69,58,255) };
                  Glass::SwipeRow("sw1", "Swipe this row left \xE2\x80\xA6", acts, ac, 2); }
                Glass::EndPanel();
                Glass::BeginPanel("PRESENT");
                if (Glass::Button("Alert", false))
                    Glass::AlertOpen("Delete file?", "This item will be permanently removed. This action cannot be undone.", "Delete", "Cancel", true);
                ImGui::SameLine();
                if (Glass::Button("Action Sheet", false)) {
                    static const char* as[3] = { "Take Photo", "Photo Library", "Choose File" };
                    Glass::ActionSheetOpen("Add Attachment", as, 3, -1);
                }
                ImGui::SameLine();
                if (Glass::Button("Command \xE2\x8C\x98K", false)) Glass::CommandPaletteToggle();
                ImGui::Dummy(ImVec2(0, 6));
                if (Glass::Button("Share Sheet", false)) {
                    static Glass::Icon shi[6] = { Glass::Icon::Person, Glass::Icon::Globe, Glass::Icon::Cloud, Glass::Icon::Heart, Glass::Icon::Star, Glass::Icon::Box };
                    static const char* shl[6] = { "Nearby", "Browser", "Cloud", "Favorite", "Star", "More" };
                    Glass::ShareSheetOpen("Share", shi, shl, 6);
                }
                ImGui::SameLine(); if (Glass::Button("Onboarding", false)) showOnboard = true;
                ImGui::Dummy(ImVec2(0, 6));
                { bool sc = Glass::Button("Success", false); Glass::SuccessHUD("succ", sc);
                  ImGui::SameLine(); if (Glass::Button("Banner", false))
                      Glass::BannerOpen(Glass::Icon::Bell, IM_COL32(255,149,0,255), "now", "Heads up", "A glass banner slid in from the top."); }
                Glass::EndPanel();
                if (Glass::Button("Save Changes", true)) {}
                ImGui::SameLine(); if (Glass::Button("Sign Out", false)) { if (confirmSignout) signoutPending = true; else logged_in = false; }
            }
