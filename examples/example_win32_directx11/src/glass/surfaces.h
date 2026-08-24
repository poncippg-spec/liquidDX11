#pragma once
#include "glass.h"

namespace Glass {
int          SurfaceCount();
const char*  SurfaceName(int i);
const char*  SurfaceSubtitle(int i);
Icon         SurfaceIcon(int i);
ImU32        SurfaceTint(int i);
void         SurfaceDraw(int i, float w);
int          SurfaceLaunchpad(float w, const char* query);
int          SurfaceDock(float cx, float bottomY, float maxW, int activeSurface = -1);
}
