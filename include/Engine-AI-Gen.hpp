#pragma once

#include <SFML/Graphics.hpp>

#include "Math.hpp"

#include <cstdint>
#include <vector>

class Engine
{
public:
    Engine();

    void init();
    void run();

private:
    void handleKeyPressed(sf::Keyboard::Key key);
    void addCubeInFrontOfCamera();
    void update(float deltaTime);
    void render();
    void updatePerformanceTitle(float deltaTime);

    sf::RenderWindow window;
    sf::Image frameBuffer;
    sf::Texture frameTexture;
    std::vector<float> depthBuffer;
    std::vector<std::uint8_t> shadowReceiverBuffer;
    std::vector<Vector3> cubePositions;
    float elapsedTime{};
    Vector3 cameraPosition{};
    float cameraYaw{};
    float cameraPitch{};
    float movementSpeed{};
    bool menuVisible{true};
    float lastUpdateMilliseconds{};
    float lastRenderMilliseconds{};
    float lastFrameMilliseconds{};
    float performanceTimer{};
    unsigned int performanceFrameCount{};
};
