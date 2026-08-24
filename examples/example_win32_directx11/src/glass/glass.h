#pragma once
#include <d3d11.h>
#include <vector>
#include <cstdint>
#include "imgui.h"
#include "spring.h"

namespace Glass {

enum class Material : uint8_t {
    Thin, Regular, Thick, Accent, Knob, Clear,
};

enum class Icon : int {
    None, Wifi, Bluetooth, Bolt, Sun, Speaker, Bell, Moon, Display, Palette,
    Person, Lock, Gear, Search, Star, Cloud, Heart, Airplane, Check, ChevronR,
    Cube, Eye, Globe, Gauge, Crosshairs, Box, Wrench, LocationDot, Spark,

    Music, Play, Pause, Next, Prev, Forward, Backward, Shuffle, Repeat,
    VolumeLow, VolumeMute, Microphone, Video, Camera, Image, Images, Film, Headphones,

    CalendarDays, CalendarDay, Clock, Stopwatch, Hourglass, BellSlash,

    Envelope, Comment, CommentDots, Phone, PaperPlane, ShareNodes,

    Map, MapPin, Compass, LocationArrow,

    CloudSun, CloudMoon, CloudRain, CloudBolt, CloudShowers, Snowflake, Wind,
    Droplet, Thermometer, Umbrella,

    Folder, FolderOpen, File, FileLines, Trash, Download, Upload, Paperclip, Copy,

    ArrowUp, ArrowDown, ArrowLeft, ArrowRight, ChevronL, ChevronU, ChevronD,
    Refresh, Expand, Compress,

    Plus, Minus, Xmark, Sliders, List, Grid, Bars, Ellipsis, House, Users,
    InfoCircle, WarnTriangle, CheckCircle, XCircle, Lightbulb, Paintbrush, Font,
    PenEdit, Link,

    BatteryFull, BatteryHalf, BatteryEmpty, PowerOff,

    Cart, CreditCard, Gift, Tag, Bookmark, Flag, Wallet, Bag,

    Key, Fingerprint, ShieldHalf, EyeSlash, LockOpen,

    Code, Terminal, Database, Server, LayerGroup, Signal, Qrcode, ToggleOn, ToggleOff,
    Gamepad, Keyboard, Rocket, Fire, Leaf, MugHot, Calculator, ThumbsUp, StarHalf,

    Github, Spotify, Youtube, Xtwitter, Google, Windows, Android, Discord,

    Newspaper, Book, BookOpen, Tv, Trophy, Medal, Crown, Gem, Dumbbell, Running,
    Walking, Bed, Pills, Stethoscope, Car, Train, Bus, Bicycle, Utensils, Pizza,
    Fish, Tree, Mountain, Dice, Ticket, Guitar,
    Count_
};
void DrawIcon(ImDrawList* dl, Icon ic, ImVec2 center, float size, ImU32 col, float thick = 2.2f);
void SetIconFont(ImFont* f);
void SetIconFontBrands(ImFont* f);

struct MaterialParams {
    float blur_mix; float saturation; float brightness; float contrast;
    float tint_rgb[3]; float tint_opacity; float grain; float refr_strength;
    float refr_band; float highlight; float inner_shadow; float border_intensity;
    float border_width; float shadow_strength; float shadow_radius; float shadow_off_x;
    float shadow_off_y; float sheen; float chroma;
};

const MaterialParams& Params(Material m);
MaterialParams& EditParams(Material m);
void SetAccent(float r, float g, float b);
void SetGlobalMaterial(float blur, float opacity, float sat, float refr,
                       float chroma, float edge, float shadow);

void  SetAppearance(int mode);
int   Appearance();
ImU32 InkColor();
ImU32 InkSoftColor();
void  SetInk(ImU32 primary, ImU32 soft);
void  SetDensity(float d);
void  SetWidgetScale(float s);
float WidgetScale();
void  SetLiquidFlow(float f);
void  Set24Hour(bool v);
bool  Use24Hour();

enum class Sfx : int {
    Tap=0, TapSoft, ButtonPrimary, ButtonSecondary, IconTap,
    ToggleOn, ToggleOff, SwitchTick, SegmentSelect, TabChange,
    RadioSelect, CheckOn, CheckOff, ChipTap, StepperUp,
    StepperDown, SliderGrab, SliderTick, SliderRelease, ScrubGrab,
    NavPush, NavPop, ModalOpen, ModalClose, SheetUp,
    SheetDown, PopoverOpen, PaletteOpen, MenuOpen, Dismiss,
    Success, Error, Warning, Notify, Banner,
    Delete, Send, Receive, Play, Pause,
    Next, Prev, Shutter, RecordTone, Lock,
    Unlock, KeyTap, KeySpace, KeyReturn, Backspace,
    DiceRoll, CounterTick, CalcKey, Refresh, Pop,
    Reward, PageTurn, Charm,
    Count
};
void  PlaySfx(Sfx id);
void  SetSfxEnabled(bool on);
bool  SfxEnabled();
bool  SfxConsumed();
void  SfxResetConsumed();
extern void (*g_sfxTap)(const unsigned char*);

void  SetPhotoTextures(void* const* tex, int count);
int   PhotoCount();
void* PhotoTex(int i);

struct Primitive {
    float cx = 0, cy = 0;
    float hw = 0, hh = 0;
    float corner_radius = 0;
    float fade = 0;
    float elevate = 1.0f;
    float clip[4] = { -1e6f, -1e6f, 1e6f, 1e6f };
    Material material = Material::Regular;

    float b1x = 0, b1y = 0, b1w = 0, b1h = 0;
    float b2x = 0, b2y = 0, b2w = 0, b2h = 0;
    float blob_k = 0;
};

void SubmitBlob(float cx1, float cy1, float hw1, float hh1,
                float cx2, float cy2, float hw2, float hh2,
                float radius, float k, float fade, Material m);

struct GlassEdgeConfig {
    float fresnel_exp  = 2.5f;
    float bevel_scale  = 0.6f;
    float lip          = 0.16f;
    float specular_exp = 40.0f;
    float specular_amt = 1.05f;
    float front_amt    = 1.0f;
    float back_amt     = 0.85f;
    float sat_pop      = 1.4f;
    float bevel_tilt   = 2.6f;
    float light_height = 0.8f;
    float edge_shadow  = 0.45f;
    float cursor_size  = 170.0f;
    float cursor_glow  = 0.0f;
    float sheen_amt    = 1.0f;
    float grain_amt    = 1.0f;
    float ambient_rim  = 0.55f;
    float front_spread = 1.0f;
    float back_spread  = 1.0f;
    float panel_br_spread = 1.0f;
    float panel_br_smooth = 1.0f;
    float panel_rim_angle = 135.0f;
    float tbl_fill        = 0.0f;
    float adapt_strength  = 0.0f;
    float adapt_pivot     = 0.6f;
    float adapt_soft      = 0.3f;
    float smooth_refraction = 0.0f;

    float squircle_power = 2.0f;
    float lens_amount    = 0.0f;
    float lens_power     = 2.0f;
    float lens_a = 0.7f, lens_b = 2.3f, lens_c = 5.2f, lens_d = 6.9f;
    float glow_weight = 0.0f;
    float glow_phase  = 0.5f;
    float glow_edge0  = 0.5f;
    float glow_edge1  = -0.5f;
    float glow_bias   = 0.0f;
    float glow_anim   = 0.0f;
};

class Renderer {
public:
    bool Init(ID3D11Device* device, ID3D11DeviceContext* ctx);
    void Shutdown();
    void SetEdgeConfig(const GlassEdgeConfig& c) { edge_cfg_ = c; }

    void SetLights(float angle1, float amt1, float angle2, float amt2) {
        light_angle_deg_ = angle1; light1_amt_ = amt1;
        light2_angle_deg_ = angle2; light2_amt_ = amt2;
    }

    void BeginFrame(int window_w, int window_h,
                    int window_origin_x, int window_origin_y,
                    int desktop_w, int desktop_h,
                    ImVec2 cursor_pos_in_window);
    void SetTilt(float tx, float ty) { tilt_x_ = tx; tilt_y_ = ty; }
    void SetSubmitFade(float f) { submit_fade_ = f; }
    float SubmitFadeValue() const { return submit_fade_; }
    void SetClipRect(float x0, float y0, float x1, float y1) { clip_[0]=x0; clip_[1]=y0; clip_[2]=x1; clip_[3]=y1; }
    void ClearClipRect() { clip_[0]=-1e6f; clip_[1]=-1e6f; clip_[2]=1e6f; clip_[3]=1e6f; }
    size_t Submit(const Primitive& p);
    Primitive& At(size_t i);
    void SubmitOverlay(const Primitive& p);

    void Render(ID3D11ShaderResourceView* backdrop_heavy,
                ID3D11ShaderResourceView* backdrop_soft);

    SpringCache& springs() { return springs_; }
    float Entrance() const { return entrance_; }
    void SetRenderScale(float s) { render_scale_ = s < 1.0f ? 1.0f : s; }

private:
    bool CompileShaders();

    ID3D11Device*            device_  = nullptr;
    ID3D11DeviceContext*     ctx_     = nullptr;

    ID3D11VertexShader*      vs_      = nullptr;
    ID3D11PixelShader*       ps_      = nullptr;
    ID3D11Buffer*            cb_      = nullptr;
    ID3D11SamplerState*      sampler_ = nullptr;
    ID3D11BlendState*        blend_   = nullptr;
    ID3D11RasterizerState*   raster_  = nullptr;
    ID3D11DepthStencilState* depth_   = nullptr;

    int   win_w_ = 0, win_h_ = 0, win_x_ = 0, win_y_ = 0, desk_w_ = 0, desk_h_ = 0;
    float render_scale_ = 1.0f;
    float cursor_x_ = 0.0f, cursor_y_ = 0.0f;
    float light_x_ = 0.0f, light_y_ = -1.0f;
    float light2_x_ = 0.0f, light2_y_ = -1.0f;
    float light_angle_deg_  = 341.0f;
    float light2_angle_deg_ = 161.0f;
    float light1_amt_ = 1.0f;
    float light2_amt_ = 0.0f;
    float time_    = 0.0f;
    float entrance_ = 0.0f;
    float tilt_x_  = 0.0f, tilt_y_ = 0.0f;
    float submit_fade_ = 1.0f;
    float clip_[4] = { -1e6f, -1e6f, 1e6f, 1e6f };

    std::vector<Primitive> queue_;
    std::vector<Primitive> overlay_;
    SpringCache            springs_;
    GlassEdgeConfig        edge_cfg_;
};

extern Renderer* g;

extern ImTextureID g_uiBlurTex;
extern ImVec2      g_uiBlurOrigin;
extern ImVec2      g_uiBlurSize;

bool DrawUIBlurPanel(ImDrawList* dl, ImVec2 pmin, ImVec2 pmax, float rounding, float alpha, ImU32 tint);

bool BeginCard(const char* id, Material m = Material::Regular, ImVec2 size = ImVec2(0,0));
void EndCard();

bool Button(const char* label, bool primary = false, ImVec2 size = ImVec2(0,0));
bool IconButton(const char* id, Icon ic, float size = 36.0f);
bool Toggle(const char* label, bool* v);
bool Slider(const char* label, float* v, float vmin, float vmax, const char* fmt = "%.2f");

int  SegmentedControl(const char* id, const char* const* labels, int count, int current);
bool Checkbox(const char* label, bool* v);
int  RadioGroup(const char* id, const char* const* opts, int count, int current);
void Stepper(const char* label, int* v, int step, int vmin, int vmax);
void ProgressBar(float frac, ImVec2 size = ImVec2(0,0));
bool Chip(const char* label, bool selected);
bool Keybind(const char* label, int* key);
void StatRow(const char* label, const char* value);
bool Dropdown(const char* label, const char* const* items, int count, int* current);
bool ColorButton(const char* label, float rgb[3]);
void DrawColorPickerModal();
bool TextField(const char* label, char* buf, int buflen, const char* hint = nullptr, Icon leading = Icon::None);
bool SearchField(char* buf, int buflen, const char* hint = "Search", float width = 0.0f);
bool PasswordField(const char* label, char* buf, int buflen, const char* hint = nullptr);
bool FieldRaw(const char* label, char* buf, int buflen, const char* hint, Icon leading, bool password, float height);
bool FieldWasFocused();

void ProgressRing(const char* id, float frac, float radius, const char* center = nullptr);
void Badge(const char* text, ImU32 color);
void Avatar(const char* initials, float size, ImU32 tint);
int  Rating(const char* id, int stars, int maxstars = 5);
bool VSlider(const char* id, float* v, float vmin, float vmax, ImVec2 size);
int  SegmentedIcons(const char* id, const Icon* icons, int count, int current);
void Tooltip(const char* text);
void SeparatorText(const char* text);
bool NumberDrag(const char* label, float* v, float speed, float vmin, float vmax, const char* fmt = "%.1f");
void StatusPill(const char* text, ImU32 color);
void SectionHeader(const char* text);
void Divider();

void BeginPanel(const char* title);
void EndPanel();

bool RowToggle(Icon ic, ImU32 tile, const char* label, bool* v);
void RowValue(Icon ic, ImU32 tile, const char* label, const char* value);

void Notify(Icon ic, ImU32 tile, const char* app, const char* title, const char* msg);
void DrawNotifications(float win_w, float win_h);

int SidebarNav(const char* id, const Icon* icons, const char* const* labels,
               const ImU32* tints, int count, int current, float width, float itemH);

enum class ProfileAction {
    None = 0, ViewProfile, AccountSettings, GlassSettings, SignOut,
    StatusChanged, ThemeChanged
};
ProfileAction ProfileMenu(const char* id, const char* name, const char* email,
                          const char* initials, ImU32 tint,
                          float x, float y, float w,
                          int* status, int* theme, float master, bool active = false);

void Spinner(const char* id, float radius = 13.0f, ImU32 col = 0);

bool BeginDisclosure(const char* title, bool* open);
void EndDisclosure();

void AlertOpen(const char* title, const char* msg, const char* okLabel,
               const char* cancelLabel = nullptr, bool destructive = false);
int  DrawAlert();

void ActionSheetOpen(const char* title, const char* const* items, int count, int destructiveIdx = -1);
int  DrawActionSheet();

void ContextMenuOpen(float x, float y, const char* const* items, int count, int destructiveIdx = -1);
int  DrawContextMenu();

void CommandPaletteToggle();
bool CommandPaletteOpen();
int  DrawCommandPalette(const char* const* items, int count);

int  TabBar(const char* id, const Icon* icons, const char* const* labels, int count, int current);

bool ControlTile(Icon ic, const char* label, bool* v, float size = 76.0f);

int  DrumPicker(const char* id, const char* const* items, int count, int current, ImVec2 size);

void DynamicIsland(Icon ic, const char* title, const char* trailing, bool expanded, float center_x, float top_y, float max_w);

bool Scrubber(const char* id, float* v, float vmin, float vmax, float width = 0.0f);

void  Skeleton(const char* id, ImVec2 size);
void  AttentionRing(const char* id, ImVec2 center, float radius);
void  LevelIndicator(float frac, int segments, ImVec2 size);
int   PageDots(const char* id, int count, int current);
void  ActivityRings(const char* id, const float* fracs, const ImU32* cols, int count, float radius);
float Gauge(const char* id, float v, float vmin, float vmax, float radius, const char* fmt = "%.0f");

enum class MorphIcon { PlayPause, PlusClose, MenuClose };
bool  MorphButton(const char* id, MorphIcon kind, bool* state, float size = 38.0f);
void  Odometer(const char* id, int value, float fontScale = 1.0f);
void  SuccessHUD(const char* id, bool trigger);
bool  TokenField(const char* id, char tokens[][24], int* count, int maxTokens, char* input, int inputLen);
bool  ComboBox(const char* label, char* buf, int buflen, const char* const* items, int count);

bool  Calendar(const char* id, int* day, int* month, int* year);
int   OutlineTree(const char* id, const char* const* labels, const int* depth, const Icon* icons, int count, int current);

void  BannerOpen(Icon ic, ImU32 tile, const char* app, const char* title, const char* msg);
void  DrawBanner(float win_w);
void  ShareSheetOpen(const char* title, const Icon* icons, const char* const* labels, int count);
int   DrawShareSheet();
void  GlassWidget(const char* id, const char* title, ImVec2 size);
void  EndGlassWidget();
int   Onboarding(const char* id, const char* const* titles, const char* const* bodies, const Icon* icons, int count);

int   TrafficLights(const char* id, float x, float y);
int   MenuBar(const char* id, const char* const* menus, int count);
bool  CollapsingTitle(const char* text, float scrollY);
int   SwipeRow(const char* id, const char* label, const char* const* actions, const ImU32* actionCols, int actionCount);

}
