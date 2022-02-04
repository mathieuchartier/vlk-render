#include "sdl_window.h"
#include "vkr_engine.h"

#include <iostream>

#pragma comment(lib, "vlk-render")
vkr::Engine engine;

int main(int argc, char *argv[]) {
  std::unique_ptr<SDLWindow> window(SDLWindow::Create("Viewer", 1440, 900, true));

  engine.CreateInstance(window->VkGetInstanceExtensions()).Assert("Failed creating instance");
  VkSurfaceKHR surface = nullptr;
  if (window->VkCreateSurface(engine.Instance(), &surface) == SDL_FALSE) {
    std::cerr << "Failed creating VkCreateSurface" << std::endl;
    return 1;
  }
  engine.ChoosePhysicalDevice(
    surface,
    {VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU, VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU})
    .Assert("Failed to find physical device");
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
