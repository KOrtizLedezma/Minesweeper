#include <fstream>
#include <string>
#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>

using namespace std;

void setText(sf::Text &text, float x, float y);
void readConfigFile(int& numCol, int& numRow, int& numMines);
vector<vector<sf::Vector2f>> createBoard(int& numCol, int& numRow, int& numMines);
void printBoard(const vector<vector<sf::Vector2f>>& board);
void drawGame(sf::RenderWindow& window, const int& numCol, const int& numRow);
void drawFace(sf::RenderWindow& window, const int& numCol, const int& numRow);
void drawMineButton(sf::RenderWindow& window, const int& numCol, const int& numRow);
void drawPauseButton(sf::RenderWindow& window, const int& numCol, const int& numRow);
void drawLeaderboardButton(sf::RenderWindow& window, const int& numCol, const int& numRow);

int main() {

    enum State { Welcome, Game };
    State currentState = Welcome;

    int rowCount = 0;
    int colCount = 0;
    int minesCount = 0;

    readConfigFile(colCount, rowCount, minesCount);

    const int width = (colCount * 32);
    const int height = (rowCount * 32) + 100;

    vector<vector<sf::Vector2f>> board = createBoard(colCount, rowCount, minesCount);
    printBoard(board);

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
                drawGame(window, colCount, rowCount);
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

vector<vector<sf::Vector2f>> createBoard(int& numCol, int& numRow, int& numMines){
    cout << "createBoard" << endl;

    vector<vector<sf::Vector2f>> board(numRow, vector<sf::Vector2f>(numCol));

    for(int i = 0 ;  i < numRow ; i++){
        for (int j = 0 ; j < numCol; j++){
            board[i][j] = sf::Vector2f(float(j) * 30.f, float(i) * 30.f);
        }
    }

    srand(static_cast<unsigned>(time(nullptr)));

    for(int i = 0 ; i < numMines ; i++){
        int randomRow = rand() % numRow;
        int randomCol = rand() % numCol;

        board[randomRow][randomCol] = sf::Vector2f(-1.f, -1.f);
    }

    for (int i = 0 ; i < numRow ; i++){
        for (int j = 0 ; j < numCol ; j++){
            if(board[i][j].x == -1.f && board[i][j].y == -1.f){
                continue;
            }

            int counter = 0;
            for(int x = max(i-1, 0); x <= min(i+1, numRow-1) ; x++){
                for(int y = max(j-1, 0); y <= min(j+1, numCol-1) ; y++){
                    if(board[x][y].x == -1.f && board[x][y].y == -1.f){
                        counter++;
                    }
                }
            }

            board[i][j].x = float(counter);
            board[i][j].y = float(counter);
        }
    }
    return board;
}

void printBoard(const vector<vector<sf::Vector2f>>& board){
    for(const auto& row : board){
        for(const auto& cell : row){
            if(cell.x == -1.f && cell.y == -1.f){
                cout << "X  ";
            }
            else{
                cout << cell.x << "  ";
            }
        }

        cout << endl;
    }
}

void drawGame(sf::RenderWindow& window, const int& numCol, const int& numRow){
    drawFace(window, numCol, numRow);
    drawMineButton(window, numCol, numRow);
    drawPauseButton(window, numCol, numRow);
    drawLeaderboardButton(window, numCol, numRow);
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

void drawPauseButton(sf::RenderWindow& window, const int& numCol, const int& numRow){
    float x = ((float(numCol)) * 32) - 240;
    float y = 32 * (float(numRow) + 0.5f);
    sf::Texture pauseTexture;
    pauseTexture.loadFromFile("Images/pause.png");

    sf::RectangleShape pauseRectangle(sf::Vector2f(64,64));
    pauseRectangle.setPosition(x, y);

    sf::Sprite pauseSprite;
    pauseSprite.setTexture(pauseTexture);
    pauseSprite.setTextureRect(sf::IntRect(0, 0, 64, 64));
    pauseSprite.setPosition(x, y);
    window.draw(pauseRectangle);
    window.draw(pauseSprite);
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