#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <SFML/Graphics.hpp>
#include "Settl.h"

#define SIZE_X 100
#define SIZE_Y 100
#define MULTI 8

sf::RenderWindow window(sf::VideoMode(SIZE_X * MULTI, SIZE_Y * MULTI), "Settlers");
sf::Image img;
sf::Texture txr;
sf::Sprite spr;

int main()
{
    srand(time(NULL));
    window.setFramerateLimit(20);
    Settl::generate(100, 0, SIZE_X, 0, SIZE_Y);
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        if(Settl::sett_vec.size() > 1)
        {
            img.create(SIZE_X * MULTI, SIZE_Y * MULTI, sf::Color(50, 10, 10));
            Settl::move_all();
            for(int i_1(0); i_1 < SIZE_X * MULTI; i_1++)
            {
                for(int i_2(0); i_2 < SIZE_Y * MULTI; i_2++)
                {
                    if((i_1 > Settl::get_area(false).x * MULTI && i_1 < Settl::get_area(true).x * MULTI)
                    && (i_2 > Settl::get_area(false).y * MULTI && i_2 < Settl::get_area(true).y * MULTI))
                            img.setPixel(i_1, i_2, sf::Color(0, 0, 0));
                }
            }
            for(int i(0); i < Settl::sett_vec.size(); i++)
            {
                for(int x(0); x < MULTI && Settl::sett_vec[i]->position().x * MULTI + x >= 0 && Settl::sett_vec[i]->position().x * MULTI + x < SIZE_X * MULTI; x++)
                {
                    for(int y(0); y < MULTI && Settl::sett_vec[i]->position().y * MULTI + x >= 0
                                            && Settl::sett_vec[i]->position().y * MULTI + x < SIZE_Y * MULTI; y++)
                        img.setPixel(Settl::sett_vec[i]->position().x * MULTI + x,
                                     Settl::sett_vec[i]->position().y * MULTI + y,
                                     sf::Color(((Settl::sett_vec[i]->personality() % 3) ? 255 : 0),
                                               ((Settl::sett_vec[i]->personality() > 1) ? 255 : 0),
                                               ((Settl::sett_vec[i]->personality() == 0) ? 255 : 0)));
                }
            }
        }
        else
        {
            printf("%s\n", Settl::sett_vec[0]->c_stats());
            window.close();
        }
        txr.loadFromImage(img);
        spr.setTexture(txr);
        window.clear();
        window.draw(spr);
        window.display();
    }
    for(int i(Settl::sett_vec.size() - 1); i >= 0; i--)
    {
        delete Settl::sett_vec[i];
        Settl::sett_vec[i] = nullptr;
        Settl::sett_vec.pop_back();
    }
    return 0;
}
