//
// Created by kenet on 11/17/2023.
//

#ifndef MINESWEEPER_CELL_H
#define MINESWEEPER_CELL_H


#include <string>
#include <SFML/Graphics.hpp>
using namespace std;

class Cell {
public:
    int value;
    string imagePath;
    sf::Vector2f position;
    bool revealed;
    bool flagged;
    bool itsMine;
    vector<Cell*> surroundingCells;

    Cell() {
       this->value = value;
       this-> imagePath = "Images/tile_hidden.png";
       this->position = position;
       this->revealed = false;
       this->flagged = false;
       this->itsMine = false;
    }
};


#endif //MINESWEEPER_CELL_H
