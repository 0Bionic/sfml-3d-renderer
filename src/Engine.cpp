#include "Engine.hpp"

Engine::Engine()
    : projectedLines(sf::PrimitiveType::Lines)
{
}

void Engine::init()
{
    window.create(sf::VideoMode({800, 600}), "SFML 3D Software Rasterizer");
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
    // TODO: Implement math.
}

void Engine::render()
{
    window.clear(sf::Color::Black);
    window.display();
}
