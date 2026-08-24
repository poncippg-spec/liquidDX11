#define _CRT_SECURE_NO_WARNINGS
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "../../../data/fonts.h"
#include "glass/backdrop.h"
#include "glass/glass.h"
#include "glass/surfaces.h"
#include <d3d11.h>
#include <tchar.h>
#include <dwmapi.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <cmath>
#include <objbase.h>
#include <wincodec.h>
#include <mmsystem.h>
#include <string>
#include <vector>
#pragma comment(lib, "dwmapi.lib")
#pragma comment(lib, "windowscodecs.lib")
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "winmm.lib")
static ID3D11Device*           g_pd3dDevice          = nullptr;
static ID3D11DeviceContext*    g_pd3dDeviceContext   = nullptr;
static IDXGISwapChain*         g_pSwapChain          = nullptr;
static bool                    g_SwapChainOccluded   = false;
static UINT                    g_ResizeWidth = 0, g_ResizeHeight = 0;
static ID3D11RenderTargetView* g_mainRenderTargetView = nullptr;
static ID3D11Texture2D*          g_ssaaTex = nullptr;
static ID3D11RenderTargetView*   g_ssaaRTV = nullptr;
static ID3D11ShaderResourceView* g_ssaaSRV = nullptr;
static int                       g_ssaaW = 0, g_ssaaH = 0;
static float                     g_ssaaScale = 1.0f;
static ID3D11VertexShader*       g_resolveVS = nullptr;
static ID3D11PixelShader*        g_resolvePS = nullptr;
static ID3D11SamplerState*       g_resolveSmp = nullptr;
static ID3D11BlendState*         g_resolveBlend = nullptr;
static Glass::Backdrop         g_backdrop;
static Glass::Renderer         g_glass_renderer;
static bool                    g_backdrop_uiblur = true;
static ImFont*                 g_fontSemi = nullptr;
static HANDLE                  g_rec_pipe = nullptr;
static HANDLE                  g_rec_proc = nullptr;
static bool                    g_recording = false;
static LONGLONG                g_rec_time  = 0;
static double                  g_rec_acc   = 0.0;
static int                     g_rec_w = 0, g_rec_h = 0;
static unsigned char*          g_rec_buf = nullptr;
static ID3D11Texture2D*        g_rec_stg = nullptr;
static int                     g_rec_stg_w = 0, g_rec_stg_h = 0;
static const char*             g_ffmpeg_exe = "ffmpeg.exe";
static char                    g_rec_path[260] = "glass_recording.mp4";
static bool                    g_recAudio = true;
static char                    g_rec_tmp[260] = "";
static std::vector<short>      g_recAudioBuf;
static LARGE_INTEGER           g_recAudioStart = {};
static LARGE_INTEGER           g_recAudioQpf = {};
bool  CreateDeviceD3D(HWND);
void  CleanupDeviceD3D();
void  CreateRenderTarget();
void  CleanupRenderTarget();
static bool RecorderStart(int W, int H);
static void RecorderFrame(float dt);
static void RecorderStop();
static void RecorderTapSfx(const unsigned char*);
static void InitResolve();
static void EnsureSSAA(int w, int h);
static void ResolveSSAA();
static void CleanupSSAA();
LRESULT WINAPI WndProc(HWND, UINT, WPARAM, LPARAM);
static inline float Glass_Clampf(float x, float a, float b){ return x < a ? a : (x > b ? b : x); }
#include "app/megasettings.inc"
static std::string ExeDir() {
    char buf[MAX_PATH] = {};
    DWORD n = ::GetModuleFileNameA(nullptr, buf, MAX_PATH);
    std::string p(buf, (n && n < MAX_PATH) ? n : 0u);
    size_t s = p.find_last_of("\\/");
    return (s == std::string::npos) ? std::string(".") : p.substr(0, s);
}
static bool FileExists(const std::string& p) {
    DWORD a = ::GetFileAttributesA(p.c_str());
    return a != INVALID_FILE_ATTRIBUTES && !(a & FILE_ATTRIBUTE_DIRECTORY);
}
static std::string AssetPath(const char* rel, const char* abs_fallback) {
    std::string up = ExeDir();
    for (int d = 0; d <= 6; ++d) {
        std::string cand = up + "\\" + rel;
        if (FileExists(cand)) return cand;
        up += "\\..";
    }
    char env[MAX_PATH] = {};
    DWORD en = ::GetEnvironmentVariableA("GLASS_ASSET_DIR", env, MAX_PATH);
    if (en > 0 && en < MAX_PATH) {
        std::string cand = std::string(env) + "\\" + rel;
        if (FileExists(cand)) return cand;
    }
    return abs_fallback ? std::string(abs_fallback) : std::string(rel);
}
#include "app/config.inc"
static ID3D11ShaderResourceView* LoadWICTexture(ID3D11Device* dev, const wchar_t* path){
    IWICImagingFactory* fac=nullptr;
    if (FAILED(CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&fac)))) return nullptr;
    ID3D11ShaderResourceView* srv=nullptr; IWICBitmapDecoder* dec=nullptr;
    if (SUCCEEDED(fac->CreateDecoderFromFilename(path, nullptr, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &dec))) {
        IWICBitmapFrameDecode* frame=nullptr;
        if (SUCCEEDED(dec->GetFrame(0, &frame))) {
            UINT ow=0, oh=0; frame->GetSize(&ow,&oh);
            UINT tw=ow, th=oh; const UINT cap=512;
            if (ow>cap || oh>cap){ float s=(float)cap/(float)(ow>oh?ow:oh); tw=(UINT)(ow*s); th=(UINT)(oh*s); if(tw<1)tw=1; if(th<1)th=1; }
            IWICBitmapSource* srcImg=frame; IWICBitmapScaler* scaler=nullptr;
            if ((tw!=ow || th!=oh) && SUCCEEDED(fac->CreateBitmapScaler(&scaler)))
                if (SUCCEEDED(scaler->Initialize(frame, tw, th, WICBitmapInterpolationModeFant))) srcImg=scaler;
            IWICFormatConverter* conv=nullptr;
            if (SUCCEEDED(fac->CreateFormatConverter(&conv))) {
                if (SUCCEEDED(conv->Initialize(srcImg, GUID_WICPixelFormat32bppRGBA, WICBitmapDitherTypeNone, nullptr, 0.0, WICBitmapPaletteTypeCustom))) {
                    UINT w=tw, h=th; conv->GetSize(&w,&h);
                    if (w>0 && h>0) {
                        unsigned char* px = new unsigned char[(size_t)w*h*4];
                        if (SUCCEEDED(conv->CopyPixels(nullptr, w*4, w*h*4, px))) {
                            D3D11_TEXTURE2D_DESC td={}; td.Width=w; td.Height=h; td.MipLevels=1; td.ArraySize=1;
                            td.Format=DXGI_FORMAT_R8G8B8A8_UNORM; td.SampleDesc.Count=1; td.Usage=D3D11_USAGE_IMMUTABLE; td.BindFlags=D3D11_BIND_SHADER_RESOURCE;
                            D3D11_SUBRESOURCE_DATA sd={}; sd.pSysMem=px; sd.SysMemPitch=w*4;
                            ID3D11Texture2D* tex=nullptr;
                            if (SUCCEEDED(dev->CreateTexture2D(&td,&sd,&tex)) && tex) { dev->CreateShaderResourceView(tex,nullptr,&srv); tex->Release(); }
                        }
                        delete[] px;
                    }
                }
                if (conv) conv->Release();
            }
            if (scaler) scaler->Release();
            frame->Release();
        }
        dec->Release();
    }
    fac->Release();
    return srv;
}
static void LoadPhotos(ID3D11Device* dev){
    static ID3D11ShaderResourceView* srvs[32]; int n=0;
    wchar_t up[MAX_PATH] = {}; DWORD ul = ::GetEnvironmentVariableW(L"USERPROFILE", up, MAX_PATH);
    wchar_t dir[MAX_PATH];
    if (!(ul > 0 && ul < MAX_PATH)) return;
    wsprintfW(dir, L"%s\\Downloads\\photo", up);
    wchar_t pat[MAX_PATH]; wsprintfW(pat, L"%s\\*.jpg", dir);
    WIN32_FIND_DATAW fd; HANDLE hf = FindFirstFileW(pat, &fd);
    if (hf != INVALID_HANDLE_VALUE) {
        do {
            wchar_t path[MAX_PATH]; wsprintfW(path, L"%s\\%s", dir, fd.cFileName);
            ID3D11ShaderResourceView* s = LoadWICTexture(dev, path);
            if (s && n<32) srvs[n++]=s;
        } while (n<32 && FindNextFileW(hf,&fd));
        FindClose(hf);
    }
    if (n>0) Glass::SetPhotoTextures((void* const*)srvs, n);
}
int main(int, char**)
{
    HANDLE hOneInstance = ::CreateMutexW(nullptr, TRUE, L"Local\\GlassOverlaySingleInstance");
    if (hOneInstance && ::GetLastError() == ERROR_ALREADY_EXISTS) {
        if (HWND prev = ::FindWindowW(L"GlassOverlay", nullptr)) ::SetForegroundWindow(prev);
        return 0;
    }
    ImGui_ImplWin32_EnableDpiAwareness();
    float scale = ImGui_ImplWin32_GetDpiScaleForMonitor(::MonitorFromPoint(POINT{0,0}, MONITOR_DEFAULTTOPRIMARY));
    const float baseScale = scale;
    LoadOrCreateConfig();
    WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"GlassOverlay", nullptr };
    ::RegisterClassExW(&wc);
    const int win_w = (int)(g_cfg.win_w * scale);
    const int win_h = (int)(g_cfg.win_h * scale);
    int sx = ::GetSystemMetrics(SM_CXSCREEN), sy = ::GetSystemMetrics(SM_CYSCREEN);
    int px = (g_cfg.win_x >= 0) ? g_cfg.win_x : (sx - win_w) / 2;
    int py = (g_cfg.win_y >= 0) ? g_cfg.win_y : (sy - win_h) / 2;
    HWND hwnd = ::CreateWindowExW(WS_EX_LAYERED, wc.lpszClassName, L"Glass Overlay",
                                  WS_POPUP, px, py, win_w, win_h,
                                  nullptr, nullptr, wc.hInstance, nullptr);
    ::SetLayeredWindowAttributes(hwnd, RGB(0,0,0), 255, LWA_ALPHA);
    MARGINS margins = { -1 };
    ::DwmExtendFrameIntoClientArea(hwnd, &margins);
    if (!CreateDeviceD3D(hwnd)) { CleanupDeviceD3D(); ::UnregisterClassW(wc.lpszClassName, wc.hInstance); return 1; }
    InitResolve();
    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);
    ::SetForegroundWindow(hwnd);
    ::SetActiveWindow(hwnd);
    ::SetFocus(hwnd);
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.IniFilename = nullptr;
    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 0.0f;
    ImVec4* gcol = style.Colors;
    auto frost = [](float a){ return ImVec4(1.0f, 1.0f, 1.0f, a); };
    gcol[ImGuiCol_Text]               = ImVec4(0.925f, 0.929f, 0.953f, 1.00f);
    gcol[ImGuiCol_TextDisabled]       = ImVec4(0.592f, 0.604f, 0.659f, 1.00f);
    gcol[ImGuiCol_NavCursor]          = ImVec4(0, 0, 0, 0);
    gcol[ImGuiCol_Border]             = frost(0.07f);
    gcol[ImGuiCol_Separator]          = frost(0.10f);
    gcol[ImGuiCol_Button]             = frost(0.06f);
    gcol[ImGuiCol_ButtonHovered]      = frost(0.12f);
    gcol[ImGuiCol_ButtonActive]       = frost(0.18f);
    gcol[ImGuiCol_FrameBg]            = frost(0.06f);
    gcol[ImGuiCol_FrameBgHovered]     = frost(0.10f);
    gcol[ImGuiCol_FrameBgActive]      = frost(0.14f);
    gcol[ImGuiCol_Header]             = frost(0.10f);
    gcol[ImGuiCol_HeaderHovered]      = frost(0.14f);
    gcol[ImGuiCol_HeaderActive]       = frost(0.18f);
    gcol[ImGuiCol_CheckMark]          = ImVec4(1, 1, 1, 0.95f);
    gcol[ImGuiCol_SliderGrab]         = frost(0.55f);
    gcol[ImGuiCol_SliderGrabActive]   = frost(0.75f);
    gcol[ImGuiCol_PopupBg]            = ImVec4(0.10f, 0.105f, 0.125f, 0.94f);
    gcol[ImGuiCol_ScrollbarBg]        = ImVec4(0, 0, 0, 0);
    gcol[ImGuiCol_ScrollbarGrab]      = frost(0.16f);
    gcol[ImGuiCol_ScrollbarGrabHovered] = frost(0.24f);
    gcol[ImGuiCol_ScrollbarGrabActive]  = frost(0.30f);
    style.FrameRounding     = 10.0f;
    style.GrabRounding      = 10.0f;
    style.PopupRounding     = 13.0f;
    style.ChildRounding     = 13.0f;
    style.ScrollbarRounding = 12.0f;
    style.FrameBorderSize   = 0.0f;
    style.WindowBorderSize  = 0.0f;
    style.PopupBorderSize   = 0.0f;
    style.ScrollbarSize     = 10.0f;
    style.AntiAliasedLines        = true;
    style.AntiAliasedLinesUseTex  = true;
    style.AntiAliasedFill         = true;
    style.CircleTessellationMaxError = 0.10f;
    style.CurveTessellationTol       = 0.80f;
    style.ScaleAllSizes(scale);
    style.FontScaleDpi = scale;
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);
    ImFontConfig font_cfg; font_cfg.FontDataOwnedByAtlas = false;
    ImFont* font_medium   = io.Fonts->AddFontFromMemoryTTF(inter_medium.data(),   (int)inter_medium.size(),   17.0f, &font_cfg);
    ImFont* font_semibold = io.Fonts->AddFontFromMemoryTTF(inter_semibold.data(), (int)inter_semibold.size(), 26.0f, &font_cfg);
    IM_ASSERT(font_medium && font_semibold);
    io.FontDefault = font_medium;
    g_fontSemi = font_semibold;
    std::string fa_solid_path  = AssetPath("data\\fa\\fa-solid-900.ttf",  nullptr);
    ImFont* fa_solid = io.Fonts->AddFontFromFileTTF(fa_solid_path.c_str(), 32.0f);
    if (fa_solid) Glass::SetIconFont(fa_solid);
    std::string fa_brands_path = AssetPath("data\\fa\\fa-brands-400.ttf", nullptr);
    ImFont* fa_brands = io.Fonts->AddFontFromFileTTF(fa_brands_path.c_str(), 32.0f);
    if (fa_brands) Glass::SetIconFontBrands(fa_brands);
    std::string hello_path = AssetPath("data\\Sacramento-Regular.ttf", nullptr);
    ImFont* hello_font = io.Fonts->AddFontFromFileTTF(hello_path.c_str(), 150.0f);
    if (!g_glass_renderer.Init(g_pd3dDevice, g_pd3dDeviceContext)) {
        ::MessageBoxA(hwnd, "Glass renderer init failed (shader compile?)", "Error", MB_OK | MB_ICONERROR);
        return 1;
    }
    Glass::g = &g_glass_renderer;
    Glass::g_sfxTap = &RecorderTapSfx;
    g_backdrop.Init(g_pd3dDevice, g_pd3dDeviceContext, hwnd);
    ::CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    LoadPhotos(g_pd3dDevice);
    { char up[MAX_PATH] = {}; DWORD ul = ::GetEnvironmentVariableA("USERPROFILE", up, MAX_PATH);
      if (ul > 0 && ul < MAX_PATH) std::snprintf(g_rec_path, sizeof(g_rec_path), "%s\\glass_recording.mp4", up); }
#include "app/ui_state.inc"
#include "app/config_apply.inc"
    ApplyCfg(g_cfg);
    page = g_cfg.start_page;
    LARGE_INTEGER s_qpf = {}; ::QueryPerformanceFrequency(&s_qpf);
    LARGE_INTEGER lastBlurTick = {}, lastFrameTick = {};
    bool done = false;
    while (!done)
    {
        MSG msg;
        while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE)) {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT) done = true;
        }
        if (done) break;
        if (g_SwapChainOccluded && g_pSwapChain->Present(0, DXGI_PRESENT_TEST) == DXGI_STATUS_OCCLUDED) {
            ::Sleep(10); continue;
        }
        g_SwapChainOccluded = false;
        if (g_ResizeWidth != 0 && g_ResizeHeight != 0) {
            CleanupRenderTarget();
            g_pSwapChain->ResizeBuffers(0, g_ResizeWidth, g_ResizeHeight, DXGI_FORMAT_UNKNOWN, 0);
            g_ResizeWidth = g_ResizeHeight = 0;
            CreateRenderTarget();
        }
        LARGE_INTEGER nowTick; ::QueryPerformanceCounter(&nowTick);
        double sinceBlur = lastBlurTick.QuadPart ? (double)(nowTick.QuadPart - lastBlurTick.QuadPart) / (double)s_qpf.QuadPart : 1.0;
        double blurMinDt = 1.0 / (blurFps >= 1.0f ? (double)blurFps : 1.0);
        bool doBlur = (sinceBlur >= blurMinDt);
        if (doBlur) { lastBlurTick = nowTick; g_backdrop.Capture(); }
        RECT cr; ::GetClientRect(hwnd, &cr);
        RECT wr; ::GetWindowRect(hwnd, &wr);
        const int cw = cr.right - cr.left, ch = cr.bottom - cr.top;
        const int origin_x = wr.left - g_backdrop.originX();
        const int origin_y = wr.top  - g_backdrop.originY();
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
        if (io.DeltaTime > 0.05f) io.DeltaTime = 1.0f / 30.0f;
        ImVec2 cursor_local = ImVec2(io.MousePos.x - (float)wr.left, io.MousePos.y - (float)wr.top);
        g_glass_renderer.BeginFrame(cw, ch, origin_x, origin_y,
                                    g_backdrop.width(), g_backdrop.height(), cursor_local);
        if (ImGui::IsKeyPressed(ImGuiKey_F11, false)) {
            screenshotMode = !screenshotMode;
            ::SetWindowDisplayAffinity(hwnd, screenshotMode ? 0u  : 0x11u );
        }
        if (logged_in && io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_K, false)) Glass::CommandPaletteToggle();
        if (logged_in && io.KeyCtrl) {
            if (ImGui::IsKeyPressed(ImGuiKey_Comma, false)) page = 4;
            if (ImGui::IsKeyPressed(ImGuiKey_L,     false)) appearance ^= 1;
            if (ImGui::IsKeyPressed(ImGuiKey_1, false)) page = 0;
            if (ImGui::IsKeyPressed(ImGuiKey_2, false)) page = 1;
            if (ImGui::IsKeyPressed(ImGuiKey_3, false)) page = 2;
            if (ImGui::IsKeyPressed(ImGuiKey_4, false)) page = 3;
            if (ImGui::IsKeyPressed(ImGuiKey_5, false)) page = 4;
            if (ImGui::IsKeyPressed(ImGuiKey_0, false)) page = 7;
        }
        static float idleT = 0.0f;
        if (io.MouseDelta.x != 0.0f || io.MouseDelta.y != 0.0f || io.MouseWheel != 0.0f || ImGui::IsAnyMouseDown()) idleT = 0.0f; else idleT += io.DeltaTime;
        bool hideChrome = fxAutoHide && idleT > 3.0f;
        if (logged_in && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
            static const char* cm[] = { "Search\xE2\x80\xA6", "Account Settings", "Refresh", "Sign Out" };
            Glass::ContextMenuOpen(io.MousePos.x, io.MousePos.y, cm, 4, 3);
        }
        if (ImGui::IsKeyPressed(ImGuiKey_F9, false)) {
            if (g_recording) RecorderStop();
            else { screenshotMode = false; ::SetWindowDisplayAffinity(hwnd, 0x11u); RecorderStart(cw, ch); }
        }
        if (screenshotMode || g_recording) {
            g_glass_renderer.SetSubmitFade(1.0f);
            Glass::Primitive bgfill{};
            bgfill.cx = cw * 0.5f; bgfill.cy = ch * 0.5f; bgfill.hw = cw * 0.5f; bgfill.hh = ch * 0.5f;
            bgfill.corner_radius = 0.0f; bgfill.fade = 1.0f; bgfill.material = Glass::Material::Clear;
            g_glass_renderer.Submit(bgfill);
        }
        static int prevL = wr.left, prevT = wr.top;
        static float tiltx = 0.0f, tilty = 0.0f;
        float vx = (float)(wr.left - prevL), vy = (float)(wr.top - prevT);
        prevL = wr.left; prevT = wr.top;
        float mvx = -vx * 0.05f; mvx = mvx < -1.2f ? -1.2f : (mvx > 1.2f ? 1.2f : mvx);
        float mvy = -vy * 0.05f; mvy = mvy < -1.2f ? -1.2f : (mvy > 1.2f ? 1.2f : mvy);
        float tgtx = mvx;
        float tgty = mvy;
        float aa = io.DeltaTime * 9.0f; if (aa > 1.0f) aa = 1.0f;
        tiltx += (tgtx - tiltx) * aa; tilty += (tgty - tilty) * aa;
        g_glass_renderer.SetTilt(fxParallax ? tiltx : 0.0f, fxParallax ? tilty : 0.0f);
        float ent = g_glass_renderer.Entrance();
        ent = (ent >= 0.0f) ? (ent > 1.0f ? 1.0f : ent) : 0.0f;
        style.Alpha = ent;
        float rise = (1.0f - ent) * 18.0f;
        ImGui::SetNextWindowPos(ImVec2(cw * 0.5f, ch * 0.5f + rise), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
        Glass::SetAppearance(appearance);
        if (blackText) Glass::SetInk(IM_COL32(18,19,24,255), IM_COL32(95,98,110,255));
        io.FontDefault = boldText ? font_semibold : font_medium;
        (void)baseScale; (void)uiScale;
        io.FontGlobalScale = fontScale;
        { float ds = uiDensity==0 ? 0.72f : (uiDensity==2 ? 1.28f : 1.0f); Glass::SetDensity(ds); }
        g_ssaaScale = ssaaOn ? ssaaScale : 1.0f;
        Glass::Set24Hour(time24);
        { static int lastTop = -1; if ((int)alwaysTop != lastTop) {
            ::SetWindowPos(hwnd, alwaysTop ? HWND_TOPMOST : HWND_NOTOPMOST, 0,0,0,0, SWP_NOMOVE|SWP_NOSIZE);
            lastTop = (int)alwaysTop; } }
        { static float accSm[3] = { accent[0], accent[1], accent[2] };
          float ka = 1.0f - std::exp(-io.DeltaTime * 12.0f);
          accSm[0] += (accent[0]-accSm[0])*ka; accSm[1] += (accent[1]-accSm[1])*ka; accSm[2] += (accent[2]-accSm[2])*ka;
          Glass::SetAccent(accSm[0], accSm[1], accSm[2]); }
        float eBlur=gBlur, eOp=gOpacity, eSat=gSat, eRefr=gRefr, eChroma=gChroma, eEdge=gEdge, eShadow=gShadow;
        if (reduceTransp) { eOp = eOp*0.45f + 0.55f; eBlur = eBlur*0.4f + 0.6f; eRefr *= 0.5f; eChroma *= 0.3f; }
        if (hiContrast)   { eEdge += 0.45f; eShadow += 0.25f; eSat += 0.4f; }
        if (!vibrantUI)   { eSat *= 0.6f; }
        Glass::SetGlobalMaterial(eBlur, eOp, eSat, eRefr, eChroma, eEdge, eShadow);
        Glass::SetLiquidFlow(reduceMotion ? 0.0f : gFlow);
        g_backdrop.SetUIBlurSpread(uiBlurAmt);
        { Glass::GlassEdgeConfig ec;
          ec.fresnel_exp  = eFresExp; ec.bevel_scale = eBevel;   ec.lip       = eLip;     ec.specular_exp = eSpecExp;
          ec.specular_amt = eSpecAmt; ec.front_amt   = eFront;   ec.back_amt  = eBack;    ec.sat_pop      = eSatPop;
          ec.bevel_tilt = eBevelTilt; ec.light_height = eLightH; ec.edge_shadow = eEdgeSh; ec.cursor_size = eCurSize;
          ec.cursor_glow = eCurGlow;  ec.sheen_amt = eSheen;     ec.grain_amt = eGrain;    ec.ambient_rim = eAmbRim;
          ec.front_spread = eFrontSpread * 0.01f; ec.back_spread = eBackSpread * 0.01f;
          ec.panel_br_spread = ePanelBRSpread * 0.01f; ec.panel_br_smooth = ePanelBRSmooth * 0.01f;
          ec.panel_rim_angle = ePanelRimAngle;
          ec.tbl_fill = eTBLRemove ? eTBLFill : 0.0f;
          ec.adapt_strength = eAdaptOn ? eAdaptStrength : 0.0f;
          ec.adapt_pivot = eAdaptPivot; ec.adapt_soft = eAdaptSoft;
          ec.smooth_refraction = eSmoothRefr ? 1.0f : 0.0f;
          ec.squircle_power = eSquircle ? eSquirclePow : 2.0f;
          ec.lens_amount = eLensAmt; ec.lens_power = eLensPow;
          ec.lens_a = eLensA; ec.lens_b = eLensB; ec.lens_c = eLensC; ec.lens_d = eLensD;
          ec.glow_weight = eGlowW; ec.glow_phase = eGlowPhase; ec.glow_edge0 = eGlowE0;
          ec.glow_edge1 = eGlowE1; ec.glow_bias = eGlowBias; ec.glow_anim = eGlowAnim;
          g_glass_renderer.SetEdgeConfig(ec); }
        g_glass_renderer.SetLights(eLightAngle, eLight1Amt, eLight2Angle, eLight2Amt);
        prog += io.DeltaTime * 0.16f; if (prog > 1.0f) prog -= 1.0f;
        static int dispPage = 0; static float pageT = 1.0f;
        static int dispScreen = 0; static float authT = 1.0f;
        int screen = logged_in ? 1 : 0;
        static float helloT = -1.0f;
        if (screen != dispScreen) { dispScreen = screen; authT = 0.2f; if (screen == 1) helloT = 0.0f; }
        authT += io.DeltaTime * (reduceMotion ? 30.0f : 7.0f * animSpeed); if (authT > 1.0f) authT = 1.0f;
        float authA = authT * authT * (3.0f - 2.0f * authT);
        const float HELLO_DUR = 3.4f, HELLO_FI = 0.5f, HELLO_FO = 0.8f;
        float helloSplash = 0.0f;
        if (helloT >= 0.0f) {
            helloT += io.DeltaTime;
            if (helloT >= HELLO_DUR) helloT = -1.0f;
        }
        if (helloT >= 0.0f) {
            helloSplash = (helloT < HELLO_FI) ? (helloT / HELLO_FI)
                        : (helloT > HELLO_DUR - HELLO_FO ? (HELLO_DUR - helloT) / HELLO_FO : 1.0f);
            helloSplash = helloSplash < 0 ? 0 : (helloSplash > 1 ? 1 : helloSplash);
            float dashReveal = (helloT - (HELLO_DUR - HELLO_FO)) / HELLO_FO;
            dashReveal = dashReveal < 0 ? 0 : (dashReveal > 1 ? 1 : dashReveal);
            dashReveal = dashReveal * dashReveal * (3.0f - 2.0f * dashReveal);
            authA *= dashReveal;
        }
        if (page != dispPage) { dispPage = page; pageT = 0.25f; scrollVel = 0.0f; scrollResetReq = true; }
        pageT += io.DeltaTime * (reduceMotion ? 34.0f : 8.5f * animSpeed); if (pageT > 1.0f) pageT = 1.0f;
        float pageA = pageT * pageT * (3.0f - 2.0f * pageT);
        auto center_text = [&](const char* t, bool disabled) {
            float tw = ImGui::CalcTextSize(t).x;
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (ImGui::GetContentRegionAvail().x - tw) * 0.5f);
            if (disabled) ImGui::TextDisabled("%s", t); else ImGui::TextUnformatted(t);
        };
        if (dispScreen == 0) {
            if (Glass::BeginCard("login", Glass::Material::Regular, ImVec2(404.0f * scale, 0))) {
                g_glass_renderer.SetSubmitFade(authA);
                style.Alpha = ent * authA; if (style.Alpha > 1.0f) style.Alpha = 1.0f;
                float cwid = ImGui::GetContentRegionAvail().x;
                {
                    ImVec2 c = ImGui::GetCursorScreenPos();
                    float bs = 74.0f;
                    Glass::Primitive badge; badge.cx = c.x + cwid * 0.5f; badge.cy = c.y + bs * 0.5f;
                    badge.hw = bs * 0.5f; badge.hh = bs * 0.5f; badge.corner_radius = 21.0f; badge.fade = 1.0f;
                    badge.material = Glass::Material::Accent;
                    g_glass_renderer.Submit(badge);
                    Glass::DrawIcon(ImGui::GetWindowDrawList(), Glass::Icon::Spark, ImVec2(badge.cx, badge.cy - bs*0.02f), bs * 0.58f, IM_COL32(255,255,255,255), 3.2f);
                    ImGui::Dummy(ImVec2(cwid, bs + 16));
                }
                ImGui::PushFont(font_semibold);
                center_text("Glass", false);
                ImGui::PopFont();
                center_text("Sign in to continue", true);
                ImGui::Dummy(ImVec2(0, 22));
                {
                    float gw = ImGui::GetContentRegionAvail().x;
                    const float fh = 52.0f;
                    ImVec2 gc = ImGui::GetCursorScreenPos();
                    Glass::Primitive box; box.cx = gc.x + gw*0.5f; box.cy = gc.y + fh;
                    box.hw = gw*0.5f; box.hh = fh; box.corner_radius = 18.0f; box.fade = 1.0f; box.material = Glass::Material::Thin;
                    g_glass_renderer.Submit(box);
                    static int hrTarget = -1, hrLastRow = 0; static float hrVis = 0.0f;
                    {
                        float dt = io.DeltaTime;
                        if (hrTarget >= 0) hrLastRow = hrTarget;
                        Glass::Spring& head = Glass::g->springs().Get(0x106F1Eu, 0, Glass::SpringStyle::Bouncy, (float)hrLastRow);
                        Glass::Spring& tail = Glass::g->springs().Get(0x106F1Eu, 1, Glass::SpringStyle::Overdamped, (float)hrLastRow);
                        head.target = (float)hrLastRow; head.Tick(dt);
                        tail.target = head.x;           tail.Tick(dt);
                        hrVis += ((hrTarget >= 0 ? 1.0f : 0.0f) - hrVis) * Glass_Clampf(dt*11.0f, 0.0f, 1.0f);
                        if (hrVis > 0.01f) {
                            float spd = Glass_Clampf(std::fabs(head.v) * 0.5f, 0.0f, 1.0f);
                            float wob = 0.12f * spd * sinf((float)ImGui::GetTime() * 7.0f);
                            float cyH = gc.y + (head.x + 0.5f)*fh, cyT = gc.y + (tail.x + 0.5f)*fh;
                            float hhw = (gw*0.5f - 5.0f) * (1.0f + wob), hhh = (fh*0.5f - 4.0f) * (1.0f - wob*0.5f), fade = hrVis * 0.75f;
                            if (std::fabs(cyH - cyT) > 1.0f)
                                Glass::SubmitBlob(gc.x+gw*0.5f, cyH, hhw, hhh, gc.x+gw*0.5f, cyT, hhw*0.9f, hhh*0.9f, 13.0f, 24.0f, fade, Glass::Material::Regular);
                            else { Glass::Primitive hp; hp.cx=gc.x+gw*0.5f; hp.cy=cyH; hp.hw=hhw; hp.hh=hhh; hp.corner_radius=13.0f; hp.fade=fade; hp.material=Glass::Material::Regular; g_glass_renderer.Submit(hp); }
                        }
                    }
                    ImDrawList* gdl = ImGui::GetWindowDrawList();
                    gdl->AddLine(ImVec2(gc.x + 16.0f, gc.y + 1.0f), ImVec2(gc.x + gw - 16.0f, gc.y + 1.0f), IM_COL32(255,255,255,20), 1.0f);
                    ImGui::SetCursorScreenPos(gc);
                    Glass::FieldRaw("user", username, (int)sizeof(username), "Glass ID", Glass::Icon::Person, false, fh);
                    bool userFoc = Glass::FieldWasFocused();
                    gdl->AddLine(ImVec2(gc.x + 44.0f, gc.y + fh), ImVec2(gc.x + gw, gc.y + fh), IM_COL32(255,255,255,24), 1.0f);
                    ImGui::SetCursorScreenPos(ImVec2(gc.x, gc.y + fh));
                    Glass::FieldRaw("pass", password, (int)sizeof(password), "Password", Glass::Icon::Lock, true, fh);
                    bool passFoc = Glass::FieldWasFocused();
                    hrTarget = userFoc ? 0 : (passFoc ? 1 : -1);
                    ImGui::SetCursorScreenPos(ImVec2(gc.x, gc.y + fh*2.0f));
                    ImGui::Dummy(ImVec2(gw, 0));
                }
                ImGui::Dummy(ImVec2(0, 22));
                if (Glass::Button("Sign In", true, ImVec2(ImGui::GetContentRegionAvail().x, 48)))
                    { logged_in = true; Glass::Notify(Glass::Icon::CheckCircle, IM_COL32(10,132,255,255), "Glass", "Welcome back", "You're signed in."); }
                ImGui::Dummy(ImVec2(0, 10));
                {
                    const char* label = "Sign in with Glass";
                    float av = ImGui::GetContentRegionAvail().x, bh = 48.0f;
                    ImVec2 bp = ImGui::GetCursorScreenPos();
                    ImGui::InvisibleButton("siwa", ImVec2(av, bh));
                    bool hov = ImGui::IsItemHovered(), act = ImGui::IsItemActive();
                    Glass::Spring& sc = Glass::g->springs().Get((uint32_t)ImGui::GetID("siwa"), 1, Glass::SpringStyle::Bouncy, 1.0f);
                    sc.target = act ? 0.96f : (hov ? 1.02f : 1.0f); sc.Tick(io.DeltaTime);
                    float gel = 1.0f - sc.x;
                    Glass::Primitive p; p.cx = bp.x + av * 0.5f; p.cy = bp.y + bh * 0.5f;
                    p.hw = av * 0.5f * sc.x * (1.0f + gel * 0.7f); p.hh = bh * 0.5f * sc.x * (1.0f - gel * 0.5f);
                    p.corner_radius = 13.0f; p.fade = 1.0f; p.elevate = hov ? 1.3f : 1.0f; p.material = Glass::Material::Thick;
                    g_glass_renderer.Submit(p);
                    ImDrawList* d = ImGui::GetWindowDrawList();
                    ImVec2 ts = ImGui::CalcTextSize(label);
                    float isz = 19.0f, gp = 9.0f, tot = isz + gp + ts.x, ix = bp.x + (av - tot) * 0.5f;
                    Glass::DrawIcon(d, Glass::Icon::Spark, ImVec2(ix + isz * 0.5f, bp.y + bh * 0.5f), isz, IM_COL32(255,255,255,255), 2.5f);
                    d->AddText(ImVec2(ix + isz + gp, bp.y + (bh - ts.y) * 0.5f), IM_COL32(255,255,255,255), label);
                    if (ImGui::IsItemDeactivated() && hov)
                        { logged_in = true; Glass::Notify(Glass::Icon::Spark, IM_COL32(255,255,255,255), "Glass", "Signed in with Glass", "Welcome back."); }
                }
                ImGui::Dummy(ImVec2(0, 18));
                {
                    const char* gl = "Continue as guest";
                    float avw = ImGui::GetContentRegionAvail().x;
                    ImVec2 ts = ImGui::CalcTextSize(gl);
                    ImVec2 lp = ImGui::GetCursorScreenPos();
                    float lx = lp.x + (avw - ts.x) * 0.5f;
                    ImGui::SetCursorScreenPos(ImVec2(lx, lp.y));
                    ImGui::InvisibleButton("guest", ts);
                    bool gh = ImGui::IsItemHovered();
                    ImGui::GetWindowDrawList()->AddText(ImVec2(lx, lp.y), gh ? IM_COL32(120,180,255,255) : IM_COL32(150,153,168,255), gl);
                    if (ImGui::IsItemDeactivated() && gh)
                        { logged_in = true; Glass::Notify(Glass::Icon::Person, IM_COL32(142,142,147,255), "Glass", "Guest mode", "Browsing as guest."); }
                    ImGui::SetCursorScreenPos(ImVec2(lp.x, lp.y + ts.y + 2.0f));
                    ImGui::Dummy(ImVec2(avw, 0));
                }
                g_glass_renderer.SetSubmitFade(1.0f);
                style.Alpha = ent;
            }
            Glass::EndCard();
        } else {
        float dashW = (float)win_w - 92.0f;
        float dashH = (float)win_h - 92.0f;
        if (Glass::BeginCard("dash", Glass::Material::Regular, ImVec2(dashW, dashH))) {
            g_glass_renderer.SetSubmitFade(authA);
            style.Alpha = ent * authA; if (style.Alpha > 1.0f) style.Alpha = 1.0f;
            ImVec2 o = ImGui::GetCursorScreenPos();
            float innerW = ImGui::GetContentRegionAvail().x;
            float innerH = ImGui::GetContentRegionAvail().y;
            float sideW = sidebarW * scale;
            ImDrawList* wdl = ImGui::GetWindowDrawList();
            Glass::Icon navIcons[] = { Glass::Icon::Person, Glass::Icon::Globe, Glass::Icon::Eye, Glass::Icon::Box, Glass::Icon::Gear };
            const char* navLabels[] = { "General", "Display", "Battery", "Storage", "Customize" };
            ImU32 navTints[] = { IM_COL32(10,132,255,255), IM_COL32(52,199,89,255), IM_COL32(255,149,0,255), IM_COL32(255,69,58,255), IM_COL32(94,92,230,255) };
            const int navCount = 5;
            { float bs = 40.0f * uiScale;
              Glass::Primitive badge; badge.cx = o.x + bs*0.5f; badge.cy = o.y + bs*0.5f;
              badge.hw = bs*0.5f; badge.hh = bs*0.5f; badge.corner_radius = 12.0f*uiScale; badge.fade = 1.0f; badge.material = Glass::Material::Thin;
              g_glass_renderer.Submit(badge);
              Glass::DrawIcon(wdl, Glass::Icon::Spark, ImVec2(badge.cx, badge.cy - bs*0.02f), 40.0f*0.62f, IM_COL32(236,237,243,255), 2.8f);
              wdl->AddText(font_semibold, 22.0f*uiScale, ImVec2(o.x + bs + 12.0f*uiScale, o.y + 8.0f*uiScale), IM_COL32(236,237,243,255), "Glass"); }
            ImGui::SetCursorScreenPos(ImVec2(o.x, o.y + 64.0f*uiScale));
            int navCur = (page < navCount) ? page : -1;
            int navRes = Glass::SidebarNav("nav", navIcons, navLabels, navTints, navCount, navCur, sideW - 8.0f, 50.0f*uiScale);
            if (navRes >= 0) page = navRes;
            float sepX = o.x + sideW + 10.0f;
            wdl->AddLine(ImVec2(sepX, o.y + 4.0f),       ImVec2(sepX, o.y + innerH - 4.0f),       IM_COL32(0,0,0,28), 1.0f);
            wdl->AddLine(ImVec2(sepX + 1.0f, o.y + 4.0f), ImVec2(sepX + 1.0f, o.y + innerH - 4.0f), IM_COL32(255,255,255,40), 1.0f);
            float contentX = o.x + sideW + 30.0f;
            float contentW = innerW - sideW - 30.0f;
            bool wideSurface = (dispPage == 7 || dispPage >= 100);
            if (!wideSurface && contentW > 700.0f * scale) contentW = 700.0f * scale;
            float searchW = 240.0f * scale;
            ImGui::SetCursorScreenPos(ImVec2(contentX, o.y));
            Glass::SearchField(searchbuf, (int)sizeof(searchbuf), "Search", searchW);
            ImGui::SetCursorScreenPos(ImVec2(contentX + searchW + 12.0f, o.y));
            if (Glass::IconButton("hdr_refresh", Glass::Icon::Bolt, 40.0f)) page = 5;
            ImGui::SameLine(0, 8);
            if (Glass::IconButton("hdr_home", Glass::Icon::Gauge, 40.0f)) page = 0;
            ImGui::SameLine(0, 8);
            if (Glass::IconButton("hdr_gear", Glass::Icon::Gear, 40.0f)) page = 6;
            ImGui::SameLine(0, 8);
            if (Glass::IconButton("hdr_apps", Glass::Icon::Grid, 40.0f)) page = 7;
            float bodyY = o.y + 50.0f;
            const float dockReserve = 72.0f;
            float viewH = o.y + innerH - dockReserve - bodyY;
            ImGui::SetCursorScreenPos(ImVec2(contentX, bodyY + overscroll));
            ImGui::BeginChild("content", ImVec2(contentW, viewH), 0,
                              ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
            ImGui::PushClipRect(ImVec2(contentX - 16.0f, bodyY), ImVec2(contentX + contentW + 4.0f, o.y + innerH - dockReserve), true);
            g_glass_renderer.SetClipRect(contentX - 14.0f, bodyY, contentX + contentW, o.y + innerH - dockReserve);
            {
                const float dt = io.DeltaTime;
                if (scrollResetReq) { ImGui::SetScrollY(0.0f); scrollVel = 0.0f; overscroll = 0.0f; scrollResetReq = false; }
                float maxs = scrollMaxStored;
                bool  hoverScroll = ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows);
                if (hoverScroll && io.MouseWheel != 0.0f) {
                    float atTop = ImGui::GetScrollY() <= 0.5f, atBot = ImGui::GetScrollY() >= maxs - 0.5f;
                    if ((io.MouseWheel > 0 && atTop) || (io.MouseWheel < 0 && atBot))
                        overscroll += io.MouseWheel * 22.0f / (1.0f + fabsf(overscroll) * 0.06f);
                    else
                        scrollVel -= io.MouseWheel * 1500.0f;
                }
                scrollVel = Glass_Clampf(scrollVel, -9000.0f, 9000.0f);
                float syT = ImGui::GetScrollY() + scrollVel * dt;
                float fr = 1.0f - 6.0f * dt; if (fr < 0.0f) fr = 0.0f;
                scrollVel *= fr;
                if (scrollVel < 2.0f && scrollVel > -2.0f) scrollVel = 0.0f;
                if (syT < 0.0f)   { overscroll -= syT;          syT = 0.0f; scrollVel *= 0.30f; }
                if (syT > maxs)   { overscroll -= (syT - maxs); syT = maxs; scrollVel *= 0.30f; }
                ImGui::SetScrollY(syT);
                overscroll *= 1.0f - Glass_Clampf(13.0f * dt, 0.0f, 1.0f);
                if (overscroll < 0.4f && overscroll > -0.4f) overscroll = 0.0f;
                overscroll = Glass_Clampf(overscroll, -100.0f, 100.0f);
                bool moving = (scrollVel != 0.0f) || (overscroll != 0.0f) || (hoverScroll && io.MouseWheel != 0.0f);
                scrollIndA += ((moving ? 1.0f : 0.0f) - scrollIndA) * Glass_Clampf((moving ? 16.0f : 4.0f) * dt, 0.0f, 1.0f);
            }
            g_glass_renderer.SetSubmitFade(authA * pageA);
            style.Alpha = ent * authA * pageA; if (style.Alpha > 1.0f) style.Alpha = 1.0f;
            ImGui::Dummy(ImVec2(0, 4.0f + (1.0f - pageA) * 18.0f));
            #include "app/pages.inc"
            ImGui::Dummy(ImVec2(0, 72.0f));
            g_glass_renderer.SetSubmitFade(authA);
            style.Alpha = ent * authA; if (style.Alpha > 1.0f) style.Alpha = 1.0f;
            scrollMaxStored = ImGui::GetScrollMaxY();
            scrollYView     = ImGui::GetScrollY();
            ImGui::PopClipRect();
            ImGui::EndChild();
            g_glass_renderer.ClearClipRect();
            (void)scrollYView; (void)scrollIndA;
            Glass::DrawColorPickerModal();
            { static bool soAlert = false;
              if (signoutPending) { Glass::AlertOpen("Sign Out?", "You'll be returned to the login screen.", "Sign Out", "Cancel", true); signoutPending = false; soAlert = true; }
              int ar = Glass::DrawAlert();
              if (soAlert && ar != -1) { if (ar == 0) logged_in = false; soAlert = false; } }
            Glass::DrawActionSheet();
            {
              static const char* cmds[200]; static int ncmds = 0;
              if (ncmds == 0) {
                  const char* pages[8] = { "General","Display","Battery","Storage","Customize","View Profile","Account Settings","Apps" };
                  for (int i=0;i<8;i++) cmds[ncmds++] = pages[i];
                  int sc = Glass::SurfaceCount();
                  for (int i=0;i<sc && ncmds<200;++i) cmds[ncmds++] = Glass::SurfaceName(i);
              }
              int r = Glass::DrawCommandPalette(cmds, ncmds);
              if (r >= 0) page = (r < 8) ? r : (100 + (r - 8)); }
            { int r = Glass::DrawContextMenu();
              if (r == 0) Glass::CommandPaletteToggle();
              else if (r == 1) page = 6;
              else if (r == 3) { if (confirmSignout) signoutPending = true; else logged_in = false; } }
            {
                float pbH = 56.0f, pbX = o.x + 6.0f, pbW = sideW - 12.0f;
                float pbY = o.y + innerH - pbH - 6.0f;
                Glass::ProfileAction pa = Glass::ProfileMenu(
                    "acct", "poncipp", "@pondot", "PO",
                    IM_COL32(10,132,255,255), pbX, pbY, pbW, &profStatus, &profTheme, authA,
                    page == 5 || page == 6);
                switch (pa) {
                    case Glass::ProfileAction::ViewProfile:
                        page = 5; break;
                    case Glass::ProfileAction::AccountSettings:
                        page = 6; break;
                    case Glass::ProfileAction::GlassSettings:
                        page = navCount - 1; break;
                    case Glass::ProfileAction::SignOut:
                        if (confirmSignout) signoutPending = true; else logged_in = false; break;
                    case Glass::ProfileAction::StatusChanged: {
                        const char* sn[4] = { "Online", "Away", "Do Not Disturb", "Invisible" };
                        ImU32 sc2[4] = { IM_COL32(52,209,88,255), IM_COL32(255,159,10,255), IM_COL32(255,69,58,255), IM_COL32(142,142,147,255) };
                        Glass::Notify(Glass::Icon::Person, sc2[profStatus], "Status", sn[profStatus], "Status updated."); break; }
                    case Glass::ProfileAction::ThemeChanged: {
                        const char* tn[3] = { "Dark", "Light", "Auto" };
                        Glass::Notify(Glass::Icon::Display, IM_COL32(94,92,230,255), "Appearance", tn[profTheme], "Theme preference saved."); break; }
                    default: break;
                }
            }
            if (authA > 0.5f && !hideChrome) {
                float islandCx = o.x + sideW + contentW * 0.5f, islandTop = o.y - 32.0f;
                bool isHov = ImGui::IsMouseHoveringRect(ImVec2(islandCx - 90.0f, islandTop - 6.0f),
                                                        ImVec2(islandCx + 90.0f, islandTop + 40.0f));
                Glass::DynamicIsland(Glass::Icon::Bolt, "Glass", g_recording ? "REC" : "Ready",
                                     isHov || g_recording, islandCx, islandTop, contentW + sideW);
            }
            if (authA > 0.5f && !hideChrome) {
                float tlx = o.x + innerW - (3*13.0f + 2*8.0f) - 4.0f;
                int tl = Glass::TrafficLights("wtl", tlx, o.y + 8.0f);
                if (tl == 0) ::PostMessage(hwnd, WM_CLOSE, 0, 0);
                else if (tl == 1) ::ShowWindow(hwnd, SW_MINIMIZE);
            }
            if (authA > 0.5f && !hideChrome) {
                float dockCx = o.x + sideW + (innerW - sideW) * 0.5f;
                int d = Glass::SurfaceDock(dockCx, o.y + innerH - 6.0f, innerW - sideW - 20.0f,
                                           dispPage >= 100 ? dispPage - 100 : -1);
                if (d >= 0) page = 100 + d;
            }
            if (authA > 0.5f && fxCursor) {
                static float lcx = 0.0f, lcy = 0.0f;
                float k = io.DeltaTime * 52.0f; if (k > 1.0f) k = 1.0f;
                lcx += (io.MousePos.x - lcx) * k; lcy += (io.MousePos.y - lcy) * k;
                if (io.MousePos.x > o.x && io.MousePos.x < o.x + innerW &&
                    io.MousePos.y > o.y - 20.0f && io.MousePos.y < o.y + innerH)
                    Glass::SubmitBlob(io.MousePos.x, io.MousePos.y, 6.0f, 6.0f, lcx, lcy, 8.0f, 8.0f, 14.0f, 12.0f, 0.32f, Glass::Material::Knob);
            }
            Glass::DrawShareSheet();
            if (showOnboard) {
                static const char* ot[3] = { "Welcome", "Glass Material", "You're set" };
                static const char* ob[3] = { "A frosted-glass debug console, built on a real-time glass material.",
                                             "Specular, refraction, dispersion and metaball merges \xE2\x80\x94 all real-time.",
                                             "Press Get Started to dive in. Right-click anywhere for a menu." };
                static Glass::Icon oi[3] = { Glass::Icon::Spark, Glass::Icon::Bolt, Glass::Icon::Check };
                if (Glass::Onboarding("onb", ot, ob, oi, 3) >= 3) showOnboard = false;
            }
            g_glass_renderer.SetSubmitFade(1.0f);
            style.Alpha = ent;
        }
        Glass::EndCard();
        }
        static bool dragging = false;
        static POINT dragCur0; static int dragWin0X = 0, dragWin0Y = 0;
        bool overItem = ImGui::IsAnyItemHovered() || ImGui::IsAnyItemActive();
        bool anyPopup = ImGui::IsPopupOpen("", ImGuiPopupFlags_AnyPopupId | ImGuiPopupFlags_AnyPopupLevel);
        if (!dragging && io.MouseClicked[0] && !overItem && !anyPopup) {
            dragging = true; ::GetCursorPos(&dragCur0); dragWin0X = wr.left; dragWin0Y = wr.top;
        }
        if (dragging) {
            if (!io.MouseDown[0]) dragging = false;
            else {
                POINT p; ::GetCursorPos(&p);
                ::SetWindowPos(hwnd, nullptr, dragWin0X + (p.x - dragCur0.x), dragWin0Y + (p.y - dragCur0.y),
                               0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
            }
        }
        static float appClock = 0.0f; static bool welcomed = false;
        appClock += io.DeltaTime;
        if (!welcomed && appClock > 1.0f) {
            Glass::Notify(Glass::Icon::CheckCircle, IM_COL32(52,209,88,255), "Glass",
                          "Welcome", "Everything is up to date.");
            welcomed = true;
        }
        Glass::DrawNotifications((float)cw, (float)ch);
        Glass::DrawBanner((float)cw);
        if (helloT >= 0.0f && helloSplash > 0.001f) {
            ImDrawList* fdl = ImGui::GetForegroundDrawList();
            const float mgn = 46.0f;
            fdl->AddRectFilled(ImVec2(mgn, mgn), ImVec2((float)cw - mgn, (float)ch - mgn),
                               IM_COL32(9, 10, 13, (int)(255 * helloSplash)), 26.0f);
            float logoA = helloSplash * (helloT < 0.9f ? (helloT / 0.9f) : 1.0f);
            logoA = logoA < 0 ? 0 : (logoA > 1 ? 1 : logoA);
            Glass::DrawIcon(fdl, Glass::Icon::Spark, ImVec2(cw * 0.5f, ch * 0.5f - 120.0f * scale),
                            116.0f * scale, IM_COL32(255, 255, 255, (int)(235 * logoA)), 2.5f);
            static const char* greet[] = { "hello", "Bonjour", "Hola" };
            const int N = 3;
            float body = HELLO_DUR - HELLO_FO;
            float per  = body / (float)N;
            int gi = (int)(helloT / per); if (gi < 0) gi = 0; if (gi >= N) gi = N - 1;
            float local = helloT - gi * per;
            float rev  = local / (per * 0.5f); rev = rev < 0 ? 0 : (rev > 1 ? 1 : rev);
            float ease = 1.0f - (1.0f - rev) * (1.0f - rev);
            float gout = 1.0f - (local - per * 0.74f) / (per * 0.26f);
            gout = gout < 0 ? 0 : (gout > 1 ? 1 : gout);
            float gA = helloSplash * gout;
            const char* txt = greet[gi];
            float fsz = 138.0f * scale;
            ImVec2 ts = hello_font ? hello_font->CalcTextSizeA(fsz, 99999.0f, 0.0f, txt)
                                   : ImGui::CalcTextSize(txt);
            ImVec2 pos(cw * 0.5f - ts.x * 0.5f, ch * 0.5f - ts.y * 0.5f + 38.0f * scale);
            fdl->PushClipRect(ImVec2(pos.x - 12.0f, pos.y - 50.0f),
                              ImVec2(pos.x + ts.x * ease + 8.0f, pos.y + ts.y + 50.0f), true);
            ImU32 hc = IM_COL32(255, 255, 255, (int)(255 * gA));
            if (hello_font) fdl->AddText(hello_font, fsz, pos, hc, txt);
            else            fdl->AddText(pos, hc, txt);
            fdl->PopClipRect();
        }
        if (g_recording) {
            ImDrawList* fdl = ImGui::GetForegroundDrawList();
            int blink = ((int)(ImGui::GetTime() * 2.0)) & 1;
            ImVec2 c(64.0f, 30.0f);
            fdl->AddCircleFilled(c, 7.0f, IM_COL32(255, 59, 48, blink ? 255 : 130));
            int secs = (int)(g_rec_time / 10000000LL);
            char rb[32]; std::snprintf(rb, sizeof(rb), "REC  %d:%02d", secs / 60, secs % 60);
            fdl->AddText(ImVec2(c.x + 14.0f, c.y - 7.0f), IM_COL32(255, 255, 255, 235), rb);
        }
        if (fxShowFps && logged_in) {
            ImDrawList* fdl = ImGui::GetForegroundDrawList();
            char fb[32]; std::snprintf(fb, sizeof(fb), "%.0f FPS", io.Framerate);
            ImVec2 fs = ImGui::GetFont()->CalcTextSizeA(13.0f, 99999.0f, 0.0f, fb);
            ImVec2 fp(cw * 0.5f - fs.x * 0.5f, 8.0f);
            fdl->AddRectFilled(ImVec2(fp.x - 8, fp.y - 3), ImVec2(fp.x + fs.x + 8, fp.y + fs.y + 3), IM_COL32(0,0,0,120), 6.0f);
            fdl->AddText(ImGui::GetFont(), 13.0f, fp, IM_COL32(120,255,140,255), fb);
        }
        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && ImGui::IsAnyItemHovered() && !Glass::SfxConsumed())
            Glass::PlaySfx(Glass::Sfx::Tap);
        Glass::SfxResetConsumed();
        ImGui::Render();
        const float clear[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
        bool useSSAA = (g_ssaaScale > 1.01f && g_resolveVS && g_resolvePS);
        if (useSSAA) {
            EnsureSSAA((int)(cw * g_ssaaScale + 0.5f), (int)(ch * g_ssaaScale + 0.5f));
            if (g_ssaaRTV) {
                g_glass_renderer.SetRenderScale(g_ssaaScale);
                g_pd3dDeviceContext->OMSetRenderTargets(1, &g_ssaaRTV, nullptr);
                g_pd3dDeviceContext->ClearRenderTargetView(g_ssaaRTV, clear);
                D3D11_VIEWPORT vps{}; vps.Width = (float)g_ssaaW; vps.Height = (float)g_ssaaH; vps.MaxDepth = 1.0f;
                g_pd3dDeviceContext->RSSetViewports(1, &vps);
                g_glass_renderer.Render(g_backdrop.heavySRV(), g_backdrop.softSRV());
                g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
                D3D11_VIEWPORT vp{}; vp.Width = (float)cw; vp.Height = (float)ch; vp.MaxDepth = 1.0f;
                g_pd3dDeviceContext->RSSetViewports(1, &vp);
                ResolveSSAA();
                ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
            } else useSSAA = false;
        }
        if (!useSSAA) {
            g_glass_renderer.SetRenderScale(1.0f);
            g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
            g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear);
            D3D11_VIEWPORT vp{}; vp.Width = (float)cw; vp.Height = (float)ch; vp.MaxDepth = 1.0f;
            g_pd3dDeviceContext->RSSetViewports(1, &vp);
            g_glass_renderer.Render(g_backdrop.heavySRV(), g_backdrop.softSRV());
            ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
        }
        if (g_recording) {
            RecorderFrame(io.DeltaTime);
        }
        { ID3D11Texture2D* bb = nullptr;
          if (doBlur && g_backdrop_uiblur && SUCCEEDED(g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&bb))) && bb) {
              g_backdrop.BlurUI(bb, cw, ch); bb->Release();
              Glass::g_uiBlurTex    = (ImTextureID)(intptr_t)g_backdrop.uiSRV();
              Glass::g_uiBlurOrigin = ImVec2(0.0f, 0.0f);
              Glass::g_uiBlurSize   = ImVec2((float)cw, (float)ch);
          } }
        { double uiTarget = 1.0 / (uiFps >= 1.0f ? (double)uiFps : 1.0);
          LARGE_INTEGER fc; ::QueryPerformanceCounter(&fc);
          if (lastFrameTick.QuadPart) {
              double el = (double)(fc.QuadPart - lastFrameTick.QuadPart) / (double)s_qpf.QuadPart;
              double rem = uiTarget - el; if (rem > 0.001) ::Sleep((DWORD)(rem * 1000.0));
          }
          ::QueryPerformanceCounter(&lastFrameTick); }
        HRESULT hr = g_pSwapChain->Present(1, 0);
        g_SwapChainOccluded = (hr == DXGI_STATUS_OCCLUDED);
    }
    if (g_recording) RecorderStop();
    CleanupSSAA();
    if (g_resolveBlend){g_resolveBlend->Release();g_resolveBlend=nullptr;}
    if (g_resolveSmp){g_resolveSmp->Release();g_resolveSmp=nullptr;}
    if (g_resolvePS){g_resolvePS->Release();g_resolvePS=nullptr;}
    if (g_resolveVS){g_resolveVS->Release();g_resolveVS=nullptr;}
    g_backdrop.Shutdown();
    g_glass_renderer.Shutdown();
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
    CleanupDeviceD3D();
    ::DestroyWindow(hwnd);
    ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
    return 0;
}
#include "app/render_capture.inc"
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam)) return true;
    switch (msg) {
    case WM_SIZE:
        if (wParam == SIZE_MINIMIZED) return 0;
        g_ResizeWidth  = (UINT)LOWORD(lParam);
        g_ResizeHeight = (UINT)HIWORD(lParam);
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) return 0;
        break;
    case WM_KEYDOWN:
        if (wParam == VK_ESCAPE) { ::PostMessageW(hWnd, WM_CLOSE, 0, 0); return 0; }
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}
