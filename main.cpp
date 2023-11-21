#include <fstream>
#include <string>
#include <SFML/Graphics.hpp>
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

chrono::time_point<chrono::steady_clock> start;

bool started = false;

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

    printBoard(board, numRow, numCol);

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
    int flagCounter = 0;
    int mineCounter = 0;
    for (int i = 0 ; i < board.size() ; i++){
        for (int j = 0 ; j < board[i].size() ; j++){
            if(board[i][j].flagged) flagCounter++;
            if(board[i][j].itsMine) mineCounter++;
            sf::RectangleShape tileRectangle(sf::Vector2f(32,32));
            tileRectangle.setPosition(board[i][j].position.x * 32, board[i][j].position.y * 32);

            drawTile(window, board[i][j]);

            if(sf::Mouse::isButtonPressed(sf::Mouse::Left)){
                sf::Vector2i clickPosition = sf::Mouse::getPosition(window);
                if(tileRectangle.getGlobalBounds().contains(sf::Vector2f(clickPosition))){
                    if(board[i][j].value == 9){
                        revealAllMines(board);
                        if(board[i][j].flagged) {board[i][j].flagged = false;}
                    }
                    else if(0 < board[i][j].value && board[i][j].value < 9){
                        board[i][j].revealed = true;
                    }
                    else{
                        board[i][j].revealed = true;
                        showNearByTiles(board, board.size(), board[i].size(), i, j);
                        if(board[i][j].flagged) {board[i][j].flagged = false;}
                    }
                }
            }
            if(sf::Mouse::isButtonPressed(sf::Mouse::Right)){
                sf::Vector2i clickPosition = sf::Mouse::getPosition(window);
                if(tileRectangle.getGlobalBounds().contains(sf::Vector2f(clickPosition))){
                    if(!board[i][j].revealed){
                        changeFlagValue(board[i][j]);
                    }
                }
            }
            if(board[i][j].revealed){
                board[i][j].imagePath = "Images/tile_revealed.png";
                drawNumber(window, board[i][j]);
            }
            if(board[i][j].flagged && !board[i][j].revealed){
                drawFlag(window, board[i][j]);
            }

            if(board[i][j].itsMine && buttons[2].clicked){
                board[i][j].imagePath = "Images/tile_revealed.png";
                drawMine(window, board[i][j]);
            }

            //drawCounter(window, minesCount, board);
        }
    }


    makeTime(window, board);

    string counter = to_string(mineCounter - flagCounter);
    counter = validateNumber(counter);
    drawCounter(window, board, counter);
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
    mineTexture.loadFromFile("Images/flag.png");
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
    if(!started){
        start = chrono::steady_clock::now();
        started = true;
    }
    auto end = chrono::steady_clock::now();
    auto ms = chrono::duration_cast<chrono::milliseconds>(end - start);
    string time = to_string(ms.count() / 1000);
    string t = secondsToMinutes(time);
    drawClock(window, board, t);
}
