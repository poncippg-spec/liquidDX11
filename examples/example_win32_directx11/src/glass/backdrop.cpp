#include "backdrop.h"
#include "shaders.h"
#include <d3dcompiler.h>
#include <algorithm>
#include <cstring>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "user32.lib")

#ifndef WDA_EXCLUDEFROMCAPTURE
#define WDA_EXCLUDEFROMCAPTURE 0x00000011
#endif

namespace Glass {

bool Backdrop::Init(ID3D11Device* device, ID3D11DeviceContext* ctx, HWND hwnd) {
    dev_ = device; ctx_ = ctx; hwnd_ = hwnd;
    ::SetWindowDisplayAffinity(hwnd_, WDA_EXCLUDEFROMCAPTURE);
    if (!CompileBlurShaders()) { Shutdown(); return false; }
    { D3D11_BUFFER_DESC bd = {};
      bd.ByteWidth = 16; bd.Usage = D3D11_USAGE_DYNAMIC;
      bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER; bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
      if (FAILED(dev_->CreateBuffer(&bd, nullptr, &blur_cb_))) { Shutdown(); return false; } }
    { D3D11_SAMPLER_DESC sd = {};
      sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
      sd.AddressU = sd.AddressV = sd.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
      sd.ComparisonFunc = D3D11_COMPARISON_NEVER; sd.MaxLOD = D3D11_FLOAT32_MAX;
      if (FAILED(dev_->CreateSamplerState(&sd, &sampler_))) { Shutdown(); return false; } }
    { D3D11_BLEND_DESC bd = {};
      bd.RenderTarget[0].BlendEnable = FALSE;
      bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
      if (FAILED(dev_->CreateBlendState(&bd, &blend_opaque_))) { Shutdown(); return false; } }
    { D3D11_RASTERIZER_DESC rd = {};
      rd.FillMode = D3D11_FILL_SOLID; rd.CullMode = D3D11_CULL_NONE; rd.DepthClipEnable = TRUE;
      if (FAILED(dev_->CreateRasterizerState(&rd, &raster_))) { Shutdown(); return false; } }
    { D3D11_DEPTH_STENCIL_DESC dd = {};
      dd.DepthEnable = FALSE; dd.StencilEnable = FALSE;
      if (FAILED(dev_->CreateDepthStencilState(&dd, &depth_off_))) { Shutdown(); return false; } }
    CreateDuplication();
    if (!up_[0].srv) {
        int W = ::GetSystemMetrics(SM_CXSCREEN);
        int H = ::GetSystemMetrics(SM_CYSCREEN);
        if (W <= 0) W = 1920; if (H <= 0) H = 1080;
        out_x_ = 0; out_y_ = 0;
        CreateChain(W, H);
    }
    for (int i = 0; i < 8 && !have_first_; ++i) Capture();
    return true;
}

void Backdrop::Shutdown() {
    ReleaseDuplication();
    ReleaseChain();
    if (depth_off_)    { depth_off_->Release();    depth_off_ = nullptr; }
    if (raster_)       { raster_->Release();       raster_ = nullptr; }
    if (blend_opaque_) { blend_opaque_->Release(); blend_opaque_ = nullptr; }
    if (sampler_)      { sampler_->Release();      sampler_ = nullptr; }
    if (blur_cb_)      { blur_cb_->Release();      blur_cb_ = nullptr; }
    if (ps_up_)        { ps_up_->Release();        ps_up_ = nullptr; }
    if (ps_down_)      { ps_down_->Release();      ps_down_ = nullptr; }
    if (vs_full_)      { vs_full_->Release();      vs_full_ = nullptr; }
}

bool Backdrop::CompileBlurShaders() {
    ID3DBlob *vb = nullptr, *db = nullptr, *ub = nullptr, *err = nullptr;
    HRESULT hr; size_t len = std::strlen(kBlurHLSL);
    hr = D3DCompile(kBlurHLSL, len, "BlurVS", nullptr, nullptr, "VSFull", "vs_5_0", 0, 0, &vb, &err);
    if (FAILED(hr)) { if (err) { OutputDebugStringA((char*)err->GetBufferPointer()); err->Release(); } return false; }
    hr = D3DCompile(kBlurHLSL, len, "BlurDown", nullptr, nullptr, "PSDown", "ps_5_0", 0, 0, &db, &err);
    if (FAILED(hr)) { if (err) { OutputDebugStringA((char*)err->GetBufferPointer()); err->Release(); } vb->Release(); return false; }
    hr = D3DCompile(kBlurHLSL, len, "BlurUp", nullptr, nullptr, "PSUp", "ps_5_0", 0, 0, &ub, &err);
    if (FAILED(hr)) { if (err) { OutputDebugStringA((char*)err->GetBufferPointer()); err->Release(); } vb->Release(); db->Release(); return false; }
    bool ok = true;
    ok &= SUCCEEDED(dev_->CreateVertexShader(vb->GetBufferPointer(), vb->GetBufferSize(), nullptr, &vs_full_));
    ok &= SUCCEEDED(dev_->CreatePixelShader (db->GetBufferPointer(), db->GetBufferSize(), nullptr, &ps_down_));
    ok &= SUCCEEDED(dev_->CreatePixelShader (ub->GetBufferPointer(), ub->GetBufferSize(), nullptr, &ps_up_));
    vb->Release(); db->Release(); ub->Release();
    return ok;
}

bool Backdrop::CreateRT(RT& rt, int w, int h, DXGI_FORMAT fmt, bool want_rtv, bool want_srv) {
    ReleaseRT(rt);
    rt.w = w; rt.h = h;
    D3D11_TEXTURE2D_DESC td = {};
    td.Width = w; td.Height = h; td.MipLevels = 1; td.ArraySize = 1;
    td.Format = fmt; td.SampleDesc.Count = 1; td.Usage = D3D11_USAGE_DEFAULT;
    td.BindFlags = (want_rtv ? D3D11_BIND_RENDER_TARGET : 0) | (want_srv ? D3D11_BIND_SHADER_RESOURCE : 0);
    if (FAILED(dev_->CreateTexture2D(&td, nullptr, &rt.tex))) return false;
    if (want_rtv && FAILED(dev_->CreateRenderTargetView(rt.tex, nullptr, &rt.rtv))) return false;
    if (want_srv && FAILED(dev_->CreateShaderResourceView(rt.tex, nullptr, &rt.srv))) return false;
    return true;
}

void Backdrop::ReleaseRT(RT& rt) {
    if (rt.srv) { rt.srv->Release(); rt.srv = nullptr; }
    if (rt.rtv) { rt.rtv->Release(); rt.rtv = nullptr; }
    if (rt.tex) { rt.tex->Release(); rt.tex = nullptr; }
    rt.w = rt.h = 0;
}

bool Backdrop::CreateChain(int W, int H) {
    out_w_ = W; out_h_ = H;
    const DXGI_FORMAT F = DXGI_FORMAT_R8G8B8A8_UNORM;
    for (int k = 0; k < 6; ++k) {
        int w = std::max(1, W >> (k + 1));
        int h = std::max(1, H >> (k + 1));
        if (!CreateRT(down_[k], w, h, F, true, true)) return false;
    }
    for (int k = 0; k < 5; ++k)
        if (!CreateRT(up_[k], down_[k].w, down_[k].h, F, true, true)) return false;
    if (!CreateRT(soft_,  W, H, F, true, true)) return false;
    if (!CreateRT(heavy_, W, H, F, true, true)) return false;
    const float grey[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
    ctx_->ClearRenderTargetView(up_[0].rtv, grey);
    ctx_->ClearRenderTargetView(soft_.rtv,  grey);
    ctx_->ClearRenderTargetView(heavy_.rtv, grey);
    return true;
}

void Backdrop::ReleaseChain() {
    ReleaseRT(desktop_);
    for (auto& r : down_) ReleaseRT(r);
    for (auto& r : up_)   ReleaseRT(r);
    ReleaseRT(soft_);
    ReleaseRT(heavy_);
    ReleaseRT(ui_cap_);
    for (auto& r : ui_down_) ReleaseRT(r);
    for (auto& r : ui_up_)   ReleaseRT(r);
    ReleaseRT(ui_heavy_);
}

void Backdrop::BlurUI(ID3D11Texture2D* src, int w, int h) {
    if (!src || w <= 0 || h <= 0 || !vs_full_ || !ps_down_ || !ps_up_) return;
    if (ui_cap_.w != w || ui_cap_.h != h || !ui_heavy_.srv) {
        D3D11_TEXTURE2D_DESC sd = {}; src->GetDesc(&sd);
        ReleaseRT(ui_cap_);
        for (auto& r : ui_down_) ReleaseRT(r);
        for (auto& r : ui_up_)   ReleaseRT(r);
        ReleaseRT(ui_heavy_);
        const DXGI_FORMAT F = DXGI_FORMAT_R8G8B8A8_UNORM;
        if (!CreateRT(ui_cap_, w, h, sd.Format, false, true)) return;
        for (int k = 0; k < 8; ++k)
            if (!CreateRT(ui_down_[k], std::max(1, w >> (k + 1)), std::max(1, h >> (k + 1)), F, true, true)) return;
        for (int k = 0; k < 7; ++k)
            if (!CreateRT(ui_up_[k], ui_down_[k].w, ui_down_[k].h, F, true, true)) return;
        if (!CreateRT(ui_heavy_, w, h, F, true, true)) return;
    }
    if (!ui_cap_.tex || !ui_heavy_.rtv) return;
    ctx_->CopyResource(ui_cap_.tex, src);
    const float amt = ui_blur_amt_;
    blur_spread_ = amt * 0.7f;
    if (blur_spread_ < 0.25f) blur_spread_ = 0.25f; else if (blur_spread_ > 2.2f) blur_spread_ = 2.2f;
    int extra = (int)((amt - 3.0f) / 2.0f); if (extra < 0) extra = 0; else if (extra > 2) extra = 2;
    const int DOWN = 6 + extra;
    ctx_->IASetInputLayout(nullptr);
    ctx_->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    const float bf[4] = { 0, 0, 0, 0 };
    ctx_->OMSetBlendState(blend_opaque_, bf, 0xffffffff);
    ctx_->OMSetDepthStencilState(depth_off_, 0);
    ctx_->RSSetState(raster_);
    BlurPass(ps_down_, ui_cap_.srv, ui_down_[0], ui_cap_.w, ui_cap_.h);
    for (int k = 1; k < DOWN; ++k)
        BlurPass(ps_down_, ui_down_[k - 1].srv, ui_down_[k], ui_down_[k - 1].w, ui_down_[k - 1].h);
    BlurPass(ps_up_, ui_down_[DOWN - 1].srv, ui_up_[DOWN - 2], ui_down_[DOWN - 1].w, ui_down_[DOWN - 1].h);
    for (int k = DOWN - 3; k >= 0; --k)
        BlurPass(ps_up_, ui_up_[k + 1].srv, ui_up_[k], ui_up_[k + 1].w, ui_up_[k + 1].h);
    BlurPass(ps_up_, ui_up_[0].srv, ui_heavy_, ui_up_[0].w, ui_up_[0].h);
    ID3D11ShaderResourceView* nullsrv[2] = { nullptr, nullptr };
    ctx_->PSSetShaderResources(0, 2, nullsrv);
    ID3D11RenderTargetView* nullrtv = nullptr;
    ctx_->OMSetRenderTargets(1, &nullrtv, nullptr);
}

bool Backdrop::CreateDuplication() {
    ReleaseDuplication();
    IDXGIDevice* dxgi = nullptr;
    if (FAILED(dev_->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgi))) return false;
    IDXGIAdapter* adapter = nullptr;
    if (FAILED(dxgi->GetAdapter(&adapter))) { dxgi->Release(); return false; }
    dxgi->Release();
    RECT wr = {}; ::GetWindowRect(hwnd_, &wr);
    POINT c = { (wr.left + wr.right) / 2, (wr.top + wr.bottom) / 2 };
    IDXGIOutput* chosen = nullptr;
    DXGI_OUTPUT_DESC chosen_desc = {};
    for (UINT i = 0; ; ++i) {
        IDXGIOutput* o = nullptr;
        if (adapter->EnumOutputs(i, &o) == DXGI_ERROR_NOT_FOUND) break;
        DXGI_OUTPUT_DESC od = {}; o->GetDesc(&od);
        const RECT& dc = od.DesktopCoordinates;
        bool inside = c.x >= dc.left && c.x < dc.right && c.y >= dc.top && c.y < dc.bottom;
        if (inside || chosen == nullptr) {
            if (chosen) chosen->Release();
            chosen = o; chosen_desc = od;
            if (inside) break;
        } else {
            o->Release();
        }
    }
    adapter->Release();
    if (!chosen) return false;
    const RECT& dc = chosen_desc.DesktopCoordinates;
    out_x_ = dc.left; out_y_ = dc.top;
    int W = dc.right - dc.left, H = dc.bottom - dc.top;
    if (FAILED(chosen->QueryInterface(__uuidof(IDXGIOutput1), (void**)&output1_))) { chosen->Release(); return false; }
    chosen->Release();
    if (FAILED(output1_->DuplicateOutput(dev_, &dupl_))) { return false; }
    if (down_[0].tex == nullptr || W != down_[0].w * 2)
        if (!CreateChain(W, H)) return false;
    out_w_ = W; out_h_ = H;
    return true;
}

void Backdrop::ReleaseDuplication() {
    if (dupl_)    { dupl_->Release();    dupl_ = nullptr; }
    if (output1_) { output1_->Release(); output1_ = nullptr; }
}

void Backdrop::BlurPass(ID3D11PixelShader* ps, ID3D11ShaderResourceView* src,
                        const RT& dst, int src_w, int src_h) {
    ID3D11ShaderResourceView* nullsrv[2] = { nullptr, nullptr };
    ctx_->PSSetShaderResources(0, 2, nullsrv);
    ctx_->OMSetRenderTargets(1, &dst.rtv, nullptr);
    D3D11_VIEWPORT vp = {}; vp.Width = (float)dst.w; vp.Height = (float)dst.h; vp.MaxDepth = 1.0f;
    ctx_->RSSetViewports(1, &vp);
    D3D11_MAPPED_SUBRESOURCE m;
    if (SUCCEEDED(ctx_->Map(blur_cb_, 0, D3D11_MAP_WRITE_DISCARD, 0, &m))) {
        float* f = (float*)m.pData;
        f[0] = 1.0f / (float)src_w; f[1] = 1.0f / (float)src_h; f[2] = blur_spread_; f[3] = 0.0f;
        ctx_->Unmap(blur_cb_, 0);
    }
    ctx_->VSSetShader(vs_full_, nullptr, 0);
    ctx_->PSSetShader(ps, nullptr, 0);
    ctx_->PSSetConstantBuffers(0, 1, &blur_cb_);
    ctx_->PSSetSamplers(0, 1, &sampler_);
    ctx_->PSSetShaderResources(0, 1, &src);
    ctx_->Draw(3, 0);
}

void Backdrop::RunBlur() {
    if (!desktop_.srv || !down_[0].rtv) return;
    blur_spread_ = 1.0f;
    ctx_->IASetInputLayout(nullptr);
    ctx_->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    const float bf[4] = { 0, 0, 0, 0 };
    ctx_->OMSetBlendState(blend_opaque_, bf, 0xffffffff);
    ctx_->OMSetDepthStencilState(depth_off_, 0);
    ctx_->RSSetState(raster_);
    BlurPass(ps_down_, desktop_.srv, down_[0], desktop_.w, desktop_.h);
    for (int k = 1; k < 6; ++k)
        BlurPass(ps_down_, down_[k - 1].srv, down_[k], down_[k - 1].w, down_[k - 1].h);
    BlurPass(ps_up_, down_[5].srv, up_[4], down_[5].w, down_[5].h);
    BlurPass(ps_up_, up_[4].srv,   up_[3], up_[4].w,   up_[4].h);
    BlurPass(ps_up_, up_[3].srv,   up_[2], up_[3].w,   up_[3].h);
    BlurPass(ps_up_, up_[2].srv,   up_[1], up_[2].w,   up_[2].h);
    BlurPass(ps_up_, up_[1].srv,   up_[0], up_[1].w,   up_[1].h);
    BlurPass(ps_up_, up_[0].srv, heavy_, up_[0].w, up_[0].h);
    BlurPass(ps_up_, desktop_.srv, soft_, desktop_.w, desktop_.h);
    ID3D11ShaderResourceView* nullsrv[2] = { nullptr, nullptr };
    ctx_->PSSetShaderResources(0, 2, nullsrv);
    ID3D11RenderTargetView* nullrtv = nullptr;
    ctx_->OMSetRenderTargets(1, &nullrtv, nullptr);
}

void Backdrop::Capture() {
    if (!dupl_) { if (!CreateDuplication()) return; }
    IDXGIResource* res = nullptr;
    DXGI_OUTDUPL_FRAME_INFO info = {};
    HRESULT hr = dupl_->AcquireNextFrame(have_first_ ? 0 : 250, &info, &res);
    if (hr == DXGI_ERROR_WAIT_TIMEOUT) return;
    if (hr == DXGI_ERROR_ACCESS_LOST) { ReleaseDuplication(); return; }
    if (FAILED(hr)) return;
    bool new_image = (info.LastPresentTime.QuadPart != 0) || !have_first_;
    if (new_image) {
        ID3D11Texture2D* frame = nullptr;
        if (SUCCEEDED(res->QueryInterface(__uuidof(ID3D11Texture2D), (void**)&frame))) {
            D3D11_TEXTURE2D_DESC fd = {}; frame->GetDesc(&fd);
            if (!desktop_.tex || (int)fd.Width != desktop_.w ||
                (int)fd.Height != desktop_.h || fd.Format != desktop_fmt_) {
                desktop_fmt_ = fd.Format;
                CreateRT(desktop_, (int)fd.Width, (int)fd.Height, fd.Format, false, true);
            }
            ctx_->CopyResource(desktop_.tex, frame);
            frame->Release();
            RunBlur();
            have_first_ = true;
        }
    }
    res->Release();
    dupl_->ReleaseFrame();
}

}
