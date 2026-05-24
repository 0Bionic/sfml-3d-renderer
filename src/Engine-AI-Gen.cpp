#include "Engine-AI-Gen.hpp"

#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <iomanip>
#include <limits>
#include <sstream>
#include <string>
#include <string_view>

namespace
{
constexpr unsigned int WindowWidth = 800;
constexpr unsigned int WindowHeight = 600;
constexpr float WindowWidthF = static_cast<float>(WindowWidth);
constexpr float WindowHeightF = static_cast<float>(WindowHeight);
constexpr float MouseSensitivity = 0.0025f;
constexpr float InitialCameraMoveSpeed = 4.0f;
constexpr float MinCameraMoveSpeed = 0.5f;
constexpr float MaxCameraMoveSpeed = 20.0f;
constexpr float SpeedStep = 0.5f;
constexpr float SprintMultiplier = 2.0f;
constexpr float NewCubeDistance = 6.0f;
constexpr float MaxPitch = 1.5f;
constexpr float PerformanceUpdateInterval = 0.5f;
constexpr float GroundY = -1.5f;
constexpr float NearPlane = 0.1f;
constexpr float FogStart = 8.0f;
constexpr float FogEnd = 24.0f;
constexpr float AmbientLight = 0.22f;
constexpr float ShadowStrength = 0.35f;
constexpr sf::Color FogColor{8, 10, 16};
const Vector3 LightDirection{-0.45f, -1.0f, -0.35f};
const sf::Vector2i WindowCenter{
    static_cast<int>(WindowWidth / 2),
    static_cast<int>(WindowHeight / 2),
};

const std::array<Vector3, 8> CubeVertices{
    Vector3{-1.0f, -1.0f, -1.0f},
    Vector3{ 1.0f, -1.0f, -1.0f},
    Vector3{ 1.0f,  1.0f, -1.0f},
    Vector3{-1.0f,  1.0f, -1.0f},
    Vector3{-1.0f, -1.0f,  1.0f},
    Vector3{ 1.0f, -1.0f,  1.0f},
    Vector3{ 1.0f,  1.0f,  1.0f},
    Vector3{-1.0f,  1.0f,  1.0f},
};

struct MeshTriangle
{
    std::array<int, 3> indices;
    sf::Color color;
    bool receivesShadows{};
    bool castsShadows{};
};

const std::array<MeshTriangle, 12> CubeTriangles{
    MeshTriangle{{0, 1, 2}, sf::Color{220,  60,  60}, false, true},
    MeshTriangle{{0, 2, 3}, sf::Color{180,  40,  40}, false, true},
    MeshTriangle{{5, 4, 7}, sf::Color{ 60, 220,  60}, false, true},
    MeshTriangle{{5, 7, 6}, sf::Color{ 40, 180,  40}, false, true},
    MeshTriangle{{4, 0, 3}, sf::Color{ 60,  60, 220}, false, true},
    MeshTriangle{{4, 3, 7}, sf::Color{ 40,  40, 180}, false, true},
    MeshTriangle{{1, 5, 6}, sf::Color{220, 220,  60}, false, true},
    MeshTriangle{{1, 6, 2}, sf::Color{180, 180,  40}, false, true},
    MeshTriangle{{3, 2, 6}, sf::Color{220,  60, 220}, false, true},
    MeshTriangle{{3, 6, 7}, sf::Color{180,  40, 180}, false, true},
    MeshTriangle{{4, 5, 1}, sf::Color{ 60, 220, 220}, false, true},
    MeshTriangle{{4, 1, 0}, sf::Color{ 40, 180, 180}, false, true},
};

const std::array<Vector3, 9> InitialCubePositions{
    Vector3{ 0.0f,  0.0f,  5.0f},
    Vector3{-3.0f,  0.0f,  8.0f},
    Vector3{ 3.0f,  0.0f,  8.0f},
    Vector3{ 0.0f,  2.5f, 10.0f},
    Vector3{ 0.0f,  0.0f, 11.0f},
    Vector3{-5.0f,  0.0f, 13.0f},
    Vector3{ 5.0f,  0.0f, 13.0f},
    Vector3{-2.5f,  2.5f, 16.0f},
    Vector3{ 2.5f,  0.0f, 16.0f},
};

const std::array<Vector3, 12> PlaneVertices{
    Vector3{-20.0f, GroundY,  0.0f},
    Vector3{ 20.0f, GroundY,  0.0f},
    Vector3{ 20.0f, GroundY, 30.0f},
    Vector3{-20.0f, GroundY, 30.0f},
    Vector3{-20.0f, GroundY, 24.0f},
    Vector3{ 20.0f, GroundY, 24.0f},
    Vector3{ 20.0f,  8.0f, 24.0f},
    Vector3{-20.0f,  8.0f, 24.0f},
    Vector3{-12.0f, GroundY,  0.0f},
    Vector3{-12.0f, GroundY, 24.0f},
    Vector3{-12.0f,  8.0f, 24.0f},
    Vector3{-12.0f,  8.0f,  0.0f},
};

const std::array<MeshTriangle, 6> PlaneTriangles{
    MeshTriangle{{0, 2, 1}, sf::Color{ 90,  95, 100}, true, false},
    MeshTriangle{{0, 3, 2}, sf::Color{ 90,  95, 100}, true, false},
    MeshTriangle{{4, 6, 5}, sf::Color{ 55,  62,  78}, false, false},
    MeshTriangle{{4, 7, 6}, sf::Color{ 55,  62,  78}, false, false},
    MeshTriangle{{8, 10, 9}, sf::Color{ 48,  58,  72}, false, false},
    MeshTriangle{{8, 11, 10}, sf::Color{ 48,  58,  72}, false, false},
};

sf::Vector2f toScreenSpace(const Vector3& point)
{
    return {
        (point.x + 1.0f) * 0.5f * WindowWidthF,
        (1.0f - point.y) * 0.5f * WindowHeightF,
    };
}

void drawRect(sf::Image& frameBuffer, int left, int top, int width, int height, sf::Color color)
{
    const int startX = std::clamp(left, 0, static_cast<int>(WindowWidth) - 1);
    const int endX = std::clamp(left + width, 0, static_cast<int>(WindowWidth));
    const int startY = std::clamp(top, 0, static_cast<int>(WindowHeight) - 1);
    const int endY = std::clamp(top + height, 0, static_cast<int>(WindowHeight));

    for (int y = startY; y < endY; ++y)
    {
        for (int x = startX; x < endX; ++x)
        {
            frameBuffer.setPixel({static_cast<unsigned int>(x), static_cast<unsigned int>(y)}, color);
        }
    }
}

std::array<std::uint8_t, 7> glyphRows(char character)
{
    switch (character)
    {
        case '0': return {0x0E, 0x11, 0x13, 0x15, 0x19, 0x11, 0x0E};
        case '1': return {0x04, 0x0C, 0x04, 0x04, 0x04, 0x04, 0x0E};
        case '2': return {0x0E, 0x11, 0x01, 0x02, 0x04, 0x08, 0x1F};
        case '3': return {0x1E, 0x01, 0x01, 0x0E, 0x01, 0x01, 0x1E};
        case '4': return {0x02, 0x06, 0x0A, 0x12, 0x1F, 0x02, 0x02};
        case '5': return {0x1F, 0x10, 0x10, 0x1E, 0x01, 0x01, 0x1E};
        case '6': return {0x0E, 0x10, 0x10, 0x1E, 0x11, 0x11, 0x0E};
        case '7': return {0x1F, 0x01, 0x02, 0x04, 0x08, 0x08, 0x08};
        case '8': return {0x0E, 0x11, 0x11, 0x0E, 0x11, 0x11, 0x0E};
        case '9': return {0x0E, 0x11, 0x11, 0x0F, 0x01, 0x01, 0x0E};
        case 'A': return {0x0E, 0x11, 0x11, 0x1F, 0x11, 0x11, 0x11};
        case 'B': return {0x1E, 0x11, 0x11, 0x1E, 0x11, 0x11, 0x1E};
        case 'C': return {0x0F, 0x10, 0x10, 0x10, 0x10, 0x10, 0x0F};
        case 'D': return {0x1E, 0x11, 0x11, 0x11, 0x11, 0x11, 0x1E};
        case 'E': return {0x1F, 0x10, 0x10, 0x1E, 0x10, 0x10, 0x1F};
        case 'F': return {0x1F, 0x10, 0x10, 0x1E, 0x10, 0x10, 0x10};
        case 'G': return {0x0F, 0x10, 0x10, 0x13, 0x11, 0x11, 0x0F};
        case 'H': return {0x11, 0x11, 0x11, 0x1F, 0x11, 0x11, 0x11};
        case 'I': return {0x0E, 0x04, 0x04, 0x04, 0x04, 0x04, 0x0E};
        case 'L': return {0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x1F};
        case 'M': return {0x11, 0x1B, 0x15, 0x15, 0x11, 0x11, 0x11};
        case 'N': return {0x11, 0x19, 0x15, 0x13, 0x11, 0x11, 0x11};
        case 'O': return {0x0E, 0x11, 0x11, 0x11, 0x11, 0x11, 0x0E};
        case 'P': return {0x1E, 0x11, 0x11, 0x1E, 0x10, 0x10, 0x10};
        case 'Q': return {0x0E, 0x11, 0x11, 0x11, 0x15, 0x12, 0x0D};
        case 'R': return {0x1E, 0x11, 0x11, 0x1E, 0x14, 0x12, 0x11};
        case 'S': return {0x0F, 0x10, 0x10, 0x0E, 0x01, 0x01, 0x1E};
        case 'T': return {0x1F, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04};
        case 'U': return {0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x0E};
        case 'V': return {0x11, 0x11, 0x11, 0x11, 0x11, 0x0A, 0x04};
        case 'W': return {0x11, 0x11, 0x11, 0x15, 0x15, 0x15, 0x0A};
        case 'X': return {0x11, 0x11, 0x0A, 0x04, 0x0A, 0x11, 0x11};
        case 'Y': return {0x11, 0x11, 0x0A, 0x04, 0x04, 0x04, 0x04};
        case '+': return {0x00, 0x04, 0x04, 0x1F, 0x04, 0x04, 0x00};
        case '-': return {0x00, 0x00, 0x00, 0x1F, 0x00, 0x00, 0x00};
        case '/': return {0x01, 0x01, 0x02, 0x04, 0x08, 0x10, 0x10};
        case ':': return {0x00, 0x04, 0x04, 0x00, 0x04, 0x04, 0x00};
        case '.': return {0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x0C};
        default: return {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    }
}

void drawText(sf::Image& frameBuffer, int left, int top, std::string_view text, sf::Color color, int scale = 2)
{
    int cursorX = left;

    for (char character : text)
    {
        if (character == ' ')
        {
            cursorX += 4 * scale;
            continue;
        }

        const std::array<std::uint8_t, 7> rows = glyphRows(character);
        for (int row = 0; row < 7; ++row)
        {
            for (int col = 0; col < 5; ++col)
            {
                if ((rows[static_cast<std::size_t>(row)] & (1 << (4 - col))) == 0)
                {
                    continue;
                }

                drawRect(frameBuffer, cursorX + (col * scale), top + (row * scale), scale, scale, color);
            }
        }

        cursorX += 6 * scale;
    }
}

std::string formatOneDecimal(float value)
{
    std::ostringstream stream;
    stream << std::fixed << std::setprecision(1) << value;
    return stream.str();
}

void drawMenuOverlay(sf::Image& frameBuffer, std::size_t cubeCount, float movementSpeed)
{
    drawRect(frameBuffer, 10, 10, 270, 120, sf::Color{0, 0, 0});
    drawRect(frameBuffer, 12, 12, 266, 116, sf::Color{18, 22, 30});

    drawText(frameBuffer, 22, 22, "MENU", sf::Color{220, 230, 240});
    drawText(frameBuffer, 22, 44, "C ADD CUBE", sf::Color{170, 220, 180});
    drawText(frameBuffer, 22, 62, "+/- SPEED " + formatOneDecimal(movementSpeed), sf::Color{210, 210, 170});
    drawText(frameBuffer, 22, 80, "Q EXIT", sf::Color{230, 170, 170});
    drawText(frameBuffer, 22, 98, "CUBES " + std::to_string(cubeCount), sf::Color{180, 200, 235});
}

struct ScreenVertex
{
    sf::Vector2f position;
    float depth{};
};

Vector3 vectorSubtract(const Vector3& lhs, const Vector3& rhs)
{
    return {lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z};
}

Vector3 vectorAdd(const Vector3& lhs, const Vector3& rhs)
{
    return {lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z};
}

Vector3 vectorScale(const Vector3& vector, float scale)
{
    return {vector.x * scale, vector.y * scale, vector.z * scale};
}

float dot(const Vector3& lhs, const Vector3& rhs)
{
    return (lhs.x * rhs.x) + (lhs.y * rhs.y) + (lhs.z * rhs.z);
}

Vector3 cross(const Vector3& lhs, const Vector3& rhs)
{
    return {
        (lhs.y * rhs.z) - (lhs.z * rhs.y),
        (lhs.z * rhs.x) - (lhs.x * rhs.z),
        (lhs.x * rhs.y) - (lhs.y * rhs.x),
    };
}

float length(const Vector3& vector)
{
    return std::sqrt(dot(vector, vector));
}

Vector3 normalize(const Vector3& vector)
{
    const float vectorLength = length(vector);
    if (vectorLength == 0.0f)
    {
        return {};
    }

    return vectorScale(vector, 1.0f / vectorLength);
}

Vector3 calculateFaceNormal(const Vector3& a, const Vector3& b, const Vector3& c)
{
    return normalize(cross(vectorSubtract(b, a), vectorSubtract(c, a)));
}

float clamp01(float value)
{
    return std::clamp(value, 0.0f, 1.0f);
}

sf::Color scaleColor(sf::Color color, float scale)
{
    return {
        static_cast<std::uint8_t>(std::clamp(static_cast<int>(color.r * scale), 0, 255)),
        static_cast<std::uint8_t>(std::clamp(static_cast<int>(color.g * scale), 0, 255)),
        static_cast<std::uint8_t>(std::clamp(static_cast<int>(color.b * scale), 0, 255)),
        color.a,
    };
}

sf::Color blendColor(sf::Color from, sf::Color to, float amount)
{
    const float blendAmount = clamp01(amount);
    const float inverseAmount = 1.0f - blendAmount;

    return {
        static_cast<std::uint8_t>((from.r * inverseAmount) + (to.r * blendAmount)),
        static_cast<std::uint8_t>((from.g * inverseAmount) + (to.g * blendAmount)),
        static_cast<std::uint8_t>((from.b * inverseAmount) + (to.b * blendAmount)),
        from.a,
    };
}

sf::Color shadeTriangleColor(const Vector3& a, const Vector3& b, const Vector3& c, sf::Color baseColor)
{
    const Vector3 normal = calculateFaceNormal(a, b, c);
    const Vector3 lightToSurface = vectorScale(normalize(LightDirection), -1.0f);
    const float diffuse = std::max(dot(normal, lightToSurface), 0.0f);
    const float brightness = AmbientLight + ((1.0f - AmbientLight) * diffuse);

    return scaleColor(baseColor, brightness);
}

sf::Color applyDistanceFog(sf::Color color, float depth)
{
    const float fogAmount = clamp01((depth - FogStart) / (FogEnd - FogStart));
    return blendColor(color, FogColor, fogAmount);
}

float edgeFunction(const sf::Vector2f& a, const sf::Vector2f& b, const sf::Vector2f& point)
{
    return (point.x - a.x) * (b.y - a.y) - (point.y - a.y) * (b.x - a.x);
}

void putPixel(
    sf::Image& frameBuffer,
    std::vector<float>& depthBuffer,
    std::vector<std::uint8_t>* shadowReceiverBuffer,
    bool receivesShadows,
    int x,
    int y,
    float depth,
    sf::Color color)
{
    const auto pixelIndex = static_cast<std::size_t>(y * static_cast<int>(WindowWidth) + x);

    if (depth >= depthBuffer[pixelIndex])
    {
        return;
    }

    depthBuffer[pixelIndex] = depth;
    frameBuffer.setPixel({static_cast<unsigned int>(x), static_cast<unsigned int>(y)}, color);

    if (shadowReceiverBuffer != nullptr)
    {
        (*shadowReceiverBuffer)[pixelIndex] = receivesShadows ? 1U : 0U;
    }
}

void darkenReceiverPixel(sf::Image& frameBuffer, const std::vector<std::uint8_t>& shadowReceiverBuffer, int x, int y)
{
    const auto pixelIndex = static_cast<std::size_t>(y * static_cast<int>(WindowWidth) + x);

    if (shadowReceiverBuffer[pixelIndex] == 0U)
    {
        return;
    }

    const sf::Vector2u coords{static_cast<unsigned int>(x), static_cast<unsigned int>(y)};
    const sf::Color current = frameBuffer.getPixel(coords);
    frameBuffer.setPixel(coords, scaleColor(current, 1.0f - ShadowStrength));
}

void rasterizeTriangle(
    sf::Image& frameBuffer,
    std::vector<float>& depthBuffer,
    std::vector<std::uint8_t>* shadowReceiverBuffer,
    bool receivesShadows,
    const ScreenVertex& a,
    const ScreenVertex& b,
    const ScreenVertex& c,
    sf::Color color)
{
    const float minX = std::min({a.position.x, b.position.x, c.position.x});
    const float maxX = std::max({a.position.x, b.position.x, c.position.x});
    const float minY = std::min({a.position.y, b.position.y, c.position.y});
    const float maxY = std::max({a.position.y, b.position.y, c.position.y});

    const int startX = std::clamp(static_cast<int>(std::floor(minX)), 0, static_cast<int>(WindowWidth) - 1);
    const int endX = std::clamp(static_cast<int>(std::ceil(maxX)), 0, static_cast<int>(WindowWidth) - 1);
    const int startY = std::clamp(static_cast<int>(std::floor(minY)), 0, static_cast<int>(WindowHeight) - 1);
    const int endY = std::clamp(static_cast<int>(std::ceil(maxY)), 0, static_cast<int>(WindowHeight) - 1);

    const float area = edgeFunction(a.position, b.position, c.position);
    if (area == 0.0f)
    {
        return;
    }

    for (int y = startY; y <= endY; ++y)
    {
        for (int x = startX; x <= endX; ++x)
        {
            const sf::Vector2f pixelCenter{static_cast<float>(x) + 0.5f, static_cast<float>(y) + 0.5f};
            const float weightA = edgeFunction(b.position, c.position, pixelCenter) / area;
            const float weightB = edgeFunction(c.position, a.position, pixelCenter) / area;
            const float weightC = edgeFunction(a.position, b.position, pixelCenter) / area;

            if (weightA < 0.0f || weightB < 0.0f || weightC < 0.0f)
            {
                continue;
            }

            const float depth = (a.depth * weightA) + (b.depth * weightB) + (c.depth * weightC);
            putPixel(frameBuffer, depthBuffer, shadowReceiverBuffer, receivesShadows, x, y, depth, color);
        }
    }
}

void rasterizeShadowTriangle(
    sf::Image& frameBuffer,
    const std::vector<std::uint8_t>& shadowReceiverBuffer,
    const ScreenVertex& a,
    const ScreenVertex& b,
    const ScreenVertex& c)
{
    const float minX = std::min({a.position.x, b.position.x, c.position.x});
    const float maxX = std::max({a.position.x, b.position.x, c.position.x});
    const float minY = std::min({a.position.y, b.position.y, c.position.y});
    const float maxY = std::max({a.position.y, b.position.y, c.position.y});

    const int startX = std::clamp(static_cast<int>(std::floor(minX)), 0, static_cast<int>(WindowWidth) - 1);
    const int endX = std::clamp(static_cast<int>(std::ceil(maxX)), 0, static_cast<int>(WindowWidth) - 1);
    const int startY = std::clamp(static_cast<int>(std::floor(minY)), 0, static_cast<int>(WindowHeight) - 1);
    const int endY = std::clamp(static_cast<int>(std::ceil(maxY)), 0, static_cast<int>(WindowHeight) - 1);

    const float area = edgeFunction(a.position, b.position, c.position);
    if (area == 0.0f)
    {
        return;
    }

    for (int y = startY; y <= endY; ++y)
    {
        for (int x = startX; x <= endX; ++x)
        {
            const sf::Vector2f pixelCenter{static_cast<float>(x) + 0.5f, static_cast<float>(y) + 0.5f};
            const float weightA = edgeFunction(b.position, c.position, pixelCenter) / area;
            const float weightB = edgeFunction(c.position, a.position, pixelCenter) / area;
            const float weightC = edgeFunction(a.position, b.position, pixelCenter) / area;

            if (weightA < 0.0f || weightB < 0.0f || weightC < 0.0f)
            {
                continue;
            }

            darkenReceiverPixel(frameBuffer, shadowReceiverBuffer, x, y);
        }
    }
}

void renderWorldTriangle(
    sf::Image& frameBuffer,
    std::vector<float>& depthBuffer,
    std::vector<std::uint8_t>* shadowReceiverBuffer,
    const Matrix4x4& view,
    const Matrix4x4& projection,
    const Vector3& worldA,
    const Vector3& worldB,
    const Vector3& worldC,
    sf::Color baseColor,
    bool receivesShadows)
{
    const Vector3 cameraA = multiply(view, worldA);
    const Vector3 cameraB = multiply(view, worldB);
    const Vector3 cameraC = multiply(view, worldC);

    if (cameraA.z <= NearPlane || cameraB.z <= NearPlane || cameraC.z <= NearPlane)
    {
        return;
    }

    const float averageDepth = (cameraA.z + cameraB.z + cameraC.z) / 3.0f;
    const sf::Color litColor = shadeTriangleColor(worldA, worldB, worldC, baseColor);
    const sf::Color finalColor = applyDistanceFog(litColor, averageDepth);

    const Vector3 projectedA = multiply(projection, cameraA);
    const Vector3 projectedB = multiply(projection, cameraB);
    const Vector3 projectedC = multiply(projection, cameraC);

    rasterizeTriangle(
        frameBuffer,
        depthBuffer,
        shadowReceiverBuffer,
        receivesShadows,
        ScreenVertex{toScreenSpace(projectedA), cameraA.z},
        ScreenVertex{toScreenSpace(projectedB), cameraB.z},
        ScreenVertex{toScreenSpace(projectedC), cameraC.z},
        finalColor);
}

bool projectPointOntoGround(const Vector3& point, Vector3& projectedPoint)
{
    if (std::abs(LightDirection.y) < 0.0001f)
    {
        return false;
    }

    const float lightTravel = (GroundY - point.y) / LightDirection.y;
    if (lightTravel < 0.0f)
    {
        return false;
    }

    projectedPoint = vectorAdd(point, vectorScale(LightDirection, lightTravel));
    projectedPoint.y = GroundY;

    return true;
}

void renderProjectedShadow(
    sf::Image& frameBuffer,
    const std::vector<std::uint8_t>& shadowReceiverBuffer,
    const Matrix4x4& view,
    const Matrix4x4& projection,
    const Vector3& worldA,
    const Vector3& worldB,
    const Vector3& worldC)
{
    Vector3 shadowA;
    Vector3 shadowB;
    Vector3 shadowC;

    if (!projectPointOntoGround(worldA, shadowA) ||
        !projectPointOntoGround(worldB, shadowB) ||
        !projectPointOntoGround(worldC, shadowC))
    {
        return;
    }

    const Vector3 cameraA = multiply(view, shadowA);
    const Vector3 cameraB = multiply(view, shadowB);
    const Vector3 cameraC = multiply(view, shadowC);

    if (cameraA.z <= NearPlane || cameraB.z <= NearPlane || cameraC.z <= NearPlane)
    {
        return;
    }

    const Vector3 projectedA = multiply(projection, cameraA);
    const Vector3 projectedB = multiply(projection, cameraB);
    const Vector3 projectedC = multiply(projection, cameraC);

    rasterizeShadowTriangle(
        frameBuffer,
        shadowReceiverBuffer,
        ScreenVertex{toScreenSpace(projectedA), cameraA.z},
        ScreenVertex{toScreenSpace(projectedB), cameraB.z},
        ScreenVertex{toScreenSpace(projectedC), cameraC.z});
}

bool isKeyDown(sf::Keyboard::Key key)
{
    return sf::Keyboard::isKeyPressed(key);
}

Vector3 yawForward(float yaw)
{
    return {std::sin(yaw), 0.0f, std::cos(yaw)};
}

Vector3 yawRight(float yaw)
{
    return {std::cos(yaw), 0.0f, -std::sin(yaw)};
}

template <typename StartTime, typename EndTime>
float elapsedMilliseconds(StartTime start, EndTime end)
{
    return std::chrono::duration<float, std::milli>(end - start).count();
}
}

Engine::Engine()
    : movementSpeed(InitialCameraMoveSpeed)
{
}

void Engine::init()
{
    window.create(sf::VideoMode({WindowWidth, WindowHeight}), "SFML 3D Software Rasterizer");
    window.setFramerateLimit(60);
    window.setMouseCursorVisible(false);
    sf::Mouse::setPosition(WindowCenter, window);

    frameBuffer.resize({WindowWidth, WindowHeight}, sf::Color::Black);
    if (!frameTexture.resize({WindowWidth, WindowHeight}))
    {
        window.close();
    }
    depthBuffer.resize(static_cast<std::size_t>(WindowWidth * WindowHeight));
    shadowReceiverBuffer.resize(static_cast<std::size_t>(WindowWidth * WindowHeight));
    cubePositions.assign(InitialCubePositions.begin(), InitialCubePositions.end());
}

void Engine::run()
{
    init();

    sf::Clock clock;

    while (window.isOpen())
    {
        const auto frameStart = std::chrono::steady_clock::now();

        while (const auto event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }

            if (event->is<sf::Event::FocusGained>())
            {
                sf::Mouse::setPosition(WindowCenter, window);
            }

            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
            {
                handleKeyPressed(keyPressed->code);
            }
        }

        const float deltaTime = clock.restart().asSeconds();

        const auto updateStart = std::chrono::steady_clock::now();
        update(deltaTime);
        const auto updateEnd = std::chrono::steady_clock::now();

        render();
        const auto renderEnd = std::chrono::steady_clock::now();

        lastUpdateMilliseconds = elapsedMilliseconds(updateStart, updateEnd);
        lastRenderMilliseconds = elapsedMilliseconds(updateEnd, renderEnd);
        lastFrameMilliseconds = elapsedMilliseconds(frameStart, renderEnd);
        updatePerformanceTitle(deltaTime);
    }
}

void Engine::handleKeyPressed(sf::Keyboard::Key key)
{
    if (key == sf::Keyboard::Key::Q)
    {
        window.close();
        return;
    }

    if (key == sf::Keyboard::Key::Tab)
    {
        menuVisible = !menuVisible;
        return;
    }

    if (key == sf::Keyboard::Key::C)
    {
        addCubeInFrontOfCamera();
        return;
    }

    if (key == sf::Keyboard::Key::Equal || key == sf::Keyboard::Key::Add)
    {
        movementSpeed = std::min(movementSpeed + SpeedStep, MaxCameraMoveSpeed);
        return;
    }

    if (key == sf::Keyboard::Key::Hyphen || key == sf::Keyboard::Key::Subtract)
    {
        movementSpeed = std::max(movementSpeed - SpeedStep, MinCameraMoveSpeed);
    }
}

void Engine::addCubeInFrontOfCamera()
{
    const Vector3 forward = yawForward(cameraYaw);
    cubePositions.push_back({
        cameraPosition.x + (forward.x * NewCubeDistance),
        std::max(cameraPosition.y, 0.0f),
        cameraPosition.z + (forward.z * NewCubeDistance),
    });
}

void Engine::update(float deltaTime)
{
    elapsedTime += deltaTime;
    frameBuffer.resize({WindowWidth, WindowHeight}, sf::Color::Black);
    std::fill(depthBuffer.begin(), depthBuffer.end(), std::numeric_limits<float>::max());
    std::fill(shadowReceiverBuffer.begin(), shadowReceiverBuffer.end(), 0U);

    if (window.hasFocus())
    {
        const sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
        const sf::Vector2i mouseDelta = mousePosition - WindowCenter;

        cameraYaw += static_cast<float>(mouseDelta.x) * MouseSensitivity;
        cameraPitch += static_cast<float>(mouseDelta.y) * MouseSensitivity;
        cameraPitch = std::clamp(cameraPitch, -MaxPitch, MaxPitch);

        sf::Mouse::setPosition(WindowCenter, window);

        const Vector3 forward = yawForward(cameraYaw);
        const Vector3 right = yawRight(cameraYaw);
        const float speedMultiplier = isKeyDown(sf::Keyboard::Key::LShift) ? SprintMultiplier : 1.0f;
        const float movement = movementSpeed * speedMultiplier * deltaTime;

        if (isKeyDown(sf::Keyboard::Key::W))
        {
            cameraPosition.x += forward.x * movement;
            cameraPosition.z += forward.z * movement;
        }
        if (isKeyDown(sf::Keyboard::Key::S))
        {
            cameraPosition.x -= forward.x * movement;
            cameraPosition.z -= forward.z * movement;
        }
        if (isKeyDown(sf::Keyboard::Key::D))
        {
            cameraPosition.x += right.x * movement;
            cameraPosition.z += right.z * movement;
        }
        if (isKeyDown(sf::Keyboard::Key::A))
        {
            cameraPosition.x -= right.x * movement;
            cameraPosition.z -= right.z * movement;
        }
        if (isKeyDown(sf::Keyboard::Key::Space))
        {
            cameraPosition.y += movement;
        }
        if (isKeyDown(sf::Keyboard::Key::LControl))
        {
            cameraPosition.y -= movement;
        }
    }

    const Matrix4x4 viewTranslation = makeTranslationMatrix(-cameraPosition.x, -cameraPosition.y, -cameraPosition.z);
    const Matrix4x4 viewRotationY = makeRotationYMatrix(-cameraYaw);
    const Matrix4x4 viewRotationX = makeRotationXMatrix(-cameraPitch);
    const Matrix4x4 view = multiply(multiply(viewRotationX, viewRotationY), viewTranslation);
    const Matrix4x4 projection = makeProjectionMatrix(90.0f, WindowWidthF / WindowHeightF, 0.1f, 100.0f);

    const Matrix4x4 cubeRotationX = makeRotationXMatrix(elapsedTime * 0.4f);
    const Matrix4x4 cubeRotationY = makeRotationYMatrix(elapsedTime * 0.6f);
    const Matrix4x4 cubeRotation = multiply(cubeRotationY, cubeRotationX);

    for (const MeshTriangle& triangle : PlaneTriangles)
    {
        renderWorldTriangle(
            frameBuffer,
            depthBuffer,
            &shadowReceiverBuffer,
            view,
            projection,
            PlaneVertices[triangle.indices[0]],
            PlaneVertices[triangle.indices[1]],
            PlaneVertices[triangle.indices[2]],
            triangle.color,
            triangle.receivesShadows);
    }

    for (const Vector3& cubePosition : cubePositions)
    {
        const Matrix4x4 cubeTranslation = makeTranslationMatrix(cubePosition.x, cubePosition.y, cubePosition.z);
        const Matrix4x4 world = multiply(cubeTranslation, cubeRotation);

        for (const MeshTriangle& triangle : CubeTriangles)
        {
            if (!triangle.castsShadows)
            {
                continue;
            }

            renderProjectedShadow(
                frameBuffer,
                shadowReceiverBuffer,
                view,
                projection,
                multiply(world, CubeVertices[triangle.indices[0]]),
                multiply(world, CubeVertices[triangle.indices[1]]),
                multiply(world, CubeVertices[triangle.indices[2]]));
        }
    }

    for (const Vector3& cubePosition : cubePositions)
    {
        const Matrix4x4 cubeTranslation = makeTranslationMatrix(cubePosition.x, cubePosition.y, cubePosition.z);
        const Matrix4x4 world = multiply(cubeTranslation, cubeRotation);

        for (const MeshTriangle& triangle : CubeTriangles)
        {
            renderWorldTriangle(
                frameBuffer,
                depthBuffer,
                &shadowReceiverBuffer,
                view,
                projection,
                multiply(world, CubeVertices[triangle.indices[0]]),
                multiply(world, CubeVertices[triangle.indices[1]]),
                multiply(world, CubeVertices[triangle.indices[2]]),
                triangle.color,
                triangle.receivesShadows);
        }
    }

    if (menuVisible)
    {
        drawMenuOverlay(frameBuffer, cubePositions.size(), movementSpeed);
    }
    else
    {
        drawText(frameBuffer, 12, 12, "TAB MENU", sf::Color{180, 190, 205});
    }

    frameTexture.update(frameBuffer);
}

void Engine::render()
{
    window.clear(sf::Color::Black);
    window.draw(sf::Sprite{frameTexture});
    window.display();
}

void Engine::updatePerformanceTitle(float deltaTime)
{
    performanceTimer += deltaTime;
    ++performanceFrameCount;

    if (performanceTimer < PerformanceUpdateInterval)
    {
        return;
    }

    const float fps = static_cast<float>(performanceFrameCount) / performanceTimer;
    const float cpuMilliseconds = lastUpdateMilliseconds + lastRenderMilliseconds;

    std::ostringstream title;
    title << std::fixed << std::setprecision(2)
          << "SFML 3D Software Rasterizer"
          << " | FPS: " << std::setprecision(1) << fps
          << " | Frame: " << std::setprecision(2) << lastFrameMilliseconds << " ms"
          << " | CPU: " << cpuMilliseconds << " ms"
          << " | Raster: " << lastUpdateMilliseconds << " ms"
          << " | Present: " << lastRenderMilliseconds << " ms"
          << " | Speed: " << movementSpeed
          << " | Cubes: " << cubePositions.size()
          << " | Q quit";

    window.setTitle(title.str());

    performanceTimer = 0.0f;
    performanceFrameCount = 0;
}
