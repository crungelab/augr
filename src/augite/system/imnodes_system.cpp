#include "imnodes.h"

#include "imnodes_system.h"


void ImNodesSystem::Create() {
  ImNodes::CreateContext();
  ImNodes::PushAttributeFlag(ImNodesAttributeFlags_EnableLinkDetachWithDragClick);

  ImNodesIO &io = ImNodes::GetIO();
  io.LinkDetachWithModifierClick.Modifier = &ImGui::GetIO().KeyCtrl;
  ImNodes::LoadCurrentEditorStateFromIniFile("imnodes.ini");
}

void ImNodesSystem::Destroy() {
  ImNodes::SaveCurrentEditorStateToIniFile("imnodes.ini");
  ImNodes::DestroyContext();
}