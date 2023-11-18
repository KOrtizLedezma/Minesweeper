//
// Created by kenet on 11/17/2023.
//

#ifndef MINESWEEPER_CELL_H
#define MINESWEEPER_CELL_H


#import <string>
#include <SFML/Graphics.hpp>
using namespace std;

class Cell {
public:
    int value;
    string imagePath;
    sf::Vector2f position;

    Cell() {
       this->value = value;
       this-> imagePath = "Images/tile_hidden.png";
       this->position = position;
    }
};


#endif //MINESWEEPER_CELL_H
