#include "imnodes.h"

#include <augite/system/imnodes_system.h>

void ImNodesSystem::Create() {
    ImNodes::CreateContext();
    ImNodes::PushAttributeFlag(
        ImNodesAttributeFlags_EnableLinkDetachWithDragClick);

    ImNodesIO &io = ImNodes::GetIO();
    io.LinkDetachWithModifierClick.Modifier = &ImGui::GetIO().KeyCtrl;
    // ImNodes::LoadCurrentEditorStateFromIniFile("imnodes.ini");

    ImNodesStyle &style = ImNodes::GetStyle();
    style.Colors[ImNodesCol_TitleBar] = IM_COL32(45, 55, 65, 255); // dark slate
    style.Colors[ImNodesCol_TitleBarHovered] =
        IM_COL32(60, 75, 90, 255); // slightly lighter on hover
    style.Colors[ImNodesCol_TitleBarSelected] =
        IM_COL32(70, 130, 180, 255); // steel blue when selected
}

void ImNodesSystem::Destroy() {
    // ImNodes::SaveCurrentEditorStateToIniFile("imnodes.ini");
    ImNodes::DestroyContext();
}