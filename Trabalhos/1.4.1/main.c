#include<stdio.h>
#include<SDL2/SDL.h>
#include<math.h>

#define TRUE 1
#define FALSE 0

#define WINDOW_TITLE  "1.4.1"
#define WINDOW_WIDTH  1280
#define WINDOW_HEIGHT 720
#define WINDOW_POS_X  SDL_WINDOWPOS_UNDEFINED
#define WINDOW_POS_Y  SDL_WINDOWPOS_UNDEFINED
#define MAX_RECTS     10

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

typedef struct {
    SDL_Rect* rect;
    struct RectNode* next;
    int r, g, b;
} RectNode;

typedef struct {
    SDL_Rect* head;
    int capacity;
    int length;
} RectList;

RectList* create_rect_list(void) {
    RectList* rl = malloc(sizeof(RectList));
    rl->capacity = MAX_RECTS;
    rl->length = 0;
    rl->head = NULL;

    return rl;
}

RectNode* create_rect_node(void) {
    RectNode* node = malloc(sizeof(RectNode));
    SDL_Rect* rect = malloc(sizeof(SDL_Rect));
    rect->w, rect->h, rect->x, rect->y = 0,0,0,0;

    node->next = NULL;
    node->rect = rect;

    return node;
}

void insert_new_rect(RectList* rl, int x, int y, int w, int h, int r, int g, int b) {
    if(rl->capacity == rl->length)
        return;    

    RectNode* rect = create_rect_node();

    rect->next = rl->head;    
    rl->head = rect;

    rect->rect->w = w;
    rect->rect->h = h;
    rect->rect->x = x;
    rect->rect->y = y;
    rect->r = r;
    rect->g = g;
    rect->b = b;

    rl->length++;
}

void render_rect_list(SDL_Renderer* ren, RectList* rl) {
    RectNode* head = rl->head;

    for(int i=0; i<rl->length;i++) {
        SDL_SetRenderDrawColor(ren, head->r, head->g, head->b, SDL_ALPHA_OPAQUE);
        SDL_RenderFillRect(ren, head->rect);
        head = head->next;
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
    int speed = 5;
    int letters_are_gone = FALSE;
    RectList* rect_list = create_rect_list();
    rect_list->head = NULL;
    while (!quit){
        SDL_SetRenderDrawColor(ren, 0xFF, 0xFF, 0xFF, 0x00);
        SDL_RenderClear(ren);

        while(SDL_PollEvent(&event)) {
            if(event.type == SDL_QUIT) {
                quit = TRUE;
            }
            if(event.type == SDL_MOUSEBUTTONDOWN) {
                int mouse_x, mouse_y;
                SDL_GetMouseState(&mouse_x, &mouse_y);
                printf("left click x:%i y:%i\n", mouse_x, mouse_y);
                if(event.button.button == SDL_BUTTON_LEFT) {
                    printf("Inserting new rectangle.\n");
                    int red, green, blue;
                    red = rand()%255;
                    green = rand()%255;
                    blue = rand()%255;
                    insert_new_rect(rect_list, mouse_x, mouse_y, sqr_size, sqr_size, red, green, blue);
                }
            }
        }

        if(introduction) {
            if(opacity+speed > 0xFF) {
                speed *= -1;
                opacity = 0xFF;
                introduction = FALSE;
                speed = 5;
                SDL_Delay(2000);
            }
            else 
                opacity += speed;
        } else {
            make_letters_go_away(ren, letters, &speed);
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

        render_rect_list(ren, rect_list);

        SDL_RenderPresent(ren);
        SDL_Delay(1000/60);
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
