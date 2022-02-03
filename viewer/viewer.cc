#include "sdl_window.h"

#include <iostream>

int main(int argc, char *argv[]) {
  std::unique_ptr<SDLWindow> window(SDLWindow::Create("Viewer", 1440, 900, true));
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
