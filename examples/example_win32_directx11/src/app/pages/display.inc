            else if (dispPage == 1) {
                Glass::BeginPanel("BRIGHTNESS");
                ImGui::SetNextItemWidth(220.0f*scale); Glass::Slider("Brightness",   &timeOfDay, 0.0f, 24.0f, "%.0f");
                ImGui::SetNextItemWidth(220.0f*scale); Glass::Slider("Auto-Lock",    &gravity, 0.0f, 30.0f, "%.0f min");
                ImGui::SetNextItemWidth(220.0f*scale); Glass::Slider("Color Temp.",  &fogDensity, 0.0f, 100.0f, "%.0f%%");
                Glass::EndPanel();
                Glass::BeginPanel("APPEARANCE");
                Glass::RowToggle(Glass::Icon::Sun,  IM_COL32(52,199,89,255), "True Tone",   &showGrid);
                Glass::Tooltip("Automatically adapt the display to ambient light");
                Glass::RowToggle(Glass::Icon::Moon, IM_COL32(255,204,0,255), "Night Shift", &showLightVolumes);
                Glass::EndPanel();
                Glass::BeginPanel("VOLUME");
                static float vWind = 0.4f, vRain = 0.1f, vAmb = 0.7f;
                Glass::VSlider("ring",  &vWind, 0.0f, 1.0f, ImVec2(34, 92)); ImGui::SameLine(0, 28);
                Glass::VSlider("media", &vRain, 0.0f, 1.0f, ImVec2(34, 92)); ImGui::SameLine(0, 28);
                Glass::VSlider("alarm", &vAmb,  0.0f, 1.0f, ImVec2(34, 92));
                Glass::EndPanel();
                Glass::BeginPanel("INFORMATION");
                { char b[48];
                  Glass::StatRow("Resolution", "2556 \xC3\x97 1179"); Glass::StatRow("Refresh Rate", "120 Hz");
                  std::snprintf(b, sizeof(b), "%.0f nits", drawDist); Glass::StatRow("Peak Brightness", b);
                  Glass::StatRow("Color", "P3 Wide Gamut"); }
                Glass::EndPanel();
                if (Glass::Button("Reset Display Settings", true))
                    Glass::Notify(Glass::Icon::Sun, IM_COL32(255,149,0,255), "Settings", "Display", "Display settings reset.");
            }
