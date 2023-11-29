//
// Created by kenet on 11/29/2023.
//

#include <string>
#include <iostream>

using namespace std;

#ifndef MINESWEEPER_GAMEMANAGER_H
#define MINESWEEPER_GAMEMANAGER_H


class gameManager {
public:
    bool gameOver;
    bool leaderboardClicked;
    bool leaderboardOpen;
    bool paused;
    bool started;
    bool winner;
    bool winnerWrote;

    string winnerTime;
    string name;
    string stopped;

    sf::Clock clockNew;
    sf::Clock pausedClock;

    gameManager(){
        this->gameOver = false;
        this->leaderboardClicked = false;
        this->leaderboardOpen = false;
        this->paused = false;
        this->started = false;
        this->winner = false;
        this->winnerWrote = false;
    }

    void restartValues(){
        this->gameOver = false;
        this->leaderboardClicked = false;
        this->leaderboardOpen = false;
        this->paused = false;
        this->started = false;
        this->winner = false;
        this->winnerWrote = false;
        this->winnerTime = "";
        this->stopped = "";
    }

    void setName(const string &nameA) {
        gameManager::name = nameA;
    }

};


#endif //MINESWEEPER_GAMEMANAGER_H
