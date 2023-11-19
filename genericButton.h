//
// Created by kenet on 11/19/2023.
//

#ifndef MINESWEEPER_GENERICBUTTON_H
#define MINESWEEPER_GENERICBUTTON_H


#include <string>
#include <SFML/Graphics.hpp>
using namespace std;

class genericButton {
public:
    string imagePath;
    string type;

    genericButton() {
        this-> imagePath = "";
        this-> type = "";
    }
};


#endif //MINESWEEPER_GENERICBUTTON_H
