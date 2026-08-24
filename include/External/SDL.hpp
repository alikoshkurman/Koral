#pragma once 
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <string>
#include "log.hpp"
#include "utility.hpp"

namespace Koral {
  namespace SDL {
    namespace LOPR {
      inline constexpr SDL_RendererLogicalPresentation LetterBox = SDL_LOGICAL_PRESENTATION_LETTERBOX;
      inline constexpr SDL_RendererLogicalPresentation Stretch   = SDL_LOGICAL_PRESENTATION_STRETCH;
      inline constexpr SDL_RendererLogicalPresentation Normal    = SDL_LOGICAL_PRESENTATION_DISABLED;
    }
    namespace align {
      inline constexpr SDL_Point Center = { SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED };
      inline constexpr SDL_Point Auto   = { SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED };
    }
    namespace access {
      inline constexpr SDL_TextureAccess DrawOver = SDL_TEXTUREACCESS_TARGET;    
      inline constexpr SDL_TextureAccess Normal   = SDL_TEXTUREACCESS_STREAMING; 
      inline constexpr SDL_TextureAccess RareUPD  = SDL_TEXTUREACCESS_STATIC;
    }
    namespace flip {
      inline constexpr SDL_FlipMode None = SDL_FLIP_NONE;
      inline constexpr SDL_FlipMode Horiz = SDL_FLIP_HORIZONTAL;
      inline constexpr SDL_FlipMode Vert = SDL_FLIP_VERTICAL;
      inline constexpr SDL_FlipMode Both  = SDL_FLIP_HORIZONTAL_AND_VERTICAL;
    }
    
    struct Init : public NonCopy {
      Init() { if(!SDL_Init(SDL_INIT_VIDEO)) { Koral::out.error("SDL::Init: Can't initalise: ", SDL_GetError()); }} 
      ~Init() { SDL_Quit(); } 
      void Destroy() noexcept {
        SDL_Quit();
      }
    };
    
    struct Window {
      SDL_Window* window = nullptr;

      Window(const char* str, int x, int y, SDL_WindowFlags flag = 0)
        : window(SDL_CreateWindow(str, x, y, flag)) { 
        if(!window) { Koral::out.error(Koral::f("SDL::Window: Can't create window: {}", SDL_GetError())); } 
      }
      
      Window(const char* str, SDL_Point p, int x, int y, SDL_WindowFlags flag = 0)
        : window(SDL_CreateWindow(str, x, y, flag)) { 
        if(!window) { 
          Koral::out.error(Koral::f("SDL::Window: Can't create window: {}", SDL_GetError())); 
        } else { 
          setCoord(p);
        }
      }
      
      ~Window() { Destroy(); }
      
      Window(const Window&) = delete;
      Window& operator=(const Window&) = delete;

      Window(Window&& other) noexcept : window(other.window) { other.window = nullptr; } 
      Window& operator=(Window&& other) noexcept {
        if (this != &other) {
          Destroy();
          window = other.window;
          other.window = nullptr;
        }
        return *this;
      }

      void Destroy() noexcept {
        if (window) {
          SDL_DestroyWindow(window); 
          window = nullptr;
        }
      }
      
      Window& Center() noexcept {
        if(window && !SDL_SetWindowPosition(window, align::Center.x, align::Auto.y)) {
         Koral::out.error("SDL::Window: Can't set window position for Center: ",SDL_GetError());
        }
        return *this;
      }
      Window& Auto() noexcept {
        if(window && !SDL_SetWindowPosition(window, align::Auto.x, align::Auto.y)) {
         Koral::out.error("SDL::Window: Can't set window position for Center: ",SDL_GetError());
        }
        return *this;
      }
      
      Window& setCoord(int x, int y) {
        if(window && !SDL_SetWindowPosition(window, x, y)) {
         Koral::out.error("SDL::Window: Can't set window position: ", SDL_GetError());
        }
        return *this;
      }
      Window& setCoord(SDL_Point p) {
        if(window && !SDL_SetWindowPosition(window, p.x, p.y)) {
         Koral::out.error("SDL::Window: Can't set window position");
        }
        return *this;
      }
      SDL_Window* get() const { return window; }
    };
    
    struct Texture {
      SDL_Texture* texture = nullptr;
      float w = 0, h = 0;
      
      Texture(SDL_Renderer* rend, int w, int h, SDL_TextureAccess access = access::RareUPD, SDL_PixelFormat format = SDL_PIXELFORMAT_RGBA8888): texture(SDL_CreateTexture(rend, format, access, w, h )), w(static_cast<float>(w)), h(static_cast<float>(h)) {
        if(!texture) {
         Koral::out.error(Koral::f("SDL::Texture: Can't create texture: {}", SDL_GetError()));
        }
      }
      
      Texture(SDL_Renderer* rend, SDL_Surface* surf): texture(SDL_CreateTextureFromSurface(rend, surf)) {
        if (texture) {
            SDL_GetTextureSize(texture, &w, &h);
        } else {
            Koral::out.error(Koral::f("SDL::Texture: Can't create texture from surface: {}", SDL_GetError()));
        }
      }
      
      void Destroy() noexcept {
        if(texture) {
          SDL_DestroyTexture(texture);
          texture = nullptr;
          w = 0; h = 0;
        }
      }
      
      ~Texture() { Destroy(); }
      
      Texture(const Texture&) = delete;
      Texture& operator=(const Texture&) = delete;
      
      Texture(Texture&& other) noexcept : texture(other.texture), w(other.w), h(other.h) {
        other.h = 0; other.w = 0; other.texture = nullptr;
      }
      Texture& operator=(Texture&& other) noexcept {
        if (this != &other) {
          Destroy();
          texture = other.texture;
          h = other.h; w = other.w;
          other.w = 0; other.h = 0; other.texture = nullptr;
        }
        return *this;
      }

      SDL_Texture* get() const { return texture; }
      
      Texture& check() noexcept { 
        if(!texture) {
          Koral::out.error("SDL::Texture internal check failed: ", SDL_GetError());
        }
        return *this;
      }
      
      Texture& setBlendMode(SDL_BlendMode mode) noexcept { 
        if (texture) {
          SDL_SetTextureBlendMode(texture, mode);
        }
        return *this;
      }
      Texture& setAlpha(uint8_t a) {
        if(texture && !SDL_SetTextureAlphaMod(texture, a)) {
          Koral::out.error("SDL::Texture: Can't set alpha: ", SDL_GetError());
        }
        return *this;
      }
      
      
      Texture& IMG_Load(SDL_Renderer* rend, const char* str) {
        texture = IMG_LoadTexture(rend, str);
        if (!texture) {
          out.error("SDL::Texture: Can't load image: ", SDL_GetError());
        }
        return *this;
      }
    };
    
    namespace BMode { 
      inline constexpr SDL_BlendMode None = SDL_BLENDMODE_NONE;
      inline constexpr SDL_BlendMode Blend = SDL_BLENDMODE_BLEND;
      inline constexpr SDL_BlendMode Add = SDL_BLENDMODE_ADD;
      inline constexpr SDL_BlendMode Mod = SDL_BLENDMODE_MOD;
      namespace preMul {
        inline constexpr SDL_BlendMode Blend = SDL_BLENDMODE_BLEND_PREMULTIPLIED;
        inline constexpr SDL_BlendMode Add = SDL_BLENDMODE_ADD_PREMULTIPLIED;
      }
    }
    
    struct Renderer {
      SDL_Renderer* renderer = nullptr;

      template <bool CPU = false>
      Renderer(SDL_Window* window, const char* driver = nullptr) { 
        if constexpr (!CPU) { renderer = SDL_CreateRenderer(window, driver); } 
        else { renderer = SDL_CreateRenderer(window, "software"); }
        if (!renderer) {
         Koral::out.error(Koral::f("SDL::Renderer: Can't create renderer: {}", SDL_GetError()));
        }
      }
      
      ~Renderer() { Destroy(); }

      void Destroy() noexcept { if(renderer) SDL_DestroyRenderer(renderer); renderer = nullptr; }
      
      Renderer(const Renderer&) = delete;
      Renderer& operator=(const Renderer&) = delete;
      
      Renderer(Renderer&& other) noexcept : renderer(other.renderer) { other.renderer = nullptr; }
      Renderer& operator=(Renderer&& other) noexcept {
        if (this != &other) {
          Destroy();
          renderer = other.renderer;
          other.renderer = nullptr;
        }
        return *this;
      }
      
      Renderer& setTarget(Texture* tex = nullptr) {
        if (renderer && !SDL_SetRenderTarget(renderer, tex ? tex->get() : nullptr)) {
          Koral::out.error(Koral::f("SDL::Renderer: Can't set target: {}", SDL_GetError()));
        }
        return *this;
      }
      
      Renderer& setView(int x, int y, SDL_RendererLogicalPresentation mode) { 
        if(renderer && !SDL_SetRenderLogicalPresentation(renderer, x, y, mode)) { 
          Koral::out.error(Koral::f("SDL::Renderer: Can't set view: {}", SDL_GetError())); 
        } 
        return *this;
      }
      
      Renderer& setColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) { 
        if(renderer && !SDL_SetRenderDrawColor(renderer, r, g, b, a)){
          Koral::out.error(Koral::f("SDL::Renderer: Can't set Color: {}", SDL_GetError()));
        } 
        return *this; 
      }
      
      Renderer& clear() { if(renderer) SDL_RenderClear(renderer); return *this; }
      Renderer& present() { if(renderer) SDL_RenderPresent(renderer); return *this; }
      
      Renderer& drawRect(float x, float y, float w, float h) {
        if(renderer) {
          SDL_FRect rect = {x, y, w, h};
          SDL_RenderFillRect(renderer, &rect);
        }
        return *this;
      }
      
      Renderer& drawRect(const SDL_FRect& rect) noexcept {
        if(renderer) {
          SDL_RenderFillRect(renderer, &rect);
        }
        return *this;
      }
      
      Renderer& drawTexture(const Texture& tex, float x, float y) {
        if (renderer && tex.get()) {
          SDL_FRect dstRect = { x, y, static_cast<float>(tex.w), static_cast<float>(tex.h) };
          if (!SDL_RenderTexture(renderer, tex.get(), nullptr, &dstRect)) {
            Koral::out.error(Koral::f("SDL::Renderer: Can't draw texture: {}", SDL_GetError()));
          }
        }
        return *this;
      }
      Renderer& drawTexture(const Texture& tex, SDL_Point p) {
        if (renderer && tex.get()) {
          SDL_FRect dstRect = { static_cast<float>(p.x), static_cast<float>(p.y), static_cast<float>(tex.w), static_cast<float>(tex.h) };
          if (!SDL_RenderTexture(renderer, tex.get(), nullptr, &dstRect)) {
            Koral::out.error(Koral::f("SDL::Renderer: Can't draw texture: {}", SDL_GetError()));
          }
        }
        return *this;
      }
      
      Renderer& drawTexture(const Texture& tex, float x, float y, double angle, SDL_FlipMode flip = SDL_FLIP_NONE) {
        if (renderer && tex.get()) {
          SDL_FRect dstRect = { x, y, static_cast<float>(tex.w), static_cast<float>(tex.h) };
          if (!SDL_RenderTextureRotated(renderer, tex.get(), nullptr, &dstRect, angle, nullptr, flip)) {
            Koral::out.error(Koral::f("SDL::Renderer: Can't draw texture rotated: {}", SDL_GetError()));
          }
        }
        return *this;
      }
      Renderer& drawTexture(const Texture& tex, SDL_Point p, double angle, SDL_FlipMode flip = SDL_FLIP_NONE) {
        if (renderer && tex.get()) {
          SDL_FRect dstRect = { static_cast<float>(p.x), static_cast<float>(p.y), static_cast<float>(tex.w), static_cast<float>(tex.h) };
          if (!SDL_RenderTextureRotated(renderer, tex.get(), nullptr, &dstRect, angle, nullptr, flip)) {
            Koral::out.error(Koral::f("SDL::Renderer: Can't draw texture rotated: {}", SDL_GetError()));
          }
        }
        return *this;
      }

      
      Renderer& drawRect(SDL_Point p, float w, float h) {
        if(renderer) {
          SDL_FRect rect = {static_cast<float>(p.x), static_cast<float>(p.y), w, h};
          SDL_RenderFillRect(renderer, &rect);
        }
        return *this;
      }
      SDL_Renderer* get() const { return renderer; }
    };
    
    struct Surface : public NonCopy {
      SDL_Surface* surface = nullptr;

      Surface(int x, int y, SDL_PixelFormat fmt = SDL_PIXELFORMAT_RGBA8888) : surface(SDL_CreateSurface(x, y, fmt)) {
        if(!surface) {
          Koral::out.error(Koral::f("SDL::Surface: Can't create surface: {}", SDL_GetError()));
        }
      }
      ~Surface() { Destroy(); }

      Surface(Surface&& other) noexcept : surface(other.surface) { other.surface = nullptr; }
      Surface& operator=(Surface&& other) noexcept {
        if (this != &other) {
          Destroy();
          surface = other.surface;
          other.surface = nullptr;
        }
        return *this;
      }
      
      Surface& IMG(const char* str) noexcept {
        Destroy();
        surface = IMG_Load(str);
        if(!surface) {
          Koral::out.error(Koral::f("SDL::Surface: Can't load image: {}", SDL_GetError()));
        }
        return *this;
      }
      Surface& Destroy() noexcept {
        if (surface) { 
          SDL_DestroySurface(surface); 
          surface = nullptr; 
        }
        return *this;
      }
      Surface& check() noexcept {
        if(!surface) {
          Koral::out.error(Koral::f("SDL::Surface internal check failed: {}", SDL_GetError()));
        }
        return *this;
      }
      SDL_Surface* get() noexcept { return surface; }
    };
    
    struct FPS {
      uint32_t start = 0;
      uint32_t fps = 0;
      uint32_t elapsed = 0;

      FPS(int fps) : start(static_cast<uint32_t>(SDL_GetTicks())), fps(static_cast<uint32_t>(fps)) {}
      ~FPS() = default; 
      
      FPS& Delay() noexcept { 
        if (fps <= 0) return *this;
        uint32_t target_ms = 1000 / fps;
        if(target_ms > elapsed) {
          SDL_Delay(static_cast<uint32_t>(target_ms - elapsed));
        }
        return *this;
      }
    };
    
    struct Event {
      SDL_Event event;

      [[nodiscard]] bool isQuit() const noexcept {
        return event.type == SDL_EVENT_QUIT;
      }

      [[nodiscard]] bool isKeyDown() const noexcept {
        return event.type == SDL_EVENT_KEY_DOWN;
      }
      
      [[nodiscard]] bool isKeyUp() const noexcept {
        return event.type == SDL_EVENT_KEY_UP;
      }

      [[nodiscard]] bool OnEvent() noexcept { 
        return SDL_PollEvent(&event); 
      }
      
      [[nodiscard]] bool OnClickKey(SDL_Scancode scancode) const noexcept {
        return isKeyDown() && (event.key.scancode == scancode);
      }


      [[nodiscard]] const SDL_Event* get() const noexcept { return &event; }
      
      [[nodiscard]] bool OnButtonDown() noexcept {
        return event.type == SDL_EVENT_MOUSE_BUTTON_DOWN;
      }
    };
  }
}
