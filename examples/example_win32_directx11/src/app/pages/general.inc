            if (dispPage == 0) {
                Glass::BeginPanel("ACCOUNT");
                Glass::Avatar("PO", 46.0f, IM_COL32(10,132,255,255)); ImGui::SameLine(0, 14);
                ImGui::BeginGroup();
                ImGui::TextUnformatted("poncipp"); ImGui::Dummy(ImVec2(0, 3));
                Glass::StatusPill("Online", IM_COL32(52,209,88,255));
                ImGui::SameLine(0, 8); Glass::Badge("Cloud+", IM_COL32(255,149,0,255));
                ImGui::EndGroup(); ImGui::Dummy(ImVec2(0, 8));
                static int skill = 4; skill = Glass::Rating("skill", skill);
                Glass::EndPanel();
                Glass::BeginPanel("DISPLAY & BRIGHTNESS");
                ImGui::SetNextItemWidth(220.0f*scale); Glass::Slider("Brightness", &posX, -1000.0f, 1000.0f, "%.0f");
                ImGui::SetNextItemWidth(220.0f*scale); Glass::Slider("Warmth",     &posY, -1000.0f, 1000.0f, "%.0f");
                ImGui::SetNextItemWidth(220.0f*scale); Glass::Slider("Contrast",   &posZ, -1000.0f, 1000.0f, "%.0f");
                ImGui::SetNextItemWidth(220.0f*scale); Glass::Slider("Text Size",  &pscale, 0.1f, 5.0f, "%.2f");
                Glass::EndPanel();
                Glass::BeginPanel("ACCESSIBILITY");
                Glass::RowToggle(Glass::Icon::Font,       IM_COL32(255,69,58,255),  "Bold Text",       &showHitbox);
                Glass::RowToggle(Glass::Icon::Bolt,       IM_COL32(255,149,0,255),  "Reduce Motion",   &dbgSkeleton);
                Glass::RowToggle(Glass::Icon::Lock,       IM_COL32(142,142,147,255),"Lock Rotation",   &freezePos);
                ImGui::SetNextItemWidth(220.0f*scale); Glass::Slider("Screen Reader Rate", &phealth, 0.0f, 100.0f, "%.0f");
                ImGui::SetNextItemWidth(220.0f*scale); Glass::Slider("Touch Hold Delay", &pspeed, 0.0f, 20.0f, "%.1f");
                Glass::EndPanel();
                Glass::BeginPanel("SHORTCUTS");
                Glass::Keybind("Screenshot",     &kbJump);
                Glass::Keybind("Search",         &kbSprint);
                Glass::Keybind("Lock Screen",    &kbInteract);
                Glass::Keybind("Do Not Disturb", &kbCrouch);
                Glass::EndPanel();
                Glass::BeginPanel("ABOUT");
                { Glass::StatRow("Name", "Glass Pro"); Glass::StatRow("Software Version", "1.0"); Glass::StatRow("Model Name", "Glass15,2"); Glass::StatRow("Capacity", "256 GB"); Glass::StatRow("Available", "84.2 GB"); }
                Glass::EndPanel();
                if (Glass::Button("Software Update", true))
                    Glass::Notify(Glass::Icon::Download, IM_COL32(10,132,255,255), "Settings", "Up to Date", "Your software is up to date.");
                ImGui::SameLine(); Glass::Button("Sign Out", false);
            }
