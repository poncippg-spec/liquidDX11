            else if (dispPage == 3) {
                Glass::BeginPanel("CATEGORIES");
                Glass::RowToggle(Glass::Icon::Images,   IM_COL32(255,69,58,255),  "Photos", &itPotion);
                Glass::RowToggle(Glass::Icon::Music,    IM_COL32(255,204,0,255),  "Music",  &itSword);
                Glass::RowToggle(Glass::Icon::Tv,       IM_COL32(94,92,230,255),  "TV",     &itShield);
                Glass::RowToggle(Glass::Icon::Envelope, IM_COL32(255,149,0,255),  "Mail",   &itKey);
                Glass::EndPanel();
                Glass::BeginPanel("MANAGE");
                Glass::Stepper("Keep Backups", &spawnCount, 1, 1, 99); ImGui::Dummy(ImVec2(0, 4));
                if (Glass::Chip("Apps", catWeapons)) catWeapons = !catWeapons;
                ImGui::SameLine(); if (Glass::Chip("Media", catArmor)) catArmor = !catArmor;
                ImGui::SameLine(); if (Glass::Chip("Documents", catMisc)) catMisc = !catMisc;
                Glass::EndPanel();
                Glass::BeginPanel("SUMMARY");
                { char b[48]; int total = (itPotion?1:0)+(itSword?1:0)+(itShield?1:0)+(itKey?1:0);
                  std::snprintf(b, sizeof(b), "%d categories", total); Glass::StatRow("Optimized", b);
                  std::snprintf(b, sizeof(b), "%d", spawnCount); Glass::StatRow("Backups", b);
                  Glass::StatRow("Used", "171.8 GB"); Glass::StatRow("Available", "84.2 GB"); }
                Glass::EndPanel();
                if (Glass::Button("Offload Unused Apps", true))
                    Glass::Notify(Glass::Icon::Box, IM_COL32(255,69,58,255), "Settings", "Storage", "Unused apps were offloaded.");
            }
