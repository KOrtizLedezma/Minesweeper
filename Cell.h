//
// Created by kenet on 11/17/2023.
//

#ifndef MINESWEEPER_CELL_H
#define MINESWEEPER_CELL_H


#import <string>
using namespace std;

class Cell {
public:
    int value;
    string imagePath;
    bool isMine; //True if its mine, false if its a regular tile

    Cell(int value, bool isMine){
       this->value = value;
       this-> imagePath = "Images/tile_hidden.png";
       this->isMine = isMine;
    }
    void generateRandomCell();
};


#endif //MINESWEEPER_CELL_H
