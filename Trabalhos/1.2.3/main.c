// - Faça uma animação em ciclo com algum padrão (ex, andar em círculo).
// - Use somente as funções vistas nos slides anteriores.
// - Use a velocidade de 10 pixels/segundo.
#include<stdio.h>
#include<SDL2/SDL.h>
#include<math.h>

#define TRUE 1
#define FALSE 0

#define WINDOW_TITLE  "1.2.3"
#define WINDOW_WIDTH  1280
#define WINDOW_HEIGHT 720
#define WINDOW_POS_X  SDL_WINDOWPOS_UNDEFINED
#define WINDOW_POS_Y  SDL_WINDOWPOS_UNDEFINED

void SDL_init(void);
SDL_Window* create_window(void);
SDL_Renderer* create_renderer(SDL_Window* win);

void make_letters_go_away(SDL_Renderer* ren, SDL_Rect** letters, int* speed) {
    if(letters[1]->y >= WINDOW_HEIGHT) {
        letters[1]->y = WINDOW_HEIGHT;
        *speed = -(*speed);
    }
    else if(letters[1]->y <= ((WINDOW_HEIGHT/2)-(WINDOW_WIDTH/15.0))) {
        letters[1]->y = ((WINDOW_HEIGHT/2)-(WINDOW_WIDTH/15.0));
        *speed = -(*speed);
    }

    for(int i = 0; i < 6; i++) {
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

void main(int argc, char** argv) {
    SDL_init();

    SDL_Window* win = create_window();
    SDL_Renderer* ren = create_renderer(win);

    SDL_Event event;
    uint quit = FALSE;

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

    SDL_Rect* letters[6] = { &g, &o, &o_2, &g_2, &l, &e };

    int opacity = 0x00;
    int introduction = TRUE;
    int speed = 30;
    int letters_are_gone = FALSE;
    while (!quit){
        while (SDL_PollEvent(&event)){
            if(event.type == SDL_QUIT)
                quit = TRUE;
        }

        if(introduction) {
            if(opacity+speed > 0xFF) {
                speed *= -1;
                opacity = 0xFF;
                introduction = FALSE;
                speed = 20;
                SDL_Delay(2000);
            }
            else 
                opacity += speed;
        } else {
            make_letters_go_away(ren, letters, &speed);
        }

        SDL_SetRenderDrawColor(ren, 0xFF, 0xFF, 0xFF, 0x00);
        SDL_RenderClear(ren);

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
        SDL_Delay(100);
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
