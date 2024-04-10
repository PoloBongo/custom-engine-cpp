#pragma once
#include <imgui.h>

class ImGUIInterface {
public:
    static ImVec4 mainColor;
    static ImVec4 accentColor;
    static ImVec4 textColor;
    static ImVec4 areaBgColor;
    static ImVec4 secondaryColor;

    static void ApplyCustomTheme();
    static void EditTheme();
};