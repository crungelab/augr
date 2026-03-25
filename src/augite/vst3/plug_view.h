#pragma once

#include <thread>
#include <atomic>
#include <optional>

#include "public.sdk/source/vst/vsteditcontroller.h"

#include <augr/core/rack/rack.h>

using namespace Steinberg;

class PlugEditor;

class PlugView : public Steinberg::Vst::EditorView
{
public:
	PlugView(PlugEditor *editor, ViewRect *size = nullptr);
	tresult PLUGIN_API isPlatformTypeSupported(FIDString type) override;
	// Accessors
	PlugEditor &editor() { return *((PlugEditor *)controller.get()); }
	//
	// Data members
	std::thread renderThread_;
	std::atomic<bool> running_{false};
	int targetFps_ = 60; // tweak as you like
};
