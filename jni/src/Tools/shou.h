#ifndef SHOU_H
#define SHOU_H

uintptr_t 数组 = 0;

bool 初始化 = false;

void 温迪(){
		ImGuiStyle * style = &ImGui::GetStyle();
		style->Colors[ImGuiCol_TitleBg] = ImColor(133, 182, 42,255); // 标题栏背景颜色改为浅蓝色
        style->Colors[ImGuiCol_TitleBgActive] = ImColor(133, 182, 42,255);//激活的标题栏背景颜色
        style->Colors[ImGuiCol_TitleBgCollapsed] = ImColor(133, 182, 42,255);//折叠的标题栏背景颜色
        style->Colors[ImGuiCol_ResizeGripHovered] = ImColor(63, 87, 13,255);//动的时候右下角拖动大小那个颜色
        style->Colors[ImGuiCol_ResizeGrip] = ImColor(133, 182, 42,255);//不动的调整大小手柄颜色
        style->Colors[ImGuiCol_Text] = ImColor(255, 255, 255, 255);//文本颜色
        style->Colors[ImGuiCol_WindowBg] = ImColor(0, 0, 0, 255);//窗口背景颜色
        style->Colors[ImGuiCol_PopupBg] = ImVec4(133, 182, 42,255);//弹出窗口背景颜色
        style->Colors[ImGuiCol_Border] = ImColor(133, 182, 42,255); // 整体边框颜色
        style->Colors[ImGuiCol_FrameBg] = ImColor(133, 182, 42,255);//框架背景颜色(按钮没动的颜色)
        style->Colors[ImGuiCol_FrameBgActive] = ImColor(133, 182, 42,255);//激活的框架背景颜色
        style->Colors[ImGuiCol_FrameBgHovered] = ImColor(133, 182, 42,255);//鼠标悬停的框架背景颜色
        style->Colors[ImGuiCol_ScrollbarBg] = ImColor(0, 0, 0, 255);//滚动区域背景颜色
        style->Colors[ImGuiCol_ScrollbarGrab] = ImColor(133, 182, 42,255);//滚动条抓取颜色
        style->Colors[ImGuiCol_ScrollbarGrabHovered] = ImColor(63, 87, 13,255);//鼠标悬停时的滚动条抓取颜色
        style->Colors[ImGuiCol_ScrollbarGrabActive] = ImColor(63, 87, 13,255);//激活时的滚动条抓取颜色
        style->Colors[ImGuiCol_CheckMark] = ImColor(63, 87, 13,255);//选中标记颜色
        style->Colors[ImGuiCol_SliderGrab] = ImColor(63, 87, 13,255);//滑块抓取颜色
        style->Colors[ImGuiCol_SliderGrabActive] = ImColor(63, 87, 13,255);//激活时的滑块抓取颜色
        style->Colors[ImGuiCol_Separator] = ImColor(63, 87, 13,255);//分隔线颜色
        style->Colors[ImGuiCol_SeparatorActive] = ImColor(63, 87, 13,255);//激活时的分隔线颜色//深灰色
        style->Colors[ImGuiCol_SeparatorHovered] = ImColor(63, 87, 13,255);//鼠标悬停时的分隔线颜色
        style->Colors[ImGuiCol_Button] = ImColor(133, 182, 42,255);//正常按钮颜色//酷安绿
        style->Colors[ImGuiCol_ButtonActive] = ImColor(63, 87, 13,255);//激活的按钮颜色//微信绿
        style->Colors[ImGuiCol_ButtonHovered] = ImColor(63, 87, 13,255);//鼠标悬停时的按钮颜色
        style->Colors[ImGuiCol_HeaderActive] = ImColor(63, 87, 13,255);//激活的标题颜色
        style->Colors[ImGuiCol_HeaderHovered] = ImColor(63, 87, 13,255);//鼠标悬停时的标题颜色
		style->Colors[ImGuiCol_TextSelectedBg] = ImColor(87, 59, 128,255);//选中文本背景颜色
		style->Colors[ImGuiCol_Header] = ImColor(133, 182, 42,255);//设置下展标题颜色
}

#endif
