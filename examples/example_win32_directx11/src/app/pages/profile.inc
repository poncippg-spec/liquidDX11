            else if (dispPage == 5) {
                ImU32 stc[4] = { IM_COL32(52,209,88,255), IM_COL32(255,159,10,255), IM_COL32(255,69,58,255), IM_COL32(142,142,147,255) };
                const char* stn[4] = { "Online", "Away", "Do Not Disturb", "Invisible" };
                int sci = profStatus < 0 ? 0 : (profStatus > 3 ? 3 : profStatus);
                Glass::BeginPanel("PROFILE");
                Glass::Avatar("JA", 64.0f, IM_COL32(10,132,255,255));
                ImGui::SameLine(0, 16); ImGui::BeginGroup();
                ImGui::PushFont(font_semibold); ImGui::TextUnformatted(acctName); ImGui::PopFont();
                ImGui::Dummy(ImVec2(0, 2)); ImGui::TextDisabled("%s", acctEmail);
                ImGui::Dummy(ImVec2(0, 5)); Glass::StatusPill(stn[sci], stc[sci]);
                ImGui::SameLine(0, 8); Glass::Badge("PRO", IM_COL32(255,149,0,255));
                ImGui::EndGroup(); Glass::EndPanel();
                Glass::BeginPanel("LEVEL");
                Glass::ProgressRing("xp", 0.72f, 30.0f, "72%");
                ImGui::SameLine(0, 16); ImGui::BeginGroup(); ImGui::Dummy(ImVec2(0, 9));
                ImGui::TextUnformatted("Level 24"); ImGui::TextDisabled("3,480 / 5,000 XP");
                ImGui::EndGroup();
                ImGui::Dummy(ImVec2(0, 6)); Glass::ProgressBar(0.72f);
                Glass::EndPanel();
                Glass::BeginPanel("STATS");
                Glass::StatRow("Sessions", "1,204"); Glass::StatRow("Play Time", "389 h");
                Glass::StatRow("Rank", "Diamond II"); Glass::StatRow("Member Since", "2021");
                Glass::EndPanel();
                Glass::BeginPanel("SKILL");
                static int psk = 4; psk = Glass::Rating("psk", psk);
                Glass::EndPanel();
                if (Glass::Button("Edit Profile", true)) page = 6;
                ImGui::SameLine(); if (Glass::Button("Back", false)) page = 0;
            }
