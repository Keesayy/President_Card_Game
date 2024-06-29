#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include <time.h>

#include "main.h"

#define DELAY (1000 / 60) // 60fps
#define DELAY_BOT 1200
#define SCREEN_WIDTH 1200
#define SCREEN_HEIGHT 700
#define CARD_WIDTH 71 + 20
#define CARD_HEIGHT 96 + 20
#define CARD_SPACE 20
#define ANGLE_ROTATION 25

#define NB_CARDS_HAND 13
#define TOTAL_CARDS 104 // (13 * 4 * 2)

#define DIM_I 13
#define DIM_J 4  
#define DIM_K 2

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
SDL_Texture* card_textures[DIM_I][DIM_J][DIM_K];
SDL_Texture* card_back_texture = NULL;
SDL_Texture* background = NULL;
SDL_Texture* card_selected_left = NULL;
SDL_Texture* card_selected_right = NULL;
TTF_Font* font = NULL;
TTF_Font* font1 = NULL;

float random_float(float min, float max) {
    return min + ((float)rand() / RAND_MAX) * (max - min);
}

G_hand G_hand_init(Game g){
    G_hand gh;
    gh.cl = Cardlist_from_in_hand(g.hands[0], NB_CARDS_HAND);

    int cl_size = Cardlistsize(gh.cl);
    gh.pos = (SDL_Rect*)malloc(cl_size * sizeof(SDL_Rect));
    gh.selected_left = (bool*)malloc(cl_size * sizeof(bool));
    gh.selected_right = (bool*)malloc(cl_size * sizeof(bool));
    gh.target_pos = (SDL_Rect*)malloc(cl_size * sizeof(SDL_Rect)); 
    gh.rotation = (float*)malloc(cl_size * sizeof(float)); 
    gh.target_rotation = (float*)malloc(cl_size * sizeof(float)); 

    for (int i = 0; i < cl_size; i++) {
        gh.pos[i].x = (SCREEN_WIDTH - cl_size * (CARD_WIDTH - CARD_SPACE))/2.0f - 15; // Example position, left 
        gh.pos[i].y = SCREEN_HEIGHT - CARD_HEIGHT - 70;
        gh.selected_left[i] = false;
        gh.selected_right[i] = false;
            gh.target_pos[i] = gh.pos[i]; 
        gh.rotation[i] = 0.0f; 
        gh.target_rotation[i] = 0.0f; 

    }

    return gh;
}

void Animate_card(SDL_Rect* pos, SDL_Rect* target_pos, float *rotation, float *target_rotation, float speed) {
    pos->x += (target_pos->x - pos->x) * speed;
    pos->y += (target_pos->y - pos->y) * speed;
    *rotation += (*target_rotation - *rotation) * speed;
}

G_stack G_stack_init(Game g){
    G_stack gs;
    gs.n = Cardlistsize(g.stack.stack);
    gs.rotation = (double*)malloc(gs.n * sizeof(double));
    gs.cards = (Card*)malloc(gs.n * sizeof(Card));

    for(int i=0; i<gs.n; ++i){
        gs.rotation[i] = (double)(rand() % (int)(2 * ANGLE_ROTATION + 1)) - ANGLE_ROTATION;
        gs.cards[gs.n -1 -i] = g.stack.stack->c;
        g.stack.stack = g.stack.stack->next;
    }
    return gs;
}


Play Play_select_left(G_hand gh){
    int size = Cardlistsize(gh.cl);
    Play p = NULL;
    for(int i=0; i<size; ++i){
        if(gh.selected_left[i])
            p = listAdd(p, gh.cl->i);
        gh.cl = gh.cl->next;
    }
    return p;
}

Play Play_select_right(G_hand gh){
    int size = Cardlistsize(gh.cl);
    Play p = NULL;
    for(int i=0; i<size; ++i){
        if(gh.selected_right[i])
            p = listAdd(p, gh.cl->i);
        gh.cl = gh.cl->next;
    }
    return p;
}

int Count_select_right(G_hand gh){
    int size = Cardlistsize(gh.cl);
    int count = 0;
    for(int i=0; i<size; ++i){
        if(gh.selected_right[i])
            count++;
    }
    return count;
}

SDL_Texture *Load_texture_from_image(const char* file_image_name, SDL_Window* window, SDL_Renderer* renderer) {
   (void)window;
   SDL_Surface *my_image = NULL;
   SDL_Texture* my_texture = NULL;         

   my_image = IMG_Load(file_image_name);
   if (my_image == NULL){ 
      printf("Echec image : %s\n", IMG_GetError());
      exit(EXIT_FAILURE);
   }

   my_texture = SDL_CreateTextureFromSurface(renderer, my_image);
   SDL_FreeSurface(my_image); 
   if (my_texture == NULL){
      printf("Echec transfo texture: %s\n", IMG_GetError());
      exit(EXIT_FAILURE);
   }
   return my_texture;
}

void Draw_background_texture(SDL_Texture *my_texture, SDL_Window *window, SDL_Renderer *renderer) {
   SDL_Rect 
   source = {0},           
   window_dimensions = {0},
   destination = {0};      

   SDL_GetWindowSize(window, &window_dimensions.w, &window_dimensions.h); 
   SDL_QueryTexture(my_texture, NULL, NULL, &source.w, &source.h);       
   destination = window_dimensions;

   SDL_RenderCopy(renderer, my_texture, &source, &destination);                
}

void Draw_text(SDL_Renderer* renderer, TTF_Font* font, const char* text, int x, int y, SDL_Color couleur) {
    SDL_Surface* surface = TTF_RenderText_Solid(font, text, couleur);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect dstRect = { x, y, surface->w, surface->h };
    SDL_RenderCopy(renderer, texture, NULL, &dstRect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void Render_text(const char* message, int x, int y, SDL_Color c) {
    SDL_Surface* surface = TTF_RenderText_Solid(font, message, c);
    if (surface) {
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_Rect dst = { x, y, surface->w, surface->h };
        SDL_RenderCopy(renderer, texture, NULL, &dst);
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);
    } else {
        fprintf(stderr, "Failed to render text: %s\n", TTF_GetError());
    }
}

void Render_game(Game g, G_hand gh, G_stack gs, int mouse_x, int mouse_y) {
    int size = Cardlistsize(gh.cl);

    for (int i = 0; i < size; i++) {
        Animate_card(&gh.pos[i], &gh.target_pos[i], &gh.rotation[i], &gh.target_rotation[i], 0.1f);
        SDL_Rect card_rect = {
            gh.pos[i].x + i * (CARD_WIDTH - CARD_SPACE),
            gh.pos[i].y,
            CARD_WIDTH,
            CARD_HEIGHT
        };

        if (!gh.selected_left[i] && !gh.selected_right[i] && mouse_x >= card_rect.x && mouse_x <= card_rect.x + CARD_WIDTH &&
            mouse_y >= card_rect.y && mouse_y <= card_rect.y + CARD_HEIGHT) {
            card_rect.y -= 40; 
        }

        if(gh.selected_left[i]){

            SDL_RenderCopy(renderer, card_textures[gh.cl->c.type -3][gh.cl->c.s -1][gh.cl->c.c -1], NULL, &card_rect);
            SDL_RenderCopy(renderer, card_selected_left, NULL, &card_rect);
        }else if(gh.selected_right[i]){

            SDL_RenderCopy(renderer, card_textures[gh.cl->c.type -3][gh.cl->c.s -1][gh.cl->c.c -1], NULL, &card_rect);
            SDL_RenderCopy(renderer, card_selected_right, NULL, &card_rect);
        }else
            SDL_RenderCopy(renderer, card_textures[gh.cl->c.type -3][gh.cl->c.s -1][gh.cl->c.c -1], NULL, &card_rect);

        gh.cl = gh.cl->next;
    }

    const SDL_Rect pile_positions[4] = {
        {SCREEN_WIDTH - CARD_WIDTH - 100, SCREEN_HEIGHT / 2 - CARD_HEIGHT / 2, CARD_WIDTH, CARD_HEIGHT}, 
        {SCREEN_WIDTH / 2 - CARD_WIDTH / 2, 50, CARD_WIDTH, CARD_HEIGHT},
        {50, SCREEN_HEIGHT / 2 - CARD_HEIGHT / 2, CARD_WIDTH, CARD_HEIGHT}, 
        {SCREEN_WIDTH / 2 - CARD_WIDTH / 2, SCREEN_HEIGHT / 2 - CARD_HEIGHT / 2 , CARD_WIDTH, CARD_HEIGHT}  
    };

    char num_cards_text[10];
    for (int i = 0; i < 3; i++) {
        SDL_RenderCopy(renderer, card_back_texture, NULL, &pile_positions[i]);

        snprintf(num_cards_text, sizeof(num_cards_text), "%d", Nb_card_hand(g.hands[i+1], NB_CARDS_HAND));
        Render_text(num_cards_text, pile_positions[i].x + CARD_WIDTH + 10, pile_positions[i].y + CARD_HEIGHT / 2, WHITE_color);
    }
    
    if(g.stack.nb != 0){
        for (int i = 0; i < gs.n; i++) {
            SDL_Rect card_rect = pile_positions[3];  // Utilise la position de la pile du milieu
            card_rect.x += 5;
            card_rect.y -= 5;

            SDL_RenderCopyEx(
                renderer,
                card_textures[gs.cards[i].type - 3][gs.cards[i].s - 1][gs.cards[i].c - 1],
                NULL,
                &card_rect,
                gs.rotation[i],
                NULL,
                SDL_FLIP_NONE
            );
        }
        snprintf(num_cards_text, sizeof(num_cards_text), "%d", g.stack.nb);
        Render_text(num_cards_text, pile_positions[3].x + CARD_WIDTH + 10, pile_positions[3].y + CARD_HEIGHT / 2, WHITE_color);
    }
}

int easeOutQuad(int x){
    return 1 - (1 - x) * (1 - x);
}

void Animate_play_card(Game g, G_hand gh, G_stack gs, Play p, int mouse_x, int mouse_y){
    const SDL_Rect pile_positions[4] = {
        {SCREEN_WIDTH - CARD_WIDTH - 100, SCREEN_HEIGHT / 2 - CARD_HEIGHT / 2, CARD_WIDTH, CARD_HEIGHT}, 
        {SCREEN_WIDTH / 2 - CARD_WIDTH / 2, 50, CARD_WIDTH, CARD_HEIGHT},
        {50, SCREEN_HEIGHT / 2 - CARD_HEIGHT / 2, CARD_WIDTH, CARD_HEIGHT}, 
        {SCREEN_WIDTH / 2 - CARD_WIDTH / 2, SCREEN_HEIGHT / 2 - CARD_HEIGHT / 2 , CARD_WIDTH, CARD_HEIGHT}  
    };

    SDL_Rect card_rect;
    int n = 40;
    int rotation[4];
    
    for(int i=0; i<4; ++i)
        rotation[i] = (double)(rand() % (int)(2 * ANGLE_ROTATION + 1)) - ANGLE_ROTATION;

    for(int i=0; i<n; ++i){
        switch(g.turn){
        case 0:
            // card_rect = {
            //     SCREEN_WIDTH/2,
            //     SCREEN_HEIGHT - CARD_HEIGHT - 70 -i,
            //     CARD_WIDTH,
            //     CARD_HEIGHT
            // };
            break;
        case 1:
            card_rect = pile_positions[0];
            card_rect.x -= i * (SCREEN_WIDTH - CARD_WIDTH - 100 - SCREEN_WIDTH / 2 - CARD_WIDTH / 2) / n;
            break;
        case 2:
            card_rect = pile_positions[1];
            card_rect.y += i * (SCREEN_HEIGHT / 2 - CARD_HEIGHT / 2 - 50) / n;
            break;
        case 3:
            card_rect = pile_positions[2];
            card_rect.x += i * (SCREEN_WIDTH / 2 - CARD_WIDTH / 2 - 50) / n;
            break;
        }
        Play copy = p;
        Draw_background_texture(background, window, renderer);
        Render_game(g, gh, gs, mouse_x, mouse_y);
        SDL_Delay(DELAY);

        char player_turn[30];
        sprintf(player_turn, "Turn : Player %d", g.turn);
        Render_text(player_turn, 0, 0, WHITE_color);
        Render_leaderboard(g, WHITE_color);

        int i = 0;
        while(copy != NULL){
            card_rect.x += 5;
            card_rect.y -= 5;

            SDL_RenderCopyEx(
                renderer,
                card_textures[g.hands[g.turn][copy->value].type -3][g.hands[g.turn][copy->value].s -1][g.hands[g.turn][copy->value].c -1],
                NULL,
                &card_rect,
                rotation[i],
                NULL,
                SDL_FLIP_NONE
            );
            copy = copy->next;
            ++i;
        }

        SDL_RenderPresent(renderer);
    }

}

void Render_leaderboard(Game g, SDL_Color c){
    char president[30] = "President :", 
         vice_president[30] = "Vice President :", 
         asshole[30] = "Asshole :", 
         vice_asshole[30] = "Vice Asshole :";
    
    if(g.leaderboard[0] != -1) sprintf(president, "President : %d", g.leaderboard[0]);
    if(g.leaderboard[1] != -1) sprintf(vice_president, "Vice President : %d", g.leaderboard[1]);
    if(g.leaderboard[2] != -1) sprintf(vice_asshole, "Vice Asshole : %d", g.leaderboard[2]);
    if(Is_game_over(g)) sprintf(asshole, "Asshole : %d", g.turn);

    Render_text(president,      SCREEN_WIDTH - 10.5*19, 0, c);
    Render_text(vice_president, SCREEN_WIDTH - 10.5*19, 24, c);
    Render_text(vice_asshole,   SCREEN_WIDTH - 10.5*19, 2*24, c);
    Render_text(asshole,        SCREEN_WIDTH - 10.5*19, 3*24, c);
}

void END_SDL(char ok, const char* msg, SDL_Window* window, SDL_Renderer* renderer){
    char msg_formated[255];
    int l;

    if (!ok) {
        strncpy(msg_formated, msg, 250);
        l = strlen(msg_formated);
        strcpy(msg_formated + l, " : %s\n");
        SDL_Log(msg_formated, SDL_GetError());
    }

    if (renderer) {
        SDL_DestroyRenderer(renderer);
    }

    if (window) {
        SDL_DestroyWindow(window);
    }

    SDL_Quit();

    if (!ok) {
        exit(EXIT_FAILURE);
    }
}

void CLOSE_SDL(void) {
    for (int i = 0; i < DIM_I; i++) {
        for (int j = 0; j < DIM_J; j++) {
            for (int k = 0; k < DIM_K; k++) {
                if (card_textures[i][j][k]) {
                    SDL_DestroyTexture(card_textures[i][j][k]);
                }
            }
        }
    }
    
    if (card_back_texture) SDL_DestroyTexture(card_back_texture);
    if (background) SDL_DestroyTexture(background);

    if (font) TTF_CloseFont(font);
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);

    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

int main(void) {
    srand(time(NULL));
    int flags = IMG_INIT_JPG|IMG_INIT_PNG;
    int initted = IMG_Init(flags);

    if((initted&flags) != flags){
        END_SDL(0, "ERROR SUPPORT JPG PNG", window, renderer);
    }

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        END_SDL(0, "ERROR SDL INIT", window, renderer);
    }

    if (TTF_Init() == -1) {
        END_SDL(0, "ERROR TTF INIT", window, renderer);
    }

    window = SDL_CreateWindow("President Card_Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE);
    if (!window) {
        END_SDL(0, "ERROR WINDOW CREATION", window, renderer);
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        END_SDL(0, "ERROR RENDERER CREATION", window, renderer);
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        END_SDL(0, "ERROR IMG INIT", window, renderer);
    }

    background = Load_texture_from_image("playing_cards/background2.png", window, renderer);
    if (!background) {
        END_SDL(0, "ERROR LOADING BACKGROUND", window, renderer);
    }

    card_selected_left = Load_texture_from_image("playing_cards/select_left.png", window, renderer);
    if (!card_selected_left) {
        END_SDL(0, "ERROR LOADING BACKGROUND", window, renderer);
    }

    card_selected_right = Load_texture_from_image("playing_cards/select_right.png", window, renderer);
    if (!card_selected_right) {
        END_SDL(0, "ERROR LOADING BACKGROUND", window, renderer);
    }

    font = TTF_OpenFont("playing_cards/arial.ttf", 24);
    if (!font) {
        END_SDL(0, "ERROR LOADING FONT", window, renderer);
    }

    font1 = TTF_OpenFont("playing_cards/arial.ttf", 70);
    if (!font1) {
        END_SDL(0, "ERROR LOADING FONT", window, renderer);
    }

    card_back_texture = Load_texture_from_image("playing_cards/back.png", window, renderer); 
    if (!card_back_texture) {
        END_SDL(0, "ERROR LOADING CARD BACK TEXTURE", window, renderer);
    }

    char file_name[40];
    for (int i = 0; i < DIM_I; i++) {
        for (int j = 0; j < DIM_J; j++) {
            for (int k = 0; k < DIM_K; k++) {
                snprintf(file_name, sizeof(file_name), "playing_cards/cards/c-%d-%d-%d.png", i + 3, j + 1, k + 1);
                card_textures[i][j][k] = Load_texture_from_image(file_name, window, renderer);
                if (!card_textures[i][j][k]) {
                    END_SDL(0, "ERROR LOADING CARD TEXTURE", window, renderer);
                }
            }
        }
    }

    Game g = Game_init(NB_PLAYERS);
    G_hand gh = G_hand_init(g);
    G_stack gs = G_stack_init(g);
    Playlist *pl = NULL;

    bool quit = false;
    bool ready = false;
    int count = 0;
    SDL_Event e;

    while (!quit) {
        int mouse_x, mouse_y;
        SDL_GetMouseState(&mouse_x, &mouse_y);

        while (SDL_PollEvent(&e) != 0) {
            switch (e.type) {
            case SDL_QUIT:
                printf("Quitting game\n"); 
                quit = true;
                break;

            case SDL_KEYDOWN :
                switch (e.key.keysym.sym){
                case SDLK_q:
                    quit = true;
                    printf("Quitting game\n");
                    break;

                case SDLK_SPACE:
                    Play p = Play_select_left(gh);

                    if(Possible_play(g, p) && g.turn == 0){

                        // if(p != NULL)
                        //     Animate_play_card(g, gh, gs, p, mouse_x, mouse_y);

                        Apply_play(&g, p);
                        printf("Player 0 played\n");
                        gh = G_hand_init(g);

                        if(p != NULL)
                            gs = G_stack_init(g);
                        ready = false;
                        count = 0;
                    }
                    break;

                case SDLK_a:
                    ready = !ready;
                    break;
                }
                break;

            case SDL_MOUSEBUTTONDOWN:
                int size = Cardlistsize(gh.cl); 

                if(e.button.button == SDL_BUTTON_LEFT) {
                    for (int i = 0; i < size; i++) {
                        SDL_Rect card_rect = {
                            gh.pos[i].x + i * (CARD_WIDTH - CARD_SPACE),
                            gh.pos[i].y,
                            CARD_WIDTH,
                            CARD_HEIGHT
                        };

                        if (!gh.selected_right[i] && mouse_x >= card_rect.x && mouse_x <= card_rect.x + CARD_WIDTH &&
                            mouse_y >= card_rect.y && mouse_y <= card_rect.y + CARD_HEIGHT) {
                            gh.selected_left[i] = !gh.selected_left[i];

                            if(gh.selected_left[i]) 
                                gh.pos[i].y -= 40;
                            else 
                                gh.pos[i].y += 40;
                            break;
                        }
                    }

                }
                if(e.button.button == SDL_BUTTON_RIGHT){
                    for (int i = 0; i < size; i++) {
                        SDL_Rect card_rect = {
                            gh.pos[i].x + i * (CARD_WIDTH - CARD_SPACE),
                            gh.pos[i].y,
                            CARD_WIDTH,
                            CARD_HEIGHT
                        };

                        if (!gh.selected_left[i] && mouse_x >= card_rect.x && mouse_x <= card_rect.x + CARD_WIDTH &&
                            mouse_y >= card_rect.y && mouse_y <= card_rect.y + CARD_HEIGHT) {
                            gh.selected_right[i] = !gh.selected_right[i];

                            if(gh.selected_right[i]) 
                                gh.pos[i].y -= 40;
                            else 
                                gh.pos[i].y += 40;
                            break;
                        }
                    }

                    if(Count_select_right(gh) == 2){
                        Play p = Play_select_right(gh);
                        Swap(g.hands[0], p->value, p->next->value);
                        gh = G_hand_init(g);
                        listFree(p);
                    }
                }   
                break;             
            }
        }
        SDL_Delay(DELAY);
        Draw_background_texture(background, window, renderer);
        Render_game(g, gh, gs, mouse_x, mouse_y);

        char player_turn[30];
        sprintf(player_turn, "Turn : Player %d", g.turn);
        Render_text(player_turn, 0, 0, WHITE_color);
        Render_leaderboard(g, WHITE_color); 

        if(Is_game_over(g))                
            Draw_text(renderer, font1, "GAME IS OVER", SCREEN_WIDTH/2 - 12*70/3.1f, SCREEN_HEIGHT/2 - 70, WHITE_color);

        if(g.turn != 0 && !Is_game_over(g) && ready){
            pl = All_play(g);
            Playlist *copy = pl;

            if(pl != NULL && pl->next != NULL){
                while(copy->next != NULL)
                    copy = copy->next;

                Animate_play_card(g, gh, gs, copy->play, mouse_x, mouse_y);
                Apply_play(&g, copy->play);
                gs = G_stack_init(g);
            }else{
                Apply_play(&g, pl->play);
            }

            PlaylistFree(pl);
            pl = NULL;
            ready = false;
        }

        // On attend que l'ordi joue
        count ++;
        if(count == 100){
            ready = true;
            count = 0;
        }
        SDL_RenderPresent(renderer);
    }

    CLOSE_SDL();
    return 0;
}
