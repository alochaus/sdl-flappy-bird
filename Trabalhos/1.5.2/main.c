#include<stdio.h>
#include<SDL2/SDL.h>
#include<math.h>

#define TRUE 1
#define FALSE 0

#define WINDOW_TITLE  "1.5.2"
#define WINDOW_WIDTH  1280
#define WINDOW_HEIGHT 720
#define WINDOW_POS_X  SDL_WINDOWPOS_UNDEFINED
#define WINDOW_POS_Y  SDL_WINDOWPOS_UNDEFINED
#define WAIT_STARTING_POINT 500

int AUX_WaitEventTimeoutCount(SDL_Event* evt, Uint32* ms) {
    Uint32 t1 = SDL_GetTicks();
    int is_event = SDL_WaitEventTimeout(evt, *ms);
    if (is_event) {
        Uint32 tf = SDL_GetTicks();
        *ms -= (tf - t1);
        if(*ms < 0) {
            *ms = 0;
        }
    } else {
        *ms = WAIT_STARTING_POINT;
    }

    return is_event;
}

void SDL_init(void);
SDL_Window* create_window(void);
SDL_Renderer* create_renderer(SDL_Window* win);

int main(int argc, char** argv) {
    SDL_init();

    SDL_Window* win = create_window();
    SDL_Renderer* ren = create_renderer(win);

    SDL_Event event;
    uint quit = FALSE;

    double mid_y = WINDOW_HEIGHT / 2.0;
    double sqr_size = WINDOW_WIDTH / 7.0;

    SDL_Rect rect_time = {
        1*sqr_size,
        mid_y - sqr_size,
        sqr_size,
        sqr_size
    };
    SDL_Rect rect_keyboard = {
        3*sqr_size,
        mid_y - sqr_size,
        sqr_size,
        sqr_size
    };
    SDL_Rect rect_mouse = {
        5*sqr_size,
        mid_y - sqr_size,
        sqr_size,
        sqr_size
    };

    int mouse_x = 0;
    int mouse_y = 0;
    int movement_speed_by_time = 40;
    int movement_speed_by_keyboard = 20;
    int is_event = 0;
    int wait = WAIT_STARTING_POINT;
    int limit = 0;

    while (!quit){
        SDL_SetRenderDrawColor(ren, 0xFF, 0xFF, 0xFF, 0x00);
        SDL_RenderClear(ren);

        if(AUX_WaitEventTimeoutCount(&event, &wait)) {
            switch (event.type)
            {
            case SDL_QUIT:
                quit=TRUE;
                break;
            }

            switch (event.key.keysym.sym) {
            case SDLK_w:
                if (rect_keyboard.y > 0)
                    rect_keyboard.y -= movement_speed_by_keyboard;
                else
                    rect_keyboard.y = 0;
                break;
            case SDLK_a:
                if (rect_keyboard.x > 0)
                    rect_keyboard.x -= movement_speed_by_keyboard;
                else
                    rect_keyboard.y = 0;
                break;
            case SDLK_d:
                limit = WINDOW_WIDTH - rect_keyboard.w;
                if (rect_keyboard.x < limit)
                    rect_keyboard.x += movement_speed_by_keyboard;
                else
                    rect_keyboard.x = limit;
                break;
            case SDLK_s:
                limit = WINDOW_HEIGHT - rect_keyboard.h;
                if (rect_keyboard.y < limit)
                    rect_keyboard.y += movement_speed_by_keyboard;
                else
                    rect_keyboard.y = limit;
                break;
            case SDLK_ESCAPE:
                quit = TRUE;
                break;
            }
        }
        else {
            if((rect_time.y < 0) || (rect_time.y + rect_time.h > WINDOW_HEIGHT)) {
                movement_speed_by_time *= -1;
                rect_time.y += movement_speed_by_time;
            }

            rect_time.y += movement_speed_by_time;
        }

        SDL_SetRenderDrawColor(ren, 0xFF, 0x00, 0x00, SDL_ALPHA_OPAQUE);
        SDL_RenderFillRect(ren, &rect_time);
        SDL_SetRenderDrawColor(ren, 0x00, 0x00, 0xFF, SDL_ALPHA_OPAQUE);
        SDL_RenderFillRect(ren, &rect_keyboard);
        SDL_SetRenderDrawColor(ren, 0x00, 0xFF, 0x00, SDL_ALPHA_OPAQUE);

        SDL_GetMouseState(&mouse_x, &mouse_y);
        rect_mouse.x = mouse_x - (sqr_size / 2);
        rect_mouse.y = mouse_y - (sqr_size / 2);
        SDL_RenderFillRect(ren, &rect_mouse);

        SDL_RenderPresent(ren);
    }
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();

    return 0;
}

void SDL_init(void) {
    if(SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        printf("Erro de inicialização do SDL.\nSDL_Error: %s\n", SDL_GetError() );
        return;
    }
}

SDL_Window* create_window(void) {
    SDL_Window* win = SDL_CreateWindow(WINDOW_TITLE, WINDOW_POS_X, WINDOW_POS_Y, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);

    if(win==NULL) {
        printf("Janela não pôde ser criada.\nSDL_Error: %s\n", SDL_GetError());
    }

    return win;
}

SDL_Renderer* create_renderer(SDL_Window* win) {
    SDL_Renderer* ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED); 

    if(win==NULL) {
        printf("Janela não pôde ser criada.\nSDL_Error: %s\n", SDL_GetError());
    }

    SDL_SetRenderDrawBlendMode(ren, SDL_BLENDMODE_BLEND);

    return ren;
}
