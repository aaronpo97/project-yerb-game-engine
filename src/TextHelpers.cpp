#include "../includes/TextHelpers.h"

namespace TextHelpers {
  void renderLineOfText(SDL_Renderer      *renderer,
                        TTF_Font          *font,
                        const std::string &text,
                        const SDL_Color   &color,
                        const Vec2        &position) {

    // Render the text to a surface
    SDL_Surface *surface = TTF_RenderText_Solid(font, text.c_str(), color);
    if (!surface) {
      std::cerr << "Failed to create surface: " << TTF_GetError() << std::endl;
      return; // Exit if the surface creation fails
    }

    // Create a texture from the surface
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
      std::cerr << "Failed to create texture: " << SDL_GetError() << std::endl;
      SDL_FreeSurface(surface); // Free the surface if texture creation fails
      return;                   // Exit if the texture creation fails
    }

    // Set up the rectangle for the text
    SDL_Rect textRect;
    textRect.x = static_cast<int>(position.x);
    textRect.y = static_cast<int>(position.y);
    textRect.w = surface->w;
    textRect.h = surface->h;

    // Copy the texture to the renderer
    SDL_RenderCopy(renderer, texture, nullptr, &textRect);

    // Clean up
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
  }
} // namespace TextHelpers