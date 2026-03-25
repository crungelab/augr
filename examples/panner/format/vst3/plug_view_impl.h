#pragma once

#include <filesystem>

#include <augite/app/app.h>

#include "plug_view.h"

using namespace Steinberg::Panner;

class PlugViewImpl final : public PannerView
{
public:
	PlugViewImpl(PannerEditor *editor, ViewRect *size = nullptr);

	tresult PLUGIN_API attached(void *parent, FIDString type) override;
	tresult PLUGIN_API removed() override;
	tresult PLUGIN_API canResize() override
	{
		return Steinberg::kResultTrue;
	}

	tresult PLUGIN_API onSize(ViewRect *newSize) override;
	void Run();
	//
public:
	augr::App *app_ = nullptr;
	void *parent_ = nullptr;
};
