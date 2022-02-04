#include "sdl_window.h"
#include "vkr_engine.h"

#include <iostream>

#pragma comment(lib, "vlk-render")
vkr::Engine engine;

int main(int argc, char *argv[]) {
  std::unique_ptr<SDLWindow> window(SDLWindow::Create("Viewer", 1440, 900, true));

  engine.CreateInstance(window->VkGetInstanceExtensions()).Assert("Failed creating instance");
  bool first_device = false;
  if (!engine.phys_devices_.Choose([&](
    const VkPhysicalDeviceProperties& properties,
    const VkPhysicalDeviceFeatures& features) -> int {
      if (features.geometryShader) {
        if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
          return 1000;
        } else if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU) {
          return 100;
        } else if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_CPU) {
          return 10;
        }
      }
      auto ret = first_device ? 1 : -1;
      first_device = false;
      return ret;
    })) {
    std::cerr << "Failed to choose best device" << std::endl;
    return 1;
  }

  SDL_Event e;
  bool quit = false;
  while (!quit) {
    while (SDL_PollEvent(&e) != 0) {
      switch (e.type) {
        case SDL_QUIT:
          quit = true;
          break;
        case SDL_KEYDOWN:
          if (e.key.keysym.sym == SDLK_q) quit = true;
          break;
      }
    }
  }
  return 0;
}
