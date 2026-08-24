static void RecorderTapSfx(const unsigned char* wav) {
    if (!g_recording || !g_recAudio || !wav) return;
    int dataBytes = wav[40] | (wav[41] << 8) | (wav[42] << 16) | (wav[43] << 24);
    if (dataBytes <= 0) return;
    const short* pcm = (const short*)(wav + 44);
    int n = dataBytes / 2;
    LARGE_INTEGER now; QueryPerformanceCounter(&now);
    double el = (double)(now.QuadPart - g_recAudioStart.QuadPart) / (double)(g_recAudioQpf.QuadPart ? g_recAudioQpf.QuadPart : 1);
    if (el < 0) el = 0;
    size_t off = (size_t)(el * 22050.0);
    size_t need = off + (size_t)n;
    if (need > 22050ull * 3600ull) return;
    if (g_recAudioBuf.size() < need) g_recAudioBuf.resize(need, 0);
    for (int i = 0; i < n; ++i) {
        int s = (int)g_recAudioBuf[off + i] + (int)pcm[i];
        if (s > 32767) s = 32767; else if (s < -32768) s = -32768;
        g_recAudioBuf[off + i] = (short)s;
    }
}
static void RecorderStop() {
    bool wasRec = g_recording;
    if (g_rec_pipe) { CloseHandle(g_rec_pipe); g_rec_pipe = nullptr; }
    if (g_rec_proc) { WaitForSingleObject(g_rec_proc, 15000); CloseHandle(g_rec_proc); g_rec_proc = nullptr; }
    if (g_rec_stg)  { g_rec_stg->Release(); g_rec_stg = nullptr; g_rec_stg_w = g_rec_stg_h = 0; }
    if (g_rec_buf)  { free(g_rec_buf); g_rec_buf = nullptr; }
    g_recording = false;
    if (!wasRec || !g_rec_tmp[0]) { g_recAudioBuf.clear(); return; }
    const char* ffmpeg = (GetFileAttributesA(g_ffmpeg_exe) != INVALID_FILE_ATTRIBUTES) ? g_ffmpeg_exe : "ffmpeg.exe";
    char wavpath[MAX_PATH] = {}; bool haveAudio = false;
    if (g_recAudio && !g_recAudioBuf.empty()) {
        LARGE_INTEGER now; QueryPerformanceCounter(&now);
        double el = (double)(now.QuadPart - g_recAudioStart.QuadPart) / (double)(g_recAudioQpf.QuadPart ? g_recAudioQpf.QuadPart : 1);
        size_t want = (size_t)(el * 22050.0);
        if (g_recAudioBuf.size() < want) g_recAudioBuf.resize(want, 0);
        DWORD tl = GetTempPathA(MAX_PATH, wavpath);
        if (tl == 0 || tl >= MAX_PATH) lstrcpyA(wavpath, ".\\");
        lstrcatA(wavpath, "glass_rec_audio.wav");
        if (FILE* wf = fopen(wavpath, "wb")) {
            int sr = 22050, n = (int)g_recAudioBuf.size(), db = n * 2, total = 44 + db;
            unsigned char h[44];
            auto W32 = [&](int o, int v){ h[o]=(unsigned char)v; h[o+1]=(unsigned char)(v>>8); h[o+2]=(unsigned char)(v>>16); h[o+3]=(unsigned char)(v>>24); };
            auto W16 = [&](int o, int v){ h[o]=(unsigned char)v; h[o+1]=(unsigned char)(v>>8); };
            memcpy(h, "RIFF", 4); W32(4, total - 8); memcpy(h + 8, "WAVE", 4);
            memcpy(h + 12, "fmt ", 4); W32(16, 16); W16(20, 1); W16(22, 1); W32(24, sr); W32(28, sr * 2); W16(32, 2); W16(34, 16);
            memcpy(h + 36, "data", 4); W32(40, db);
            fwrite(h, 1, 44, wf); fwrite(g_recAudioBuf.data(), 2, (size_t)n, wf); fclose(wf);
            haveAudio = true;
        }
    }
    g_recAudioBuf.clear();
    char cmd[2048];
    if (haveAudio)
        std::snprintf(cmd, sizeof(cmd),
            "cmd.exe /c \"\"%s\" -hide_banner -loglevel error -y -i \"%s\" -i \"%s\" -map 0:v:0 -map 1:a:0 "
            "-vf scale=-2:1440:flags=lanczos,format=yuv420p -c:v libx264 -preset slow -crf 14 -r 240 "
            "-c:a aac -b:a 256k -movflags +faststart -shortest \"%s\" & del /q \"%s\" \"%s\"\"",
            ffmpeg, g_rec_tmp, wavpath, g_rec_path, g_rec_tmp, wavpath);
    else
        std::snprintf(cmd, sizeof(cmd),
            "cmd.exe /c \"\"%s\" -hide_banner -loglevel error -y -i \"%s\" "
            "-vf scale=-2:1440:flags=lanczos,format=yuv420p -c:v libx264 -preset slow -crf 14 -r 240 "
            "-movflags +faststart \"%s\" & del /q \"%s\"\"",
            ffmpeg, g_rec_tmp, g_rec_path, g_rec_tmp);
    STARTUPINFOA si = {}; si.cb = sizeof(si);
    PROCESS_INFORMATION pi = {};
    if (CreateProcessA(nullptr, cmd, nullptr, nullptr, FALSE, CREATE_NO_WINDOW, nullptr, nullptr, &si, &pi)) {
        CloseHandle(pi.hThread); CloseHandle(pi.hProcess);
    }
    g_rec_tmp[0] = 0;
}
static bool RecorderStart(int W, int H) {
    if (g_recording) return true;
    W &= ~1; H &= ~1;
    if (W <= 0 || H <= 0) return false;
    const char* ffmpeg = (GetFileAttributesA(g_ffmpeg_exe) != INVALID_FILE_ATTRIBUTES) ? g_ffmpeg_exe : "ffmpeg.exe";
    { DWORD tl = GetTempPathA(MAX_PATH, g_rec_tmp);
      if (tl == 0 || tl >= sizeof(g_rec_tmp)) lstrcpyA(g_rec_tmp, ".\\");
      lstrcatA(g_rec_tmp, "glass_rec_raw.mp4"); DeleteFileA(g_rec_tmp); }
    SECURITY_ATTRIBUTES sa = {}; sa.nLength = sizeof(sa); sa.bInheritHandle = TRUE;
    HANDLE rd = nullptr, wr = nullptr;
    if (!CreatePipe(&rd, &wr, &sa, 1 << 24)) return false;
    SetHandleInformation(wr, HANDLE_FLAG_INHERIT, 0);
    char cmd[1024];
    std::snprintf(cmd, sizeof(cmd),
        "\"%s\" -hide_banner -loglevel error -y "
        "-thread_queue_size 1024 -use_wallclock_as_timestamps 1 -f rawvideo -pixel_format bgra -video_size %dx%d -i - "
        "-c:v libx264 -preset ultrafast -crf 12 -pix_fmt yuv420p \"%s\"",
        ffmpeg, W, H, g_rec_tmp);
    char logpath[MAX_PATH]; DWORD tl = GetTempPathA(MAX_PATH, logpath);
    if (tl == 0 || tl >= MAX_PATH) lstrcpyA(logpath, "glass_ffmpeg_log.txt");
    else                           lstrcatA(logpath, "glass_ffmpeg_log.txt");
    HANDLE logf = CreateFileA(logpath, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, &sa, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    STARTUPINFOA si = {}; si.cb = sizeof(si);
    si.dwFlags = STARTF_USESTDHANDLES;
    si.hStdInput = rd; si.hStdOutput = logf; si.hStdError = logf;
    PROCESS_INFORMATION pi = {};
    BOOL ok = CreateProcessA(nullptr, cmd, nullptr, nullptr, TRUE, CREATE_NO_WINDOW, nullptr, nullptr, &si, &pi);
    CloseHandle(rd);
    if (logf != INVALID_HANDLE_VALUE) CloseHandle(logf);
    if (!ok) { CloseHandle(wr); return false; }
    CloseHandle(pi.hThread);
    g_rec_pipe = wr; g_rec_proc = pi.hProcess;
    g_rec_buf = (unsigned char*)malloc((size_t)W * H * 4);
    if (!g_rec_buf) { RecorderStop(); return false; }
    g_rec_w = W; g_rec_h = H; g_rec_time = 0; g_rec_acc = 0.0; g_recording = true;
    g_recAudioBuf.clear(); QueryPerformanceFrequency(&g_recAudioQpf); QueryPerformanceCounter(&g_recAudioStart);
    return true;
}
static void RecorderFrame(float dt) {
    if (!g_recording || !g_rec_pipe || !g_pSwapChain || !g_pd3dDevice || !g_rec_buf) return;
    g_rec_time += (LONGLONG)(dt * 1.0e7f);
    g_rec_acc  += (double)dt;
    if (g_rec_acc < (1.0 / 240.0)) return;
    g_rec_acc -= (1.0 / 240.0); if (g_rec_acc > 0.2) g_rec_acc = 0.0;
    ID3D11Texture2D* back = nullptr;
    if (FAILED(g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&back)))) return;
    D3D11_TEXTURE2D_DESC d; back->GetDesc(&d);
    if (!g_rec_stg || g_rec_stg_w != (int)d.Width || g_rec_stg_h != (int)d.Height) {
        if (g_rec_stg) { g_rec_stg->Release(); g_rec_stg = nullptr; }
        D3D11_TEXTURE2D_DESC sd = d; sd.Usage = D3D11_USAGE_STAGING; sd.BindFlags = 0;
        sd.CPUAccessFlags = D3D11_CPU_ACCESS_READ; sd.MiscFlags = 0;
        if (FAILED(g_pd3dDevice->CreateTexture2D(&sd, nullptr, &g_rec_stg))) { back->Release(); return; }
        g_rec_stg_w = (int)d.Width; g_rec_stg_h = (int)d.Height;
    }
    g_pd3dDeviceContext->CopyResource(g_rec_stg, back);
    D3D11_MAPPED_SUBRESOURCE m;
    if (SUCCEEDED(g_pd3dDeviceContext->Map(g_rec_stg, 0, D3D11_MAP_READ, 0, &m))) {
        const int W = g_rec_w, H = g_rec_h, srcW = (int)d.Width, srcH = (int)d.Height;
        const int wcopy = W < srcW ? W : srcW;
        for (int y = 0; y < H; ++y) {
            unsigned char* dst = g_rec_buf + (size_t)y * W * 4;
            if (y >= srcH) { memset(dst, 0, W * 4); continue; }
            const unsigned char* src = (const unsigned char*)m.pData + (size_t)y * m.RowPitch;
            for (int x = 0; x < wcopy; ++x) { dst[x*4+0] = src[x*4+2]; dst[x*4+1] = src[x*4+1]; dst[x*4+2] = src[x*4+0]; dst[x*4+3] = 255; }
            for (int x = wcopy; x < W; ++x) { dst[x*4+0] = 0; dst[x*4+1] = 0; dst[x*4+2] = 0; dst[x*4+3] = 255; }
        }
        g_pd3dDeviceContext->Unmap(g_rec_stg, 0);
        DWORD wrote = 0;
        if (!WriteFile(g_rec_pipe, g_rec_buf, (DWORD)((size_t)W * H * 4), &wrote, nullptr))
            RecorderStop();
    }
    back->Release();
}
