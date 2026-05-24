#pragma once

#include <SFML/Graphics.hpp>

class Engine
{
public:
    Engine();

    void init();
    void run();

private:
    void update(float deltaTime);
    void render();

    sf::RenderWindow window;
    sf::VertexArray projectedLines;
};