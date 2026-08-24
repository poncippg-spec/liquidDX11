#include "glass.h"
#include "shaders.h"
#include <d3dcompiler.h>
#include <d3d11shader.h>
#include <mmsystem.h>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <algorithm>

#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "winmm.lib")

namespace Glass {

Renderer* g = nullptr;

ImTextureID g_uiBlurTex   = 0;
ImVec2      g_uiBlurOrigin = ImVec2(0.0f, 0.0f);
ImVec2      g_uiBlurSize   = ImVec2(1.0f, 1.0f);

bool DrawUIBlurPanel(ImDrawList* dl, ImVec2 pmin, ImVec2 pmax, float rounding, float alpha, ImU32 tint) {
    if (!g_uiBlurTex) return false;
    if (alpha < 0.0f) alpha = 0.0f; else if (alpha > 1.0f) alpha = 1.0f;
    const float iw = g_uiBlurSize.x > 1.0f ? g_uiBlurSize.x : 1.0f;
    const float ih = g_uiBlurSize.y > 1.0f ? g_uiBlurSize.y : 1.0f;
    ImVec2 uv0((pmin.x - g_uiBlurOrigin.x) / iw, (pmin.y - g_uiBlurOrigin.y) / ih);
    ImVec2 uv1((pmax.x - g_uiBlurOrigin.x) / iw, (pmax.y - g_uiBlurOrigin.y) / ih);
    const int ia = (int)(alpha * 255.0f);
    dl->AddImageRounded(g_uiBlurTex, pmin, pmax, uv0, uv1, IM_COL32(255, 255, 255, ia), rounding);
    const int tA = (int)(((tint >> IM_COL32_A_SHIFT) & 0xFF) * alpha);
    dl->AddRectFilled(pmin, pmax, (tint & ~IM_COL32_A_MASK) | ((ImU32)tA << IM_COL32_A_SHIFT), rounding);
    return true;
}

static inline float Clampf(float x, float a, float b) { return x < a ? a : (x > b ? b : x); }
static ImU32 kInk     = IM_COL32(236, 237, 243, 255);
static ImU32 kInkSoft = IM_COL32(151, 154, 168, 255);
static int   g_appearance = 0;
ImU32 InkColor()     { return kInk; }
ImU32 InkSoftColor() { return kInkSoft; }
void  SetInk(ImU32 primary, ImU32 soft) { kInk = primary; kInkSoft = soft; }
static float g_density = 1.0f;
void  SetDensity(float d) { g_density = d < 0.5f ? 0.5f : (d > 1.6f ? 1.6f : d); }
static float g_ui = 1.0f;
void  SetWidgetScale(float s) { g_ui = s < 0.25f ? 0.25f : (s > 4.0f ? 4.0f : s); }
float WidgetScale() { return g_ui; }
static inline float US(float v) { return v * g_ui; }
static float g_flow = 0.0f;
void  SetLiquidFlow(float f) { g_flow = f < 0.0f ? 0.0f : (f > 24.0f ? 24.0f : f); }
int   Appearance()   { return g_appearance; }
static bool g_use24 = false;
void  Set24Hour(bool v) { g_use24 = v; }
bool  Use24Hour()       { return g_use24; }
static void* g_photoTex[64]; static int g_photoN = 0;
void  SetPhotoTextures(void* const* t, int n) { if (n>64) n=64; for (int i=0;i<n;i++) g_photoTex[i]=t[i]; g_photoN = n<0?0:n; }
int   PhotoCount() { return g_photoN; }
void* PhotoTex(int i) { if (g_photoN<=0) return nullptr; int k=((i%g_photoN)+g_photoN)%g_photoN; return g_photoTex[k]; }
static const float kPI = 3.14159265358979f;
static ImU32 AccentCol();

static ImFont* g_icon_font = nullptr;
static ImFont* g_icon_font_brands = nullptr;
void SetIconFont(ImFont* f) { g_icon_font = f; }
void SetIconFontBrands(ImFont* f) { g_icon_font_brands = f; }

static unsigned IconCodepoint(Icon ic) {
    switch (ic) {
    case Icon::Wifi:      return 0xF1EB;
    case Icon::Bluetooth: return 0xF293;
    case Icon::Bolt:      return 0xF0E7;
    case Icon::Sun:       return 0xF185;
    case Icon::Speaker:   return 0xF028;
    case Icon::Bell:      return 0xF0F3;
    case Icon::Moon:      return 0xF186;
    case Icon::Display:   return 0xF108;
    case Icon::Palette:   return 0xF53F;
    case Icon::Person:    return 0xF007;
    case Icon::Lock:      return 0xF023;
    case Icon::Gear:      return 0xF013;
    case Icon::Search:    return 0xF002;
    case Icon::Star:      return 0xF005;
    case Icon::Cloud:     return 0xF0C2;
    case Icon::Heart:     return 0xF004;
    case Icon::Airplane:  return 0xF072;
    case Icon::Check:     return 0xF00C;
    case Icon::ChevronR:  return 0xF054;
    case Icon::Cube:      return 0xF1B2;
    case Icon::Eye:       return 0xF06E;
    case Icon::Globe:     return 0xF0AC;
    case Icon::Gauge:     return 0xF625;
    case Icon::Crosshairs:return 0xF05B;
    case Icon::Box:       return 0xF466;
    case Icon::Wrench:    return 0xF0AD;
    case Icon::LocationDot:return 0xF3C5;
    case Icon::Spark:     return 0xF179;
    case Icon::Music:     return 0xF001;
    case Icon::Play:      return 0xF04B;
    case Icon::Pause:     return 0xF04C;
    case Icon::Next:      return 0xF051;
    case Icon::Prev:      return 0xF048;
    case Icon::Forward:   return 0xF04E;
    case Icon::Backward:  return 0xF04A;
    case Icon::Shuffle:   return 0xF074;
    case Icon::Repeat:    return 0xF363;
    case Icon::VolumeLow: return 0xF027;
    case Icon::VolumeMute:return 0xF6A9;
    case Icon::Microphone:return 0xF130;
    case Icon::Video:     return 0xF03D;
    case Icon::Camera:    return 0xF030;
    case Icon::Image:     return 0xF03E;
    case Icon::Images:    return 0xF302;
    case Icon::Film:      return 0xF008;
    case Icon::Headphones:return 0xF025;
    case Icon::CalendarDays: return 0xF073;
    case Icon::CalendarDay:  return 0xF783;
    case Icon::Clock:     return 0xF017;
    case Icon::Stopwatch: return 0xF2F2;
    case Icon::Hourglass: return 0xF254;
    case Icon::BellSlash: return 0xF1F6;
    case Icon::Envelope:  return 0xF0E0;
    case Icon::Comment:   return 0xF075;
    case Icon::CommentDots:return 0xF4AD;
    case Icon::Phone:     return 0xF095;
    case Icon::PaperPlane:return 0xF1D8;
    case Icon::ShareNodes:return 0xF1E0;
    case Icon::Map:       return 0xF279;
    case Icon::MapPin:    return 0xF276;
    case Icon::Compass:   return 0xF14E;
    case Icon::LocationArrow:return 0xF124;
    case Icon::CloudSun:  return 0xF6C4;
    case Icon::CloudMoon: return 0xF6C3;
    case Icon::CloudRain: return 0xF73D;
    case Icon::CloudBolt: return 0xF76C;
    case Icon::CloudShowers:return 0xF740;
    case Icon::Snowflake: return 0xF2DC;
    case Icon::Wind:      return 0xF72E;
    case Icon::Droplet:   return 0xF043;
    case Icon::Thermometer:return 0xF2C9;
    case Icon::Umbrella:  return 0xF0E9;
    case Icon::Folder:    return 0xF07B;
    case Icon::FolderOpen:return 0xF07C;
    case Icon::File:      return 0xF15B;
    case Icon::FileLines: return 0xF15C;
    case Icon::Trash:     return 0xF1F8;
    case Icon::Download:  return 0xF019;
    case Icon::Upload:    return 0xF093;
    case Icon::Paperclip: return 0xF0C6;
    case Icon::Copy:      return 0xF0C5;
    case Icon::ArrowUp:   return 0xF062;
    case Icon::ArrowDown: return 0xF063;
    case Icon::ArrowLeft: return 0xF060;
    case Icon::ArrowRight:return 0xF061;
    case Icon::ChevronL:  return 0xF053;
    case Icon::ChevronU:  return 0xF077;
    case Icon::ChevronD:  return 0xF078;
    case Icon::Refresh:   return 0xF021;
    case Icon::Expand:    return 0xF065;
    case Icon::Compress:  return 0xF066;
    case Icon::Plus:      return 0xF067;
    case Icon::Minus:     return 0xF068;
    case Icon::Xmark:     return 0xF00D;
    case Icon::Sliders:   return 0xF1DE;
    case Icon::List:      return 0xF03A;
    case Icon::Grid:      return 0xF00A;
    case Icon::Bars:      return 0xF0C9;
    case Icon::Ellipsis:  return 0xF141;
    case Icon::House:     return 0xF015;
    case Icon::Users:     return 0xF0C0;
    case Icon::InfoCircle:return 0xF05A;
    case Icon::WarnTriangle:return 0xF071;
    case Icon::CheckCircle:return 0xF058;
    case Icon::XCircle:   return 0xF057;
    case Icon::Lightbulb: return 0xF0EB;
    case Icon::Paintbrush:return 0xF1FC;
    case Icon::Font:      return 0xF031;
    case Icon::PenEdit:   return 0xF044;
    case Icon::Link:      return 0xF0C1;
    case Icon::BatteryFull: return 0xF240;
    case Icon::BatteryHalf: return 0xF242;
    case Icon::BatteryEmpty:return 0xF244;
    case Icon::PowerOff:  return 0xF011;
    case Icon::Cart:      return 0xF07A;
    case Icon::CreditCard:return 0xF09D;
    case Icon::Gift:      return 0xF06B;
    case Icon::Tag:       return 0xF02B;
    case Icon::Bookmark:  return 0xF02E;
    case Icon::Flag:      return 0xF024;
    case Icon::Wallet:    return 0xF555;
    case Icon::Bag:       return 0xF290;
    case Icon::Key:       return 0xF084;
    case Icon::Fingerprint:return 0xF577;
    case Icon::ShieldHalf:return 0xF3ED;
    case Icon::EyeSlash:  return 0xF070;
    case Icon::LockOpen:  return 0xF09C;
    case Icon::Code:      return 0xF121;
    case Icon::Terminal:  return 0xF120;
    case Icon::Database:  return 0xF1C0;
    case Icon::Server:    return 0xF233;
    case Icon::LayerGroup:return 0xF5FD;
    case Icon::Signal:    return 0xF012;
    case Icon::Qrcode:    return 0xF029;
    case Icon::ToggleOn:  return 0xF205;
    case Icon::ToggleOff: return 0xF204;
    case Icon::Gamepad:   return 0xF11B;
    case Icon::Keyboard:  return 0xF11C;
    case Icon::Rocket:    return 0xF135;
    case Icon::Fire:      return 0xF06D;
    case Icon::Leaf:      return 0xF06C;
    case Icon::MugHot:    return 0xF7B6;
    case Icon::Calculator:return 0xF1EC;
    case Icon::ThumbsUp:  return 0xF164;
    case Icon::StarHalf:  return 0xF5C0;
    case Icon::Github:    return 0xF09B;
    case Icon::Spotify:   return 0xF1BC;
    case Icon::Youtube:   return 0xF167;
    case Icon::Xtwitter:  return 0xE61B;
    case Icon::Google:    return 0xF1A0;
    case Icon::Windows:   return 0xF17A;
    case Icon::Android:   return 0xF17B;
    case Icon::Discord:   return 0xF392;
    case Icon::Newspaper: return 0xF1EA;
    case Icon::Book:      return 0xF02D;
    case Icon::BookOpen:  return 0xF518;
    case Icon::Tv:        return 0xF26C;
    case Icon::Trophy:    return 0xF091;
    case Icon::Medal:     return 0xF5A2;
    case Icon::Crown:     return 0xF521;
    case Icon::Gem:       return 0xF3A5;
    case Icon::Dumbbell:  return 0xF44B;
    case Icon::Running:   return 0xF70C;
    case Icon::Walking:   return 0xF554;
    case Icon::Bed:       return 0xF236;
    case Icon::Pills:     return 0xF484;
    case Icon::Stethoscope:return 0xF0F1;
    case Icon::Car:       return 0xF1B9;
    case Icon::Train:     return 0xF238;
    case Icon::Bus:       return 0xF207;
    case Icon::Bicycle:   return 0xF206;
    case Icon::Utensils:  return 0xF2E7;
    case Icon::Pizza:     return 0xF818;
    case Icon::Fish:      return 0xF578;
    case Icon::Tree:      return 0xF1BB;
    case Icon::Mountain:  return 0xF6FC;
    case Icon::Dice:      return 0xF522;
    case Icon::Ticket:    return 0xF3FF;
    case Icon::Guitar:    return 0xF7A6;
    default:              return 0;
    }
}

static bool IconIsBrand(Icon ic) {
    switch (ic) {
    case Icon::Bluetooth: case Icon::Spark: case Icon::Github: case Icon::Spotify:
    case Icon::Youtube: case Icon::Xtwitter: case Icon::Google: case Icon::Windows:
    case Icon::Android: case Icon::Discord:
        return true;
    default:
        return false;
    }
}

static MaterialParams kMaterials[] = {
     { 0.28f, 1.65f, 0.050f, 1.04f, {0.17f,0.17f,0.21f}, 0.30f, 0.012f, 16.f, 20.f, 0.42f,0.12f,0.18f,1.4f,0.08f,28.f,0.f, 9.f, 0.45f, 1.0f },
     { 0.00f, 0.50f, 0.050f, 1.04f, {0.16f,0.16f,0.20f}, 0.00f, 0.010f, 40.f, 40.f, 1.00f,0.14f,0.20f,1.4f,0.60f,46.f,0.f,16.f, 0.40f, 6.0f },
     { 0.58f, 1.65f, 0.040f, 1.04f, {0.14f,0.14f,0.18f}, 0.52f, 0.008f, 26.f, 28.f, 0.55f,0.16f,0.22f,1.4f,0.18f,52.f,0.f,20.f, 0.50f, 1.8f },
     { 0.85f, 1.40f, 0.050f, 1.00f, {0.04f,0.52f,1.0f},  0.90f, 0.010f, 14.f, 18.f, 0.55f,0.18f,0.18f,1.2f,0.15f,24.f,0.f, 8.f, 0.80f, 0.0f },
     { 0.55f, 1.10f, 0.080f, 1.00f, {0.97f,0.97f,1.0f},  0.94f, 0.000f, 12.f, 14.f, 0.72f,0.22f,0.24f,1.0f,0.22f,13.f,0.f, 4.f, 1.20f, 1.2f },
     { 0.00f, 1.00f, 0.000f, 1.00f, {0.00f,0.00f,0.00f}, 0.00f, 0.000f,  0.f,  1.f, 0.00f,0.00f,0.00f,0.0f,0.00f, 0.f,0.f, 0.f, 0.00f, 0.0f },
};

const MaterialParams& Params(Material m) {
    int i = (int)m;
    if (i < 0 || i >= (int)(sizeof(kMaterials)/sizeof(kMaterials[0]))) i = (int)Material::Regular;
    return kMaterials[i];
}

MaterialParams& EditParams(Material m) {
    int i = (int)m;
    if (i < 0 || i >= (int)(sizeof(kMaterials)/sizeof(kMaterials[0]))) i = (int)Material::Regular;
    return kMaterials[i];
}

void SetGlobalMaterial(float blur, float opacity, float sat, float refr, float chroma, float edge, float shadow) {
    const Material ms[] = { Material::Thin, Material::Regular, Material::Thick };
    for (Material m : ms) {
        MaterialParams& p = kMaterials[(int)m];
        p.blur_mix = blur; p.tint_opacity = opacity; p.saturation = sat;
        p.refr_strength = refr; p.refr_band = refr > 8.0f ? refr : 8.0f; p.chroma = chroma;
        p.highlight = edge; p.shadow_strength = shadow;
    }
}

void SetAccent(float r, float gn, float b) {
    kMaterials[(int)Material::Accent].tint_rgb[0] = r;
    kMaterials[(int)Material::Accent].tint_rgb[1] = gn;
    kMaterials[(int)Material::Accent].tint_rgb[2] = b;
}

static MaterialParams g_matDefaults[8]; static bool g_matSaved = false;
void SetAppearance(int mode) {
    int N = (int)(sizeof(kMaterials)/sizeof(kMaterials[0]));
    if (!g_matSaved) { for (int i=0;i<N;i++) g_matDefaults[i]=kMaterials[i]; g_matSaved=true; }
    g_appearance = (mode==1) ? 1 : 0;
    if (g_appearance == 1) {
        kInk = IM_COL32(24, 25, 30, 255); kInkSoft = IM_COL32(120, 123, 134, 255);
        const Material ms[3] = { Material::Thin, Material::Regular, Material::Thick };
        for (int k=0;k<3;k++) { MaterialParams& p = kMaterials[(int)ms[k]];
            p.tint_rgb[0]=0.95f; p.tint_rgb[1]=0.96f; p.tint_rgb[2]=0.99f; p.brightness=0.34f; }
        MaterialParams& kn = kMaterials[(int)Material::Knob];
        kn.tint_rgb[0]=0.26f; kn.tint_rgb[1]=0.27f; kn.tint_rgb[2]=0.32f; kn.brightness=-0.12f;
    } else {
        kInk = IM_COL32(236, 237, 243, 255); kInkSoft = IM_COL32(151, 154, 168, 255);
        for (int i=0;i<N;i++) {
            kMaterials[i].tint_rgb[0]=g_matDefaults[i].tint_rgb[0];
            kMaterials[i].tint_rgb[1]=g_matDefaults[i].tint_rgb[1];
            kMaterials[i].tint_rgb[2]=g_matDefaults[i].tint_rgb[2];
            kMaterials[i].brightness=g_matDefaults[i].brightness;
        }
    }
}

struct GlassCB {
    float window_size[2];
    float desktop_size[2];
    float window_origin[2];
    float light_dir[2];
    float widget_center[2];
    float widget_half_size[2];
    float corner_radius, fade, blur_mix, saturation;
    float tint_color[4];
    float tint_opacity, brightness, contrast, grain;
    float refr_strength, refr_band, highlight, inner_shadow;
    float border_intensity, border_width, shadow_strength, shadow_radius;
    float shadow_offset[2];
    float time, quad_pad;
    float tilt[2];
    float sheen, chroma;
    float cursor[2];
    float _pad_cur[2];
    float blob1_center[2];
    float blob1_half[2];
    float blob2_center[2];
    float blob2_half[2];
    float blob_k;
    float _pad_blob[3];
    float edge0[4]; float edge1[4]; float edge2[4]; float edge3[4];
    float edge4[4]; float edge5[4]; float edge6[4]; float edge7[4];
    float edge8[4]; float edge9[4]; float edge10[4]; float edge11[4];
};
static_assert(sizeof(GlassCB) == 416, "GlassCB must be 416 bytes");

bool Renderer::Init(ID3D11Device* device, ID3D11DeviceContext* ctx) {
    device_ = device; ctx_ = ctx;
    if (!CompileShaders()) return false;
    { D3D11_BUFFER_DESC bd = {};
      bd.ByteWidth = sizeof(GlassCB); bd.Usage = D3D11_USAGE_DYNAMIC;
      bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER; bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
      if (FAILED(device_->CreateBuffer(&bd, nullptr, &cb_))) return false; }
    { D3D11_SAMPLER_DESC sd = {};
      sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
      sd.AddressU = sd.AddressV = sd.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
      sd.ComparisonFunc = D3D11_COMPARISON_NEVER; sd.MaxLOD = D3D11_FLOAT32_MAX;
      if (FAILED(device_->CreateSamplerState(&sd, &sampler_))) return false; }
    { D3D11_BLEND_DESC bd = {};
      bd.RenderTarget[0].BlendEnable = TRUE;
      bd.RenderTarget[0].SrcBlend  = D3D11_BLEND_ONE;  bd.RenderTarget[0].DestBlend  = D3D11_BLEND_INV_SRC_ALPHA; bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
      bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE; bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA; bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
      bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
      if (FAILED(device_->CreateBlendState(&bd, &blend_))) return false; }
    { D3D11_RASTERIZER_DESC rd = {};
      rd.FillMode = D3D11_FILL_SOLID; rd.CullMode = D3D11_CULL_NONE; rd.DepthClipEnable = TRUE;
      rd.ScissorEnable = TRUE;
      if (FAILED(device_->CreateRasterizerState(&rd, &raster_))) return false; }
    { D3D11_DEPTH_STENCIL_DESC dd = {};
      dd.DepthEnable = FALSE; dd.StencilEnable = FALSE;
      if (FAILED(device_->CreateDepthStencilState(&dd, &depth_))) return false; }
    return true;
}

void Renderer::Shutdown() {
    if (depth_)   { depth_->Release();   depth_ = nullptr; }
    if (raster_)  { raster_->Release();  raster_ = nullptr; }
    if (blend_)   { blend_->Release();   blend_ = nullptr; }
    if (sampler_) { sampler_->Release(); sampler_ = nullptr; }
    if (cb_)      { cb_->Release();      cb_ = nullptr; }
    if (ps_)      { ps_->Release();      ps_ = nullptr; }
    if (vs_)      { vs_->Release();      vs_ = nullptr; }
}

bool Renderer::CompileShaders() {
    ID3DBlob *vb = nullptr, *pb = nullptr, *err = nullptr; HRESULT hr;
    size_t len = std::strlen(kGlassHLSL);
    hr = D3DCompile(kGlassHLSL, len, "GlassVS", nullptr, nullptr, "VS_Glass", "vs_5_0", 0, 0, &vb, &err);
    if (FAILED(hr)) { if (err) { OutputDebugStringA((char*)err->GetBufferPointer()); err->Release(); } return false; }
    hr = D3DCompile(kGlassHLSL, len, "GlassPS", nullptr, nullptr, "PS_Glass", "ps_5_0", 0, 0, &pb, &err);
    if (FAILED(hr)) { if (err) { OutputDebugStringA((char*)err->GetBufferPointer()); err->Release(); } vb->Release(); return false; }
    hr = device_->CreateVertexShader(vb->GetBufferPointer(), vb->GetBufferSize(), nullptr, &vs_); vb->Release();
    if (FAILED(hr)) { pb->Release(); return false; }
    {
        ID3D11ShaderReflection* refl = nullptr;
        if (SUCCEEDED(D3DReflect(pb->GetBufferPointer(), pb->GetBufferSize(),
                                 __uuidof(ID3D11ShaderReflection), (void**)&refl)) && refl) {
            if (ID3D11ShaderReflectionConstantBuffer* rcb = refl->GetConstantBufferByName("GlassCB")) {
                D3D11_SHADER_BUFFER_DESC sbd = {};
                if (SUCCEEDED(rcb->GetDesc(&sbd)) && sbd.Size != (UINT)sizeof(GlassCB)) {
                    char msg[160];
                    std::snprintf(msg, sizeof(msg),
                        "[Glass] GlassCB size mismatch: HLSL cbuffer=%u bytes vs C++ struct=%u bytes\n",
                        sbd.Size, (unsigned)sizeof(GlassCB));
                    OutputDebugStringA(msg);
                    IM_ASSERT(false && "GlassCB layout drift: shaders.h cbuffer != glass.cpp struct");
                }
            }
            refl->Release();
        }
    }
    hr = device_->CreatePixelShader(pb->GetBufferPointer(), pb->GetBufferSize(), nullptr, &ps_); pb->Release();
    return SUCCEEDED(hr);
}

void Renderer::BeginFrame(int win_w, int win_h, int win_x, int win_y,
                          int desk_w, int desk_h, ImVec2 cursor) {
    win_w_ = win_w; win_h_ = win_h; win_x_ = win_x; win_y_ = win_y;
    desk_w_ = desk_w; desk_h_ = desk_h;
    cursor_x_ = cursor.x; cursor_y_ = cursor.y;
    queue_.clear();
    overlay_.clear();
    submit_fade_ = 1.0f;
    ClearClipRect();
    float dt = ImGui::GetIO().DeltaTime;
    time_ += dt;
    Spring& e = springs_.Get(0xE0E0E0u, 0, SpringStyle::Bouncy, 0.0f);
    e.target = 1.0f; e.Tick(dt);
    entrance_ = e.x;
    float th  = light_angle_deg_  * (kPI / 180.0f);
    float th2 = light2_angle_deg_ * (kPI / 180.0f);
    light_x_  = std::sin(th);   light_y_  = -std::cos(th);
    light2_x_ = std::sin(th2);  light2_y_ = -std::cos(th2);
}

size_t Renderer::Submit(const Primitive& p) {
    queue_.push_back(p);
    Primitive& q = queue_.back(); q.fade *= submit_fade_;
    q.clip[0] = clip_[0]; q.clip[1] = clip_[1]; q.clip[2] = clip_[2]; q.clip[3] = clip_[3];
    return queue_.size() - 1;
}
Primitive& Renderer::At(size_t i) { return queue_[i]; }
void Renderer::SubmitOverlay(const Primitive& p) { overlay_.push_back(p); }

void Renderer::Render(ID3D11ShaderResourceView* heavy, ID3D11ShaderResourceView* soft) {
    if (queue_.empty() && overlay_.empty()) return;
    ctx_->OMSetDepthStencilState(depth_, 0);
    const FLOAT bf[4] = { 0,0,0,0 };
    ctx_->OMSetBlendState(blend_, bf, 0xffffffff);
    ctx_->RSSetState(raster_);
    ctx_->VSSetShader(vs_, nullptr, 0);
    ctx_->PSSetShader(ps_, nullptr, 0);
    ctx_->IASetInputLayout(nullptr);
    ctx_->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    ctx_->VSSetConstantBuffers(0, 1, &cb_);
    ctx_->PSSetConstantBuffers(0, 1, &cb_);
    ctx_->PSSetSamplers(0, 1, &sampler_);
    ID3D11ShaderResourceView* srvs[2] = { heavy, soft };
    ctx_->PSSetShaderResources(0, 2, srvs);
    const float ent = std::min(1.0f, std::max(0.0f, entrance_));
    const float rimA_ = edge_cfg_.panel_rim_angle * (kPI / 180.0f);
    const float rimSin_ = std::sin(rimA_), rimNegCos_ = -std::cos(rimA_);
    auto draw_one = [&](const Primitive& p) {
        if (p.fade <= 0.0f || p.hw <= 0.0f || p.hh <= 0.0f) return;
        D3D11_RECT sc;
        sc.left   = (LONG)(std::max(0.0f, p.clip[0]) * render_scale_);
        sc.top    = (LONG)(std::max(0.0f, p.clip[1]) * render_scale_);
        sc.right  = (LONG)(std::min((float)win_w_, p.clip[2]) * render_scale_);
        sc.bottom = (LONG)(std::min((float)win_h_, p.clip[3]) * render_scale_);
        if (sc.right < sc.left) sc.right = sc.left;
        if (sc.bottom < sc.top) sc.bottom = sc.top;
        ctx_->RSSetScissorRects(1, &sc);
        const MaterialParams& m = Params(p.material);
        D3D11_MAPPED_SUBRESOURCE map = {};
        if (FAILED(ctx_->Map(cb_, 0, D3D11_MAP_WRITE_DISCARD, 0, &map))) return;
        GlassCB* d = (GlassCB*)map.pData;
        d->window_size[0] = (float)win_w_;  d->window_size[1] = (float)win_h_;
        d->desktop_size[0] = (float)desk_w_; d->desktop_size[1] = (float)desk_h_;
        d->window_origin[0] = (float)win_x_; d->window_origin[1] = (float)win_y_;
        d->light_dir[0] = light_x_; d->light_dir[1] = light_y_;
        d->widget_center[0] = p.cx; d->widget_center[1] = p.cy;
        d->widget_half_size[0] = std::max(0.5f, p.hw);
        d->widget_half_size[1] = std::max(0.5f, p.hh);
        d->corner_radius = std::min(p.corner_radius, std::min(p.hw, p.hh));
        d->fade = p.fade * ent;
        d->blur_mix = m.blur_mix;
        d->saturation = m.saturation;
        d->tint_color[0] = m.tint_rgb[0]; d->tint_color[1] = m.tint_rgb[1];
        d->tint_color[2] = m.tint_rgb[2]; d->tint_color[3] = 1.0f;
        d->tint_opacity = m.tint_opacity;
        d->brightness = m.brightness;
        d->contrast = m.contrast;
        d->grain = m.grain;
        d->refr_strength = m.refr_strength;
        d->refr_band = m.refr_band;
        d->highlight = m.highlight;
        d->inner_shadow = m.inner_shadow;
        float el = p.elevate;
        d->border_intensity = m.border_intensity;
        d->border_width = m.border_width;
        d->shadow_strength = std::min(0.9f, m.shadow_strength * el);
        d->shadow_radius = m.shadow_radius * el;
        d->shadow_offset[0] = m.shadow_off_x * el;
        d->shadow_offset[1] = m.shadow_off_y * el;
        d->time = time_;
        d->quad_pad = m.shadow_radius * el
                    + std::max(std::fabs(m.shadow_off_x), std::fabs(m.shadow_off_y)) * el
                    + m.border_width + 3.0f;
        d->tilt[0] = tilt_x_; d->tilt[1] = tilt_y_;
        d->sheen = m.sheen;
        d->chroma = m.chroma;
        d->cursor[0] = cursor_x_; d->cursor[1] = cursor_y_;
        d->_pad_cur[0] = g_flow;
        d->_pad_cur[1] = 0.0f;
        d->blob1_center[0] = p.b1x; d->blob1_center[1] = p.b1y;
        d->blob1_half[0]   = p.b1w; d->blob1_half[1]   = p.b1h;
        d->blob2_center[0] = p.b2x; d->blob2_center[1] = p.b2y;
        d->blob2_half[0]   = p.b2w; d->blob2_half[1]   = p.b2h;
        d->blob_k = p.blob_k;
        d->_pad_blob[0] = d->_pad_blob[1] = d->_pad_blob[2] = 0.0f;
        d->edge0[0] = edge_cfg_.fresnel_exp;  d->edge0[1] = edge_cfg_.bevel_scale;
        d->edge0[2] = edge_cfg_.lip;          d->edge0[3] = edge_cfg_.specular_exp;
        d->edge1[0] = edge_cfg_.specular_amt; d->edge1[1] = edge_cfg_.front_amt;
        d->edge1[2] = edge_cfg_.back_amt;     d->edge1[3] = edge_cfg_.sat_pop;
        d->edge2[0] = edge_cfg_.bevel_tilt;   d->edge2[1] = edge_cfg_.light_height;
        d->edge2[2] = edge_cfg_.edge_shadow;  d->edge2[3] = edge_cfg_.cursor_size;
        d->edge3[0] = edge_cfg_.cursor_glow;  d->edge3[1] = edge_cfg_.sheen_amt;
        d->edge3[2] = edge_cfg_.grain_amt;    d->edge3[3] = edge_cfg_.ambient_rim;
        d->edge4[0] = edge_cfg_.front_spread; d->edge4[1] = edge_cfg_.back_spread;
        d->edge4[2] = edge_cfg_.panel_br_spread; d->edge4[3] = edge_cfg_.panel_br_smooth;
        d->edge5[0] = light2_x_;   d->edge5[1] = light2_y_;
        d->edge5[2] = light1_amt_; d->edge5[3] = light2_amt_;
        d->edge6[0] = rimSin_; d->edge6[1] = rimNegCos_;
        d->edge6[2] = edge_cfg_.tbl_fill;
        d->edge6[3] = 0.0f;
        d->edge7[0] = edge_cfg_.adapt_strength;
        d->edge7[1] = edge_cfg_.adapt_pivot; d->edge7[2] = edge_cfg_.adapt_soft;
        d->edge7[3] = edge_cfg_.smooth_refraction;
        d->edge8[0] = edge_cfg_.squircle_power; d->edge8[1] = edge_cfg_.lens_amount;
        d->edge8[2] = edge_cfg_.lens_power;     d->edge8[3] = edge_cfg_.lens_a;
        d->edge9[0] = edge_cfg_.lens_b; d->edge9[1] = edge_cfg_.lens_c; d->edge9[2] = edge_cfg_.lens_d;
        d->edge9[3] = edge_cfg_.glow_weight;
        d->edge10[0] = edge_cfg_.glow_phase; d->edge10[1] = edge_cfg_.glow_edge0;
        d->edge10[2] = edge_cfg_.glow_edge1; d->edge10[3] = edge_cfg_.glow_bias;
        d->edge11[0] = edge_cfg_.glow_anim; d->edge11[1] = d->edge11[2] = d->edge11[3] = 0.0f;
        ctx_->Unmap(cb_, 0);
        ctx_->Draw(6, 0);
    };
    for (const Primitive& p : queue_)   draw_one(p);
    for (const Primitive& p : overlay_) draw_one(p);
    ID3D11ShaderResourceView* nullsrv[2] = { nullptr, nullptr };
    ctx_->PSSetShaderResources(0, 2, nullsrv);
}

struct CardState { Material mat; size_t queue_index; float radius; };
static std::vector<CardState> g_card_stack;

static inline float Dt() { return ImGui::GetIO().DeltaTime; }

bool BeginCard(const char* id, Material m, ImVec2 size) {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(US(22 * g_density), US(20 * g_density)));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(US(12 * g_density), US(11 * g_density)));
    ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(0,0,0,0));
    ImGuiWindowFlags fl =
        ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoCollapse   | ImGuiWindowFlags_NoMove     |
        ImGuiWindowFlags_NoScrollbar  | ImGuiWindowFlags_NoResize;
    if (size.x != 0 && size.y != 0) {
        ImGui::SetNextWindowSize(size, ImGuiCond_Always);
    } else {
        fl |= ImGuiWindowFlags_AlwaysAutoResize;
        if (size.x != 0)
            ImGui::SetNextWindowSizeConstraints(ImVec2(size.x, 0), ImVec2(size.x, 100000.0f));
    }
    bool open = ImGui::Begin(id, nullptr, fl);
    size_t idx = 0;
    if (g) idx = g->Submit(Primitive{});
    g_card_stack.push_back({ m, idx, US(26.0f) });
    return open;
}

void EndCard() {
    if (g_card_stack.empty()) { ImGui::End(); ImGui::PopStyleColor(); ImGui::PopStyleVar(2); return; }
    CardState s = g_card_stack.back(); g_card_stack.pop_back();
    ImVec2 wp = ImGui::GetWindowPos();
    ImVec2 ws = ImGui::GetWindowSize();
    if (g) {
        Primitive& p = g->At(s.queue_index);
        p.cx = wp.x + ws.x * 0.5f; p.cy = wp.y + ws.y * 0.5f;
        p.hw = ws.x * 0.5f; p.hh = ws.y * 0.5f;
        p.corner_radius = s.radius; p.fade = 1.0f; p.material = s.mat;
    }
    ImGui::End();
    ImGui::PopStyleColor();
    ImGui::PopStyleVar(2);
}

#include "widgets/audio.inc"
#include "widgets/widgets_core.inc"
#include "widgets/widgets_input.inc"
#include "widgets/fields_color.inc"
#include "widgets/widgets_misc.inc"
#include "widgets/modals.inc"
#include "widgets/widgets_extra.inc"
#include "widgets/chrome.inc"
}
