#include<stdio.h>
#include<SDL2/SDL.h>
#include<math.h>

#define TRUE 1
#define FALSE 0

#define WINDOW_TITLE  "1.5.3"
#define WINDOW_WIDTH  1280
#define WINDOW_HEIGHT 720
#define WINDOW_POS_X  SDL_WINDOWPOS_UNDEFINED
#define WINDOW_POS_Y  SDL_WINDOWPOS_UNDEFINED

void SDL_init(void);
SDL_Window* create_window(void);
SDL_Renderer* create_renderer(SDL_Window* win);

void make_letters_go_away(SDL_Renderer* ren, SDL_Rect** letters, int* speed) {
    if(letters[1]->y >= WINDOW_HEIGHT) {
        letters[1]->y = WINDOW_HEIGHT - 1;
        *speed = -(*speed);
    }
    else if(letters[1]->y <= ((WINDOW_HEIGHT/2)-(WINDOW_WIDTH/15.0))) {
        letters[1]->y = ((WINDOW_HEIGHT/2)-(WINDOW_WIDTH/15.0))-1;
        *speed = -(*speed);
    }

    for(int i = 0; i < 4; i++) {
        if(i == 0)
            letters[i]->x -= *speed;
        else if(i == 5)
            letters[i]->x += *speed;
        else if(i%2==0)
            letters[i]->y -= *speed;
        else if(i%3==0)
            letters[i]->y += *speed;
        else
            letters[i]->y += *speed;    
    }
}

int main(int argc, char** argv) {
    SDL_init();

    SDL_Window* win = create_window();
    SDL_Renderer* ren = create_renderer(win);

    SDL_Event event;
    int quit = FALSE;

    double mid_y = WINDOW_HEIGHT / 2.0;
    double sqr_size = WINDOW_WIDTH / 15.0;

    SDL_Rect g = {
        2*sqr_size,
        mid_y - sqr_size,
        sqr_size,
        2*sqr_size
    };

    SDL_Rect o = {
        4*sqr_size,
        mid_y - sqr_size,
        sqr_size,
        sqr_size
    };

    SDL_Rect o_2 = {
        6*sqr_size,
        mid_y - sqr_size,
        sqr_size,
        sqr_size
    };

    SDL_Rect g_2 = {
        8*sqr_size,
        mid_y - sqr_size,
        sqr_size,
        2*sqr_size
    };

    SDL_Rect l = {
        10*sqr_size,
        mid_y - 2*sqr_size,
        sqr_size,
        2*sqr_size
    };

    SDL_Rect e = {
        12*sqr_size,
        mid_y - sqr_size,
        sqr_size,
        sqr_size
    };

    SDL_Rect* letters[4] = { &g, &g_2, &l, &e };

    int opacity = 0x00;
    int introduction = TRUE;
    int mouse_x = 0;
    int mouse_y = 0;
    int is_event = FALSE;
    int movement_speed_by_keyboard = 20;
    int movement_speed_by_time = 20;
    int fading_speed = 5;
    Uint32 wait = 500;
    int limit = 0;
    Uint32 before = 0;

    while (!quit){
        SDL_SetRenderDrawColor(ren, 0xFF, 0xFF, 0xFF, 0x00);
        SDL_RenderDrawPoint(ren, 0,0);
        SDL_RenderClear(ren);

        if(introduction) {
            if(opacity+fading_speed > 0xFF) {
                opacity = 0xFF;
                introduction = FALSE;
            }
            else {
                SDL_Delay(50);
                opacity += fading_speed;
                printf("opacity: %d\n", opacity);
            }
        } else {
            before = SDL_GetTicks();
            if(SDL_WaitEventTimeout(&event, wait)) {
                wait -= (SDL_GetTicks() - before);
                if(wait < 0) {
                    wait = 0;
                }

                switch (event.type)
                {
                    case SDL_QUIT:
                        quit=TRUE;
                        break;
                    case SDL_MOUSEMOTION:
                        SDL_GetMouseState(&mouse_x, &mouse_y);
                        o.x = mouse_x - (sqr_size / 2);
                        o.y = mouse_y - (sqr_size / 2);
                        break;
                    case SDL_KEYDOWN:
                        switch (event.key.keysym.sym) {
                            case SDLK_w:
                                if (o_2.y > 0)
                                    o_2.y -= movement_speed_by_keyboard;
                                else
                                    o_2.y = 0;
                                break;
                            case SDLK_a:
                                if (o_2.x > 0)
                                    o_2.x -= movement_speed_by_keyboard;
                                else
                                    o_2.y = 0;
                                break;
                            case SDLK_d:
                                limit = WINDOW_WIDTH - o_2.w;
                                if (o_2.x < limit)
                                    o_2.x += movement_speed_by_keyboard;
                                else
                                    o_2.x = limit;
                                break;
                            case SDLK_s:
                                limit = WINDOW_HEIGHT - o_2.h;
                                if (o_2.y < limit)
                                    o_2.y += movement_speed_by_keyboard;
                                else
                                    o_2.y = limit;
                                break;
                            case SDLK_ESCAPE:
                                quit = TRUE;
                                break;
                        }
                    break;
                }
            }
            else {
                wait = 500;
                make_letters_go_away(ren, letters, &movement_speed_by_time);
            }
        }


        SDL_SetRenderDrawColor(ren, 0x00, 0x86, 0xF8, opacity);
        SDL_RenderFillRect(ren, &g);

        SDL_SetRenderDrawColor(ren, 0xFF, 0x41, 0x31, opacity);
        SDL_RenderFillRect(ren, &o);

        SDL_SetRenderDrawColor(ren, 0xFF, 0xBD, 0x00, opacity);
        SDL_RenderFillRect(ren, &o_2);
        
        SDL_SetRenderDrawColor(ren, 0x00, 0x86, 0xF8, opacity);
        SDL_RenderFillRect(ren, &g_2);

        SDL_SetRenderDrawColor(ren, 0x00, 0xAA, 0x4B, opacity);
        SDL_RenderFillRect(ren, &l);

        SDL_SetRenderDrawColor(ren, 0xFF, 0x41, 0x31, opacity);
        SDL_RenderFillRect(ren, &e);


        SDL_RenderPresent(ren);
    }
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();

    return 0;
}

void SDL_init(void) {
    if(SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        printf("Erro de inicialização do SDL.\nSDL_Error: %s\n", SDL_GetError());
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