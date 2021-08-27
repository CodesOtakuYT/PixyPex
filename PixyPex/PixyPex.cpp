#include <SDL.h>
#include <SDL_image.h>
#include <iostream>

int main(int argc, char* argv[])
{
    SDL_Window* window;
    SDL_Renderer* renderer;

    SDL_Point window_size;
    window_size.x = 1280;
    window_size.y = 720;

    bool is_main_loop_running = true;

    bool is_mouse_left_button_down = false;
    bool is_mouse_right_button_down = false;
    int mouse_x = 0;
    int mouse_y = 0;

    bool is_rctrl_key_down = false;
    bool is_lctrl_key_down = false;

    SDL_Point canvas_size;
    canvas_size.x = 64;
    canvas_size.y = 64;

    float canvas_zoom = 1;
    float canvas_zoom_speed = 1;
    float precise_canvas_zoom_factor = 0.1;

    //Internal
    float canvas_zoom_delta;

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
        std::cout << SDL_GetError();
    
    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags))
    {
        std::cout << IMG_GetError();
    }

    window = SDL_CreateWindow("PixyPex", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, window_size.x, window_size.y, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        std::cout << SDL_GetError();
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == NULL) {
        std::cout << SDL_GetError();
    }
    
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    SDL_Texture* canvas_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, canvas_size.x, canvas_size.y);
    if (canvas_texture == NULL) {
        std::cout << SDL_GetError();
    }

    SDL_Rect canvas_visible_rect;
    canvas_visible_rect.x = 0;
    canvas_visible_rect.y = 0;
    canvas_visible_rect.w = canvas_size.x;
    canvas_visible_rect.h = canvas_size.y;

    SDL_Rect canvas_frame_rect;
    canvas_frame_rect.x = window_size.x / 2 - canvas_size.x / 2;
    canvas_frame_rect.y = window_size.y / 2 - canvas_size.y / 2;
    canvas_frame_rect.w =  canvas_size.x*canvas_zoom;
    canvas_frame_rect.h =  canvas_size.y*canvas_zoom;

    SDL_Rect canvas_rect;
    canvas_rect.x = 0;
    canvas_rect.y = 0;
    canvas_rect.w = canvas_size.x;
    canvas_rect.h = canvas_size.y;

    SDL_SetRenderTarget(renderer, canvas_texture);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &canvas_rect);

    SDL_Event event;

    canvas_zoom = (window_size.y-8) / canvas_rect.h;

    while (is_main_loop_running) {
        //Input loop
        while (SDL_PollEvent(&event)) {
            switch (event.type)
            {
                case SDL_QUIT:
                    is_main_loop_running = false;
                    break;
                case SDL_MOUSEMOTION:
                    mouse_x = event.motion.x;
                    mouse_y = event.motion.y;
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    switch (event.button.button)
                    {
                        case SDL_BUTTON_LEFT:
                            is_mouse_left_button_down = true;
                            break;
                        case SDL_BUTTON_RIGHT:
                            is_mouse_right_button_down = true;
                            break;
                        default:
                            break;
                    }
                    break;
                case SDL_MOUSEBUTTONUP:
                    switch (event.button.button)
                    {
                        case SDL_BUTTON_LEFT:
                            is_mouse_left_button_down = false;
                            break;
                        case SDL_BUTTON_RIGHT:
                            is_mouse_right_button_down = false;
                            break;
                        default:
                            break;
                    }
                    break;
                case SDL_KEYDOWN:
                    switch(event.key.keysym.sym){
                        case SDLK_RCTRL:
                            is_rctrl_key_down = true;
                            break;
                        case SDLK_LCTRL:
                            is_lctrl_key_down = true;
                            break;
                        default:
                            std::cout << event.key.keysym.sym << std::endl;
                            break;
                    }
                    break;
                case SDL_KEYUP:
                    switch (event.key.keysym.sym) {
                        case SDLK_RCTRL:
                            is_rctrl_key_down = false;
                            break;
                        case SDLK_LCTRL:
                            is_lctrl_key_down = false;
                            break;
                        default:
                            break;
                    }
                    break;
                case SDL_MOUSEWHEEL:
                    std::cout << is_lctrl_key_down << std::endl;
                    canvas_zoom_delta = event.wheel.y * canvas_zoom_speed;

                    if (is_lctrl_key_down || is_rctrl_key_down) {
                        canvas_zoom_delta *= canvas_zoom_delta;
                    }

                    if (event.wheel.direction == SDL_MOUSEWHEEL_NORMAL) {
                        canvas_zoom += canvas_zoom_delta;
                    }
                    else {
                        canvas_zoom -= canvas_zoom_delta;
                    }
                    break;
                default:
                    break;
            }
        }
        //Update
        canvas_frame_rect.w = canvas_size.x * canvas_zoom;
        canvas_frame_rect.h = canvas_size.y * canvas_zoom;

        canvas_frame_rect.x = window_size.x / 2 - canvas_frame_rect.w / 2;
        canvas_frame_rect.y = window_size.y / 2 - canvas_frame_rect.h / 2;
        //Rendering
        SDL_SetRenderTarget(renderer, NULL);
        SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
        SDL_RenderClear(renderer);
        //DRAW START
        if(is_mouse_left_button_down){
            SDL_SetRenderTarget(renderer, canvas_texture);
            SDL_RenderDrawPoint(renderer, (mouse_x - canvas_frame_rect.x) / canvas_zoom, (mouse_y - canvas_frame_rect.y) / canvas_zoom);
            SDL_SetRenderTarget(renderer, NULL);
        }
        SDL_RenderCopy(renderer, canvas_texture, &canvas_visible_rect, &canvas_frame_rect);
        //DRAW END
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    return 0;
}