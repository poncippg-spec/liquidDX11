#pragma once
#include <d3d11.h>
#include <dxgi1_2.h>
#include <windows.h>

namespace Glass {

class Backdrop {
public:
    bool Init(ID3D11Device* device, ID3D11DeviceContext* ctx, HWND overlay_hwnd);
    void Shutdown();
    void Capture();
    ID3D11ShaderResourceView* heavySRV() const { return heavy_.srv; }
    ID3D11ShaderResourceView* softSRV()  const { return soft_.srv;  }
    int width()   const { return out_w_; }
    int height()  const { return out_h_; }
    int originX() const { return out_x_; }
    int originY() const { return out_y_; }
    bool ok()     const { return heavy_.srv != nullptr && soft_.srv != nullptr; }
    void BlurUI(ID3D11Texture2D* src, int w, int h);
    ID3D11ShaderResourceView* uiSRV() const { return ui_heavy_.srv; }
    int uiW() const { return ui_cap_.w; }
    int uiH() const { return ui_cap_.h; }
    void SetUIBlurSpread(float s) { ui_blur_amt_ = s < 0.0f ? 0.0f : (s > 12.0f ? 12.0f : s); }

private:
    struct RT {
        ID3D11Texture2D*          tex = nullptr;
        ID3D11RenderTargetView*   rtv = nullptr;
        ID3D11ShaderResourceView* srv = nullptr;
        int w = 0, h = 0;
    };

    bool CreateRT(RT& rt, int w, int h, DXGI_FORMAT fmt, bool want_rtv, bool want_srv);
    void ReleaseRT(RT& rt);
    bool CreateChain(int W, int H);
    void ReleaseChain();
    bool CreateDuplication();
    void ReleaseDuplication();
    bool CompileBlurShaders();
    void BlurPass(ID3D11PixelShader* ps, ID3D11ShaderResourceView* src,
                  const RT& dst, int src_w, int src_h);
    void RunBlur();

    ID3D11Device*        dev_  = nullptr;
    ID3D11DeviceContext* ctx_  = nullptr;
    HWND                 hwnd_ = nullptr;
    IDXGIOutputDuplication* dupl_    = nullptr;
    IDXGIOutput1*           output1_ = nullptr;
    int out_w_ = 0, out_h_ = 0, out_x_ = 0, out_y_ = 0;
    RT          desktop_;
    DXGI_FORMAT desktop_fmt_ = DXGI_FORMAT_B8G8R8A8_UNORM;
    RT          down_[6];
    RT          up_[5];
    RT          soft_;
    RT          heavy_;
    RT          ui_cap_;
    RT          ui_down_[8];
    RT          ui_up_[7];
    RT          ui_heavy_;
    float       blur_spread_ = 1.0f;
    float       ui_blur_amt_ = 2.0f;
    ID3D11VertexShader*      vs_full_      = nullptr;
    ID3D11PixelShader*       ps_down_      = nullptr;
    ID3D11PixelShader*       ps_up_        = nullptr;
    ID3D11Buffer*            blur_cb_      = nullptr;
    ID3D11SamplerState*      sampler_      = nullptr;
    ID3D11BlendState*        blend_opaque_ = nullptr;
    ID3D11RasterizerState*   raster_       = nullptr;
    ID3D11DepthStencilState* depth_off_    = nullptr;
    bool have_first_ = false;
};

}
