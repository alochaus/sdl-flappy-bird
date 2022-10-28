// Faça um desenho qualquer usando a biblioteca SDL2_gfx, com figuras mais complexas.
#include<stdio.h>
#include<SDL2/SDL.h>
#include<SDL2/SDL2_gfxPrimitives.h>

#define TRUE 1
#define FALSE 0

#define WINDOW_TITLE  "1.2.2"
#define WINDOW_WIDTH  500
#define WINDOW_HEIGHT 500
#define WINDOW_POS_X  SDL_WINDOWPOS_UNDEFINED
#define WINDOW_POS_Y  SDL_WINDOWPOS_UNDEFINED

void SDL_init(void);
SDL_Window* create_window(void);
SDL_Renderer* create_renderer(SDL_Window* win);

void main(int argc, char** argv) {
    SDL_init();

    SDL_Window* win = create_window();
    SDL_Renderer* ren = create_renderer(win);

    SDL_Event e;
    uint quit = FALSE;

    while (!quit){
        while (SDL_PollEvent(&e)){
            if (e.type == SDL_QUIT){
                quit = TRUE;
            }
        }

        SDL_SetRenderDrawColor(ren, 0xFF, 0xFF, 0xFF, 0xFF);
        SDL_RenderClear(ren);

        filledPieColor(ren, 100, 300, 200, 30, 270, 0xcd00cdFF);
        aaellipseColor(ren, WINDOW_WIDTH*0.5, WINDOW_HEIGHT*0.2, WINDOW_WIDTH*0.65, WINDOW_HEIGHT*0.65, 0xFF0000FF);
        thickLineColor(ren, WINDOW_WIDTH*0.6, -30, -30, WINDOW_HEIGHT*0.7, 30, 0xAACC00FF);

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

    return ren;
}
