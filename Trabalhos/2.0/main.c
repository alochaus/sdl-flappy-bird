#include<stdio.h>
#include<SDL2/SDL.h>
#include<math.h>

#define TRUE 1
#define FALSE 0

#define WINDOW_TITLE  "2.0"
#define WINDOW_WIDTH  1280
#define WINDOW_HEIGHT 720
#define WINDOW_POS_X  SDL_WINDOWPOS_UNDEFINED
#define WINDOW_POS_Y  SDL_WINDOWPOS_UNDEFINED
#define WAIT_STARTING_POINT 300

void SDL_init(void);
SDL_Window* create_window(void);
SDL_Renderer* create_renderer(SDL_Window* win);

int AUX_WaitEventTimeoutCount(SDL_Event* event, Uint32* wait) {
    Uint32 before = SDL_GetTicks();
    int is_event = SDL_WaitEventTimeout(event, *wait);
    if (is_event) {
        *wait -= (SDL_GetTicks() - before);

        if (*wait >= WAIT_STARTING_POINT) {
            *wait = 0;
        }
    } else {
        *wait = WAIT_STARTING_POINT;
    }

    return is_event;
}

void main(int argc, char** argv) {
    SDL_init();

    SDL_Window* win = create_window();
    SDL_Renderer* ren = create_renderer(win);

    SDL_Event event;
    uint quit = FALSE;

    double mid_y = WINDOW_HEIGHT / 2.0;
    double sqr_size = WINDOW_WIDTH / 7.0;

    SDL_Rect rect_keyboard = {
        WINDOW_WIDTH/2 - (sqr_size/2),
        mid_y - (sqr_size / 2),
        sqr_size,
        sqr_size
    };

    int mouse_x = 0;
    int mouse_y = 0;
    int new_mouse_x = 0;
    int new_mouse_y = 0;
    int original_state_x = 0;
    int original_state_y = 0;
    int movement_speed_by_time = 40;
    int movement_speed_by_keyboard = 20;
    int mouse_button_down = FALSE;
    int cursor_inside_rect = FALSE;
    Uint32 wait = WAIT_STARTING_POINT;

    while(!quit){
        if(AUX_WaitEventTimeoutCount(&event, &wait)) {
            switch (event.type)
            {
            case SDL_MOUSEBUTTONDOWN:
                mouse_button_down=TRUE;
                SDL_GetMouseState(&mouse_x, &mouse_y);
                original_state_x = rect_keyboard.x;
                original_state_y = rect_keyboard.y;

                cursor_inside_rect = mouse_x >= rect_keyboard.x && 
                                     mouse_y >= rect_keyboard.y &&
                                     mouse_x <= rect_keyboard.x + rect_keyboard.w &&
                                     mouse_y <= rect_keyboard.y + rect_keyboard.h;
                break;

            case SDL_MOUSEBUTTONUP:
                mouse_button_down=FALSE;
                cursor_inside_rect=FALSE;
                break;

            case SDL_MOUSEMOTION:
                if(!mouse_button_down) break;

                SDL_GetMouseState(&new_mouse_x, &new_mouse_y);
                int same_position = new_mouse_x == mouse_x && new_mouse_y == mouse_y;

                if(!cursor_inside_rect || same_position) break;

                int dx = new_mouse_x - mouse_x;
                int dy = new_mouse_y - mouse_y;

                rect_keyboard.x+=dx;
                rect_keyboard.y+=dy;

                mouse_x = new_mouse_x;
                mouse_y = new_mouse_y;
                break;

            case SDL_QUIT:
                quit=TRUE;
                break;
            }

            switch (event.key.keysym.sym) {
            case SDLK_ESCAPE:
                cursor_inside_rect = FALSE;
                mouse_button_down = FALSE;
                rect_keyboard.x = original_state_x;
                rect_keyboard.y = original_state_y;
                break;
            }
        } else {
            wait = WAIT_STARTING_POINT;
        }

        SDL_SetRenderDrawColor(ren, 0xFF, 0xFF, 0xFF, 0x00);
        SDL_RenderClear(ren);

        SDL_SetRenderDrawColor(ren, 0x00, 0xFF, 0x00, SDL_ALPHA_OPAQUE);
        SDL_RenderFillRect(ren, &rect_keyboard);

        SDL_RenderPresent(ren);
    }
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
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
