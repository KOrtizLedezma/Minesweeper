#include <fstream>
#include <string>
#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include "Cell.h"
#include "genericButton.h"

using namespace std;

void setText(sf::Text &text, float x, float y);
void readConfigFile(int& numCol, int& numRow, int& numMines);
vector<vector<Cell>> createBoard(int& numCol, int& numRow, int& numMines);
void printBoard(const vector<vector<Cell>>& board, int& numRow, int& numCol);
void drawGame(sf::RenderWindow& window, const int& numCol, const int& numRow, vector<vector<Cell>>& board, vector<genericButton>& buttons);
void drawFace(sf::RenderWindow& window, const int& numCol, const int& numRow);
void drawMineButton(sf::RenderWindow& window, const int& numCol, const int& numRow);
void drawPauseButton(sf::RenderWindow& window, const int& numCol, const int& numRow, vector<genericButton>& buttons);
void drawLeaderboardButton(sf::RenderWindow& window, const int& numCol, const int& numRow);
void drawPlayZone(sf::RenderWindow& window, const int& numCol, const int& numRow, vector<vector<Cell>>& board);
string chooseImage(int value);
vector<genericButton> createButtonsVector();

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
    printBoard(board, rowCount, colCount);

    sf::RenderWindow window(sf::VideoMode(width, height), "Minesweeper");
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

    string name;

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
                drawGame(window, colCount, rowCount, board, buttons);
                break;
        }
        inputName.setString(name + "|");

        window.display();
    }
}

void setText(sf::Text &text, float x, float y){
    sf::FloatRect textRect = text.getLocalBounds();
    text.setOrigin(textRect.left + textRect.width/2.0f,textRect.top + textRect.height/2.0f);
    text.setPosition(sf::Vector2f(x, y));
}

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
    srand(static_cast<unsigned>(time(nullptr)));

    for(int x = 0 ; x < numMines ; x++){
        int randomRow = rand() % numRow;
        int randomCol = rand() % numCol;

        for(int i = 0 ;  i < numRow ; i++){
            for (int j = 0 ; j < numCol; j++){
                if(board[i][j].position == sf::Vector2f (float(randomRow), float(randomCol))){
                    board[i][j].value = 9;
                }
            }
        }
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
    return board;
}

void printBoard(const vector<vector<Cell>>& board, int& numRow, int& numCol){
    for (int i = 0 ; i < numRow ; i++){
        for (int j = 0 ; j < numCol ; j++){
            if(board[i][j].value == 9){
                cout << "X  ";
            }
            else{
                cout << board[i][j].value << "  ";
            }
        }
        cout << endl;
    }
}

void drawGame(sf::RenderWindow& window, const int& numCol, const int& numRow, vector<vector<Cell>>& board, vector<genericButton>& buttons){
    drawFace(window, numCol, numRow);
    drawMineButton(window, numCol, numRow);
    drawPauseButton(window, numCol, numRow, buttons);
    drawLeaderboardButton(window, numCol, numRow);
    drawPlayZone(window, numCol, numRow, board);
}

void drawFace(sf::RenderWindow& window, const int& numCol, const int& numRow){
    float x = ((float(numCol)/2.f) * 32) - 32;
    float y = 32 * (float(numRow) + 0.5f);
    sf::Texture faceTexture;
    faceTexture.loadFromFile("Images/face_happy.png");

    sf::RectangleShape faceRectangle(sf::Vector2f(64,64));
    faceRectangle.setPosition(x, y);

    sf::Sprite faceSprite;
    faceSprite.setTexture(faceTexture);
    faceSprite.setTextureRect(sf::IntRect(0, 0, 64, 64));
    faceSprite.setPosition(x, y);

    window.draw(faceRectangle);
    window.draw(faceSprite);
}

void drawMineButton(sf::RenderWindow& window, const int& numCol, const int& numRow){
    float x = ((float(numCol)) * 32) - 304;
    float y = 32 * (float(numRow) + 0.5f);
    sf::Texture mineTexture;
    mineTexture.loadFromFile("Images/debug.png");

    sf::RectangleShape mineRectangle(sf::Vector2f(64,64));
    mineRectangle.setPosition(x, y);

    sf::Sprite mineSprite;
    mineSprite.setTexture(mineTexture);
    mineSprite.setTextureRect(sf::IntRect(0, 0, 64, 64));
    mineSprite.setPosition(x, y);
    window.draw(mineRectangle);
    window.draw(mineSprite);
}

void drawPauseButton(sf::RenderWindow& window, const int& numCol, const int& numRow, vector<genericButton>& buttons){
    float x = ((float(numCol)) * 32) - 240;
    float y = 32 * (float(numRow) + 0.5f);

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

            if(sf::Mouse::isButtonPressed(sf::Mouse::Left)){
                sf::Vector2i clickPosition = sf::Mouse::getPosition(window);
                if(pauseRectangle.getGlobalBounds().contains(sf::Vector2f(clickPosition))){
                    if(buttons[i].imagePath == "Images/play.png"){
                        buttons[i].imagePath = "Images/pause.png";
                    }
                    else if(buttons[i].imagePath == "Images/pause.png"){
                        buttons[i].imagePath = "Images/play.png";
                    }
                }
            }

            window.draw(pauseRectangle);
            window.draw(pauseSprite);
        }
    }
}

void drawLeaderboardButton(sf::RenderWindow& window, const int& numCol, const int& numRow){
    float x = ((float(numCol)) * 32) - 176;
    float y = 32 * (float(numRow) + 0.5f);
    sf::Texture leaderboardTexture;
    leaderboardTexture.loadFromFile("Images/leaderboard.png");

    sf::RectangleShape leaderboardRectangle(sf::Vector2f(64,64));
    leaderboardRectangle.setPosition(x, y);

    sf::Sprite leaderboardSprite;
    leaderboardSprite.setTexture(leaderboardTexture);
    leaderboardSprite.setTextureRect(sf::IntRect(0, 0, 64, 64));
    leaderboardSprite.setPosition(x, y);
    window.draw(leaderboardRectangle);
    window.draw(leaderboardSprite);
}

void drawPlayZone(sf::RenderWindow& window, const int& numCol, const int& numRow, vector<vector<Cell>>& board){

    for (int i = 0 ; i < numRow ; i++){
        for (int j = 0 ; j < numCol ; j++){
            sf::Texture tileTexture;
            tileTexture.loadFromFile(board[i][j].imagePath);

            sf::RectangleShape tileRectangle(sf::Vector2f(32,32));
            tileRectangle.setPosition(board[i][j].position.x * 32, board[i][j].position.y * 32);

            sf::Sprite tileSprite;
            tileSprite.setTexture(tileTexture);
            tileSprite.setTextureRect(sf::IntRect(0, 0, 32, 32));
            tileSprite.setPosition(board[i][j].position.x * 32, board[i][j].position.y * 32);

            if(sf::Mouse::isButtonPressed(sf::Mouse::Left)){
                sf::Vector2i clickPosition = sf::Mouse::getPosition(window);
                if(tileRectangle.getGlobalBounds().contains(sf::Vector2f(clickPosition))){
                    board[i][j].imagePath = chooseImage(board[i][j].value);
                }
            }

            window.draw(tileRectangle);
            window.draw(tileSprite);
        }
    }
}

string chooseImage(int value){
    string path;
    if(value == 0) path = "Images/tile_revealed.png";
    else if(value == 1) path = "Images/number_1_revealed.png";
    else if(value == 2) path = "Images/number_2_revealed.png";
    else if(value == 3) path = "Images/number_3_revealed.png";
    else if(value == 4) path = "Images/number_4_revealed.png";
    else if(value == 5) path = "Images/number_5_revealed.png";
    else if(value == 6) path = "Images/number_6_revealed.png";
    else if(value == 7) path = "Images/number_7_revealed.png";
    else if(value == 8) path = "Images/number_8_revealed.png";
    else path = "Images/mine_revealed.png";

    return path;
}

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

    return buttons;
}