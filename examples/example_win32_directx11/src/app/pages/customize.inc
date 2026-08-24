            else if (dispPage == 4) {
                Glass::BeginPanel("PERFORMANCE");
                ImGui::SetNextItemWidth(220.0f*scale); Glass::Slider("UI Refresh", &uiFps, 1.0f, 1000.0f, "%.0f Hz");
                ImGui::SetNextItemWidth(220.0f*scale); Glass::Slider("Blur Refresh", &blurFps, 1.0f, 1000.0f, "%.0f Hz");
                Glass::EndPanel();
                Glass::BeginPanel("APPEARANCE");
                { const char* ap[2] = { "Dark", "Light" }; appearance = Glass::SegmentedControl("appr", ap, 2, appearance); }
                Glass::EndPanel();
                Glass::BeginPanel("ACCENT COLOR");
                { struct AC { float r, g, b; };
                  static const AC pal[16] = {
                      {0.02f,0.48f,1.0f},{0.20f,0.78f,0.35f},{1.0f,0.58f,0.0f},{1.0f,0.27f,0.23f},
                      {0.69f,0.32f,0.87f},{1.0f,0.18f,0.33f},{0.35f,0.34f,0.84f},{0.0f,0.78f,0.75f},
                      {1.0f,0.80f,0.0f},{0.56f,0.79f,0.98f},{0.99f,0.45f,0.52f},{0.40f,0.47f,0.55f},
                      {0.30f,0.85f,0.39f},{0.58f,0.44f,0.86f},{0.0f,0.60f,0.55f},{0.93f,0.31f,0.59f} };
                  ImVec2 base = ImGui::GetCursorScreenPos(); ImDrawList* sd = ImGui::GetWindowDrawList();
                  const float sw = 30.0f, gp = 10.0f;
                  for (int i = 0; i < 16; ++i) { int r = i/8, c = i%8;
                      float cx = base.x + c*(sw+gp) + sw*0.5f, cy = base.y + r*(sw+gp) + sw*0.5f;
                      ImGui::SetCursorScreenPos(ImVec2(cx-sw*0.5f, cy-sw*0.5f)); ImGui::PushID(i);
                      ImGui::InvisibleButton("sw", ImVec2(sw, sw));
                      bool hov = ImGui::IsItemHovered(), act = ImGui::IsItemActive();
                      if (ImGui::IsItemDeactivated() && hov) { accent[0]=pal[i].r; accent[1]=pal[i].g; accent[2]=pal[i].b; accentSel=i; }
                      Glass::Spring& ss = Glass::g->springs().Get((uint32_t)ImGui::GetID("sw"), 1, Glass::SpringStyle::Bouncy, 1.0f);
                      ss.target = act ? 0.86f : (hov ? 1.18f : 1.0f); ss.Tick(io.DeltaTime);
                      Glass::Spring& sr = Glass::g->springs().Get((uint32_t)ImGui::GetID("sw"), 2, Glass::SpringStyle::Bouncy, accentSel==i?1.0f:0.0f);
                      sr.target = (accentSel==i) ? 1.0f : 0.0f; sr.Tick(io.DeltaTime);
                      ImGui::PopID();
                      float rad = sw*0.5f * Glass_Clampf(ss.x, 0.8f, 1.25f);
                      ImU32 col = IM_COL32((int)(pal[i].r*255),(int)(pal[i].g*255),(int)(pal[i].b*255),255);
                      if (hov) sd->AddCircleFilled(ImVec2(cx, cy+2.0f), rad+1.5f, IM_COL32(0,0,0,45));
                      sd->AddCircleFilled(ImVec2(cx, cy), rad, col);
                      sd->AddCircleFilled(ImVec2(cx-rad*0.28f, cy-rad*0.32f), rad*0.42f, IM_COL32(255,255,255,55));
                      float k = Glass_Clampf(sr.x, 0.0f, 1.0f);
                      if (k > 0.01f) {
                          sd->AddCircle(ImVec2(cx, cy), rad + 2.0f + 3.0f*k, Glass::InkColor(), 0, 1.6f + 1.0f*k);
                          float m = rad*0.40f*k; ImU32 wcol = IM_COL32(255,255,255,(int)(235*k));
                          sd->AddLine(ImVec2(cx-m, cy+m*0.05f), ImVec2(cx-m*0.15f, cy+m*0.7f), wcol, 2.2f);
                          sd->AddLine(ImVec2(cx-m*0.15f, cy+m*0.7f), ImVec2(cx+m, cy-m*0.6f), wcol, 2.2f);
                      }
                  }
                  ImGui::SetCursorScreenPos(base); ImGui::Dummy(ImVec2(8*(sw+gp), 2*(sw+gp)+6.0f)); }
                Glass::ColorButton("Custom\xE2\x80\xA6", accent);
                Glass::EndPanel();
                Glass::BeginPanel("TEXT & LAYOUT");
                ImGui::SetNextItemWidth(220.0f*scale); Glass::Slider("Font Scale", &fontScale, 0.80f, 1.40f, "%.2f");
                Glass::RowToggle(Glass::Icon::Font, 0, "Bold Text", &boldText);
                Glass::RowToggle(Glass::Icon::PenEdit, 0, "Black Text", &blackText);
                { const char* dn[3] = { "Compact", "Default", "Spacious" }; uiDensity = Glass::SegmentedControl("dens", dn, 3, uiDensity); }
                Glass::EndPanel();
                Glass::BeginPanel("MOTION");
                ImGui::SetNextItemWidth(220.0f*scale); Glass::Slider("Animation Speed", &animSpeed, 0.25f, 2.0f, "%.2f");
                Glass::RowToggle(Glass::Icon::Bolt, 0, "Reduce Motion", &reduceMotion);
                Glass::EndPanel();
                Glass::BeginPanel("LAYOUT & EFFECTS");
                ImGui::SetNextItemWidth(220.0f*scale); Glass::Slider("Sidebar Width", &sidebarW, 180.0f, 300.0f, "%.0f");
                Glass::RowToggle(Glass::Icon::LocationArrow, 0, "Cursor Droplet",  &fxCursor);
                Glass::RowToggle(Glass::Icon::Wind,          0, "Motion Parallax", &fxParallax);
                Glass::EndPanel();
                Glass::BeginPanel("GRAPHICS QUALITY");
                Glass::RowToggle(Glass::Icon::Speaker, 0, "Record UI Sounds", &g_recAudio);
                Glass::RowToggle(Glass::Icon::Display, 0, "Supersampling (SSAA)", &ssaaOn);
                if (ssaaOn) { ImGui::SetNextItemWidth(220.0f*scale); Glass::Slider("Quality Scale", &ssaaScale, 1.25f, 3.0f, "%.2fx"); }
                Glass::EndPanel();
                Glass::BeginPanel("ACCESSIBILITY");
                Glass::RowToggle(Glass::Icon::Eye,     0, "Reduce Transparency", &reduceTransp);
                Glass::RowToggle(Glass::Icon::Display, 0, "Increase Contrast",   &hiContrast);
                Glass::RowToggle(Glass::Icon::Palette, 0, "Vibrant Colors",      &vibrantUI);
                Glass::RowToggle(Glass::Icon::ToggleOn,0, "On/Off Labels",       &btnShapes);
                Glass::EndPanel();
                Glass::BeginPanel("SOUND & HAPTICS");
                ImGui::SetNextItemWidth(220.0f*scale); Glass::Slider("UI Volume", &fxVolume, 0.0f, 1.0f, "%.2f");
                Glass::RowToggle(Glass::Icon::Speaker, 0, "Sound Effects",   &fxSound);
                Glass::RowToggle(Glass::Icon::Bolt,    0, "Haptics",         &fxHaptics);
                Glass::RowToggle(Glass::Icon::Keyboard,0, "Keyboard Clicks", &kbClicks);
                Glass::EndPanel();
                Glass::BeginPanel("TIME & REGION");
                Glass::RowToggle(Glass::Icon::Clock, 0, "24-Hour Time", &time24);
                Glass::RowToggle(Glass::Icon::Clock, 0, "Show Seconds", &showSeconds);
                { const char* wk[3] = { "Sun", "Mon", "Sat" }; firstWeekday = Glass::SegmentedControl("wk", wk, 3, firstWeekday); }
                { const char* tu[2] = { "Fahrenheit", "Celsius" }; tempUnit = Glass::SegmentedControl("tu", tu, 2, tempUnit); }
                Glass::EndPanel();
                Glass::BeginPanel("WINDOW");
                Glass::RowToggle(Glass::Icon::ChevronU, 0, "Always on Top",    &alwaysTop);
                Glass::RowToggle(Glass::Icon::Grid,     0, "Snap to Edges",    &snapEdges);
                Glass::RowToggle(Glass::Icon::Eye,      0, "Auto-Hide Chrome", &fxAutoHide);
                Glass::RowToggle(Glass::Icon::Gauge,    0, "Show FPS",         &fxShowFps);
                Glass::EndPanel();
                Glass::BeginPanel("NOTIFICATIONS");
                Glass::RowToggle(Glass::Icon::Bell,    0, "Allow Notifications", &nfAllow);
                Glass::RowToggle(Glass::Icon::Star,    0, "Badges",        &nfBadges);
                Glass::RowToggle(Glass::Icon::Speaker, 0, "Sounds",        &nfSounds);
                Glass::RowToggle(Glass::Icon::Eye,     0, "Show Previews", &nfPreviews);
                Glass::RowToggle(Glass::Icon::Grid,    0, "Group by App",  &nfGroup);
                Glass::EndPanel();
                Glass::BeginPanel("KEYBOARD");
                Glass::RowToggle(Glass::Icon::Font,     0, "Auto-Capitalization", &kbAutoCap);
                Glass::RowToggle(Glass::Icon::Check,    0, "Auto-Correction",     &kbAutoCorr);
                Glass::RowToggle(Glass::Icon::Lightbulb, 0, "Predictive Text", &kbPredict);
                Glass::EndPanel();
                Glass::BeginPanel("GENERAL");
                Glass::RowToggle(Glass::Icon::Clock,    0, "Restore Last Page",    &genRestore);
                Glass::RowToggle(Glass::Icon::InfoCircle,0, "Show Tooltips",        &genTooltips);
                Glass::RowToggle(Glass::Icon::Plus,     0, "Larger Touch Targets", &genLargeTargets);
                Glass::RowToggle(Glass::Icon::Microphone,0, "Assistant Suggestions",&siriSuggest);
                Glass::EndPanel();
                Glass::BeginPanel("PRIVACY");
                Glass::RowToggle(Glass::Icon::Lock,         0, "Confirm Sign Out", &confirmSignout);
                Glass::RowToggle(Glass::Icon::ShieldHalf,   0, "Share Analytics",  &analytics);
                Glass::RowToggle(Glass::Icon::WarnTriangle, 0, "Crash Reports",    &crashRpt);
                Glass::EndPanel();
                Glass::BeginPanel("PRESETS");
                { struct GP { const char* n; float b, o, s, r, c, e, sh; };
                  static const GP gp[5] = {
                      { "Subtle",  0.30f, 0.20f, 1.12f, 3.0f, 0.4f, 0.00f, 0.00f },
                      { "Default", 0.50f, 0.35f, 1.30f, 6.0f, 0.8f, 0.00f, 0.00f },
                      { "Frost",   0.78f, 0.45f, 1.08f, 4.0f, 0.4f, 0.00f, 0.10f },
                      { "Vivid",   0.42f, 0.30f, 1.55f, 8.0f, 1.4f, 0.05f, 0.00f },
                      { "Deep",    0.58f, 0.50f, 1.02f, 5.0f, 0.6f, 0.00f, 0.20f },
                  };
                  for (int i = 0; i < 5; ++i) {
                      if (Glass::Button(gp[i].n, false)) {
                          gBlur = gp[i].b; gOpacity = gp[i].o; gSat = gp[i].s; gRefr = gp[i].r;
                          gChroma = gp[i].c; gEdge = gp[i].e; gShadow = gp[i].sh;
                      }
                      if (i < 4) ImGui::SameLine(0, 8);
                  } }
                Glass::EndPanel();
                Glass::BeginPanel("MATERIAL");
                ImGui::SetNextItemWidth(220.0f*scale); Glass::Slider("Blur",       &gBlur,    0.0f, 1.0f,   "%.2f");
                ImGui::SetNextItemWidth(220.0f*scale); Glass::Slider("Opacity",    &gOpacity, 0.0f, 1.0f,   "%.2f");
                ImGui::SetNextItemWidth(220.0f*scale); Glass::Slider("Saturation", &gSat,     0.0f, 12.0f,  "%.2f");
                ImGui::SetNextItemWidth(220.0f*scale); Glass::Slider("Refraction", &gRefr,    0.0f, 200.0f, "%.0f");
                ImGui::SetNextItemWidth(220.0f*scale); Glass::Slider("Dispersion", &gChroma,  0.0f, 30.0f,  "%.1f");
                ImGui::SetNextItemWidth(220.0f*scale); Glass::Slider("Edge Light", &gEdge,    0.0f, 5.0f,   "%.2f");
                ImGui::SetNextItemWidth(220.0f*scale); Glass::Slider("Shadow",     &gShadow,  0.0f, 3.0f,   "%.2f");
                ImGui::SetNextItemWidth(220.0f*scale); Glass::Slider("Liquidity",  &gFlow,    0.0f, 16.0f,  "%.1f");
                ImGui::SetNextItemWidth(220.0f*scale); Glass::Slider("Popup Blur", &uiBlurAmt, 0.0f, 12.0f, "%.2f");
                Glass::Checkbox("Smooth Refraction", &eSmoothRefr);
                ImGui::TextDisabled("creamier, gradual lensing at the rim (softer falloff + extra blur)");
                Glass::EndPanel();
                Glass::BeginPanel("GLASS EDGE");
                ImGui::SetNextItemWidth(220.0f*scale); Glass::Slider("Rim Falloff",      &eFresExp, 0.8f,  6.0f,   "%.2f");
                ImGui::SetNextItemWidth(220.0f*scale); Glass::Slider("Bevel Width",      &eBevel,   0.15f, 1.0f,   "%.2f");
                ImGui::SetNextItemWidth(220.0f*scale); Glass::Slider("Rim Lip",          &eLip,     0.0f,  0.6f,   "%.2f");
                ImGui::SetNextItemWidth(220.0f*scale); Glass::Slider("Glint Sharpness",  &eSpecExp, 6.0f,  120.0f, "%.0f");
                ImGui::SetNextItemWidth(220.0f*scale); Glass::Slider("Glint Strength",   &eSpecAmt, 0.0f,  3.0f,   "%.2f");
                ImGui::SetNextItemWidth(220.0f*scale); Glass::Slider("Top-Left Rim",     &eFront,   0.0f,  3.0f,   "%.2f");
                ImGui::SetNextItemWidth(220.0f*scale); Glass::Slider("Bottom-Right Rim", &eBack,    0.0f,  3.0f,   "%.2f");
                ImGui::SetNextItemWidth(220.0f*scale); Glass::Slider("Edge Color Pop",   &eSatPop,  0.0f,  3.0f,   "%.2f");
                ImGui::SetNextItemWidth(220.0f*scale); Glass::Slider("Top-Left Spread",     &eFrontSpread, 30.0f, 500.0f, "%.0f%%");
                ImGui::SetNextItemWidth(220.0f*scale); Glass::Slider("Bottom-Right Spread", &eBackSpread,  30.0f, 500.0f, "%.0f%%");
                ImGui::TextDisabled("each side stretches independently; higher = wraps further to the sides");
                ImGui::SetNextItemWidth(220.0f*scale); Glass::Slider("Panel Rim Location", &ePanelRimAngle, 0.0f, 360.0f, "%.0f\xC2\xB0");
                ImGui::SetNextItemWidth(220.0f*scale); Glass::Slider("Panel Rim Reach",    &ePanelBRSpread, 30.0f, 500.0f, "%.0f%%");
                ImGui::SetNextItemWidth(220.0f*scale); Glass::Slider("Panel Rim Smooth",   &ePanelBRSmooth, 30.0f, 400.0f, "%.0f%%");
                ImGui::TextDisabled("panels & buttons only (icons unchanged). Location = which corner the rim hugs");
                ImGui::TextDisabled("(0\xC2\xB0 top 90\xC2\xB0 right 180\xC2\xB0 bottom 270\xC2\xB0 left); Reach = how far it extends; Smooth = fade");
                Glass::Checkbox("Remove TR/BL Shadow", &eTBLRemove);
                ImGui::SetNextItemWidth(220.0f*scale); Glass::Slider("TR/BL Fill Amount", &eTBLFill, 0.0f, 2.0f, "%.2f");
                ImGui::TextDisabled("lifts the top-right & bottom-left corners so they don't read as dark");
                Glass::EndPanel();
                Glass::BeginPanel("ADAPTIVE OPACITY");
                Glass::Checkbox("Adaptive Opacity (auto)", &eAdaptOn);
                ImGui::SetNextItemWidth(220.0f*scale); Glass::Slider("Strength",         &eAdaptStrength, 0.0f,  1.0f, "%.2f");
                ImGui::SetNextItemWidth(220.0f*scale); Glass::Slider("Brightness Pivot",  &eAdaptPivot,    0.0f,  1.0f, "%.2f");
                ImGui::SetNextItemWidth(220.0f*scale); Glass::Slider("Softness",          &eAdaptSoft,     0.02f, 0.6f, "%.2f");
                ImGui::TextDisabled("brighter background -> more opaque glass, per-pixel. Pivot = brightness it kicks in");
                Glass::EndPanel();
                Glass::BeginPanel("CORNER SHAPE");
                Glass::Checkbox("Squircle Corners", &eSquircle);
                ImGui::SetNextItemWidth(220.0f*scale); Glass::Slider("Squircle Power", &eSquirclePow, 2.0f, 8.0f, "%.2f");
                ImGui::TextDisabled("2 = circular rounded-rect; higher = continuous-curvature squircle");
                Glass::EndPanel();
                Glass::BeginPanel("MAGNIFY LENS");
                ImGui::SetNextItemWidth(220.0f*scale); Glass::Slider("Lens Amount", &eLensAmt, 0.0f, 1.0f, "%.2f");
                ImGui::SetNextItemWidth(220.0f*scale); Glass::Slider("Lens Power",  &eLensPow, -4.0f, 4.0f, "%.2f");
                ImGui::SetNextItemWidth(220.0f*scale); Glass::Slider("Profile a", &eLensA, 0.0f, 5.0f,  "%.2f");
                ImGui::SetNextItemWidth(220.0f*scale); Glass::Slider("Profile b", &eLensB, 0.0f, 6.0f,  "%.2f");
                ImGui::SetNextItemWidth(220.0f*scale); Glass::Slider("Profile c", &eLensC, 0.0f, 6.0f,  "%.2f");
                ImGui::SetNextItemWidth(220.0f*scale); Glass::Slider("Profile d", &eLensD, 0.0f, 10.0f, "%.2f");
                ImGui::TextDisabled("whole-surface bulge: content magnifies through the glass. Power < 0 = concave/inverted");
                Glass::EndPanel();
                Glass::BeginPanel("ANGULAR GLOW");
                ImGui::SetNextItemWidth(220.0f*scale); Glass::Slider("Glow Weight", &eGlowW,     -1.0f, 1.0f, "%.2f");
                ImGui::SetNextItemWidth(220.0f*scale); Glass::Slider("Glow Angle",  &eGlowPhase, -3.14f, 3.14f, "%.2f");
                ImGui::SetNextItemWidth(220.0f*scale); Glass::Slider("Band Start",  &eGlowE0,    -1.0f, 1.0f, "%.2f");
                ImGui::SetNextItemWidth(220.0f*scale); Glass::Slider("Band End",    &eGlowE1,    -1.0f, 1.0f, "%.2f");
                ImGui::SetNextItemWidth(220.0f*scale); Glass::Slider("Glow Bias",   &eGlowBias,  -1.0f, 1.0f, "%.2f");
                ImGui::SetNextItemWidth(220.0f*scale); Glass::Slider("Sweep Speed", &eGlowAnim,   0.0f, 4.0f, "%.2f");
                ImGui::TextDisabled("directional rim sheen; Sweep Speed > 0 makes the glint rotate around the panel");
                Glass::EndPanel();
                Glass::BeginPanel("RIM LIGHT (TWO LIGHTS)");
                ImGui::SetNextItemWidth(220.0f*scale); Glass::Slider("Light 1 Angle",    &eLightAngle,  0.0f, 360.0f, "%.0f\xC2\xB0");
                ImGui::SetNextItemWidth(220.0f*scale); Glass::Slider("Light 1 Strength", &eLight1Amt,   0.0f, 3.0f,   "%.2f");
                ImGui::SetNextItemWidth(220.0f*scale); Glass::Slider("Light 2 Angle",    &eLight2Angle, 0.0f, 360.0f, "%.0f\xC2\xB0");
                ImGui::SetNextItemWidth(220.0f*scale); Glass::Slider("Light 2 Strength", &eLight2Amt,   0.0f, 3.0f,   "%.2f");
                ImGui::TextDisabled("0\xC2\xB0 top 90\xC2\xB0 right 180\xC2\xB0 bottom 270\xC2\xB0 left  -  raise Light 2 Strength to enable it");
                Glass::EndPanel();
                Glass::BeginPanel("MORE GLASS");
                ImGui::SetNextItemWidth(220.0f*scale); Glass::Slider("Bevel Curvature",  &eBevelTilt, 0.5f,  6.0f,   "%.2f");
                ImGui::SetNextItemWidth(220.0f*scale); Glass::Slider("Light Height",     &eLightH,    0.2f,  2.5f,   "%.2f");
                ImGui::SetNextItemWidth(220.0f*scale); Glass::Slider("Edge Shadow",      &eEdgeSh,    0.0f,  2.0f,   "%.2f");
                ImGui::SetNextItemWidth(220.0f*scale); Glass::Slider("Cursor Glow Size", &eCurSize,   40.0f, 400.0f, "%.0f");
                ImGui::SetNextItemWidth(220.0f*scale); Glass::Slider("Cursor Glow",      &eCurGlow,   0.0f,  3.0f,   "%.2f");
                ImGui::SetNextItemWidth(220.0f*scale); Glass::Slider("Sheen",            &eSheen,     0.0f,  3.0f,   "%.2f");
                ImGui::SetNextItemWidth(220.0f*scale); Glass::Slider("Grain",            &eGrain,     0.0f,  4.0f,   "%.2f");
                ImGui::SetNextItemWidth(220.0f*scale); Glass::Slider("Ambient Rim",      &eAmbRim,    0.0f,  2.0f,   "%.2f");
                Glass::EndPanel();
                Glass::BeginPanel("CONFIG FILE");
                if (Glass::Button("Save Current Look", true)) {
                    GlassConfig sc = CaptureCfg(g_cfg);
                    sc.win_w=(int)((wr.right-wr.left)/scale); sc.win_h=(int)((wr.bottom-wr.top)/scale);
                    sc.win_x=wr.left; sc.win_y=wr.top;
                    CfgSave(CfgSavePathStr(), sc); g_cfg = sc;
                }
                ImGui::TextDisabled("Writes glass.cfg next to the app (window size/pos included).");
                Glass::EndPanel();
                Glass::BeginPanel("PRESETS");
                ImGui::TextDisabled("Save the current look into a slot, or load one back (window size/pos unaffected).");
                for (int ps = 1; ps <= 6; ++ps) {
                    ImGui::PushID(ps);
                    char sb[24]; std::snprintf(sb, sizeof(sb), "Save %d", ps);
                    char lb[24]; std::snprintf(lb, sizeof(lb), "Load %d", ps);
                    bool exists = FileExists(PresetPathStr(ps));
                    if (Glass::Button(sb, true))  CfgSave(PresetPathStr(ps), CaptureCfg(g_cfg));
                    ImGui::SameLine();
                    if (Glass::Button(lb, false) && exists) { GlassConfig pc; if (LoadCfgFromPath(PresetPathStr(ps), pc)) ApplyCfg(pc); }
                    ImGui::SameLine(); ImGui::AlignTextToFramePadding();
                    ImGui::TextDisabled(exists ? "  saved" : "  empty");
                    ImGui::PopID();
                }
                Glass::EndPanel();
                Glass::BeginPanel("RESET");
                if (Glass::Button("Reset All Settings", false)) {
                    ApplyCfg(GlassConfig{});
                    accentSel=0; animSpeed=1.0f; reduceMotion=false; boldText=false; blackText=false;
                    reduceTransp=false; hiContrast=false; vibrantUI=true; btnShapes=false;
                    time24=false; showSeconds=false; firstWeekday=0; tempUnit=0;
                    fxHaptics=true; kbClicks=true; fxVolume=0.6f;
                    snapEdges=true; confirmSignout=true; analytics=false; crashRpt=true; sidebarW=231.0f;
                }
                Glass::EndPanel();
                Glass::SeparatorText("ALL SETTINGS");
                DrawMegaSettings();
            }
