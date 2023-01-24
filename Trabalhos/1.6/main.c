#include<stdio.h>
#include<SDL2/SDL.h>
#include<math.h>
#include<stdio.h>

#define TRUE 1
#define FALSE 0

#define WINDOW_TITLE  "1.5.1"
#define WINDOW_WIDTH  1280
#define WINDOW_HEIGHT 720
#define WINDOW_POS_X  SDL_WINDOWPOS_UNDEFINED
#define WINDOW_POS_Y  SDL_WINDOWPOS_UNDEFINED
#define WAIT_STARTING_POINT 300


int AUX_WaitEventTimeoutCount(SDL_Event *event, Uint32 *wait)
{
    Uint32 before = SDL_GetTicks();
    int isEvent = SDL_WaitEventTimeout(event, *wait);
    if (isEvent)
    {
        (*wait) -= (SDL_GetTicks() - before);

        if (*wait >= WAIT_STARTING_POINT)
            *wait = 0;
    }
    else {
        *wait = WAIT_STARTING_POINT;
    }

    return isEvent;
}

void check_colision(SDL_Rect** rects, SDL_Point* p1, SDL_Point* p2, int* time_speed, int* kb_speed, int* mouse_movement) {
    static int winner = -1;
    static char winners_name[6] = {'\0'};
    for (int i = 0; i < 3; i++)
    {
        if(rects[i]->w + rects[i]->x >= p1->x) {
            switch(i) {
                case 0:
                    *time_speed = 0;
                    break;
                case 1:
                    *kb_speed = 0;
                    break;
                case 2:
                    *mouse_movement = FALSE;
                    break;
            }

            if (winner >= 0) continue;
            winner = i;
            switch(winner) {
                case 0:
                    strcpy(winners_name, "Red");
                    break;
                case 1:
                    strcpy(winners_name, "Blue");
                    break;
                case 2:
                    strcpy(winners_name, "Green");
                    break;
            }
            printf("%s wins!\n", winners_name);
        }
    }
}

void SDL_init(void);
SDL_Window* create_window(void);
SDL_Renderer* create_renderer(SDL_Window* win);


void main(int argc, char** argv) {
    SDL_init();

    SDL_Window* win = create_window();
    SDL_Renderer* ren = create_renderer(win);

    SDL_Event event;
    int quit = FALSE;

    double sqr_size = WINDOW_HEIGHT / 7.0;

    SDL_Rect rect_time = {
        0,
        sqr_size,
        sqr_size,
        sqr_size
    };
    SDL_Rect rect_keyboard = {
        0,
        sqr_size * 3,
        sqr_size,
        sqr_size
    };
    SDL_Rect rect_mouse = {
        0,
        sqr_size * 5,
        sqr_size,
        sqr_size
    };

    int mouse_x = 0;
    int mouse_y = 0;
    int movement_speed_by_time = 40;
    int movement_speed_by_keyboard = 20;
    int is_event = 0;
    int wait = 300;
    int limit = 0;

    int mouse_movement = TRUE;
    
    SDL_Rect** rects[3] = {&rect_time, &rect_keyboard, &rect_mouse};
    SDL_Point p1 = { WINDOW_WIDTH-sqr_size, WINDOW_HEIGHT };
    SDL_Point p2 = { WINDOW_WIDTH-sqr_size, 0 };

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
            case SDLK_a:
                if (rect_keyboard.x > 0)
                    rect_keyboard.x -= movement_speed_by_keyboard;
                else
                    rect_keyboard.x = 0;
                break;
            case SDLK_d:
                limit = WINDOW_WIDTH - rect_keyboard.w;
                if (rect_keyboard.x < limit)
                    rect_keyboard.x += movement_speed_by_keyboard;
                else
                    rect_keyboard.x = limit;
                break;
            case SDLK_ESCAPE:
                quit = TRUE;
                break;
            }
        }
        else {
            if((rect_time.y < 0) || (rect_time.y + rect_time.h > WINDOW_HEIGHT)) {
                movement_speed_by_time *= -1;
                rect_time.x += movement_speed_by_time;
            }

            rect_time.x += movement_speed_by_time;
        }

        SDL_SetRenderDrawColor(ren, 0xFF, 0x00, 0x00, SDL_ALPHA_OPAQUE);
        SDL_RenderFillRect(ren, &rect_time);
        SDL_SetRenderDrawColor(ren, 0x00, 0x00, 0xFF, SDL_ALPHA_OPAQUE);
        SDL_RenderFillRect(ren, &rect_keyboard);
        SDL_SetRenderDrawColor(ren, 0x00, 0xFF, 0x00, SDL_ALPHA_OPAQUE);

        if(mouse_movement) {
            SDL_GetMouseState(&mouse_x, &mouse_y);
            rect_mouse.x = mouse_x - sqr_size;
        }
        SDL_RenderFillRect(ren, &rect_mouse);

        SDL_SetRenderDrawColor(ren, 0xFF, 0x00, 0x00, SDL_ALPHA_OPAQUE);
        SDL_RenderDrawLine(ren, p1.x, p1.y, p2.x, p2.y);

        check_colision(rects, &p1, &p2, &movement_speed_by_time, &movement_speed_by_keyboard, &mouse_movement);

        SDL_RenderPresent(ren);
        
        if(movement_speed_by_keyboard == 0 &&
           movement_speed_by_time == 0 &&
           mouse_movement == FALSE)
            quit = TRUE;
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