#pragma once

#include <iostream>

#define SIZE 5
#define BLACK 'X'
#define WHITE 'O'

class Room{
    private:
        uint32_t one;
        uint32_t two;
        char piece[2];
        uint32_t current;
        char board[SIZE][SIZE];
        char result; //X, O, E, N

        pthread_mutex_t lock;
    public:
        Room(uint32_t &id1, uint32_t &id2):one(id1),two(id2)
        {
            piece[0] = 'X';
            piece[1] = 'O';
            memset(board, ' ', sizeof(board));
            result = 'N';
            pthread_mutex_init(&lock, NULL);
        }
        ~Room()
        {
            pthread_mutex_destroy(&lock);
        }
};









