#pragma once

class UI
{
public:
  UI() {}

  virtual void declare(float* zone, const char* key, const char* value) = 0;

  virtual void addButton(const char* label, float* zone) = 0;
  virtual void addToggleButton(const char* label, float* zone) = 0;
  virtual void addCheckButton(const char* label, float* zone) = 0;
  virtual void addVerticalSlider(const char* label, float* zone, float init, float min, float max, float step) = 0;
  virtual void addHorizontalSlider(const char* label, float* zone, float init, float min, float max, float step) = 0;
  virtual void addNumEntry(const char* label, float* zone, float init, float min, float max, float step) = 0;

  // -- passive widgets
  virtual void addNumDisplay(const char* label, float* zone, int precision) = 0;
  virtual void addTextDisplay(const char* label, float* zone, char* names[], float min, float max) = 0;
  virtual void addHorizontalBargraph(const char* label, float* zone, float min, float max) = 0;
  virtual void addVerticalBargraph(const char* label, float* zone, float min, float max) = 0;

  // -- frames and labels
  virtual void openFrameBox(const char* label) = 0;
  virtual void openTabBox(const char* label) = 0;
  virtual void openHorizontalBox(const char* label) = 0;
  virtual void openVerticalBox(const char* label) = 0;
  virtual void closeBox() = 0;

  virtual void show() {}
  virtual void run() {}

  void stop() {}
  bool stopped() { return true; }
};