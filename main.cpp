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
__attribute__((unused)) void printBoard(const vector<vector<Cell>>& board, int& numRow, int& numCol);
void drawGame(sf::RenderWindow& window, vector<vector<Cell>>& board, vector<genericButton>& buttons, bool& game);
void drawFace(sf::RenderWindow& window, vector<vector<Cell>>& board);
void drawDebugButton(sf::RenderWindow& window, vector<genericButton>& buttons, vector<vector<Cell>>& board);
void drawPauseButton(sf::RenderWindow& window, vector<genericButton>& buttons, vector<vector<Cell>>& board);
void drawLeaderboardButton(sf::RenderWindow& window, vector<vector<Cell>>& board);
void drawPlayZone(sf::RenderWindow& window, vector<vector<Cell>>& board, vector<genericButton>& buttons, bool& game);
string chooseImage(int value);
vector<genericButton> createButtonsVector();
void showNearByTiles(vector<vector<Cell>>& board, int numRow, int numCol, int& click_x, int& click_y);
void printBoolBoard(const vector<vector<Cell>>& board, int numRow, int numCol);
void revealAllMines(vector<vector<Cell>>& board);

int main() {

    enum State { Welcome, Game };
    State currentState = Welcome;

    bool game = true;
    int rowCount = 0;
    int colCount = 0;
    int minesCount = 0;

    readConfigFile(colCount, rowCount, minesCount);
    vector<genericButton> buttons = createButtonsVector();

    const int width = (colCount * 32);
    const int height = (rowCount * 32) + 100;

    vector<vector<Cell>> board = createBoard(colCount, rowCount, minesCount);
    //printBoard(board, rowCount, colCount);

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
                drawGame(window, board, buttons, game);
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
    return board;
}

// Prints all the values on the board
__attribute__((unused)) void printBoard(const vector<vector<Cell>>& board, int& numRow, int& numCol){
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

// Prints all the bool revealed values on the board
void printBoolBoard(const vector<vector<Cell>>& board, int numRow, int numCol){
    for (int i = 0 ; i < numRow ; i++){
        for (int j = 0 ; j < numCol ; j++){
            cout << board[i][j].revealed <<"  ";
        }
        cout << endl;
    }
}

// Calls all the method needed to draw the game
void drawGame(sf::RenderWindow& window, vector<vector<Cell>>& board, vector<genericButton>& buttons, bool& game){
    drawFace(window, board);
    drawDebugButton(window, buttons, board);
    drawPauseButton(window, buttons, board);
    drawLeaderboardButton(window, board);
    drawPlayZone(window, board, buttons, game);
}

// Draws the face Button, and controls the clicks
void drawFace(sf::RenderWindow& window, vector<vector<Cell>>& board){
    float x = ((float(board[1].size())/2.f) * 32) - 32;
    float y = 32 * (float(board.size()) + 0.5f);
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

            if(sf::Mouse::isButtonPressed(sf::Mouse::Left)){
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
void drawPauseButton(sf::RenderWindow& window, vector<genericButton>& buttons, vector<vector<Cell>>& board){
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

// Draws the Leaderboard Button, and controls the clicks
void drawLeaderboardButton(sf::RenderWindow& window, vector<vector<Cell>>& board){
    float x = ((float(board[1].size())) * 32) - 176;
    float y = 32 * (float(board.size()) + 0.5f);
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

// Draws the Play zone, all the tiles, numbers, mines and flags are drawn here, and controls the clicks
void drawPlayZone(sf::RenderWindow& window, vector<vector<Cell>>& board, vector<genericButton>& buttons, bool& game){

    for (int i = 0 ; i < board.size() ; i++){
        for (int j = 0 ; j < board[i].size() ; j++){
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
                    if(board[i][j].value == 9){
                        revealAllMines(board);
                        game = false;
                        if(board[i][j].flagged){
                            board[i][j].flagged = false;
                        }
                    }
                    else{
                        board[i][j].revealed = true;
                        board[i][j].imagePath = chooseImage(board[i][j].value);
                        showNearByTiles(board, board.size(), board[i].size(), i, j);
                        printBoolBoard(board, board.size(), board[i].size());
                        if(board[i][j].flagged){
                            board[i][j].flagged = false;
                        }
                    }
                }
            }
            if(sf::Mouse::isButtonPressed(sf::Mouse::Right)){
                sf::Vector2i clickPosition = sf::Mouse::getPosition(window);
                if(tileRectangle.getGlobalBounds().contains(sf::Vector2f(clickPosition))){
                    board[i][j].flagged = true;
                    board[i][j].imagePath = "Images/flag_revealed.png";
                }
            }
            if(board[i][j].revealed){
                sf::Texture revealedTexture;
                revealedTexture.loadFromFile("Images/tile_revealed.png");
                sf::Sprite revealedSprite;
                revealedSprite.setTexture(revealedTexture);
                revealedSprite.setTextureRect(sf::IntRect(0, 0, 32, 32));
                revealedSprite.setPosition((board[i][j].position.x * 32), (board[i][j].position.y * 32) );
                window.draw(revealedSprite);
            }
            window.draw(tileSprite);
            if(board[i][j].flagged){
                sf::Texture revealedTexture;
                revealedTexture.loadFromFile("Images/flag.png");
                sf::Sprite revealedSprite;
                revealedSprite.setTexture(revealedTexture);
                revealedSprite.setTextureRect(sf::IntRect(0, 0, 32, 32));
                revealedSprite.setPosition((board[i][j].position.x * 32), (board[i][j].position.y * 32) );
                window.draw(revealedSprite);
            }

            if(board[i][j].itsMine && buttons[2].clicked){
                sf::Texture revealedTexture;
                revealedTexture.loadFromFile("Images/mine.png");
                sf::Sprite revealedSprite;
                revealedSprite.setTexture(revealedTexture);
                revealedSprite.setTextureRect(sf::IntRect(0, 0, 32, 32));
                revealedSprite.setPosition((board[i][j].position.x * 32), (board[i][j].position.y * 32) );
                window.draw(revealedSprite);
            }
        }
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

    return buttons;
}

// Shows all nearby tiles when its clicked
void showNearByTiles(vector<vector<Cell>>& board, int numRow, int numCol, int& click_x, int& click_y){
    for(int x = max(click_x-1, 0); x <= min(click_x+1, numRow-1) ; x++){
        for(int y = max(click_y-1, 0); y <= min(click_y+1, numCol-1) ; y++){
            if(board[x][y].value == 0 && !board[x][y].revealed){
                board[x][y].revealed = true;
                board[x][y].imagePath = chooseImage(board[x][y].value);
                showNearByTiles(board, numRow, numCol, x, y);
            }
            else if(board[x][y].value >= 1 && board[x][y].value <= 8 && !board[x][y].revealed){
                board[x][y].revealed = true;
                board[x][y].imagePath = chooseImage(board[x][y].value);
            }
        }
    }
}

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