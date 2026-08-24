static const char* kResolveHLSL =
"Texture2D Src : register(t0);\n"
"SamplerState Smp : register(s0);\n"
"struct VSO { float4 pos:SV_Position; float2 uv:TEXCOORD0; };\n"
"VSO VSr(uint id:SV_VertexID){ VSO o; float2 uv=float2((id<<1)&2, id&2); o.uv=uv; o.pos=float4(uv.x*2.0-1.0, 1.0-uv.y*2.0, 0,1); return o; }\n"
"float4 PSr(VSO i):SV_Target { return Src.Sample(Smp, i.uv); }\n";
static void InitResolve() {
    ID3DBlob *vb=nullptr,*pb=nullptr,*err=nullptr; size_t len=strlen(kResolveHLSL);
    if (SUCCEEDED(D3DCompile(kResolveHLSL,len,"ResVS",nullptr,nullptr,"VSr","vs_5_0",0,0,&vb,&err)) && vb)
        g_pd3dDevice->CreateVertexShader(vb->GetBufferPointer(),vb->GetBufferSize(),nullptr,&g_resolveVS);
    if (err){ err->Release(); err=nullptr; }
    if (SUCCEEDED(D3DCompile(kResolveHLSL,len,"ResPS",nullptr,nullptr,"PSr","ps_5_0",0,0,&pb,&err)) && pb)
        g_pd3dDevice->CreatePixelShader(pb->GetBufferPointer(),pb->GetBufferSize(),nullptr,&g_resolvePS);
    if (err){ err->Release(); }
    if (vb) vb->Release(); if (pb) pb->Release();
    D3D11_SAMPLER_DESC sd={}; sd.Filter=D3D11_FILTER_MIN_MAG_MIP_LINEAR; sd.AddressU=sd.AddressV=sd.AddressW=D3D11_TEXTURE_ADDRESS_CLAMP; sd.MaxLOD=D3D11_FLOAT32_MAX;
    g_pd3dDevice->CreateSamplerState(&sd,&g_resolveSmp);
    D3D11_BLEND_DESC bd={}; bd.RenderTarget[0].BlendEnable=FALSE; bd.RenderTarget[0].RenderTargetWriteMask=D3D11_COLOR_WRITE_ENABLE_ALL;
    g_pd3dDevice->CreateBlendState(&bd,&g_resolveBlend);
}
static void CleanupSSAA() {
    if (g_ssaaSRV){g_ssaaSRV->Release();g_ssaaSRV=nullptr;}
    if (g_ssaaRTV){g_ssaaRTV->Release();g_ssaaRTV=nullptr;}
    if (g_ssaaTex){g_ssaaTex->Release();g_ssaaTex=nullptr;}
    g_ssaaW=g_ssaaH=0;
}
static void EnsureSSAA(int w, int h) {
    if (w==g_ssaaW && h==g_ssaaH && g_ssaaTex) return;
    CleanupSSAA(); g_ssaaW=w; g_ssaaH=h; if (w<=0||h<=0||!g_pd3dDevice) return;
    D3D11_TEXTURE2D_DESC td={}; td.Width=w; td.Height=h; td.MipLevels=1; td.ArraySize=1; td.Format=DXGI_FORMAT_R8G8B8A8_UNORM; td.SampleDesc.Count=1; td.Usage=D3D11_USAGE_DEFAULT; td.BindFlags=D3D11_BIND_RENDER_TARGET|D3D11_BIND_SHADER_RESOURCE;
    if (FAILED(g_pd3dDevice->CreateTexture2D(&td,nullptr,&g_ssaaTex))) { g_ssaaW=g_ssaaH=0; return; }
    g_pd3dDevice->CreateRenderTargetView(g_ssaaTex,nullptr,&g_ssaaRTV);
    g_pd3dDevice->CreateShaderResourceView(g_ssaaTex,nullptr,&g_ssaaSRV);
}
static void ResolveSSAA() {
    if (!g_resolveVS || !g_resolvePS || !g_ssaaSRV) return;
    const float bf[4]={0,0,0,0};
    g_pd3dDeviceContext->OMSetBlendState(g_resolveBlend,bf,0xffffffff);
    g_pd3dDeviceContext->IASetInputLayout(nullptr);
    g_pd3dDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    g_pd3dDeviceContext->VSSetShader(g_resolveVS,nullptr,0);
    g_pd3dDeviceContext->PSSetShader(g_resolvePS,nullptr,0);
    g_pd3dDeviceContext->PSSetSamplers(0,1,&g_resolveSmp);
    g_pd3dDeviceContext->PSSetShaderResources(0,1,&g_ssaaSRV);
    g_pd3dDeviceContext->Draw(3,0);
    ID3D11ShaderResourceView* ns=nullptr; g_pd3dDeviceContext->PSSetShaderResources(0,1,&ns);
}
