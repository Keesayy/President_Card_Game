#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "presidents.h"

Card Card_init(int no_card, TypeCard type, Color c, Symbol s){
	Card card;
	card.no_card = no_card;
	card.type = type;
	card.c = c;
	card.s = s;
	return card;
}

Deck Deck_init(void){
	Deck deck = (Deck)malloc(NB_CARDS * sizeof(Card));
	//White Diamond
	for(int i=0; i<NB_CARDS / NB_PLAYERS; ++i)
		deck[i] = Card_init(0, (TypeCard)(i+3), White, Diamond);
	//White Heart
	for(int i=0; i<NB_CARDS / NB_PLAYERS; ++i)
		deck[i + NB_CARDS / NB_PLAYERS] = Card_init(0, (TypeCard)(i+3), White, Heart);
	//Black Clover
	for(int i=0; i<NB_CARDS / NB_PLAYERS; ++i)
		deck[i + 2*NB_CARDS / NB_PLAYERS] = Card_init(0, (TypeCard)(i+3), Black, Clover);
	//Black Pikes
	for(int i=0; i<NB_CARDS / NB_PLAYERS; ++i)
		deck[i + 3*NB_CARDS / NB_PLAYERS] = Card_init(0, (TypeCard)(i+3), Black, Pikes);

	return deck;
}

Stack Stack_init(int nb, int belongs_to, Card top, Cardlist *stack){
	Stack s;
	s.nb = nb;
	s.belongs_to = belongs_to;
	s.top = top;
	s.stack = stack;
	return s;
}

void Show_card(Card c){
	printf("%d %d %d %d\n", c.no_card, c.type, c.c, c.s);
}

void Show_deck(Card *deck, int n){
	for(int i=0; i<n; ++i)
		Show_card(deck[i]);
}

void Show_playlist(Playlist *pl){
	while(pl != NULL){
		listDisplay(pl->play);
		pl = pl->next;
	}
}

void Swap(Card *cards, int i, int j){
	Card tmp = Card_init(cards[i].no_card, cards[i].type, cards[i].c, cards[i].s);

	cards[i].no_card = cards[j].no_card;
	cards[i].type = cards[j].type;
	cards[i].c = cards[j].c;
	cards[i].s = cards[j].s;	

	cards[j].no_card = tmp.no_card;
	cards[j].type = tmp.type;
	cards[j].c = tmp.c;
	cards[j].s = tmp.s;
}

void Mix_deck(Deck deck){
	for(int i=NB_CARDS-1; i>=0; --i){
		int j = rand() % (i + 1);
		Swap(deck, i, j);
	}
}

Game Game_init(int nb_players){
	Game g;
	g.nb_players = nb_players;
	g.stack = Stack_init(0, 0, (Card){.no_card = 1}, NULL);
	g.win = 0;
	g.turn = rand() % nb_players;

	g.leaderboard = (int*)malloc(nb_players * sizeof(int));
	for(int i=0; i<nb_players; ++i)
		g.leaderboard[i] = -1;

	Deck deck = Deck_init();
	Mix_deck(deck);

	g.hands = (Hand*)malloc(nb_players * sizeof(Hand));
	for(int i=0; i<nb_players; ++i)
		g.hands[i] = (Hand)malloc((NB_CARDS / nb_players) * sizeof(Card));

	int k = 0;
	for(int i=0; i<NB_CARDS / nb_players; ++i){
		for(int j=0; j<nb_players; ++j)
			g.hands[j][i] = deck[k+j];
		k += nb_players;
	}

	free(deck);
	return g;
}

void Cardlistshow(Cardlist *cl){
	while(cl != NULL){
		printf("%d ", cl->i);
		Show_card(cl->c);
		cl = cl->next;
	}
}

Cardlist *CardlistAdd(Cardlist *cl, Card c, int i){
	Cardlist *head = (Cardlist*)malloc(sizeof(Cardlist));
	head->i = i;
	head->c = c;
	head->next = cl;
	return head;
}

int Cardlistsize(Cardlist *cl){
	int count = 0;
	while(cl != NULL){
		count++;
		cl = cl->next;
	}
	return count;
}

void CardlistFree(Cardlist *cl){
	Cardlist *copy = NULL;
	while(cl != NULL){
		copy = cl;
		cl = cl->next;
		free(copy);
	}
}

Cardlist *Cardlist_from_hand(Hand h, int n){
	Cardlist *cl = NULL;

	for(int i=0; i<n; ++i)
		cl = CardlistAdd(cl, h[i], i);

	return cl;
}

Cardlist *Cardlist_from_in_hand(Hand h, int n){
	Cardlist *cl = NULL;

	for(int i=0; i<n; ++i){
		if(!h[i].no_card)
			cl = CardlistAdd(cl, h[i], i);
	}
	return cl;
}

Cardlist *Cardlist_from_not_in_hand(Hand h, int n){
	Cardlist *cl = NULL;

	for(int i=0; i<n; ++i){
		if(h[i].no_card)
			cl = CardlistAdd(cl, h[i], i);
	}
	return cl;
}

Cardlist* CardlistInverse(Cardlist* l){
    Cardlist* l_copy = NULL;
    while (l){
        l_copy = CardlistAdd(l_copy, l->c, l->i);
        l = l->next;
    }
    return l_copy;
}

Cardlist* CardlistCopy(Cardlist* l){
    Cardlist* temp = CardlistInverse(l);
    Cardlist* result = CardlistInverse(temp);
    CardlistFree(temp);
    return result;
}

Cardlist** listPivot(Cardlist* l, TypeCard pivot){
    Cardlist* gauche = NULL;
    Cardlist* droite = NULL;

	Cardlist *copie;
	int count = 0;
    while(l != NULL){
    	if(l->c.type == pivot && count < 1){
	    	copie = l;
			l = l->next;
    		free(copie);
    		count ++;
    	}else{
    		Cardlist *next = l->next;

	    	if(l->c.type > pivot){
	    		l->next = droite;
	    		droite = l;
    		}else{
    			l->next = gauche;
    			gauche = l;
			}
			l = next;
		}
	}	
    Cardlist ** duo = (Cardlist **) malloc(2*sizeof(Cardlist *));
    if (duo){
        duo[0] = gauche;
        duo[1] = droite;
    } else{
        return NULL;
    }
    return duo;
}

Cardlist* reassemble(Cardlist* gauche, Cardlist* droite, Card c, int i){
	droite = CardlistAdd(droite, c, i);

	if(gauche == NULL) return droite;
	else
	{
		Cardlist *copie = gauche;
		while(copie->next != NULL) 
			copie = copie->next;
		copie->next = droite;
	}
    return gauche;
}

Cardlist* quickSort_rec(Cardlist* l){
    if(l == NULL)
    	return NULL;
    Card c = l->c;
    int i = l->i;
    Cardlist **duo = listPivot(l, c.type);

    Cardlist *gauche = quickSort_rec(duo[0]);
    Cardlist *droite = quickSort_rec(duo[1]);
    free(duo);

    return reassemble(gauche, droite, c, i);
}

Cardlist* quickSort(Cardlist* l){
    Cardlist *copie = CardlistCopy(l);
    return quickSort_rec(copie);
}

// Exo 2 : choix du pivot aleatoire
TypeCard getRandomElement(Cardlist* l){
    int n = Cardlistsize(l);
    int k = rand() % n;
    for(int i=0; i<k; ++i) l = l->next;

    return l->c.type;
}

Playlist *PlaylistAdd(Playlist *pl, Play p){
	Playlist *head = (Playlist*)malloc(sizeof(Playlist));
	head->play = p;
	head->next = pl;
	return head;
}

void PlaylistFree(Playlist *pl){
	Playlist *copy = NULL;
	while(pl != NULL){
		copy = pl;
		pl = pl->next;
		listFree(copy->play);
		free(copy);
	}
}

int PlaylistSize(Playlist *pl){
	int count = 0;
	while(pl != NULL){
		++count;
		pl = pl->next;
	}
	return count;
}

int Nb_card_hand(Hand h, int n){
	int count = 0;
	for(int i=0; i<n; ++i){
		if(!h[i].no_card)
			count++;
	}
	return count;
}

int Possible_play(Game game, Play indices){
	if(indices == NULL) return 1;

	int size_indices = listSize(indices);
	int size_hand = Nb_card_hand(game.hands[game.turn], NB_CARDS / game.nb_players);

	if(game.stack.nb == 0 || size_indices == game.stack.nb){
		Card tmp = game.hands[game.turn][indices->value];

		while(indices != NULL){
			if(tmp.type != game.hands[game.turn][indices->value].type)
				return 0;
			indices = indices->next;
		}

		if((size_hand == 1) && (tmp.type == Two)) return 0;

		if(!game.stack.top.no_card){
			if(tmp.type < game.stack.top.type) 
				return 0;	
		}	
	}else {return 0;}
	return 1;
}

int mod(int x, int y){
   int t = x - ((x / y) * y);
   if (t < 0) t += y;
   return t;
}

void Apply_play(Game *game, Play indices){
	Card tmp;
	Play copy = indices;
	while(copy != NULL){
		tmp = game->hands[game->turn][copy->value];

		game->stack.stack = CardlistAdd(game->stack.stack, tmp, -1);

		game->hands[game->turn][copy->value].no_card = 1;
		copy = copy->next;
	}

	if(Is_hand_empty(game->hands[game->turn], NB_CARDS / game->nb_players)){
		game->leaderboard[game->win] = game->turn;
		game->win++;
		CardlistFree(game->stack.stack);
		game->stack.nb = 0;
		game->stack.stack = NULL;
		game->stack.top.no_card = 1;
	}

	if(!Is_hand_empty(game->hands[game->turn], NB_CARDS / game->nb_players)){
		if(indices != NULL){
			game->stack.nb = listSize(indices);
			game->stack.top = tmp;
			game->stack.belongs_to = game->turn;
		}
	}
	
    if(game->stack.top.type == 15){
        game->turn = mod(game->turn -1, game->nb_players);
    }

	game->turn = (game->turn + 1) % game->nb_players;
	while(Is_hand_empty(game->hands[game->turn], NB_CARDS / game->nb_players)){
		game->turn = (game->turn + 1) % game->nb_players;
	}

	if(game->turn == game->stack.belongs_to){
		CardlistFree(game->stack.stack);
		game->stack.nb = 0;
		game->stack.stack = NULL;
		game->stack.top.no_card = 1;
	}

	// listFree(indices);
}

Playlist *Find_all_sublist(Cardlist *cl, int n, TypeCard tc) {
	if (n <= 0 || !cl) return NULL;

    Playlist *sublists = NULL;
    Cardlist *start = cl;
    int count = 0;

    while (cl) {
        if (cl->c.type >= tc) {
            if (count == 0 || cl->c.type == start->c.type) {
                count++;
            } else {
                count = 1;
                start = cl;
            }

            if (count == n) {
                Play sublist = NULL;
                Cardlist *temp = start;
                for (int i = 0; i < n; i++) {
                    sublist = listAdd(sublist, temp->i);
                    temp = temp->next;
                }
                sublists = PlaylistAdd(sublists, sublist);
                count = 0;
                start = cl->next;
            }
        } else {
            count = 0;
            start = cl->next;
        }

        cl = cl->next;
    }
    return sublists;
}

Playlist *All_play(Game game){
	Playlist *pl = NULL;
	Hand current = game.hands[game.turn];
	Cardlist *current_list = Cardlist_from_in_hand(current, NB_CARDS / game.nb_players);
	current_list = quickSort(current_list);

	if(game.stack.nb != 0)
		pl = Find_all_sublist(current_list, game.stack.nb, game.stack.top.type);
	else{
		Playlist *tmp = NULL;
		for(int i=1; i<=NB_PLAYERS; ++i){
			tmp = Find_all_sublist(current_list, i, Three);

			while(tmp != NULL){
				Playlist *copy = tmp;
				pl = PlaylistAdd(pl, tmp->play);
				tmp = tmp->next;
				free(copy);
			} 
		}
	}
	//Ajout Play NULL (Passe-tour)
	pl = PlaylistAdd(pl, NULL);
	CardlistFree(current_list);
	return pl;	
}

int Is_hand_empty(Hand h, int n){
	for(int i=0; i<n; ++i){
		if(!h[i].no_card)
			return 0;
	}
	return 1;
}

int Is_game_over(Game game){
	return game.win == (game.nb_players -1);
}
