#ifndef _MAIN_H_
#define _MAIN_H_

#include "presidents.h"

const SDL_Color WHITE_color = (SDL_Color){.r = 255, .g = 255, .b = 255, .a = 255};
const SDL_Color BLACK_color = (SDL_Color){.r = 0, .g = 0, .b = 0, . a = 255};

typedef struct {
    Cardlist *cl;
    SDL_Rect *pos;
    bool *selected_left;
    bool *selected_right;
    SDL_Rect *target_pos; 
    float *rotation;
    float *target_rotation; 
}G_hand;

typedef struct {
    Card *cards;
    double *rotation; 
    int n;
}G_stack;

G_hand G_hand_init(Game g);
G_stack G_stack_init(Game g);
Play Play_select_left(G_hand gh);
Play Play_select_right(G_hand gh);
int Count_select_right(G_hand gh);

SDL_Texture *Load_texture_from_image(const char *file_image_name, SDL_Window *window, SDL_Renderer *renderer);
void Draw_background_texture(SDL_Texture *my_texture, SDL_Window *window, SDL_Renderer *renderer);
void Draw_text(SDL_Renderer* renderer, TTF_Font* font, const char* text, int x, int y, SDL_Color couleur);

void Render_text(const char* message, int x, int y, SDL_Color c);
void Render_game(Game g, G_hand gh, G_stack gs, int mouse_x, int mouse_y);
// void Animate_play_card(Game g, Play p);
void Render_leaderboard(Game g, SDL_Color c); 


void END_SDL(char ok, const char* msg, SDL_Window* window, SDL_Renderer* renderer);
void CLOSE_SDL(void);

#endif