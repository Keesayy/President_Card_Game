#President GameCard by Arthur PERAUD

CC = gcc
CFLAGS = -Wall -Wextra -g #-Werror
LDFLAGS = -lm `sdl2-config --cflags --libs` -lSDL2_image -lSDL2_ttf
OBJ = linkedlist.o presidents.o main.o 

#main
main : main.h $(OBJ)
	$(CC) $(OBJ) -o $@ $(LDFLAGS)

#President
presidents : presidents.c presidents.h
	$(CC) -c presidents.c $(CFLAGS)

#List
linkedlist : linkedlist.h linkedlist.c 
	$(CC) -c linkedlist.c $(CFLAGS)

#Cleaning objects
clean :
	rm -f $(OBJ)