#include "Engine.hpp"

namespace
{
constexpr unsigned int WindowWidth = 800;
constexpr unsigned int WindowHeight = 600;
}

Engine::Engine()
    : projectedLines(sf::PrimitiveType::Lines)
{
}

void Engine::init()
{
    window.create(sf::VideoMode({WindowWidth, WindowHeight}), "SFML 3D Software Rasterizer");
    window.setFramerateLimit(60);
}

void Engine::run()
{
    init();

    sf::Clock clock;

    while (window.isOpen())
    {
        while (const auto event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
        }

        const float deltaTime = clock.restart().asSeconds();
        update(deltaTime);
        render();
    }
}

void Engine::update(float deltaTime)
{
    (void)deltaTime;
    projectedLines.clear();
    // TODO: Implement engine update.
}

void Engine::render()
{
    window.clear(sf::Color::Black);
    window.draw(projectedLines);
    window.display();
}