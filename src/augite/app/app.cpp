#include "imgui.h"
#include <backends/imgui_impl_sdl3.h>
#include <backends/imgui_impl_sdlgpu3.h>

#include "imnodes.h"
#include "implot.h"

#include "../widget/widget_manufacturer.h"

#include "../system/imgui_system.h"
#include "../system/imnodes_system.h"
#include "../system/implot_system.h"

#include "../event/event.h"

#include "app.h"

namespace augr {

App::App() {};

bool App::DoCreate(CreateParams params) {
    bool success = Window::DoCreate(params);
    success = InitUserEvents() && success;

    REGISTER_WIDGET_FACTORY(RackWidget)
    REGISTER_WIDGET_FACTORY(ModuleWidget)
    REGISTER_WIDGET_FACTORY(ButtonWidget)
    REGISTER_WIDGET_FACTORY(CheckButtonWidget)
    REGISTER_WIDGET_FACTORY(VBoxWidget)
    REGISTER_WIDGET_FACTORY(HBoxWidget)
    REGISTER_WIDGET_FACTORY(NumEntryWidget)
    REGISTER_WIDGET_FACTORY(HSliderWidget)
    REGISTER_WIDGET_FACTORY(VSliderWidget)
    REGISTER_WIDGET_FACTORY(HBarGraphWidget)
    REGISTER_WIDGET_FACTORY(VBarGraphWidget)
    REGISTER_WIDGET_FACTORY(KnobWidget)

    return success;
}

void App::CreateContext() {
    system_container_.Add(new ImGuiSystem());
    system_container_.Add(new ImPlotSystem());
    system_container_.Add(new ImNodesSystem());
    system_container_.Create();
}

bool App::PostCreate(CreateParams params) {
    // Setup Platform/Renderer backends
    // Create GPU Device
    gpu_device_ = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV |
                                          SDL_GPU_SHADERFORMAT_DXIL |
                                          SDL_GPU_SHADERFORMAT_METALLIB,
                                      true, nullptr);
    if (gpu_device_ == nullptr) {
        printf("Error: SDL_CreateGPUDevice(): %s\n", SDL_GetError());
        return 1;
    }

    // Claim window for GPU Device
    if (!SDL_ClaimWindowForGPUDevice(gpu_device_, window_)) {
        printf("Error: SDL_ClaimWindowForGPUDevice(): %s\n", SDL_GetError());
        return 1;
    }
    SDL_SetGPUSwapchainParameters(gpu_device_, window_,
                                  SDL_GPU_SWAPCHAINCOMPOSITION_SDR,
                                  SDL_GPU_PRESENTMODE_VSYNC);

    ImGui_ImplSDL3_InitForSDLGPU(window_);
    ImGui_ImplSDLGPU3_InitInfo init_info = {};
    init_info.Device = gpu_device_;
    init_info.ColorTargetFormat =
        SDL_GetGPUSwapchainTextureFormat(gpu_device_, window_);
    init_info.MSAASamples =
        SDL_GPU_SAMPLECOUNT_1; // Only used in multi-viewports mode.
    init_info.SwapchainComposition =
        SDL_GPU_SWAPCHAINCOMPOSITION_SDR; // Only used in multi-viewports mode.
    init_info.PresentMode = SDL_GPU_PRESENTMODE_VSYNC;
    ImGui_ImplSDLGPU3_Init(&init_info);

    return X11Window::PostCreate(params);
}

void App::Render() {
    // Start the Dear ImGui frame
    ImGui_ImplSDLGPU3_NewFrame();
    ImGui_ImplSDL3_NewFrame();

    ImGui::NewFrame();
    Draw();
    ImGui::Render();

    ImDrawData *draw_data = ImGui::GetDrawData();
    const bool is_minimized =
        (draw_data->DisplaySize.x <= 0.0f || draw_data->DisplaySize.y <= 0.0f);

    SDL_GPUCommandBuffer *command_buffer = SDL_AcquireGPUCommandBuffer(
        gpu_device_); // Acquire a GPU command buffer

    SDL_GPUTexture *swapchain_texture;
    SDL_WaitAndAcquireGPUSwapchainTexture(
        command_buffer, window_, &swapchain_texture, nullptr,
        nullptr); // Acquire a swapchain texture

    if (swapchain_texture != nullptr && !is_minimized) {
        // This is mandatory: call ImGui_ImplSDLGPU3_PrepareDrawData() to upload
        // the vertex/index buffer!
        ImGui_ImplSDLGPU3_PrepareDrawData(draw_data, command_buffer);

        // Setup and start a render pass
        SDL_GPUColorTargetInfo target_info = {};
        target_info.texture = swapchain_texture;
        target_info.clear_color = SDL_FColor{clear_color.x, clear_color.y,
                                             clear_color.z, clear_color.w};
        target_info.load_op = SDL_GPU_LOADOP_CLEAR;
        target_info.store_op = SDL_GPU_STOREOP_STORE;
        target_info.mip_level = 0;
        target_info.layer_or_depth_plane = 0;
        target_info.cycle = false;
        SDL_GPURenderPass *render_pass =
            SDL_BeginGPURenderPass(command_buffer, &target_info, 1, nullptr);

        // Render ImGui
        ImGui_ImplSDLGPU3_RenderDrawData(draw_data, command_buffer,
                                         render_pass);

        SDL_EndGPURenderPass(render_pass);
    }

    // Update and Render additional Platform Windows
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }

    //  Submit the command buffer
    SDL_SubmitGPUCommandBuffer(command_buffer);
}

void App::Destroy() {
    // Cleanup
    ImGui_ImplSDLGPU3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    Window::Destroy();
}

} // namespace augr