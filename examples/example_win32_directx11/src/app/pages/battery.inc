            else if (dispPage == 2) {
                Glass::BeginPanel("CHARGING");
                static int vmode = 0;
                Glass::Icon vmIcons[] = { Glass::Icon::BatteryFull, Glass::Icon::Bolt, Glass::Icon::Moon, Glass::Icon::Leaf };
                vmode = Glass::SegmentedIcons("vmode", vmIcons, 4, vmode);
                ImGui::Dummy(ImVec2(0, 10));
                static float exposure = 1.0f;
                Glass::NumberDrag("Charge Limit (drag)", &exposure, 0.01f, 0.1f, 4.0f, "%.2f");
                Glass::EndPanel();
                Glass::BeginPanel("POWER");
                ImGui::SetNextItemWidth(220.0f*scale); Glass::Slider("Low Power At", &drawDist, 100.0f, 4000.0f, "%.0f%%");
                ImGui::SetNextItemWidth(220.0f*scale); Glass::Slider("Dim After",    &fov, 50.0f, 120.0f, "%.0f s");
                Glass::EndPanel();
                Glass::BeginPanel("OPTIONS");
                Glass::RowToggle(Glass::Icon::Bolt,  IM_COL32(255,149,0,255), "Low Power Mode",     &rWireframe);
                Glass::RowToggle(Glass::Icon::Eye,   IM_COL32(255,69,58,255), "Battery Percentage", &rColliders);
                Glass::RowToggle(Glass::Icon::Gauge, IM_COL32(94,92,230,255), "Optimized Charging", &rNormals);
                Glass::RowToggle(Glass::Icon::Leaf,  IM_COL32(52,209,88,255), "Clean Energy Charging", &rVsync);
                Glass::EndPanel();
                Glass::BeginPanel("BATTERY HEALTH");
                const char* q[] = { "Low", "Medium", "High", "Ultra" };
                Glass::Dropdown("quality", q, 4, &quality);
                Glass::EndPanel();
                Glass::BeginPanel("USAGE");
                { Glass::StatRow("Level", "87%"); Glass::StatRow("Time Remaining", "6h 12m"); Glass::StatRow("Maximum Capacity", "94%"); Glass::StatRow("Cycle Count", "212"); }
                Glass::EndPanel();
            }
