#include<stdio.h>
#include<stdlib.h>
#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>

#define TRUE 1
#define FALSE 0

#define WINDOW_TITLE  "FlappyBird"
#define WINDOW_WIDTH  720
#define WINDOW_HEIGHT 720
#define WINDOW_POS_X  SDL_WINDOWPOS_UNDEFINED
#define WINDOW_POS_Y  SDL_WINDOWPOS_UNDEFINED
#define WAIT_STARTING_POINT 25

#define BIRD_WIDTH 17
#define BIRD_HEIGHT 12
#define BIRD_STARTING_POINT 0
#define BIRD_ENDING_POINT 51
#define BIRD_BOUNDS_1 51
#define BIRD_POSITION_X 0
#define BIRD_POSITION_Y 0

#define GRASS_COLOR "17a119"
#define SKY_COLOR   "138792"

int sprites_width, sprites_height;

void SDL_init(void);
SDL_Window* create_window(void);
SDL_Renderer* create_renderer(SDL_Window* win);
SDL_Texture* load_texture(char* image_path, SDL_Renderer* ren);
int AUX_WaitEventTimeoutCount(SDL_Event* event, Uint32* wait, int frametime);
void draw_city_and_grass(SDL_Renderer* ren, SDL_Texture* sprites);
void draw_pipes(SDL_Renderer* ren, SDL_Texture* sprites);

int main(void) {
    SDL_init();

    SDL_Window* win = create_window();
    SDL_Renderer* ren = create_renderer(win);
    SDL_Event event;

    SDL_Texture* sprites = load_texture("assets/textures.png", ren);

    SDL_QueryTexture(sprites, NULL, NULL, &sprites_width, &sprites_height);

    SDL_Rect bird_rect  = { 0, 0, 85, 60 };
    SDL_Rect bird_frame = { 0, 0, BIRD_WIDTH, BIRD_HEIGHT };

    int quit = FALSE;
    int frametime = 0;
    Uint32 wait = WAIT_STARTING_POINT;

    while (!quit) {
        if(AUX_WaitEventTimeoutCount(&event, &wait, frametime)) {
            switch (event.key.keysym.sym) {
                case SDLK_ESCAPE:
                    quit = TRUE;
                    break;
            }
        } else {
            wait = WAIT_STARTING_POINT;

            frametime++;
            if(frametime == 5) {
                frametime = 0;
                bird_frame.x += BIRD_WIDTH;
                if(bird_frame.x >= BIRD_ENDING_POINT)
                    bird_frame.x = BIRD_STARTING_POINT;
            }

        }

        SDL_SetRenderDrawColor(ren, 0x13, 0x87, 0x92, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(ren);

        draw_city_and_grass(ren, sprites);

        draw_pipes(ren, sprites);

        SDL_RenderCopy(ren, sprites, &bird_frame, &bird_rect);

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

    if(!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        printf("Erro de incialização do SDL_image.\nSDL_Error: %s\n", SDL_GetError());
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

SDL_Texture* load_texture(char* image_path, SDL_Renderer* ren) {
    SDL_Texture *texture = NULL;
    SDL_Surface *surface = IMG_Load(image_path);

    if(!surface) {
        printf("Erro ao carregar a imagem.\nSDL_Error: %s\n", SDL_GetError());
        return NULL;
    }

    texture = SDL_CreateTextureFromSurface(ren, surface);
    if(!texture) {
        printf("Erro ao criar textura a partir da superfície.\nSDL_Error: %s\n", SDL_GetError());
        return NULL;
    }

    SDL_FreeSurface(surface);

    return texture;
}

int AUX_WaitEventTimeoutCount(SDL_Event* event, Uint32* wait, int frametime) {
    Uint32 before = SDL_GetTicks();
    int is_event = SDL_WaitEventTimeout(event, *wait);
    if (is_event) {
        *wait -= (SDL_GetTicks() - before);

        if (*wait >= WAIT_STARTING_POINT) {
            *wait = 0;
        }
    } else {
        *wait = WAIT_STARTING_POINT;
        printf("frametime: %d\n", frametime);
    }

    return is_event;
}

void draw_city_and_grass(SDL_Renderer* ren, SDL_Texture* sprites) {
    SDL_Rect city_rect = { 0, (WINDOW_HEIGHT/5)*3, (144*(WINDOW_HEIGHT/5))/39, (WINDOW_HEIGHT/5)};        
    SDL_Rect city_frame = { 52, 0, 144, 39 };
    SDL_Rect grass_rect = { 0, (WINDOW_HEIGHT/5)*4, WINDOW_WIDTH, WINDOW_HEIGHT};

    do {
        // RENDER GRASS
        SDL_SetRenderDrawColor(ren, 0x17, 0xA1, 0x19, SDL_ALPHA_OPAQUE);
        SDL_RenderFillRect(ren, &grass_rect);

        // RENDER CITY
        SDL_RenderCopy(ren, sprites, &city_frame, &city_rect);

        city_rect.x += city_rect.w;
        grass_rect.x += grass_rect.w;
    } while(city_rect.x < WINDOW_WIDTH);
}

void draw_pipes(SDL_Renderer* ren, SDL_Texture* sprites) {
    SDL_Rect pipe_frame = { 0, BIRD_HEIGHT+1, 26, sprites_height-BIRD_HEIGHT };
    SDL_Rect pipe_rect = { 200, -600, 26*5, (sprites_height-BIRD_HEIGHT)*5 };

    SDL_RenderCopy(ren, sprites, &pipe_frame, &pipe_rect);
}