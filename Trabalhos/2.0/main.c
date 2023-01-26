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
#define mid_y (WINDOW_HEIGHT/2.0)
#define sqr_size (WINDOW_HEIGHT/7.0)

int mouse_x = 0;
int mouse_y = 0;
int new_mouse_x = 0;
int new_mouse_y = 0;
int original_state_x = 0;
int original_state_y = 0;
int movement_speed_by_keyboard = 20;
SDL_Rect rect_keyboard = {
    WINDOW_WIDTH/2 - (sqr_size/2),
    mid_y - (sqr_size / 2),
    sqr_size,
    sqr_size
};

typedef enum {estado_padrao, estado_clicado, estado_arrastado, estado_cancelado} estado; 

void maquina_de_estados(estado* estado_atual, SDL_Event* event) {
    switch(*estado_atual) {
        case estado_padrao: padrao(estado_atual, event); break;
        case estado_clicado: clicado(estado_atual, event); break;
        case estado_arrastado: arrastado(estado_atual, event); break;
        case estado_cancelado: cancelado(estado_atual); break;
    }
}

void padrao(estado* estado_atual, SDL_Event* event) {
    switch (event->type)
    {
    case SDL_MOUSEBUTTONDOWN:
        SDL_GetMouseState(&mouse_x, &mouse_y);
        original_state_x = rect_keyboard.x;
        original_state_y = rect_keyboard.y;

        int cursor_inside_rect = mouse_x >= rect_keyboard.x && 
                                 mouse_y >= rect_keyboard.y &&
                                 mouse_x <= rect_keyboard.x + rect_keyboard.w &&
                                 mouse_y <= rect_keyboard.y + rect_keyboard.h;
        
        if(cursor_inside_rect)
            *estado_atual = estado_clicado;
        break;
    }
}

void clicado(estado* estado_atual, SDL_Event* event) {
    switch (event->type)
    {
        case SDL_MOUSEBUTTONUP:
            printf("Clicado\n");
            *estado_atual = estado_padrao;
            break;

        case SDL_MOUSEMOTION:
            *estado_atual = estado_arrastado;
            break;
    }
}

void arrastado(estado* estado_atual, SDL_Event* event) {
    SDL_GetMouseState(&new_mouse_x, &new_mouse_y);

    int dx = new_mouse_x - mouse_x;
    int dy = new_mouse_y - mouse_y;

    rect_keyboard.x+=dx;
    rect_keyboard.y+=dy;

    mouse_x = new_mouse_x;
    mouse_y = new_mouse_y;

    if(event->type == SDL_MOUSEBUTTONUP) {
        printf("Arrastado\n");
        *estado_atual = estado_padrao;
    }
    else if(event->key.keysym.sym == SDLK_ESCAPE) {
        printf("Cancelado\n");
        *estado_atual = estado_cancelado;
    }
}

void cancelado(estado* estado_atual) {
    rect_keyboard.x = original_state_x;
    rect_keyboard.y = original_state_y;
    *estado_atual = estado_padrao;
}

void SDL_init(void);
SDL_Window* create_window(void);
SDL_Renderer* create_renderer(SDL_Window* win);

int AUX_WaitEventTimeoutCount(SDL_Event* event, Uint32* wait) {
    Uint32 before = SDL_GetTicks();
    int is_event = SDL_WaitEventTimeout(event, *wait);
    if (is_event) {
        (*wait) -= (SDL_GetTicks() - before);

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
    estado estado_atual = estado_padrao;


    Uint32 wait = WAIT_STARTING_POINT;

    while(!quit){
        if(AUX_WaitEventTimeoutCount(&event, &wait)) {
            if(event.type == SDL_QUIT)
                quit=TRUE;
            else
                maquina_de_estados(&estado_atual, &event);
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
