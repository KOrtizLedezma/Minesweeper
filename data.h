//
// Created by kenet on 11/29/2023.
//

#ifndef MINESWEEPER_DATA_H
#define MINESWEEPER_DATA_H


#include "genericButton.h"
#include "Cell.h"

class data {
public:
    vector<genericButton> buttons;
    vector<vector<Cell>> board;
    vector<vector<bool>> states;

    data() {
        // Initialize default values
        buttons = vector<genericButton>();
        board = vector<vector<Cell>>();
        states = vector<vector<bool>>();
    }
};


#endif //MINESWEEPER_DATA_H
