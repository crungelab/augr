#include "imgui.h"
#include "imnodes.h"
#include <augite/app/app.h>

using namespace augr;

class MyApp : public App {
  void Draw() override;
  virtual void CreateContext() {
    App::CreateContext();
  }
  virtual void DestroyContext() {
    App::DestroyContext();
  }
};

void MyApp::Draw() {
  App::Draw();
  ImGui::Begin("simple node editor");

  ImNodes::BeginNodeEditor();
  ImNodes::BeginNode(1);

  ImNodes::BeginNodeTitleBar();
  ImGui::TextUnformatted("simple node :)");
  ImNodes::EndNodeTitleBar();

  ImNodes::BeginInputAttribute(2);
  ImGui::Text("input");
  ImNodes::EndInputAttribute();

  ImNodes::BeginOutputAttribute(3);
  ImGui::Indent(40);
  ImGui::Text("output");
  ImNodes::EndOutputAttribute();

  ImNodes::EndNode();
  ImNodes::EndNodeEditor();

  ImGui::End();
}

int main(int, char**) {
  App& app = *new MyApp();
  app.Run();
}