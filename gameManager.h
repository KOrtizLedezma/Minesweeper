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
    bool pauseReset;
    bool isPaused;
    bool once;
    bool limbo;

    string winnerTime;
    string name;
    string stopped;
    float totalPauseTime;
    float prevTime;
    float totalTime;
    float display;

    sf::Clock clockNew;
    sf::Clock pausedClock;

    vector<float> times;

    gameManager(){
        this->gameOver = false;
        this->leaderboardClicked = false;
        this->leaderboardOpen = false;
        this->paused = false;
        this->started = false;
        this->winner = false;
        this->winnerWrote = false;
        this->pauseReset = false;
        this->isPaused = false;
        this->once = false;
        this->limbo = false;
        this->prevTime = 0;
        this->limbo = 0;
        this->totalTime = 0;
        this->display = 0;
        this->totalPauseTime = 0;

    }

    void restartValues(){
        this->gameOver = false;
        this->leaderboardClicked = false;
        this->leaderboardOpen = false;
        this->paused = false;
        this->started = false;
        this->winner = false;
        this->winnerWrote = false;
        this->pauseReset = false;
        this->isPaused = false;
        this->winnerTime = "";
        this->stopped = "";
        this->prevTime = 0;
        this->limbo = 0;
        this->totalTime = 0;
        this->display = 0;
        this->totalPauseTime = 0;
        this->once = false;
        this->limbo = false;
    }

};


#endif //MINESWEEPER_GAMEMANAGER_H
