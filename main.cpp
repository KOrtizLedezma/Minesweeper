#include <fstream>
#include <string>
#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <vector>
#include <iostream>
#include <chrono>
#include <sstream>
#include "Cell.h"
#include "genericButton.h"

using namespace std;

void setText(sf::Text &text, float x, float y);
void readConfigFile(int& numCol, int& numRow, int& numMines);
vector<vector<Cell>> createBoard(int& numCol, int& numRow, int& numMines);
void drawGame(sf::RenderWindow& window, vector<vector<Cell>>& board, vector<genericButton>& buttons, vector<vector<bool>>& states);
void drawFace(sf::RenderWindow& window, vector<vector<Cell>>& board, vector<genericButton>& buttons);
void drawDebugButton(sf::RenderWindow& window, vector<genericButton>& buttons, vector<vector<Cell>>& board);
void drawPauseButton(sf::RenderWindow& window, vector<genericButton>& buttons, vector<vector<Cell>>& board, vector<vector<bool>>& states);
void drawLeaderboardButton(sf::RenderWindow& window, vector<vector<Cell>>& board, vector<genericButton>& buttons, vector<vector<bool>>& states);
void drawPlayZone(sf::RenderWindow& window, vector<vector<Cell>>& board, vector<genericButton>& buttons, vector<vector<bool>>& states);
string chooseImage(int value);
vector<genericButton> createButtonsVector();
void revealAllMines(vector<vector<Cell>>& board);
void drawCounter(sf::RenderWindow& window, const vector<vector<Cell>>& board, const string& value);
void drawTile(sf::RenderWindow& window, const Cell& cell);
void changeFlagValue(Cell& cell);
void drawFlag(sf::RenderWindow& window, Cell& cell);
void drawMine(sf::RenderWindow& window, Cell& cell);
void drawNumber(sf::RenderWindow& window, Cell& cell);
int chooseRightValue(const char& value);
string validateNumber(string& number);
void drawClock(sf::RenderWindow& window, const vector<vector<Cell>>& board, const string & value);
string secondsToMinutes(string seconds);
void makeTime(sf::RenderWindow& window, vector<vector<Cell>>& board);
void togglePause();
bool checkIfWinner(vector<vector<Cell>>& board);
int getMinesValue(vector<vector<Cell>>& board);
void restartGame(vector<vector<Cell>>& board, vector<genericButton>& buttons);
void saveStateOfTiles(vector<vector<Cell>>& board, vector<vector<bool>>& states);
void revealAllTiles(vector<vector<Cell>>& board);
void hideAllTiles(vector<vector<Cell>>& board);
void returnValueBoard(vector<vector<Cell>>& board, vector<vector<bool>>& states);
vector<vector<bool>> initializeStatesBoard(int& row, int& col);
void drawLeaderboardStuff(sf::RenderWindow& leaderboardWindow, vector<vector<Cell>>& board);
void showNearbyTiles(Cell& cell);
void drawLeaderboard(vector<vector<Cell>>& board, vector<vector<bool>>& states);
void toggleLeaderboardState(genericButton& button);
vector<string> split(const string& s, char delim);
void readAllScores(vector<string>& scores);
void isNewHighScore(vector<string>& scores);
void writeLeaderboard(vector<string>& scores);
void checkScores(vector<string>& scores);
string getScores();

// Global variables, needed to avoid clock, or states in the game update each frame
sf::Clock clockNew;
string stoppedTime;

bool started = false;
bool paused = false;
bool gameOver = false;
bool winner = false;
bool winnerTimeObtained = false;
bool leaderboardClicked = false;
bool winnerWrote = false;
string winnerTime;
string name;

int main() {

    enum State { Welcome, Game };
    State currentState = Welcome;

    int rowCount = 0;
    int colCount = 0;
    int minesCount = 0;

    readConfigFile(colCount, rowCount, minesCount);
    vector<genericButton> buttons = createButtonsVector();

    const int width = (colCount * 32);
    const int height = (rowCount * 32) + 100;

    vector<vector<Cell>> board = createBoard(colCount, rowCount, minesCount);
    vector<vector<bool>> states = initializeStatesBoard(rowCount, colCount);
    //printBoard(board, rowCount, colCount);

    sf::RenderWindow window(sf::VideoMode(width, height), "Minesweeper");
    window.setFramerateLimit(10);
    sf::Font font;
    font.loadFromFile("Font/font.ttf");

    sf::Text welcomeMessage("WELCOME TO MINESWEEPER!", font, 24);
    setText(welcomeMessage, (float(width)/2) , ((float(height)/2)-150));
    welcomeMessage.setFillColor(sf::Color::White);
    welcomeMessage.setStyle(sf::Text::Bold | sf::Text::Underlined);

    sf::Text orderMessage("Enter your name:", font, 20);
    setText(orderMessage, (float(width)/2) , ((float(height)/2)-75));
    orderMessage.setFillColor(sf::Color::White);
    orderMessage.setStyle(sf::Text::Bold);

    sf::Text inputName("", font, 20);
    setText(inputName, (float(width)/2) , ((float(height)/2)-40));
    inputName.setFillColor(sf::Color::Yellow);
    inputName.setStyle(sf::Text::Bold);

    while(window.isOpen()) {
        sf::Event event{};
        while(window.pollEvent(event)) {
            if(event.type == sf::Event::Closed) {
                window.close();
            }
            else if(event.type == sf::Event::TextEntered){
                if(event.text.unicode == 13){
                    if(!name.empty()){
                        currentState = Game;
                    }
                }
                else if(event.text.unicode == 8){
                    if(!name.empty()){
                        name.pop_back();
                    }
                    sf::FloatRect textRect = inputName.getLocalBounds();
                    inputName.setOrigin(textRect.left + textRect.width/2.0f,textRect.top + textRect.height/2.0f);
                }
                else if(isalpha(event.text.unicode)){
                    if(name.empty()){
                        char  c = static_cast<char>(toupper(event.text.unicode));
                        name += c;
                    }
                    else{
                        if(name.length() < 10){
                            char  c = static_cast<char>(tolower(event.text.unicode));
                            name += c;
                        }
                    }
                    sf::FloatRect textRect = inputName.getLocalBounds();
                    inputName.setOrigin(textRect.left + textRect.width/2.0f,textRect.top + textRect.height/2.0f);
                }
            }
        }

        switch (currentState) {
            case Welcome:
                window.clear(sf::Color::Blue);
                window.draw(welcomeMessage);
                window.draw(orderMessage);
                window.draw(inputName);
                break;
            case Game:
                window.clear(sf::Color::White);
                drawGame(window, board, buttons, states);
                break;
        }
        inputName.setString(name + "|");

        window.display();
    }
}

// Sets Text on Welcome Page
void setText(sf::Text &text, float x, float y){
    sf::FloatRect textRect = text.getLocalBounds();
    text.setOrigin(textRect.left + textRect.width/2.0f,textRect.top + textRect.height/2.0f);
    text.setPosition(sf::Vector2f(x, y));
}

// Reads the config file to get values imported
void readConfigFile(int& numCol, int& numRow, int& numMines){
    string fileName = "Files/config.cfg";
    ifstream configFile(fileName);
    int value;
    vector<int> values;

    if(configFile.is_open()){
        do{
            configFile >> value;
            values.push_back(value);
        }
        while(!configFile.fail());
    }
    numCol = values[0];
    numRow = values[1];
    numMines = values[2];
}

// Generates the board with numbers and mines
vector<vector<Cell>> createBoard(int& numCol, int& numRow, int& numMines){

    vector<vector<Cell>> board(numRow, vector<Cell>(numCol));

    //initiate vectors
    for(int i = 0 ;  i < numRow ; i++){
        for (int j = 0 ; j < numCol; j++){
            board[i][j].position = sf::Vector2f(float(j), float(i));
            board[i][j].imagePath = "Images/tile_hidden.png";
            board[i][j].value = 0;
        }
    }


    //initiate mines at random positions
    srand(time(0));

    for(int x = 0 ; x < numMines ; x++){

        int randomRow = rand() % numRow;
        int randomCol = rand() % numCol;

        while(board[randomRow][randomCol].value == 9){
            randomRow = rand() % numRow;
            randomCol = rand() % numCol;
        }
        board[randomRow][randomCol].value = 9;
        board[randomRow][randomCol].itsMine = true;
    }

    for (int i = 0 ; i < numRow ; i++){
        for (int j = 0 ; j < numCol ; j++){
            if(board[i][j].value == 9){
                continue;
            }

            int counter = 0;
            for(int x = max(i-1, 0); x <= min(i+1, numRow-1) ; x++){
                for(int y = max(j-1, 0); y <= min(j+1, numCol-1) ; y++){
                    if(board[x][y].value == 9){
                        counter++;
                    }
                }
            }

            board[i][j].value = counter;
        }
    }

    for(int i = 0; i < numRow; i++) {
        for(int j = 0; j < numCol; j++) {
            Cell& current = board[i][j];

            for(int di = -1; di <= 1; di++) {
                for(int dj = -1; dj <= 1; dj++) {
                    if (i + di >= 0 && i + di < numRow &&
                        j + dj >= 0 && j + dj < numCol) {

                        current.surroundingCells.push_back(&board[i + di][j + dj]);
                    }
                }
            }
        }
    }

    return board;
}

// Calls all the method needed to draw the game
void drawGame(sf::RenderWindow& window, vector<vector<Cell>>& board, vector<genericButton>& buttons, vector<vector<bool>>& states){
    drawFace(window, board, buttons);
    drawDebugButton(window, buttons, board);
    drawPauseButton(window, buttons, board, states);
    drawLeaderboardButton(window, board, buttons, states);
    drawPlayZone(window, board, buttons, states);
}

// Draws the face Button, and controls the clicks
void drawFace(sf::RenderWindow& window, vector<vector<Cell>>& board, vector<genericButton>& buttons){
    float x = ((float(board[1].size())/2.f) * 32) - 32;
    float y = 32 * (float(board.size()) + 0.5f);
    sf::Texture faceTexture;
    if(gameOver){
        faceTexture.loadFromFile("Images/face_lose.png");
    }
    else if(winner){
        faceTexture.loadFromFile("Images/face_win.png");
    }
    else{
        faceTexture.loadFromFile("Images/face_happy.png");
    }


    sf::RectangleShape faceRectangle(sf::Vector2f(64,64));
    faceRectangle.setPosition(x, y);

    sf::Sprite faceSprite;
    faceSprite.setTexture(faceTexture);
    faceSprite.setTextureRect(sf::IntRect(0, 0, 64, 64));
    faceSprite.setPosition(x, y);

    window.draw(faceRectangle);
    window.draw(faceSprite);

    if(sf::Mouse::isButtonPressed(sf::Mouse::Left)){
        sf::Vector2i clickPosition = sf::Mouse::getPosition(window);
        if(faceRectangle.getGlobalBounds().contains(sf::Vector2f(clickPosition))){
            restartGame(board, buttons);
        }
    }
}

// Draws the Debug Button, and controls the clicks
void drawDebugButton(sf::RenderWindow& window, vector<genericButton>& buttons, vector<vector<Cell>>& board){
    float x = ((float(board[1].size())) * 32) - 304;
    float y = 32 * (float(board.size()) + 0.5f);

    for(int i = 0 ; i < buttons.size() ; i++){
        if(buttons[i].type == "debug"){
            sf::Texture mineTexture;
            mineTexture.loadFromFile("Images/debug.png");

            sf::RectangleShape mineRectangle(sf::Vector2f(64,64));
            mineRectangle.setPosition(x, y);

            sf::Sprite mineSprite;
            mineSprite.setTexture(mineTexture);
            mineSprite.setTextureRect(sf::IntRect(0, 0, 64, 64));
            mineSprite.setPosition(x, y);

            if(sf::Mouse::isButtonPressed(sf::Mouse::Left) && !gameOver && !winner){
                sf::Vector2i clickPosition = sf::Mouse::getPosition(window);
                if(mineRectangle.getGlobalBounds().contains(sf::Vector2f(clickPosition))){
                    if(!buttons[i].clicked){
                        buttons[i].clicked = true;
                    }
                    else{
                        buttons[i].clicked = false;
                    }

                }
            }


            window.draw(mineRectangle);
            window.draw(mineSprite);
        }
    }

}

// Draws the pause Button, and controls the clicks
void drawPauseButton(sf::RenderWindow& window, vector<genericButton>& buttons, vector<vector<Cell>>& board, vector<vector<bool>>& states){
    float x = ((float(board[1].size())) * 32) - 240;
    float y = 32 * (float(board.size()) + 0.5f);

    for(int i = 0 ; i < buttons.size() ; i++){
        if(buttons[i].type == "pause"){
            sf::Texture pauseTexture;
            pauseTexture.loadFromFile(buttons[i].imagePath);

            sf::RectangleShape pauseRectangle(sf::Vector2f(64,64));
            pauseRectangle.setPosition(x, y);

            sf::Sprite pauseSprite;
            pauseSprite.setTexture(pauseTexture);
            pauseSprite.setTextureRect(sf::IntRect(0, 0, 64, 64));
            pauseSprite.setPosition(x, y);

            if(sf::Mouse::isButtonPressed(sf::Mouse::Left) && !gameOver && !winner){
                sf::Vector2i clickPosition = sf::Mouse::getPosition(window);
                if(pauseRectangle.getGlobalBounds().contains(sf::Vector2f(clickPosition))){
                    if(buttons[i].imagePath == "Images/play.png"){
                        hideAllTiles(board);
                        returnValueBoard(board, states);
                        buttons[i].imagePath = "Images/pause.png";
                        togglePause();
                    }
                    else if(buttons[i].imagePath == "Images/pause.png"){
                        saveStateOfTiles(board, states);
                        revealAllTiles(board);
                        buttons[i].imagePath = "Images/play.png";
                        togglePause();
                    }
                }
            }

            window.draw(pauseRectangle);
            window.draw(pauseSprite);
        }
    }
}

// Draws the Leaderboard Button, and controls the clicks
void drawLeaderboardButton(sf::RenderWindow& window, vector<vector<Cell>>& board, vector<genericButton>& buttons, vector<vector<bool>>& states){
    float x = ((float(board[1].size())) * 32) - 176;
    float y = 32 * (float(board.size()) + 0.5f);

    for(int i = 0 ; i < buttons.size() ; i++){
        if(buttons[i].type == "leaderboard"){
            sf::Texture leaderboardTexture;
            leaderboardTexture.loadFromFile(buttons[i].imagePath);

            sf::RectangleShape leaderboardRectangle(sf::Vector2f(64,64));
            leaderboardRectangle.setPosition(x, y);

            sf::Sprite leaderboardSprite;
            leaderboardSprite.setTexture(leaderboardTexture);
            leaderboardSprite.setTextureRect(sf::IntRect(0, 0, 64, 64));
            leaderboardSprite.setPosition(x, y);

            if(sf::Mouse::isButtonPressed(sf::Mouse::Left) && !gameOver && !winner){
                sf::Vector2i clickPosition = sf::Mouse::getPosition(window);

                if(leaderboardRectangle.getGlobalBounds().contains(sf::Vector2f(clickPosition))) {
                    leaderboardClicked = true;
                    saveStateOfTiles(board, states);
                    revealAllTiles(board);
                    toggleLeaderboardState(buttons[i]);
                }
            }

            window.draw(leaderboardRectangle);
            window.draw(leaderboardSprite);
        }
    }

}

// Draws the Play zone, all the tiles, numbers, mines and flags are drawn here, and controls the clicks
void drawPlayZone(sf::RenderWindow& window, vector<vector<Cell>>& board, vector<genericButton>& buttons, vector<vector<bool>>& states){
    int flagCounter = 0;
    int mineCounter = 0;
    for (int i = 0 ; i < board.size() ; i++){
        for (int j = 0 ; j < board[i].size() ; j++){
            if(board[i][j].flagged) flagCounter++;
            if(board[i][j].itsMine) mineCounter++;
            sf::RectangleShape tileRectangle(sf::Vector2f(32,32));
            tileRectangle.setPosition(board[i][j].position.x * 32, board[i][j].position.y * 32);

            drawTile(window, board[i][j]);

            if(sf::Mouse::isButtonPressed(sf::Mouse::Left) && !paused && !gameOver && !winner && !leaderboardClicked){
                sf::Vector2i clickPosition = sf::Mouse::getPosition(window);
                if(tileRectangle.getGlobalBounds().contains(sf::Vector2f(clickPosition))){
                    if(board[i][j].value == 9 && !paused){
                        gameOver = true;
                        revealAllMines(board);
                        if(board[i][j].flagged) {board[i][j].flagged = false;}
                    }
                    else if(0 < board[i][j].value && board[i][j].value < 9){
                        board[i][j].revealed = true;
                    }
                    else{
                        board[i][j].revealed = true;
                        //showNearByTiles(board, board.size(), board[i].size(), i, j);
                        showNearbyTiles(board[i][j]);
                        if(board[i][j].flagged) {board[i][j].flagged = false;}
                    }
                }
            }
            if(sf::Mouse::isButtonPressed(sf::Mouse::Right) && !paused && !gameOver && !winner && !leaderboardClicked){
                sf::Vector2i clickPosition = sf::Mouse::getPosition(window);
                if(tileRectangle.getGlobalBounds().contains(sf::Vector2f(clickPosition))){
                    if(!board[i][j].revealed){
                        changeFlagValue(board[i][j]);
                    }
                }
            }
            if(board[i][j].revealed && !paused && !leaderboardClicked){
                board[i][j].imagePath = "Images/tile_revealed.png";
                drawNumber(window, board[i][j]);
            }
            if(board[i][j].flagged && !board[i][j].revealed && !paused && !leaderboardClicked){
                drawFlag(window, board[i][j]);
            }

            if(board[i][j].itsMine && buttons[2].clicked && !paused && !leaderboardClicked){
                drawMine(window, board[i][j]);
            }
            //drawCounter(window, minesCount, board);
        }
    }


    makeTime(window, board);

    string counter = to_string(mineCounter - flagCounter);
    counter = validateNumber(counter);
    drawCounter(window, board, counter);

    if(!paused && !leaderboardClicked){
        winner = checkIfWinner(board);
    }
    if(winner && !winnerWrote){
        if(!winnerTime.empty()) {
            vector<string> scores;
            readAllScores(scores);
            checkScores(scores);
            isNewHighScore(scores);
            writeLeaderboard(scores);
            winnerWrote = true;
            drawLeaderboard(board, states);
            restartGame(board, buttons);
        }
    }

    if(leaderboardClicked){
        drawLeaderboard(board, states);
    }

}

// Choose the correct image path
string chooseImage(int value){
    string path;
    if(value == 0) path = "Images/tile_revealed.png";
    else if(value == 1) path = "Images/number_1.png";
    else if(value == 2) path = "Images/number_2.png";
    else if(value == 3) path = "Images/number_3.png";
    else if(value == 4) path = "Images/number_4.png";
    else if(value == 5) path = "Images/number_5.png";
    else if(value == 6) path = "Images/number_6.png";
    else if(value == 7) path = "Images/number_7.png";
    else if(value == 8) path = "Images/number_8.png";
    else path = "Images/mine.png";

    return path;
}

// Creates a vector that holds all the buttons
vector<genericButton> createButtonsVector(){

    vector<genericButton> buttons;

    genericButton pauseButton;
    pauseButton.imagePath =  "Images/pause.png";
    pauseButton.type = "pause";
    buttons.push_back(pauseButton);

    genericButton faceButton;
    faceButton.imagePath =  "Images/face_happy.png";
    faceButton.type = "face";
    buttons.push_back(faceButton);

    genericButton debugButton;
    debugButton.imagePath = "Images/debug.png";
    debugButton.type = "debug";
    buttons.push_back(debugButton);

    genericButton leaderboardButton;
    leaderboardButton.imagePath = "Images/leaderboard.png";
    leaderboardButton.type = "leaderboard";
    buttons.push_back(leaderboardButton);

    return buttons;
}

// Show all the mines on the board
void revealAllMines(vector<vector<Cell>>& board){
    for(int i = 0 ; i < board.size() ; i++){
        for(int j = 0 ; j < board[i].size() ; j++){
            if(board[i][j].itsMine){
                board[i][j].revealed = true;
                board[i][j].imagePath = "Images/mine.png";
            }
        }
    }
}

// Draws the mine counter
void drawCounter(sf::RenderWindow& window, const vector<vector<Cell>>& board, const string & value){
    int sprite1Pos = chooseRightValue(value[0]);
    int sprite2Pos = chooseRightValue(value[1]);
    int sprite3Pos = chooseRightValue(value[2]);
    sf::Texture tileTexture1;
    tileTexture1.loadFromFile("Images/digits.png");
    sf::Sprite tileSprite1;
    tileSprite1.setTexture(tileTexture1);
    tileSprite1.setTextureRect(sf::IntRect(21*sprite1Pos, 0, 21, 32));
    tileSprite1.setPosition(33, 32 * (board.size() + 0.5) + 16);
    window.draw(tileSprite1);
    sf::Texture tileTexture2;
    tileTexture2.loadFromFile("Images/digits.png");
    sf::Sprite tileSprite2;
    tileSprite2.setTexture(tileTexture2);
    tileSprite2.setTextureRect(sf::IntRect(21*sprite2Pos, 0, 21, 32));
    tileSprite2.setPosition(54, 32 * (board.size() + 0.5) + 16);
    window.draw(tileSprite2);
    sf::Texture tileTexture3;
    tileTexture3.loadFromFile("Images/digits.png");
    sf::Sprite tileSprite3;
    tileSprite3.setTexture(tileTexture3);
    tileSprite3.setTextureRect(sf::IntRect(21*sprite3Pos, 0, 21, 32));
    tileSprite3.setPosition(75, 32 * (board.size() + 0.5) + 16);
    window.draw(tileSprite3);
}

// Draws the tiles on the board
void drawTile(sf::RenderWindow& window, const Cell& cell){
    sf::Texture tileTexture;
    tileTexture.loadFromFile(cell.imagePath);
    sf::Sprite tileSprite;
    tileSprite.setTexture(tileTexture);
    tileSprite.setTextureRect(sf::IntRect(0, 0, 32, 32));
    tileSprite.setPosition(cell.position.x * 32, cell.position.y * 32);
    window.draw(tileSprite);
}

// Changes the value flagged on the cell
void changeFlagValue(Cell& cell){
    cell.flagged = !cell.flagged;
}

// Draws the flag on the board
void drawFlag(sf::RenderWindow& window, Cell& cell){
    sf::Texture flagTexture;
    flagTexture.loadFromFile("Images/flag.png");
    sf::Sprite flagSprite;
    flagSprite.setTexture(flagTexture);
    flagSprite.setTextureRect(sf::IntRect(0, 0, 32, 32));
    flagSprite.setPosition((cell.position.x * 32), (cell.position.y * 32) );
    window.draw(flagSprite);
}

// Draws the mines on the board
void drawMine(sf::RenderWindow& window, Cell& cell){
    sf::Texture mineTexture;
    mineTexture.loadFromFile("Images/mine.png");
    sf::Sprite mineSprite;
    mineSprite.setTexture(mineTexture);
    mineSprite.setTextureRect(sf::IntRect(0, 0, 32, 32));
    mineSprite.setPosition((cell.position.x * 32), (cell.position.y * 32) );
    window.draw(mineSprite);
}

// Draws the numbers on the board
void drawNumber(sf::RenderWindow& window, Cell& cell){
    sf::Texture numberTexture;
    numberTexture.loadFromFile(chooseImage(cell.value));
    sf::Sprite numberSprite;
    numberSprite.setTexture(numberTexture);
    numberSprite.setTextureRect(sf::IntRect(0, 0, 32, 32));
    numberSprite.setPosition((cell.position.x * 32), (cell.position.y * 32) );
    window.draw(numberSprite);
}

// choose the value to move the image for the counter
int chooseRightValue(const char& value){
    if(value == '-') return 10;
    else if(value == '0') return 0;
    else if(value == '1') return 1;
    else if(value == '2') return 2;
    else if(value == '3') return 3;
    else if(value == '4') return 4;
    else if(value == '5') return 5;
    else if(value == '6') return 6;
    else if(value == '7') return 7;
    else if(value == '8') return 8;
    else return 9;
}

// Makes sure that the string to display is 3 digits long
string validateNumber(string& number){
    while(number.size() < 3){
        number = "0" + number;
    }
    return number;
}

void drawClock(sf::RenderWindow& window, const vector<vector<Cell>>& board, const string & value){
    int sprite1Pos = chooseRightValue(value[0]);
    int sprite2Pos = chooseRightValue(value[1]);
    int sprite3Pos = chooseRightValue(value[2]);
    int sprite4Pos = chooseRightValue(value[3]);
    sf::Texture tileTexture1;
    tileTexture1.loadFromFile("Images/digits.png");
    sf::Sprite tileSprite1;
    tileSprite1.setTexture(tileTexture1);
    tileSprite1.setTextureRect(sf::IntRect(21*sprite1Pos, 0, 21, 32));
    tileSprite1.setPosition((board[1].size() * 32) - 97, 32 * (board.size() + 0.5) + 16);
    window.draw(tileSprite1);
    sf::Texture tileTexture2;
    tileTexture2.loadFromFile("Images/digits.png");
    sf::Sprite tileSprite2;
    tileSprite2.setTexture(tileTexture2);
    tileSprite2.setTextureRect(sf::IntRect(21*sprite2Pos, 0, 21, 32));
    tileSprite2.setPosition((board[1].size() * 32) - 76, 32 * (board.size() + 0.5) + 16);
    window.draw(tileSprite2);
    sf::Texture tileTexture3;
    tileTexture3.loadFromFile("Images/digits.png");
    sf::Sprite tileSprite3;
    tileSprite3.setTexture(tileTexture3);
    tileSprite3.setTextureRect(sf::IntRect(21*sprite3Pos, 0, 21, 32));
    tileSprite3.setPosition((board[1].size() * 32) - 54, 32 * (board.size() + 0.5) + 16);
    window.draw(tileSprite3);
    sf::Texture tileTexture4;
    tileTexture4.loadFromFile("Images/digits.png");
    sf::Sprite tileSprite4;
    tileSprite4.setTexture(tileTexture4);
    tileSprite4.setTextureRect(sf::IntRect(21*sprite4Pos, 0, 21, 32));
    tileSprite4.setPosition((board[1].size() * 32) - 33, 32 * (board.size() + 0.5) + 16);
    window.draw(tileSprite4);
}

string secondsToMinutes(string seconds){
    int sec = stoi(seconds);
    int minutes = sec / 60;
    int remainingSeconds = sec % 60;

    stringstream ss;
    if(to_string(minutes).size() < 2){
        ss << "0" << minutes;
    }
    else{
        ss << minutes;
    }
    if(to_string(remainingSeconds).size() < 2){
        ss << "0" << remainingSeconds;
    }
    else{
        ss << remainingSeconds;
    }
    return ss.str();
}

void makeTime(sf::RenderWindow& window, vector<vector<Cell>>& board){

    string time;

    if (!started && !paused) {
        clockNew.restart();  // Start the clock
        started = true;
    }

    if (!paused) {
        sf::Time elapsed = clockNew.getElapsedTime();
        time = to_string(elapsed.asSeconds());
        string t = secondsToMinutes(time);
        stoppedTime = t;
        drawClock(window, board, t);
    }
    if(paused || leaderboardClicked){
        drawClock(window, board, stoppedTime);
    }
    if(gameOver){
        clockNew.restart();
    }

    if(winner && !winnerTimeObtained){
        sf::Time elapsed = clockNew.getElapsedTime();
        time = to_string(elapsed.asSeconds());
        string t = secondsToMinutes(time);
        winnerTime = t;
        winnerTimeObtained = true;
        clockNew.restart();
    }
}

void togglePause() {
    paused = !paused;
}

bool checkIfWinner(vector<vector<Cell>>& board){
    bool trueWinner = true;
    for(int i = 0 ; i < board.size() ; i++){
        for(int j = 0; j < board[i].size() ; j++){
            if(!board[i][j].revealed && board[i][j].value != 9){
                trueWinner = false;
            }
        }
    }
    return trueWinner;
}

void restartGame(vector<vector<Cell>>& board, vector<genericButton>& buttons){
    int col = board[1].size();
    int row = board.size();
    int mines = getMinesValue(board);
    board = createBoard(col, row, mines);
    gameOver = false;
    winner = false;
    paused = false;
    started = false;
    winnerTimeObtained = false;
    winnerWrote = false;
    buttons[2].clicked = false;
}

int getMinesValue(vector<vector<Cell>>& board){
    int counter = 0;
    for(int i = 0 ; i < board.size() ; i++){
        for(int j = 0 ; j < board[1].size() ; j++){
            if(board[i][j].value == 9){
                counter++;
            }
        }
    }
    return counter;
}

void saveStateOfTiles(vector<vector<Cell>>& board, vector<vector<bool>>& states){
    for(int i = 0 ; i < board.size() ; i++){
        for(int j = 0 ; j < board[i].size() ; j++){
            states[i][j] = board[i][j].revealed;
        }
    }
}

void revealAllTiles(vector<vector<Cell>>& board){
    for(int i = 0 ; i < board.size() ; i++){
        for(int j = 0 ; j < board[1].size() ; j++){
            board[i][j].imagePath = "Images/tile_revealed.png";
            board[i][j].revealed = true;
        }
    }
}

void hideAllTiles(vector<vector<Cell>>& board){
    for(int i = 0 ; i < board.size() ; i++){
        for(int j = 0 ; j < board[1].size() ; j++){
            board[i][j].revealed = false;
            board[i][j].imagePath = "Images/tile_hidden.png";
        }
    }
}

void returnValueBoard(vector<vector<Cell>>& board, vector<vector<bool>>& states){
    for(int i = 0 ; i < board.size() ; i++){
        for(int j = 0 ; j < board[i].size() ; j++){
            board[i][j].revealed = states[i][j];
        }
    }
}

vector<vector<bool>> initializeStatesBoard(int& row, int& col){
    vector<vector<bool>> states(row, vector<bool>(col));
    for(int i = 0 ; i < row ; i++){
        for(int j = 0 ; j < col ; j++){
            states[i][j] = false;
        }
    }
    return states;
}

void drawLeaderboardStuff(sf::RenderWindow& leaderboardWindow, vector<vector<Cell>>& board){

    int width = board[1].size() * 16;
    int height = (board.size() * 16) + 50;

    sf::Font font;
    font.loadFromFile("Font/font.ttf");

    sf::Text tittle("LEADERBOARD", font, 20);
    setText(tittle, (float(width)/2) , ((float(height)/2)-120));
    tittle.setFillColor(sf::Color::White);
    tittle.setStyle(sf::Text::Bold | sf::Text::Underlined);

    leaderboardWindow.draw(tittle);

    string leaderboard = getScores();

    sf::Text Message(leaderboard, font, 20);
    setText(Message, (float(width)/2) , ((float(height)/2)+20));
    Message.setFillColor(sf::Color::White);
    Message.setStyle(sf::Text::Bold);

    leaderboardWindow.draw(Message);
}

void showNearbyTiles(Cell& cell) {
    for(Cell* surrounding : cell.surroundingCells) {
        if(surrounding->value == 0 && !surrounding->revealed) {
            surrounding->revealed=true;
            showNearbyTiles(*surrounding);
        }
        else if(!surrounding->revealed) {
            surrounding->revealed=true;
        }
    }
}

void drawLeaderboard(vector<vector<Cell>>& board, vector<vector<bool>>& states){
    int width = board[1].size() * 16;
    int height = (board.size() * 16) + 50;
    sf::RenderWindow leaderboardWindow(sf::VideoMode(width, height), "Minesweeper");

    while(leaderboardWindow.isOpen()) {
        sf::Event event{};
        while(leaderboardWindow.pollEvent(event)) {
            if(event.type == sf::Event::Closed) {
                leaderboardClicked = false;
                hideAllTiles(board);
                returnValueBoard(board, states);
                leaderboardWindow.close();
            }
        }
        leaderboardWindow.clear(sf::Color::Blue);
        drawLeaderboardStuff(leaderboardWindow, board);
        leaderboardWindow.display();
    }
}

void toggleLeaderboardState(genericButton& button) {
    button.clicked = !button.clicked;
}

void readAllScores(vector<string>& scores){
    ifstream file("Files/leaderboard.txt");
    string line;
    while(getline(file, line)) {
        vector<string> parts = split(line, ',');
        string time = parts[0];
        string nameNew = parts[1];
        string data = time + "," + nameNew;
        scores.push_back(data);
    }

    sort(scores.begin(), scores.end());
    if(scores.size() > 5) {
        scores.erase(scores.begin()+5, scores.end());
    }
    else{
        if(scores.size() < 5){
            for (int i = scores.size(); i <=5; i++) {
                scores.push_back("59:59,----------");
            }
        }
    }
}

vector<string> split(const string& s, char delim) {
    vector<string> result;
    stringstream ss(s);
    string token;

    while (getline(ss, token, delim)) {
        result.push_back(token);
    }
    return result;
}

void isNewHighScore( vector<string>& scores){
    for(int i = 0; i < scores.size() ; i++) {
        if(stoi(winnerTime.substr(0,2)) < stoi(scores[i].substr(0,2))) {
            scores.insert(scores.begin() + i, winnerTime.substr(0,2) + ":" + winnerTime.substr(2,2) + "," + name);
            break;
        }
        else if(stoi(winnerTime.substr(0,2)) == stoi(scores[i].substr(0,2))) {
            if(stoi(winnerTime.substr(3,2)) < stoi(scores[i].substr(3,2))) {
                scores.insert(scores.begin() + i, winnerTime.substr(0,2) + ":" + winnerTime.substr(2,2) + "," + name);
                break;
            }
        }
    }
}

void writeLeaderboard(vector<string>& scores){
    if(scores.size() > 5) {
        scores.erase(scores.begin()+5, scores.end());
    }
    ofstream file("Files/leaderboard.txt");
    for(auto& score : scores) {
        file << score << endl;
    }
}

void checkScores(vector<string>& scores){
    if(scores.size() < 5){
        for (int i = scores.size(); i <=5; i++) {
            scores.push_back("  59:59,----------");
        }
    }
}

string getScores(){

    vector<string> scores;
    readAllScores(scores);

    string firstPlace, secondPlace, thirdPlace, fourthPlace, fifthPlace;
    if(name == scores[0].substr(6) && winnerTime == scores[0].substr(0,2)+scores[0].substr(3,2)){
        firstPlace = "1.\t"+scores[0].substr(0,5)+"\t"+scores[0].substr(6)+"*";
    }
    else{
        firstPlace = "1.\t"+scores[0].substr(0,5)+"\t"+scores[0].substr(6);
    }

    if(name == scores[1].substr(6) && winnerTime == scores[1].substr(0,2)+scores[1].substr(3,2)){
        secondPlace = "2.\t"+scores[1].substr(0,5)+"\t"+scores[1].substr(6)+"*";
    }
    else{
        secondPlace = "2.\t"+scores[1].substr(0,5)+"\t"+scores[1].substr(6);
    }
    if(name == scores[2].substr(6) && winnerTime == scores[2].substr(0,2)+scores[2].substr(3,2)){
        thirdPlace = "3.\t"+scores[2].substr(0,5)+"\t"+scores[2].substr(6)+"*";
    }
    else{
        thirdPlace = "3.\t"+scores[2].substr(0,5)+"\t"+scores[2].substr(6);
    }
    if(name == scores[3].substr(6) && winnerTime == scores[3].substr(0,2)+scores[3].substr(3,2)){
        fourthPlace = "4.\t"+scores[3].substr(0,5)+"\t"+scores[3].substr(6)+"*";
    }
    else{
        fourthPlace = "4.\t"+scores[3].substr(0,5)+"\t"+scores[3].substr(6);
    }
    if(name == scores[4].substr(6) && winnerTime == scores[4].substr(0,2)+scores[4].substr(3,2)){
        fifthPlace = "5.\t"+scores[4].substr(0,5)+"\t"+scores[4].substr(6);
    }
    else{
        fifthPlace = "5.\t"+scores[4].substr(0,5)+"\t"+scores[4].substr(6);
    }
    string leaderboard = firstPlace + "\n" + secondPlace + "\n" + thirdPlace + "\n" + fourthPlace + "\n" + fifthPlace;

    return leaderboard;
}
