#ifndef _PRESIDENTS_H_
#define _PRESIDENTS_H_

#include "linkedlist.h"

#define NB_CARDS 52
#define MIN_PLAYERS 3
#define NB_PLAYERS 4

typedef enum{
	White = 1, Black
}Color;

typedef enum{
	Diamond = 1, Heart, Clover, Pikes
}Symbol;

typedef enum{
	Three = 3, 
	Four, Five, Six, Seven, Eight, Nine, Ten,
	Jack, Queen, King,
	As, Two
}TypeCard;

typedef struct{
	int no_card;
	TypeCard type;
	Color c;
	Symbol s;
}Card;

typedef struct Cardlist{
	Card c;
	int i;
	struct Cardlist *next;
}Cardlist;

void Cardlistshow(Cardlist *cl);
Cardlist *CardlistAdd(Cardlist *cl, Card c, int i);
int Cardlistsize(Cardlist *cl);
void CardlistFree(Cardlist *cl);

Cardlist* CardlistInverse(Cardlist* l);
Cardlist* CardlistCopy(Cardlist* l);
Cardlist** listPivot(Cardlist* l, TypeCard pivot);
Cardlist* reassemble(Cardlist* gauche, Cardlist* droite, Card c, int i);
Cardlist* quickSort_rec(Cardlist* l);
Cardlist* quickSort(Cardlist* l);

typedef Card* Hand;
typedef Card* Deck;

typedef struct{
	int nb;
	int belongs_to;
	Card top;
	Cardlist *stack;
}Stack;

typedef struct{
	int nb_players;
	int turn;
	Hand *hands;
	Stack stack;
	int *leaderboard;
	int win;
}Game;

// NULL correspond à passer son tour
typedef list* Play;

typedef struct Playlist{
	Play play;
	struct Playlist *next;
}Playlist;

Card Card_init(int no_card, TypeCard type, Color c, Symbol s);
Deck Deck_init(void);
Stack Stack_init(int nb, int belongs_to, Card top, Cardlist *stack);

Cardlist *Cardlist_from_hand(Hand h, int n);
Cardlist *Cardlist_from_in_hand(Hand h, int n);
Cardlist *Cardlist_from_not_in_hand(Hand h, int n);

void Show_card(Card c);
void Show_deck(Card *deck, int n);
void Show_playlist(Playlist *pl);

void Swap(Card *cards, int i, int j);
void Mix_deck(Deck deck);
Game Game_init(int nb_players);

Playlist *PlaylistAdd(Playlist *pl, Play p);
void PlaylistFree(Playlist *pl);
int PlaylistSize(Playlist *pl);

int Nb_card_hand(Hand h, int n);

int Possible_play(Game game, Play indices);
void Apply_play(Game *game, Play indices);

Playlist *Find_all_sublist(Cardlist *cl, int n,  TypeCard tc);
Playlist *All_play(Game game);

int Is_hand_empty(Hand h, int n);
int Is_game_over(Game game);

#endif 