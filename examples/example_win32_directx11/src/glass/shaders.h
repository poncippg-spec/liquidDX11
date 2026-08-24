#pragma once

namespace Glass {

inline constexpr const char* kBlurHLSL = R"HLSL(
cbuffer BlurCB : register(b0)
{
    float2 texel;
    float  spread;
    float  _pad;
};

Texture2D    Src        : register(t0);
SamplerState LinearClamp : register(s0);

struct VSO { float4 pos : SV_Position; float2 uv : TEXCOORD0; };

VSO VSFull(uint id : SV_VertexID)
{
    VSO o;
    float2 uv = float2((id << 1) & 2, id & 2);
    o.uv  = uv;
    o.pos = float4(uv.x * 2.0 - 1.0, 1.0 - uv.y * 2.0, 0.0, 1.0);
    return o;
}

float4 PSDown(VSO i) : SV_Target
{
    float2 h = texel * (0.5 * spread);
    float4 s = Src.Sample(LinearClamp, i.uv) * 4.0;
    s += Src.Sample(LinearClamp, i.uv + float2(-h.x, -h.y));
    s += Src.Sample(LinearClamp, i.uv + float2( h.x, -h.y));
    s += Src.Sample(LinearClamp, i.uv + float2(-h.x,  h.y));
    s += Src.Sample(LinearClamp, i.uv + float2( h.x,  h.y));
    return s / 8.0;
}

float4 PSUp(VSO i) : SV_Target
{
    float2 h = texel * (0.5 * spread);
    float4 s = Src.Sample(LinearClamp, i.uv + float2(-h.x * 2.0, 0.0));
    s += Src.Sample(LinearClamp, i.uv + float2(-h.x,  h.y)) * 2.0;
    s += Src.Sample(LinearClamp, i.uv + float2( 0.0,  h.y * 2.0));
    s += Src.Sample(LinearClamp, i.uv + float2( h.x,  h.y)) * 2.0;
    s += Src.Sample(LinearClamp, i.uv + float2( h.x * 2.0, 0.0));
    s += Src.Sample(LinearClamp, i.uv + float2( h.x, -h.y)) * 2.0;
    s += Src.Sample(LinearClamp, i.uv + float2( 0.0, -h.y * 2.0));
    s += Src.Sample(LinearClamp, i.uv + float2(-h.x, -h.y)) * 2.0;
    return s / 12.0;
}
)HLSL";

inline constexpr const char* kGlassHLSL = R"HLSL(
cbuffer GlassCB : register(b0)
{
    float2 window_size;
    float2 desktop_size;
    float2 window_origin;
    float2 light_dir;

    float2 widget_center;
    float2 widget_half_size;

    float  corner_radius;
    float  fade;
    float  blur_mix;
    float  saturation;

    float4 tint_color;

    float  tint_opacity;
    float  brightness;
    float  contrast;
    float  grain;

    float  refr_strength;
    float  refr_band;
    float  highlight;
    float  inner_shadow;

    float  border_intensity;
    float  border_width;
    float  shadow_strength;
    float  shadow_radius;

    float2 shadow_offset;
    float  time;
    float  quad_pad;

    float2 tilt;
    float  sheen;
    float  chroma;

    float2 cursor;
    float2 _pad_cur;

    float2 blob1_center;
    float2 blob1_half;
    float2 blob2_center;
    float2 blob2_half;
    float  blob_k;
    float3 _pad_blob;

    float4 edge_p0;
    float4 edge_p1;
    float4 edge_p2;
    float4 edge_p3;
    float4 edge_p4;
    float4 edge_p5;
    float4 edge_p6;
    float4 edge_p7;
    float4 edge_p8;
    float4 edge_p9;
    float4 edge_p10;
    float4 edge_p11;
};

Texture2D    BlurHeavy  : register(t0);
Texture2D    BlurSoft   : register(t1);
SamplerState LinearClamp : register(s0);

struct VSOut {
    float4 pos    : SV_Position;
    float2 local  : TEXCOORD0;
    float2 client : TEXCOORD1;
};

VSOut VS_Glass(uint vid : SV_VertexID)
{
    static const float2 corners[6] = {
        float2(-1,-1), float2( 1,-1), float2(-1, 1),
        float2(-1, 1), float2( 1,-1), float2( 1, 1)
    };
    float2 sgn   = corners[vid];
    float2 local = sgn * (widget_half_size + quad_pad);
    float2 client = widget_center + local;

    float2 ndc;
    ndc.x = (client.x / window_size.x) * 2.0 - 1.0;
    ndc.y = 1.0 - (client.y / window_size.y) * 2.0;

    VSOut o;
    o.pos    = float4(ndc, 0, 1);
    o.local  = local;
    o.client = client;
    return o;
}

float sdRoundedRect(float2 p, float2 hs, float r)
{
    float2 q  = abs(p) - hs + r;
    float2 qm = max(q, 0.0);

    float  n  = max(edge_p8.x, 2.0);
    float  corner = (n <= 2.001) ? length(qm)
                                 : pow(pow(qm.x, n) + pow(qm.y, n), 1.0 / n);
    return min(max(q.x, q.y), 0.0) + corner - r;
}

float hash21(float2 p)
{
    p = frac(p * float2(123.34, 345.45));
    p += dot(p, p + 34.345);
    return frac(p.x * p.y);
}

float3 sampleBackdrop(float2 uv)
{
    float3 h = BlurHeavy.SampleLevel(LinearClamp, uv, 0).rgb;
    float3 s = BlurSoft .SampleLevel(LinearClamp, uv, 0).rgb;
    return lerp(s, h, blur_mix);
}

float3 sampleBackdropFrost(float2 uv, float frost)
{
    float3 h = BlurHeavy.SampleLevel(LinearClamp, uv, 0).rgb;
    float3 s = BlurSoft .SampleLevel(LinearClamp, uv, 0).rgb;
    return lerp(s, h, saturate(blur_mix + frost));
}

float sdScene(float2 p)
{
    if (blob_k > 0.001)
    {
        float dA = sdRoundedRect(p - (blob1_center - widget_center), blob1_half, corner_radius);
        float dB = sdRoundedRect(p - (blob2_center - widget_center), blob2_half, corner_radius);
        float h  = saturate(0.5 + 0.5 * (dB - dA) / blob_k);
        return lerp(dB, dA, h) - blob_k * h * (1.0 - h);
    }
    return sdRoundedRect(p, widget_half_size, corner_radius);
}

float DirLight(float2 L, float2 nrm, float3 N3, float2 np, float fresR, float band)
{
    float front_amt = edge_p1.y, back_amt = edge_p1.z;
    float fSpread   = edge_p4.x, bSpread  = edge_p4.y;
    float specExp   = edge_p0.w, specAmt  = edge_p1.x;
    float lightH    = edge_p2.y, sheenAmt = edge_p3.y;

    float l  = dot(nrm, L);

    float fr = pow(max(0.0,  l), max(1.2, 2.0 / fSpread));
    float bk = pow(max(0.0, -l), max(1.2, 2.0 / bSpread));
    float c  = fresR * highlight * (fr * front_amt + bk * back_amt);

    float3 Lp = normalize(float3(L * 0.9, lightH));
    float3 Hh = normalize(Lp + float3(0.0, 0.0, 1.0));
    c += pow(saturate(dot(N3, Hh)), specExp) * highlight * specAmt;

    float tp = saturate(0.5 + 0.62 * dot(np, L)); tp *= tp;
    float ur = pow(saturate(dot(nrm, L)), 2.0) * band;
    c += (tp * 0.11 + ur * 0.20) * sheen * sheenAmt;
    return c;
}
)HLSL" R"HLSL(
float4 PS_Glass(VSOut i) : SV_Target
{
    float2 hs = widget_half_size;
    float  r  = corner_radius;

    float ecFresExp = edge_p0.x, ecBevel = edge_p0.y, ecLip = edge_p0.z, ecSpecExp = edge_p0.w;
    float ecSpecAmt = edge_p1.x, ecFront = edge_p1.y, ecBack = edge_p1.z, ecSatPop = edge_p1.w;
    float ecBevelTilt = edge_p2.x, ecLightH = edge_p2.y, ecEdgeSh = edge_p2.z, ecCurSize = edge_p2.w;
    float ecCurGlow = edge_p3.x, ecSheen = edge_p3.y, ecGrain = edge_p3.z, ecAmbRim = edge_p3.w;
    float ecFrontSpread = edge_p4.x, ecBackSpread = edge_p4.y;

    float  sizeMin  = min(hs.x, hs.y);
    float  refScale = clamp(44.0 / max(sizeMin, 6.0), 0.35, 1.7);
    float  d  = sdScene(i.local);
    float  aa = max(fwidth(d), 0.75);
    float  distN = saturate(-d / max(min(hs.x, hs.y), 1.0));

    float dS  = sdScene(i.local - shadow_offset);
    float saf = 1.0 - smoothstep(0.0, shadow_radius, dS);
    float shadow_a = saf * saf * shadow_strength;

    if (d > shadow_radius + 2.0 && shadow_a <= 0.003)
        discard;

    float shape = 1.0 - smoothstep(-aa, aa, d);

    const float e = 1.5;
    float dx = sdScene(i.local + float2(e,0)) - sdScene(i.local - float2(e,0));
    float dy = sdScene(i.local + float2(0,e)) - sdScene(i.local - float2(0,e));
    float2 nrm = normalize(float2(dx, dy) + 1e-5);

    float  bevelW = clamp(min(hs.x, hs.y) * ecBevel, 5.0, 24.0);
    float  rimT   = saturate(1.0 - smoothstep(0.0, bevelW, -d));
    float  slope  = rimT * rimT;
    float3 N3     = normalize(float3(nrm * slope * ecBevelTilt, 1.0));
    float  fresR  = pow(saturate(1.0 - N3.z), ecFresExp);

    float edge = saturate(1.0 - smoothstep(0.0, refr_band, -d));

    float smR    = edge_p7.w;
    float erUse  = edge * edge;
    erUse        = lerp(erUse, erUse * erUse * (3.0 - 2.0 * erUse), smR);
    float rblur  = smR * edge * 0.4;
    float2 refr_px = nrm * erUse * refr_strength * refScale;
    float2 lens    = (i.client - widget_center) * (0.05 * (1.0 - edge*edge)) * refScale;

    float2 effClient = i.client;
    if (edge_p8.y > 0.001) {
        float lensProf  = 1.0 - edge_p9.x * pow(edge_p9.y * 2.71828183, -edge_p9.z * distN - edge_p8.w);
        float lensScale = pow(max(lensProf, 0.001), edge_p8.z);
        float2 lensClient = widget_center + (i.client - widget_center) * lensScale;
        effClient = lerp(i.client, lensClient, saturate(edge_p8.y));
    }
    float2 base_uv = (effClient + window_origin + refr_px - lens) / desktop_size;

    float liq = _pad_cur.x;
    if (liq > 0.01) {
        float2 q = i.client * 0.011;
        float2 warp = float2(sin(q.y + time*0.55) + 0.6*sin(q.y*2.3 - time*0.37),
                             cos(q.x + time*0.48) + 0.6*cos(q.x*2.1 + time*0.31));
        base_uv += warp * liq / desktop_size;
    }
    float2 cdir    = (nrm * erUse * chroma * refScale) / desktop_size;
    float  frost   = erUse;
    float  tpx     = 1.0 + frost * 6.0 + smR * edge * 5.0;
    float2 tuv     = float2(tpx / desktop_size.x, tpx / desktop_size.y);

    float3 B;
    B.r = sampleBackdropFrost(base_uv + cdir, frost*0.6 + rblur).r;
    B.g = sampleBackdropFrost(base_uv,        frost*0.6 + rblur).g;
    B.b = sampleBackdropFrost(base_uv - cdir, frost*0.6 + rblur).b;

    if (frost > 0.01 || smR > 0.01)
    {
        float3 ft = sampleBackdropFrost(base_uv + float2( tuv.x,  tuv.y), frost + rblur)
                  + sampleBackdropFrost(base_uv + float2(-tuv.x,  tuv.y), frost + rblur)
                  + sampleBackdropFrost(base_uv + float2( tuv.x, -tuv.y), frost + rblur)
                  + sampleBackdropFrost(base_uv + float2(-tuv.x, -tuv.y), frost + rblur);
        B = lerp(B, ft * 0.25, saturate(frost * 0.7 + smR * edge * 0.3));
    }

    float  lum = dot(B, float3(0.2126, 0.7152, 0.0722));
    B = lerp(float3(lum, lum, lum), B, saturation + fresR * ecSatPop);
    B = (B - 0.5) * contrast + 0.5 + brightness;
    B = max(B, 0.0);

    float bgLum  = dot(B, float3(0.2126, 0.7152, 0.0722));
    float adapt  = edge_p7.x * smoothstep(edge_p7.y - edge_p7.z, edge_p7.y + edge_p7.z, bgLum);
    float opEff  = saturate(tint_opacity + adapt);
    float3 glass = lerp(B, tint_color.rgb, opEff);

    float2 np0 = i.local / hs;
    float litfield = saturate(dot(np0, normalize(light_dir)) * 0.5 + 0.5);
    glass += (litfield - 0.5) * 0.05;

    float g = hash21(i.client + float2(time * 3.0, time * 2.0)) - 0.5;
    glass += g * grain * 0.3 * ecGrain;

    float2 np   = i.local / hs;
    float2 Ld1  = normalize(light_dir  + tilt * 0.45);
    float2 Ld2  = normalize(edge_p5.xy + tilt * 0.45);
    float  band = saturate(1.0 - smoothstep(0.0, max(border_width, 1.0) * 3.0, -d));

    float dcov = max(abs(dot(nrm, Ld1)), abs(dot(nrm, Ld2)));
    float bigf = smoothstep(25.0, 90.0, min(hs.x, hs.y));
    float pbf  = smoothstep(16.0, 70.0, max(hs.x, hs.y));

    float2 BR = edge_p6.xy;
    float panelMul    = max(edge_p4.z, 0.05);
    float panelSmooth = clamp(edge_p4.w, 0.3, 4.0);
    float sP    = saturate(dot(np, BR));
    float reach = pow(sP, max(0.2, 2.0 / panelMul));
    reach = lerp(reach, smoothstep(0.0, 1.0, reach), saturate((panelSmooth - 1.0) / 3.0));
    float brShape = lerp(1.0, reach, pbf * saturate(dot(nrm, BR)));

    float rim = fresR * (ecLip + border_intensity * ecAmbRim) * lerp(1.0, dcov, bigf)
              + DirLight(Ld1, nrm, N3, np, fresR, band) * edge_p5.z
              + DirLight(Ld2, nrm, N3, np, fresR, band) * edge_p5.w;

    glass += rim * lerp(1.0, brShape, band);

    float l1 = dot(nrm, Ld1);
    float f1 = pow(max(0.0,  l1), max(1.2, 2.0 / ecFrontSpread));
    float b1 = pow(max(0.0, -l1), max(1.2, 2.0 / ecBackSpread));
    glass -= fresR * (1.0 - f1) * (1.0 - b1) * inner_shadow * ecEdgeSh;

    glass += fresR * (1.0 - f1) * (1.0 - b1) * edge_p6.z;

    float cdist = length(i.client - cursor);
    float cspec = exp(-cdist * cdist / (ecCurSize * ecCurSize));
    glass += cspec * (0.07 + 0.13 * band) * ecCurGlow;

    {
        float ang  = atan2(np.y, np.x);
        float gph  = edge_p10.x + time * edge_p11.x;
        float gmul = sin(ang - gph) * edge_p9.w * smoothstep(edge_p10.y, edge_p10.z, distN) + 1.0 + edge_p10.w;
        glass *= gmul;
    }

    glass = saturate(glass);

    float a   = shape * fade;
    float outA = a + shadow_a * (1.0 - a);
    return float4(glass * a, outA);
}
)HLSL";

}
