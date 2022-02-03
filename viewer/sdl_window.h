#pragma once

#include <SDL.h>
#include <SDL_vulkan.h>
#pragma comment(lib, "SDL2.lib")
#pragma comment(lib, "SDL2main.lib")

#include <string>
#include <string_view>
#include <vector>

class SDLWindow {
  SDL_Window* window_ = nullptr;
  SDL_Surface* surface_ = nullptr;
  SDLWindow(SDL_Window* window, SDL_Surface* surface) : window_(window), surface_(surface) {}
public:
  SDL_Surface* GetSurface() { return surface_; }
  SDL_Window* GetWindow() { return window_; }

  static SDLWindow* Create(const char* title, int width, int height, bool resizable = true) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
      return nullptr;
    }
    auto flags = SDL_WINDOW_SHOWN | SDL_WINDOW_VULKAN;
    if (resizable) flags |= SDL_WINDOW_RESIZABLE;
    auto* window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, flags);
    if (window == nullptr) {
      return nullptr;
    }
    auto* surface = SDL_GetWindowSurface(window);
    if (surface == nullptr) {
      return nullptr;
    }
    return new SDLWindow(window, surface);
  }

  void Update() {
    SDL_UpdateWindowSurface(window_);
  }

  void BeginFrame() {
    SDL_LockSurface(surface_);
  }

  void EndFrame() {
    SDL_UnlockSurface(surface_);
  }

  void SetTitle(const std::string& title) {
    SDL_SetWindowTitle(window_, title.c_str());
  }

  std::vector<const char*> VkGetInstanceExtensions() {
    unsigned int count = 0;
    SDL_Vulkan_GetInstanceExtensions(GetWindow(), &count, nullptr);
    std::vector<const char*> ext(count);
    SDL_Vulkan_GetInstanceExtensions(GetWindow(), &count, &ext[0]);
    return ext;
  }

  SDL_bool VkCreateSurface(VkInstance instance, VkSurfaceKHR* surface) {
    return SDL_Vulkan_CreateSurface(GetWindow(), instance, surface);
  }

  virtual ~SDLWindow() {
    SDL_FreeSurface(surface_);
    SDL_DestroyWindow(window_);
    SDL_Quit();
  }
};
