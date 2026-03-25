#include "plug_editor.h"
#include "plug_view.h"

IPlugView* PLUGIN_API PlugEditor::createView (FIDString name) {
  return new PlugView(this);
}