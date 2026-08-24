namespace {

struct SfxSpec {
    unsigned char wave;
    float f0, f1;
    int   ms;
    float decay, amp, m2, a2, m3, a3, noise, atkMs;
};

static inline float SfxOsc(unsigned char wave, float ph, unsigned int& rng){
    switch (wave){
        case 1: return (2.0f/3.14159265f) * asinf(sinf(ph));
        case 2: return sinf(ph) >= 0.0f ? 1.0f : -1.0f;
        case 3: { float u = ph * 0.15915494f; u -= floorf(u); return 2.0f*u - 1.0f; }
        case 4: { rng = rng*1664525u + 1013904223u; return ((int)(rng>>9)&0x7FFF)/16384.0f - 1.0f; }
        default: return sinf(ph);
    }
}

static unsigned char* SfxBuild(const SfxSpec& s){
    const int sr = 22050; int n = sr * s.ms / 1000; if (n < 1) n = 1;
    int dataBytes = n * 2, total = 44 + dataBytes;
    unsigned char* b = (unsigned char*)malloc((size_t)total); if (!b) return nullptr;
    auto W32 = [&](int o,int v){ b[o]=(unsigned char)v; b[o+1]=(unsigned char)(v>>8); b[o+2]=(unsigned char)(v>>16); b[o+3]=(unsigned char)(v>>24); };
    auto W16 = [&](int o,int v){ b[o]=(unsigned char)v; b[o+1]=(unsigned char)(v>>8); };
    memcpy(b,"RIFF",4); W32(4,total-8); memcpy(b+8,"WAVE",4);
    memcpy(b+12,"fmt ",4); W32(16,16); W16(20,1); W16(22,1); W32(24,sr); W32(28,sr*2); W16(32,2); W16(34,16);
    memcpy(b+36,"data",4); W32(40,dataBytes);
    short* out = (short*)(b + 44);
    unsigned int rng = 2463534242u ^ (unsigned)(s.ms*131) ^ (unsigned)(s.f0);
    float ph1=0.0f, ph2=0.0f, ph3=0.0f; const float k = 6.2831853f / (float)sr;
    int atk = (int)(s.atkMs * sr / 1000.0f); int amin = (int)(0.003f*sr); if (atk < amin) atk = amin;
    const float GAIN = 0.58f; const float lpA = 0.55f; float lp = 0.0f;
    for (int i=0;i<n;i++){
        float u = (float)i/(float)n, t = (float)i/(float)sr;
        float fr = s.f0 + (s.f1 - s.f0) * u;
        ph1 += k*fr; ph2 += k*fr*s.m2; ph3 += k*fr*s.m3;
        float v = SfxOsc(s.wave, ph1, rng);
        if (s.a2 > 0.0f) v += s.a2 * SfxOsc(s.wave, ph2, rng);
        if (s.a3 > 0.0f) v += s.a3 * SfxOsc(s.wave, ph3, rng);
        if (s.noise > 0.0f){ rng = rng*1664525u + 1013904223u; v += (s.noise*0.5f) * (((int)(rng>>9)&0x7FFF)/16384.0f - 1.0f); }
        lp += (v - lp) * lpA; v = lp;
        float a = (i < atk) ? (float)i/(float)atk : 1.0f; a = a*a*(3.0f-2.0f*a);
        float rel = (1.0f - u); rel *= rel;
        float env = expf(-t*s.decay) * rel * a;
        float val = v * env * s.amp * GAIN;
        if (val >  1.0f) val =  1.0f;
        if (val < -1.0f) val = -1.0f;
        out[i] = (short)(val * 32767.0f);
    }
    return b;
}

static const SfxSpec kSfx[] = {
     {0, 1850, 1050, 26,  75,  .30f, 0,   0,    0,   0,   0,    2},
     {0, 1400,  900, 24,  80,  .22f, 0,   0,    0,   0,   0,    2},
     {1,  900,  700, 30,  55,  .30f, 2.f, .25f, 0,   0,   0,    2},
     {1,  700,  520, 28,  65,  .24f, 0,   0,    0,   0,   0,    2},
     {0, 2100, 1500, 22,  90,  .26f, 0,   0,    0,   0,   0,    2},
     {1,  700, 1300, 40,  40,  .28f, 2.f, .20f, 0,   0,   0,    2},
     {1, 1300,  650, 40,  40,  .26f, 0,   0,    0,   0,   0,    2},
     {2, 1600, 1600, 14, 120,  .18f, 0,   0,    0,   0,   .15f, 1},
     {0, 1500, 1900, 22,  85,  .26f, 0,   0,    0,   0,   0,    2},
     {1, 1100, 1500, 30,  55,  .26f, 1.5f,.18f, 0,   0,   0,    2},
     {0, 1700, 1700, 20,  95,  .26f, 0,   0,    0,   0,   0,    2},
     {0, 1200, 1800, 26,  70,  .28f, 0,   0,    0,   0,   0,    2},
     {0, 1500, 1000, 24,  80,  .24f, 0,   0,    0,   0,   0,    2},
     {1, 1300, 1100, 22,  85,  .24f, 0,   0,    0,   0,   0,    2},
     {0, 1400, 2000, 22,  90,  .26f, 0,   0,    0,   0,   0,    2},
     {0, 1600, 1100, 22,  90,  .24f, 0,   0,    0,   0,   0,    2},
     {0, 1000, 1000, 18, 100,  .22f, 0,   0,    0,   0,   0,    2},
     {2, 2200, 2200,  8, 200,  .10f, 0,   0,    0,   0,   .10f, 1},
     {0, 1200,  900, 22,  85,  .22f, 0,   0,    0,   0,   0,    2},
     {1,  800, 1000, 20,  95,  .22f, 0,   0,    0,   0,   0,    2},
     {1,  800, 1600, 60,  28,  .26f, 2.f, .15f, 0,   0,   0,    3},
     {1, 1600,  800, 60,  28,  .24f, 0,   0,    0,   0,   0,    3},
     {0,  600, 1100, 70,  24,  .26f, 1.5f,.20f, 0,   0,   0,    4},
     {0, 1100,  600, 60,  30,  .22f, 0,   0,    0,   0,   0,    3},
     {1,  500,  900, 80,  22,  .26f, 0,   0,    0,   0,   0,    4},
     {1,  900,  500, 70,  26,  .22f, 0,   0,    0,   0,   0,    3},
     {0,  900, 1300, 60,  30,  .24f, 2.f, .15f, 0,   0,   0,    3},
     {0,  700, 1500, 70,  26,  .26f, 1.5f,.18f, 0,   0,   0,    4},
     {1, 1000, 1200, 40,  45,  .22f, 0,   0,    0,   0,   0,    2},
     {0,  900,  600, 40,  55,  .22f, 0,   0,    0,   0,   0,    2},
     {0,  523,  659,260,   9,  .22f, 1.26f,.70f,1.5f,.55f,0,    5},
     {2,  220,  160,180,  14,  .26f, 0,   0,    0,   0,   .10f, 3},
     {1,  500,  380,150,  18,  .24f, 0,   0,    0,   0,   0,    3},
     {0, 1318,  988,240,  10,  .24f, 1.5f,.35f, 0,   0,   0,    4},
     {0, 1050, 1300, 50,  40,  .20f, 0,   0,    0,   0,   0,    3},
     {3,  600,  120,120,  22,  .24f, 0,   0,    0,   0,   .20f, 3},
     {0,  700, 1700,120,  20,  .24f, 0,   0,    0,   0,   .08f, 3},
     {0, 1500,  700,110,  22,  .22f, 0,   0,    0,   0,   0,    3},
     {1,  700, 1050, 40,  45,  .26f, 0,   0,    0,   0,   0,    2},
     {1, 1050,  700, 36,  50,  .24f, 0,   0,    0,   0,   0,    2},
     {0, 1200, 1700, 26,  75,  .24f, 0,   0,    0,   0,   0,    2},
     {0, 1700, 1200, 26,  75,  .24f, 0,   0,    0,   0,   0,    2},
     {4,    1,    1, 70,  60,  .30f, 0,   0,    0,   0,   0,    1},
     {0, 1000, 1000, 90,  18,  .22f, 0,   0,    0,   0,   0,    3},
     {2,  400,  300, 40,  50,  .26f, 0,   0,    0,   0,   .25f, 2},
     {1,  600,  900, 40,  45,  .24f, 0,   0,    0,   0,   .10f, 2},
     {0, 1700, 1300, 18, 110,  .20f, 0,   0,    0,   0,   .05f, 1},
     {0, 1100,  900, 20, 100,  .20f, 0,   0,    0,   0,   .05f, 1},
     {1,  900, 1200, 26,  75,  .22f, 0,   0,    0,   0,   0,    2},
     {0, 1300,  800, 20, 100,  .20f, 0,   0,    0,   0,   .05f, 1},
     {4,    1,    1,140,  18,  .26f, 0,   0,    0,   0,   0,    1},
     {2, 1900, 1900, 12, 150,  .16f, 0,   0,    0,   0,   .10f, 1},
     {1, 1300, 1150, 20,  95,  .22f, 0,   0,    0,   0,   0,    2},
     {0,  900, 1400, 60,  35,  .22f, 0,   0,    0,   0,   0,    3},
     {0,  500, 1500, 28,  80,  .26f, 2.f, .20f, 0,   0,   0,    1},
     {0,  784, 1047,300,   9,  .22f, 1.5f,.60f, 2.f, .40f,0,    4},
     {4,    1,    1, 90,  35,  .20f, 0,   0,    0,   0,   0,    2},
     {0, 1800, 2600,180,  14,  .20f, 1.5f,.40f, 2.f, .30f,0,    3},
};

static_assert(sizeof(kSfx)/sizeof(kSfx[0]) == (int)Sfx::Count, "kSfx table must match enum class Sfx order/count");

static unsigned char* g_sfxBank[(int)Sfx::Count] = { nullptr };
static bool g_sfxEnabled = true; static bool g_sfxConsumed = false;

}

void (*g_sfxTap)(const unsigned char*) = nullptr;
void SetSfxEnabled(bool on){ g_sfxEnabled = on; }
bool SfxEnabled(){ return g_sfxEnabled; }
bool SfxConsumed(){ return g_sfxConsumed; }
void SfxResetConsumed(){ g_sfxConsumed = false; }

void PlaySfx(Sfx id){
    int k = (int)id;
    if (k < 0 || k >= (int)Sfx::Count) return;
    g_sfxConsumed = true;
    if (!g_sfxEnabled) return;
    if (!g_sfxBank[k]) g_sfxBank[k] = SfxBuild(kSfx[k]);
    if (g_sfxBank[k]) { PlaySoundA((LPCSTR)g_sfxBank[k], nullptr, SND_MEMORY | SND_ASYNC | SND_NODEFAULT); if (g_sfxTap) g_sfxTap(g_sfxBank[k]); }
}
