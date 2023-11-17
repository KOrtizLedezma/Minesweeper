#include <iostream>
#include <SFML/Graphics.hpp>

void setText(sf::Text &text, float x, float y);

using namespace std;

int main() {

    enum State { Welcome, Game };
    State currentState = Welcome;

    const int rowCount = 16;
    const int colCount = 25;

    const int width = (colCount * 32);
    const int height = (rowCount * 32) + 100;

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
        sf::Event event;
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

void createArray(int rowCount, int colCount){

}
