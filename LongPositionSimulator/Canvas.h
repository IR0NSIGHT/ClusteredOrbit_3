#ifndef CANVAS_H
#define CANVAS_H

#include <mutex>
#include <SFML/Graphics.hpp>
#include <vector>

#include "positionable.h"
#include "SpaceObject.h"
#include "vel3d.h"

struct CollissionPoint;
class SpaceObject;
class WorldState;

class Canvas
{
public:
    Canvas();
    void draw(std::shared_ptr<WorldState> worldState, std::vector<SpaceObject> state, double currentTime);
    void display();

    /**
     * \brief 
     */
    sf::Vector2<double> clickedPos;

private:
    int guiCursorSize = 30;
    sf::Font font;
    double scale = 1;
    sf::Texture marsTexture;
    sf::Texture spaceshipTexture;
    sf::Texture backgroundTexture;
    sf::Transform userShift = sf::Transform::Identity;
    sf::Transform shift = sf::Transform::Identity;
    sf::Transform scaleT = sf::Transform::Identity;
    sf::RenderWindow window; // Directly declare the window as a member
    sf::CircleShape circle;
    sf::Transform worldToScreen() const;
    sf::Vector2f worldToScreen(const pos3d& worldPos) const;
    pos3d screenToWorld(const sf::Vector2f& screenPos) const;
    
    void drawSpiderWeb(positionable referenceObj);
    void drawGrid();
    void simpleLineInWorld(float ax, float ay, float bx, float by, const sf::Color& color);
    void lineBetweenRel(float ax, float ay, float bx, float by, const sf::Color& color, float radius = 4);
    void drawTacticalIcon(const SpaceObject& obj, const SpaceObject& refObj, bool useRef);
    void drawTacticalRanges(const SpaceObject& obj);
    void drawPhysicalObject(const SpaceObject& obj, double currentTime);
    void drawTextWorld(const std::string& string, pos3d worldPos, int fontSize = 18, sf::Color color = sf::Color::White);
    void drawCollissions(const std::vector<CollissionPoint>& colls, double currentTime);
    void drawPath(const SpaceObject& obj, double currentTime, bool physicalVisible);
    void drawTarget(const SpaceObject& target, const SpaceObject& player, double currentTime);
    void drawPositionableInfo(const SpaceObject& obj, double currentTime);
    sf::RectangleShape guiSquare(pos3d worldPos, int iconSize, sf::Color fill, sf::Color outline, float outlineThickness = 1);

    std::shared_ptr<WorldState> current;
    double currentTime = 0;
    double lastUpdate = 0;
    void updateWorldstate();

    std::mutex worldstate_mutex;
    std::shared_ptr<WorldState> getCurrentWorldState();
};

#endif // CANVAS_H
