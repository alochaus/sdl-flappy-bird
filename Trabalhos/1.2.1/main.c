// Faça um desenho qualquer com linhas, pontos e retângulos, usando cores diversas.
#include<stdio.h>
#include<SDL2/SDL.h>

#define TRUE 1
#define FALSE 0

#define WINDOW_TITLE  "1.2.1"
#define WINDOW_WIDTH  500
#define WINDOW_HEIGHT 500
#define WINDOW_POS_X  SDL_WINDOWPOS_UNDEFINED
#define WINDOW_POS_Y  SDL_WINDOWPOS_UNDEFINED

void SDL_init();
SDL_Window* create_window();
SDL_Renderer* create_renderer(SDL_Window* win);

void main(int argc, char** argv) {
    SDL_init();

    SDL_Window* win = create_window();
    SDL_Renderer* ren = create_renderer(win);

    SDL_Event e;
    uint quit = FALSE;
    SDL_Rect rect = { WINDOW_WIDTH/4, WINDOW_HEIGHT/4, WINDOW_WIDTH*0.05, WINDOW_HEIGHT*0.05 };

    while (!quit){
        while (SDL_PollEvent(&e)){
            if (e.type == SDL_QUIT){
                quit = TRUE;
            }
        }

        SDL_SetRenderDrawColor(ren, 0xFF, 0xFF, 0xFF, 0xFF);
        SDL_RenderClear(ren);

        SDL_SetRenderDrawColor(ren, 0x00, 0xFF, 0x00, 0xFF);
        SDL_RenderFillRect(ren, &rect);

        SDL_SetRenderDrawColor(ren, 0xFF, 0x00, 0x00, 0xFF);
        SDL_RenderDrawLine(ren, WINDOW_WIDTH*0.7, WINDOW_HEIGHT*0.2, WINDOW_WIDTH, WINDOW_HEIGHT);
        
        SDL_SetRenderDrawColor(ren, 0x00, 0x00, 0xFF, 0xFF);
        for(int x=0; x<100; x++) {
            for(int y=0; y<100; y++) {
                SDL_RenderDrawPoint(ren, x, y);
            }
        }

        SDL_RenderPresent(ren);
    }

    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
}

void SDL_init() {
    if(SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        printf("Erro de inicialização do SDL.\nSDL_Error: %s\n", SDL_GetError() );
        return;
    }
}

SDL_Window* create_window() {
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

    return ren;
}