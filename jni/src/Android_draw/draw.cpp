//
// Created by 泓清 on 2022/8/26.
//

#include <draw.h>
#include <array>
// HAPUS INI: #include "native_surface/utils.h"

// TAMBAHKAN INI (pastikan folder Android_Graphics ada di jni/include atau jni/src)
#include "../Android_Graphics/ANativeWindowCreator.h"
#include <sys/system_properties.h> // Tambahin ini
#include <stdlib.h>                // Tambahin ini

// Tambahin fungsi ini biar compiler kenal
int get_android_api_level() {
    char sdk_ver[PROP_VALUE_MAX];
    __system_property_get("ro.build.version.sdk", sdk_ver);
    return atoi(sdk_ver);
}
// Var
EGLDisplay display = EGL_NO_DISPLAY;
EGLConfig config;
EGLSurface surface = EGL_NO_SURFACE;
ANativeWindow *native_window = nullptr;
EGLContext context = EGL_NO_CONTEXT;

Screen full_screen;
int Orientation = 0;
int screen_x = 0, screen_y = 0;
int init_screen_x = 0, init_screen_y = 0;
bool g_Initialized = false;

string exec(string command)
{
    char buffer[128];
    string result = "";
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) {
        return "popen failed!";
    }
    while (!feof(pipe)) {
        if (fgets(buffer, 128, pipe) != nullptr)
            result += buffer;
    }
    pclose(pipe);
    return result;
}

int init_egl(int _screen_x,int _screen_y, bool log)
{
    display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (display == EGL_NO_DISPLAY) {
        printf("eglGetDisplay error=%u\n", glGetError());
        return -1;
    }
    if(log) printf("eglGetDisplay ok\n");
    
    EGLint *version = new EGLint[2];
    if (!eglInitialize(display, &version[0], &version[1])) {
        printf("eglInitialize error=%u\n", glGetError());
        return -1;
    }
    if(log) printf("eglInitialize ok\n");
    
    const EGLint attribs[] = {EGL_BUFFER_SIZE, 32, EGL_RED_SIZE, 8, EGL_GREEN_SIZE, 8, EGL_BLUE_SIZE, 8, EGL_ALPHA_SIZE, 8, EGL_DEPTH_SIZE, 8, EGL_STENCIL_SIZE, 8, EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT, EGL_SURFACE_TYPE, EGL_WINDOW_BIT, EGL_NONE};
    EGLint num_config;
    if (!eglGetConfigs(display, nullptr, 1, &num_config)) {
        printf("eglGetConfigs  error =%u\n", glGetError());
        return -1;
    }
    if(log) printf("num_config=%d\n", num_config);
    
    if (!eglChooseConfig(display, attribs, &config, 1, &num_config)) {
        printf("eglChooseConfig  error=%u\n", glGetError());
        return -1;
    }
    if(log) printf("eglChooseConfig ok\n");
    
    int attrib_list[] = {EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE};
    context = eglCreateContext(display, config, EGL_NO_CONTEXT, attrib_list);
    if (context == EGL_NO_CONTEXT) {
        printf("eglCreateContext  error = %u\n", glGetError());
        return -1;
    }
    if(log) printf("eglCreateContext ok\n");
    
    // ====================================================================
    // PERUBAHAN UTAMA: MENGGUNAKAN ANDROIDSURFACER IMGUI ENHANCED
    // ====================================================================
    // Parameter ke-4 (bool) biasanya: true = permeate_record (bisa direkam),
    // false = anti-record (layar hitam kalau direkam/di-screenshot).
    // Sesuaikan dengan kebutuhan (biasanya cheat pakai false).
    
    native_window = android::ANativeWindowCreator::Create("SsageGUIMAX", _screen_x, _screen_y, false);

    if (native_window == nullptr) {
        printf("[ERROR] Gagal membuat Native Window!\n");
        return -1;
    }
    if(log) printf("createNativeWindow ok\n");
    // ====================================================================

    surface = eglCreateWindowSurface(display, config, native_window, nullptr);
    if (surface == EGL_NO_SURFACE) {
        printf("eglCreateWindowSurface  error = %u\n", glGetError());
        return -1;
    }
    if(log) printf("eglCreateWindowSurface ok\n");
    
    if (!eglMakeCurrent(display, surface, surface, context)) {
        printf("eglMakeCurrent  error = %u\n", glGetError());
        return -1;
    }
    if(log) printf("eglMakeCurrent ok\n");
    
    return 1;
}

#include <android/api-level.h>
void screen_config()
{   
    if (get_android_api_level() < 31) { // 安卓13支持
    	std::string window_size = exec("wm size");
    	sscanf(window_size.c_str(),"Physical size: %dx%d",&screen_x, &screen_y);
    	full_screen.ScreenX = screen_x;
   		full_screen.ScreenY = screen_y;
    	std::thread *orithread = new std::thread([&] {
        	while (true) {
            	Orientation = atoi(exec("dumpsys input | grep SurfaceOrientation | awk '{print $2}' | head -n 1").c_str());
            	if (Orientation == 0 || Orientation == 2) {
                	screen_x = full_screen.ScreenX;
                	screen_y = full_screen.ScreenY;
            	}
            	if (Orientation == 1 || Orientation == 3) {
                	screen_x = full_screen.ScreenY;
                	screen_y = full_screen.ScreenX;
            	}
            	std::this_thread::sleep_for(0.5s);
        	}
   		});
    	orithread->detach();
    } else {
    	std::string window_size = exec("wm size");
    	sscanf(window_size.c_str(),"Physical size: %dx%d",&screen_x, &screen_y);
   		full_screen.ScreenX = screen_x;
    	full_screen.ScreenY = screen_y;
    	std::thread *orithread = new std::thread([&] {
        	while (true) {
            	Orientation = atoi(exec("dumpsys display | grep 'mCurrentOrientation' | cut -d'=' -f2").c_str());
            	if (Orientation == 0 || Orientation == 2) {
                	screen_x = full_screen.ScreenX;
                	screen_y = full_screen.ScreenY;
            	}
            	if (Orientation == 1 || Orientation == 3) {
                	screen_x = full_screen.ScreenY;
                	screen_y = full_screen.ScreenX;
            	}
            	std::this_thread::sleep_for(0.5s);
        	}
    	});
    	orithread->detach();
    }
}

void ImGui_init(){
    if (g_Initialized){
        return;
    }
	
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
	ImGuiStyle*style = &ImGui::GetStyle();
    io.IniFilename = NULL;
    
    ImGui_ImplAndroid_Init(native_window);
    ImGui_ImplOpenGL3_Init("#version 300 es");
    
// Gunakan Compressed, bukan TTF biasa
io.Fonts->AddFontFromMemoryCompressedTTF((void *)OPPOSans_H, OPPOSans_H_size, 35.0f, NULL, io.Fonts->GetGlyphRangesChineseFull());
    // Sesuaikan font jika perlu
    // io.Fonts->AddFontFromMemoryTTF((void *) OPPOSans_H, OPPOSans_H_size, 35.0f, NULL, io.Fonts->GetGlyphRangesChineseFull());
    
    g_Initialized = true;
	ImGui::GetStyle().ScaleAllSizes(3.0f);
    ImGui::SetNextWindowBgAlpha(1.0f);
    style->WindowTitleAlign = ImVec2(0.5, 0.5);
    style->FramePadding = ImVec2(12, 12);
    style->ScaleAllSizes(1.0f);
    style->WindowMenuButtonPosition = ImGuiDir_None;
    style->WindowRounding = 20.0f;
    style->FrameRounding = 8.0f;
    style->ScrollbarRounding = 7.0f;
   	style->ScrollbarSize = 50.0f;
    style->GrabRounding = 7.0f;
    style->GrabMinSize = 30.0f;
}

void shutdown(){
    if (!g_Initialized){
        return;
    }
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplAndroid_Shutdown();
    ImGui::DestroyContext();
    
    if (display != EGL_NO_DISPLAY){
        eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (context != EGL_NO_CONTEXT){
            eglDestroyContext(display, context);
        }
        if (surface != EGL_NO_SURFACE){
            eglDestroySurface(display, surface);
        }
        eglTerminate(display);
    }
    display = EGL_NO_DISPLAY;
    context = EGL_NO_CONTEXT;
    surface = EGL_NO_SURFACE;
    
    if (native_window) {
        ANativeWindow_release(native_window);
        native_window = nullptr;
    }
}

void OffScreen(Vec2 Obj, float camear, ImColor color, float Radius)
{
	ImRect screen_rect = {0.0f, 0.0f, screen_x, screen_y};
	auto screen_center = screen_rect.GetCenter();
	auto angle = atan2(screen_center.y - Obj.y, screen_center.x - Obj.x);
	angle += camear > 0 ? M_PI : 0.0f;
	Vec2 arrow_center {
			screen_center.x + Radius * cosf(angle),
			screen_center.y + Radius * sinf(angle)
	};
	std::array<ImVec2, 4>points {
			ImVec2(-22.0f, -8.6f),
			ImVec2(0.0f, 0.0f),
			ImVec2(-22.0f, 8.6f),
			ImVec2(-18.0f, 0.0f)
	};
	for (auto & point : points)
	{
		auto x = point.x * 1.155f;
		auto y = point.y * 1.155f;
		point.x = arrow_center.x + x * cosf(angle) - y * sinf(angle);
		point.y = arrow_center.y + x * sinf(angle) + y * cosf(angle);
	}
	float alpha = 1.0f;
	if (camear > 0)
	{
		constexpr float nearThreshold = 200 * 200;
		ImVec2 screen_outer_diff = {
				Obj.x < 0 ? abs(Obj.x) : (Obj.x > screen_rect.Max.x ? Obj.x - screen_rect.Max.x : 0.0f),
				Obj.y < 0 ? abs(Obj.y) : (Obj.y > screen_rect.Max.y ? Obj.y - screen_rect.Max.y : 0.0f),
		};
		float distance = static_cast<float>(pow(screen_outer_diff.x, 2) + pow(screen_outer_diff.y, 2));
		alpha = camear < 0 ? 1.0f : (distance / nearThreshold);
	}
	ImColor arrowColor = color;
	arrowColor.Value.w = std::min(alpha, 1.0f);
	ImGui::GetBackgroundDrawList()->AddTriangleFilled(points[0], points[1], points[3], arrowColor);
	ImGui::GetBackgroundDrawList()->AddTriangleFilled(points[2], points[1], points[3], arrowColor);
	ImGui::GetBackgroundDrawList()->AddQuad(points[0], points[1], points[2], points[3], ImColor(0.0f, 0.0f, 0.0f, alpha), 1.335f);
}