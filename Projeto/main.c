#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <time.h>
#include <stdlib.h>

#include "queue.c"
#define TRUE 1
#define FALSE 0

#define WINDOW_TITLE "FlappyBird"
#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720
#define WINDOW_POS_X SDL_WINDOWPOS_UNDEFINED
#define WINDOW_POS_Y SDL_WINDOWPOS_UNDEFINED
#define WAIT_STARTING_POINT 30
#define PIPE_GAP 100
#define PIPE_SIZE 5

#define BIRD_WIDTH 17
#define BIRD_HEIGHT 12
#define BIRD_STARTING_POINT 0
#define BIRD_ENDING_POINT 51
#define BIRD_BOUNDS_1 51
#define BIRD_POSITION_X 0
#define BIRD_POSITION_Y 0
#define GRAVITY 8

#define GRASS_COLOR "17a119"
#define SKY_COLOR "138792"


typedef enum {MENU_INICIAL, CAINDO, BATENDO_ASAS, FIM_DE_JOGO} Estado; 

SDL_Renderer *create_renderer(SDL_Window *win);
SDL_Texture *load_texture(char *image_path, SDL_Renderer *ren);
SDL_Window *create_window(void);
int AUX_WaitEventTimeoutCount(SDL_Event *event, Uint32 *wait, int frametime);
int check_collision(SDL_Rect bird, LinkedList *pipes);
int draw_game(SDL_Renderer *ren, SDL_Texture *sprites, LinkedList *pipes, int *critical_point_y, SDL_Rect *bird_frame, SDL_Rect *bird_rect, int *score, int *bird_hit_the_wall, Estado* current_state);
int draw_game_over_menu(SDL_Renderer *ren, SDL_Texture *sprites, LinkedList *pipes, int *critical_point_y, SDL_Rect *bird_frame, SDL_Rect *bird_rect, int *score, int *bird_hit_the_wall, Estado *current_state, SDL_Rect *menu_rect, SDL_Rect *retry_rect, SDL_Rect *retry_frame, SDL_Rect *menu_frame, SDL_Rect *game_over_rect, SDL_Rect *game_over_frame);
int draw_menu(SDL_Renderer *ren, SDL_Texture *sprites, LinkedList *pipes, int *critical_point_y, SDL_Rect *bird_frame, SDL_Rect *bird_rect, int *score, int *bird_hit_the_wall, Estado *current_state, SDL_Rect *play_button_rect, SDL_Rect *logo_rect, SDL_Rect* logo_frame, SDL_Rect* play_button_frame);
void SDL_init(void);
void draw_city_and_grass(SDL_Renderer *ren, SDL_Texture *sprites);
void draw_pipes(SDL_Renderer *ren, LinkedList *pipes, SDL_Texture *sprites);
void draw_score(SDL_Renderer *ren, SDL_Texture *sprites, int *score, int middle);
void generate_new_pipes(LinkedList *pipes);
void move_em_all_to_the_left(LinkedList *pipes);
void reset_game(LinkedList *pipes, SDL_Rect *bird_rect, int *bird_hit_the_wall, int *gravity, int *score, int *critical_point, SDL_Rect *top_pipe, SDL_Rect *bottom_pipe, SDL_Rect *top_pipe_2, SDL_Rect *bottom_pipe_2);
void update_score(int *score, LinkedList *pipes, SDL_Rect *bird_rect);
void input_handler(Estado* estado, SDL_Event* event);
void time_related_tasks(Estado* estado, SDL_Event* event);
void renderer(Estado* estado, SDL_Event* event);
void menu_inicial(Estado* estado, SDL_Event* event);
void caindo(Estado* estado, SDL_Event* event);
void batendo_asas(Estado* estado, SDL_Event* event);
void fim_de_jogo(Estado* estado, SDL_Event* event);

#ifdef SDL_POINTINRECT_NOT_IMPLEMENTED
//https://github.com/libsdl-org/SDL/blob/98986f39e97727a80f733f7809ceb80a13697269/include/SDL_rect.h
SDL_FORCE_INLINE SDL_bool SDL_PointInRect(const SDL_Point *p, const SDL_Rect *r)
{
    return ( (p->x >= r->x) && (p->x < (r->x + r->w)) &&
             (p->y >= r->y) && (p->y < (r->y + r->h)) ) ? SDL_TRUE : SDL_FALSE;
}
#endif

int bird_hit_the_wall = FALSE;
int critical_point_y = WINDOW_HEIGHT;
int gravity = -GRAVITY;
int frametime = 0;
int score = 0;
int quit = FALSE;
int sprites_width, sprites_height;
int mouse_x, mouse_y;
SDL_Rect bird_rect = {WINDOW_WIDTH / 5 - 85, 0, 85, 60};
SDL_Rect bird_frame = {0, 0, BIRD_WIDTH, BIRD_HEIGHT};
SDL_Rect top_pipe, bottom_pipe, top_pipe_2, bottom_pipe_2;
SDL_Rect logo_rect;
SDL_Rect logo_frame = {52, 57, 89, 23};
SDL_Rect play_button_rect;
SDL_Rect play_button_frame = {52, 92, 52, 29};
SDL_Rect game_over_rect;
SDL_Rect game_over_frame = {52, 121, 96, 21};
SDL_Rect menu_rect;
SDL_Rect menu_frame = {104, 92, 40, 14};
SDL_Rect retry_rect;
SDL_Rect retry_frame = {104, 106, 40, 14};
SDL_Point position;
SDL_Window *win;
SDL_Renderer *ren;
SDL_Texture *sprites;
LinkedList* pipes;


// Máquina de Estados
void input_handler(Estado* estado, SDL_Event* event) {
    if(event->type == SDL_QUIT) {
        quit = TRUE;
        return;
    }
    switch(*estado) {
        case MENU_INICIAL: menu_inicial(estado, event); break;
        case CAINDO: caindo(estado, event); break;
        case BATENDO_ASAS: batendo_asas(estado, event); break;
        case FIM_DE_JOGO: fim_de_jogo(estado, event); break;
    }
}

void menu_inicial(Estado* estado, SDL_Event* event) {
    if(event->type == SDL_MOUSEBUTTONUP) {
        SDL_GetMouseState(&mouse_x, &mouse_y);
        position.x = mouse_x;
        position.y = mouse_y;

        if(SDL_PointInRect(&position, &play_button_rect)) {
            *estado = CAINDO;
            reset_game(pipes, &bird_rect, &bird_hit_the_wall, &score, &gravity, &critical_point_y, &top_pipe, &bottom_pipe, &top_pipe_2, &bottom_pipe_2);
        }
    }
}

void caindo(Estado* estado, SDL_Event* event) {
    // GARANTIR GRAVIDADE POSITIVA
    if (gravity < 0) {
        gravity = GRAVITY;
    }

    // DEFINIÇÃO DE PONTO MÁXIMO E
    // TRANSIÇÃO PARA "BATENDO ASAS"
    if(event->type == SDL_MOUSEBUTTONUP) {
        *estado = BATENDO_ASAS;
        gravity = -GRAVITY;
        critical_point_y = bird_rect.y - 70;
    }
}

void batendo_asas(Estado* estado, SDL_Event* event) {
    // GARANTIR GRAVIDADE NEGATIVA
    if (gravity > 0) {
        gravity = -GRAVITY;
    }

    // SE COLIDIR COM UM CANO, DEVE TRANSICIONAR
    // PARA O ESTADO "FIM_DE_JOGO".
    bird_hit_the_wall = check_collision(bird_rect, pipes);
    if (bird_hit_the_wall) {
        *estado = FIM_DE_JOGO;
    }

    // DEFINIR PONTO MÁXIMO
    else if(event->type == SDL_MOUSEBUTTONUP) {
        *estado = BATENDO_ASAS;
        gravity = -GRAVITY;
        critical_point_y = bird_rect.y - 70;
    }

    // SE O PÁSSARO PASSAR DA LINHA,
    // ELE DEVE TRANSICIONAR PARA O
    // ESTADO "CAINDO".
    if (critical_point_y >= bird_rect.y) {
        *estado = CAINDO;
    }
}

void fim_de_jogo(Estado* estado, SDL_Event* event) {
    if(event->type == SDL_MOUSEBUTTONUP) {
        SDL_GetMouseState(&mouse_x, &mouse_y);
        position.x = mouse_x;
        position.y = mouse_y;

        if(SDL_PointInRect(&position, &menu_rect)) {
            *estado = MENU_INICIAL;
        } else if(SDL_PointInRect(&position, &retry_rect)) {
            reset_game(pipes, &bird_rect, &bird_hit_the_wall, &score, &gravity, &critical_point_y, &top_pipe, &bottom_pipe, &top_pipe_2, &bottom_pipe_2);
            *estado = CAINDO;
        }
    }
}

int main(void)
{
    SDL_init();
    SDL_Event event;

    win = create_window();
    ren = create_renderer(win);

    sprites = load_texture("assets/textures.png", ren);

    SDL_QueryTexture(sprites, NULL, NULL, &sprites_width, &sprites_height);

    Estado estado = MENU_INICIAL;
    Uint32 wait = WAIT_STARTING_POINT;

    pipes = LinkedList_init();

    srand(time(NULL));
    int r = rand() % (WINDOW_HEIGHT - PIPE_GAP - 50);

    top_pipe.x = WINDOW_WIDTH;
    top_pipe.y = r - PIPE_GAP - (PIPE_SIZE * (sprites_height - BIRD_HEIGHT)); 
    top_pipe.w = PIPE_SIZE * 26; 
    top_pipe.h = PIPE_SIZE * (sprites_height - BIRD_HEIGHT);

    bottom_pipe.x = WINDOW_WIDTH; 
    bottom_pipe.y = r + PIPE_GAP; 
    bottom_pipe.w = PIPE_SIZE * 26; 
    bottom_pipe.h = PIPE_SIZE * (sprites_height - BIRD_HEIGHT);

    top_pipe_2.x = WINDOW_WIDTH + (WINDOW_WIDTH / 2) + (PIPE_SIZE * 13); 
    top_pipe_2.y = r - PIPE_GAP - (PIPE_SIZE * (sprites_height - BIRD_HEIGHT)); 
    top_pipe_2.w = PIPE_SIZE * 26; 
    top_pipe_2.h = PIPE_SIZE * (sprites_height - BIRD_HEIGHT);

    bottom_pipe_2.x = WINDOW_WIDTH + (WINDOW_WIDTH / 2) + (PIPE_SIZE * 13); 
    bottom_pipe_2.y = r + PIPE_GAP;
    bottom_pipe_2.w = PIPE_SIZE * 26; 
    bottom_pipe_2.h = PIPE_SIZE * (sprites_height - BIRD_HEIGHT);

    int logo_frame_mul = 4;

    logo_rect.w = logo_frame.w * logo_frame_mul;
    logo_rect.h = logo_frame.h * logo_frame_mul;
    logo_rect.x = (WINDOW_WIDTH / 2) - (logo_rect.w / 2);
    logo_rect.y = 0;

    int play_button_frame_mul = 2;
    
    play_button_rect.w = play_button_frame.w * play_button_frame_mul;
    play_button_rect.h = play_button_frame.h * play_button_frame_mul;
    play_button_rect.y = logo_rect.h + 100;
    play_button_rect.x = (WINDOW_WIDTH / 2) - (play_button_rect.w / 2);

    int height = play_button_rect.h + play_button_rect.y;

    logo_rect.y += (WINDOW_HEIGHT / 2) - (height / 2);
    play_button_rect.y = (WINDOW_HEIGHT / 2) - (height / 2) + logo_rect.h + 100;

    game_over_rect.w = game_over_frame.w * 5;
    game_over_rect.h = game_over_frame.h * 5;
    game_over_rect.x = WINDOW_WIDTH/2 - (game_over_frame.w*5/2);
    // game_over_rect.y = 0;
    game_over_rect.y = WINDOW_HEIGHT/2 - game_over_frame.y - 100;

    menu_rect.w = menu_frame.w * 5;
    menu_rect.h = menu_frame.h * 5;
    menu_rect.x = WINDOW_WIDTH/2 - (menu_frame.x*2);
    menu_rect.y = WINDOW_HEIGHT/2 - menu_frame.h + 100;

    retry_rect.w = retry_frame.w * 5;
    retry_rect.h = retry_frame.h * 5;
    retry_rect.x = WINDOW_WIDTH/2 + 15;
    retry_rect.y = WINDOW_HEIGHT/2 - retry_frame.h + 100;
    

    LinkedList_insert(top_pipe, pipes);
    LinkedList_insert(bottom_pipe, pipes);
    LinkedList_insert(top_pipe_2, pipes);
    LinkedList_insert(bottom_pipe_2, pipes);

    while (!quit)
    {
        if (AUX_WaitEventTimeoutCount(&event, &wait, frametime))
        {
            input_handler(&estado, &event);
        }
        else
        {
            time_related_tasks(&estado, &event);
        }

        printf("current_state: %d\n", estado);
        renderer(&estado, &event);
        SDL_RenderPresent(ren);
    }

    printf("Score: %d\n", score);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();

    return 0;
}

void SDL_init(void)
{
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        printf("Erro de inicialização do SDL.\nSDL_Error: %s\n", SDL_GetError());
        return;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG))
    {
        printf("Erro de incialização do SDL_image.\nSDL_Error: %s\n", SDL_GetError());
    }
}

SDL_Window *create_window(void)
{
    SDL_Window *win = SDL_CreateWindow(WINDOW_TITLE, WINDOW_POS_X, WINDOW_POS_Y, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);

    if (win == NULL)
    {
        printf("Janela não pôde ser criada.\nSDL_Error: %s\n", SDL_GetError());
    }

    return win;
}

SDL_Renderer *create_renderer(SDL_Window *win)
{
    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);

    if (win == NULL)
    {
        printf("Janela não pôde ser criada.\nSDL_Error: %s\n", SDL_GetError());
    }

    SDL_SetRenderDrawBlendMode(ren, SDL_BLENDMODE_BLEND);

    return ren;
}

SDL_Texture *load_texture(char *image_path, SDL_Renderer *ren)
{
    SDL_Texture *texture = NULL;
    SDL_Surface *surface = IMG_Load(image_path);

    if (!surface)
    {
        printf("Erro ao carregar a imagem.\nSDL_Error: %s\n", SDL_GetError());
        return NULL;
    }

    texture = SDL_CreateTextureFromSurface(ren, surface);
    if (!texture)
    {
        printf("Erro ao criar textura a partir da superfície.\nSDL_Error: %s\n", SDL_GetError());
        return NULL;
    }

    SDL_FreeSurface(surface);

    return texture;
}

int AUX_WaitEventTimeoutCount(SDL_Event *event, Uint32 *wait, int frametime)
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

void draw_score(SDL_Renderer *ren, SDL_Texture *sprites, int *score, int middle)
{
    SDL_Rect num_1_frame = {52 + (14 * (*score % 10)), 39, 12, 18};
    SDL_Rect num_2_frame = {52 + (14 * (((*score % 100) - (*score % 10)) / 10)), 39, 12, 18};
    SDL_Rect num_3_frame = {52 + (14 * (((*score % 1000) - (*score % 100))) / 100), 39, 12, 18};

    int size_multiplier = 4;
    int number_width = 12;
    int half_window_width = WINDOW_WIDTH / 2;
    int half_number_width = number_width * size_multiplier / 2;

    int y = 10;
    if(middle == TRUE) {
        y = (WINDOW_HEIGHT/2) - (y*size_multiplier/2) - 30;
    }

    SDL_Rect num_1_rect = {half_window_width - half_number_width, y, number_width * size_multiplier, 18 * size_multiplier};
    SDL_Rect num_2_rect = num_1_rect;
    SDL_Rect num_3_rect = num_1_rect;

    int display2 = FALSE;
    int display3 = FALSE;

    if (*score > 9 && *score <= 99)
    {
        num_1_rect.x += half_number_width;
        num_2_rect.x -= half_number_width - 1;
        display2 = TRUE;
    }
    if (*score > 99 && *score <= 999)
    {
        num_1_rect.x += half_number_width * 2;
        num_3_rect.x -= half_number_width * 2;
        display2 = TRUE;
        display3 = TRUE;
    }

    SDL_RenderCopy(ren, sprites, &num_1_frame, &num_1_rect);
    if (display2)
        SDL_RenderCopy(ren, sprites, &num_2_frame, &num_2_rect);
    if (display3)
        SDL_RenderCopy(ren, sprites, &num_3_frame, &num_3_rect);
}

void draw_city_and_grass(SDL_Renderer *ren, SDL_Texture *sprites)
{
    SDL_Rect city_rect = {0, (WINDOW_HEIGHT / 5) * 3, (144 * (WINDOW_HEIGHT / 5)) / 39, (WINDOW_HEIGHT / 5)};
    SDL_Rect city_frame = {52, 0, 144, 39};
    SDL_Rect leaves_rect = {0, (WINDOW_HEIGHT / 5) * 4, WINDOW_WIDTH, WINDOW_HEIGHT};

    do
    {
        SDL_SetRenderDrawColor(ren, 0x17, 0xA1, 0x19, SDL_ALPHA_OPAQUE);
        SDL_RenderFillRect(ren, &leaves_rect);

        SDL_RenderCopy(ren, sprites, &city_frame, &city_rect);

        city_rect.x += city_rect.w;
        leaves_rect.x += leaves_rect.w;
    } while (city_rect.x < WINDOW_WIDTH);
}

void draw_pipes(SDL_Renderer *ren, LinkedList *pipes, SDL_Texture *sprites)
{
    SDL_Rect top_pipe_frame = {0, 13, 26, 160};
    SDL_Rect bottom_pipe_frame = {26, 13, 26, 160};

    LinkedListNode *head = pipes->head;

    if (head == NULL)
    {
        return;
    }

    int i = 0;
    while (head)
    {
        if (i % 2 == 0)
            SDL_RenderCopy(ren, sprites, &top_pipe_frame, &(head->data));
        else
            SDL_RenderCopy(ren, sprites, &bottom_pipe_frame, &(head->data));

        head = head->next;
        i++;
    }
}

void generate_new_pipes(LinkedList *pipes)
{
    // DELETAR PIPES
    LinkedListNode *pipe = LinkedList_search(pipes, 0);

    if (pipe->data.x + pipe->data.w >= 0)
        return;

    print(pipes);
    LinkedList_pop(pipes);
    LinkedList_pop(pipes);

    // GERAR PIPES
    srand(time(NULL) * SDL_GetTicks());
    int r = rand() % (WINDOW_HEIGHT - PIPE_GAP - 50);

    SDL_Rect top_pipe = {WINDOW_WIDTH, r - PIPE_GAP - (PIPE_SIZE * (sprites_height - BIRD_HEIGHT)), PIPE_SIZE * 26, PIPE_SIZE * (sprites_height - BIRD_HEIGHT)};

    SDL_Rect bottom_pipe = {WINDOW_WIDTH, r + PIPE_GAP, PIPE_SIZE * 26, PIPE_SIZE * (sprites_height - BIRD_HEIGHT)};

    LinkedList_insert(top_pipe, pipes);
    LinkedList_insert(bottom_pipe, pipes);
}

void move_em_all_to_the_left(LinkedList *pipes)
{
    LinkedListNode *head = pipes->head;

    // CASO DE ERRO
    if (head == NULL)
    {
        printf("[]\n");
        return;
    }

    printf("[");
    while (head)
    {
        printf("{%i, %i}, ", head->data.x, head->data.y);
        head->data.x -= 10;
        head = head->next;
    }
    printf("\b\b]\n");
}

int check_collision(SDL_Rect bird, LinkedList *pipes)
{
    LinkedListNode *head = pipes->head;
    SDL_Rect pipe;

    int leftA, leftB;
    int rightA, rightB;
    int topA, topB;
    int bottomA, bottomB;

    leftA = bird.x;
    rightA = bird.x + bird.w;
    topA = bird.y;
    bottomA = bird.y + bird.h;

    while (head)
    {
        pipe = head->data;

        leftB = pipe.x;
        rightB = pipe.x + pipe.w;
        topB = pipe.y;
        bottomB = pipe.y + pipe.h;

        if (!(bottomA <= topB || topA >= bottomB || rightA <= leftB || leftA >= rightB))
            return TRUE;

        head = head->next;
    }
    return FALSE;
}

int draw_game(SDL_Renderer *ren, SDL_Texture *sprites, LinkedList *pipes, int *critical_point_y, SDL_Rect *bird_frame, SDL_Rect *bird_rect, int *score, int *bird_hit_the_wall, Estado* current_state)
{
    SDL_SetRenderDrawColor(ren, 0x13, 0x87, 0x92, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(ren);

    draw_city_and_grass(ren, sprites);

    draw_pipes(ren, pipes, sprites);

    SDL_SetRenderDrawColor(ren, 0xFF, 0x00, 0x00, SDL_ALPHA_OPAQUE);
    #ifdef DEBUG
    SDL_RenderDrawLine(ren, 0, *critical_point_y, WINDOW_WIDTH, *critical_point_y);
    #endif

    // DRAW BIRD
    SDL_RenderCopy(ren, sprites, bird_frame, bird_rect);

    update_score(score, pipes, bird_rect);

    draw_score(ren, sprites, score, FALSE);
}

int draw_menu(SDL_Renderer *ren, SDL_Texture *sprites, LinkedList *pipes, int *critical_point_y, SDL_Rect *bird_frame, SDL_Rect *bird_rect, int *score, int *bird_hit_the_wall, Estado *current_state, SDL_Rect *play_button_rect, SDL_Rect *logo_rect, SDL_Rect* logo_frame, SDL_Rect* play_button_frame)
{
    SDL_SetRenderDrawColor(ren, 0x13, 0x87, 0x92, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(ren);

    draw_city_and_grass(ren, sprites);

    SDL_RenderCopy(ren, sprites, logo_frame, logo_rect);
    SDL_RenderCopy(ren, sprites, play_button_frame, play_button_rect);

    return 0;
}

int draw_game_over_menu(SDL_Renderer *ren, SDL_Texture *sprites, LinkedList *pipes, int *critical_point_y, SDL_Rect *bird_frame, SDL_Rect *bird_rect, int *score, int *bird_hit_the_wall, Estado *current_state, SDL_Rect *menu_rect, SDL_Rect *retry_rect, SDL_Rect *retry_frame, SDL_Rect *menu_frame, SDL_Rect *game_over_rect, SDL_Rect *game_over_frame)
{
    SDL_SetRenderDrawColor(ren, 0x13, 0x87, 0x92, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(ren);

    draw_city_and_grass(ren, sprites);
    draw_score(ren, sprites, score, TRUE);

    SDL_RenderCopy(ren, sprites, game_over_frame, game_over_rect);
    SDL_RenderCopy(ren, sprites, retry_frame, retry_rect);
    SDL_RenderCopy(ren, sprites, menu_frame, menu_rect);

    return 0;
}

void update_score(int *score, LinkedList *pipes, SDL_Rect *bird_rect)
{
    LinkedListNode *head = pipes->head;
    int plus_one = FALSE;

    // CASO DE ERRO
    if (head == NULL)
    {
        printf("[]\n");
        return;
    }

    printf("[");
    while (head)
    {
        printf("%i, ", head->counted);

        int bird_ahead_of_pipe = (bird_rect->x + bird_rect->w) >= (head->data.x + head->data.w);
        if (!(head->counted) && bird_ahead_of_pipe)
        {
            head->counted = TRUE;
            plus_one = TRUE;
        }
        head = head->next;
    }
    printf("\b\b]\n");

    if (plus_one)
    {
        (*score)++;
    }
}

void reset_game(LinkedList* pipes, SDL_Rect* bird_rect, int* bird_hit_the_wall, int* score, int *gravity, int *critical_point, SDL_Rect* top_pipe, SDL_Rect* bottom_pipe, SDL_Rect* top_pipe_2, SDL_Rect* bottom_pipe_2){
    *score = 0;
    *bird_hit_the_wall = 0;
    *gravity = -GRAVITY;
    bird_rect->x = (WINDOW_WIDTH/8);
    bird_rect->y = (WINDOW_HEIGHT/2) - (bird_rect->h/2);
    *critical_point = bird_rect->y;

    LinkedList_pop(pipes);
    LinkedList_pop(pipes);
    LinkedList_pop(pipes);
    LinkedList_pop(pipes);
    LinkedList_insert(*top_pipe, pipes);
    LinkedList_insert(*bottom_pipe, pipes);
    LinkedList_insert(*top_pipe_2, pipes);
    LinkedList_insert(*bottom_pipe_2, pipes);
}

void time_related_tasks(Estado* estado, SDL_Event* event) {
    switch(*estado) {
        case CAINDO:
        case BATENDO_ASAS:
            move_em_all_to_the_left(pipes);
            generate_new_pipes(pipes);

            frametime++;
            if (frametime == 5)
            {
                frametime = 0;
                bird_frame.x += BIRD_WIDTH;
                if (bird_frame.x >= BIRD_ENDING_POINT)
                    bird_frame.x = BIRD_STARTING_POINT;
            }
            
            // Gravidade
            if (critical_point_y >= bird_rect.y)
            {
                *estado = CAINDO;
                gravity = GRAVITY;
                critical_point_y = -100;
            }
            bird_rect.y += gravity;

            // SE COLIDIR COM UM CANO, DEVE TRANSICIONAR
            // PARA O ESTADO "FIM_DE_JOGO".
            bird_hit_the_wall = check_collision(bird_rect, pipes);
            if (bird_hit_the_wall) {
                *estado = FIM_DE_JOGO;
            }

            break;
    }
}

void renderer(Estado* estado, SDL_Event* event) {
    switch(*estado) {
        case MENU_INICIAL:
            draw_menu(ren, sprites, pipes, &critical_point_y, &bird_frame, &bird_rect, &score, &bird_hit_the_wall, estado, &play_button_rect, &logo_rect, &logo_frame, &play_button_frame);
            break;
        case CAINDO:
        case BATENDO_ASAS:
            draw_game(ren, sprites, pipes, &critical_point_y, &bird_frame, &bird_rect, &score, &bird_hit_the_wall, estado);
            break;
        case FIM_DE_JOGO:
            draw_game_over_menu(ren, sprites, pipes, &critical_point_y, &bird_frame, &bird_rect, &score, &bird_hit_the_wall, estado, &menu_rect, &retry_rect, &retry_frame, &menu_frame, &game_over_rect, &game_over_frame);
            break;
    }
}
