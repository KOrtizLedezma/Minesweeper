//
// Created by kenet on 11/29/2023.
//

#include <SFML/Graphics.hpp>

#ifndef MINESWEEPER_ARTIST_H
#define MINESWEEPER_ARTIST_H


class artist {
public:

    void setText(sf::Text &text, float x, float y){
        sf::FloatRect textRect = text.getLocalBounds();
        text.setOrigin(textRect.left + textRect.width/2.0f,textRect.top + textRect.height/2.0f);
        text.setPosition(sf::Vector2f(x, y));
    }

    sf::Text drawWelcomeMessage(int width, int height, sf::Font font){
        sf::Text welcomeMessage("WELCOME TO MINESWEEPER!", font, 24);
        setText(welcomeMessage, (float(width)/2) , ((float(height)/2)-150));
        welcomeMessage.setFillColor(sf::Color::White);
        welcomeMessage.setStyle(sf::Text::Bold | sf::Text::Underlined);

        return welcomeMessage;
    }

    sf::Text drawOrderMessage(int width, int height, sf::Font font){
        sf::Text orderMessage("Enter your name:", font, 20);
        setText(orderMessage, (float(width)/2) , ((float(height)/2)-75));
        orderMessage.setFillColor(sf::Color::White);
        orderMessage.setStyle(sf::Text::Bold);

        return orderMessage;
    }

    sf::Text drawInputName(int width, int height, sf::Font font){
        sf::Text inputName("", font, 20);
        setText(inputName, (float(width)/2) , ((float(height)/2)-40));
        inputName.setFillColor(sf::Color::Yellow);
        inputName.setStyle(sf::Text::Bold);

        return inputName;
    }

    sf::RectangleShape drawFaceRectangle(int x, int y){
        sf::RectangleShape faceRectangle(sf::Vector2f(64,64));
        faceRectangle.setPosition(x, y);

        return faceRectangle;
    }

    sf::Sprite drawFaceSprite(sf::Texture& faceTexture, int x, int y){
        sf::Sprite faceSprite;
        faceSprite.setTexture(faceTexture);
        faceSprite.setTextureRect(sf::IntRect(0, 0, 64, 64));
        faceSprite.setPosition(x, y);
    }
};


#endif //MINESWEEPER_ARTIST_H
