#include "Canvas.h"

#include <corecrt_math_defines.h>
#include <iostream>
#include <sstream>
#include <SFML/Window.hpp>

#include "MagicMissile.h"
#include "PointDefenseAI.h"
#include "shipIds.h"
#include "SpaceObject.h"
#include "WorldState.h"

void Canvas::simpleLineInWorld(float ax, float ay, float bx, float by, const sf::Color& color)
{
    sf::VertexArray line(sf::PrimitiveType::Lines, 2);
    line[0].position = sf::Vector2f(ax, ay);
    line[0].color = color;
    line[1].position = sf::Vector2f(bx, by);
    line[1].color = color;
    window.draw(line, worldToScreen());
}

void Canvas::lineBetweenRel(float ax, float ay, float bx, float by, const sf::Color& color, float radius)
{
    radius *= 2; //using width
    // Calculate the start and end points of the line
    sf::Vector2f start(ax, ay);
    sf::Vector2f end(bx, by);
    if (start == end)
        return;
    // Calculate the direction vector of the line
    sf::Vector2f direction = end - start;
    float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    direction /= length; // Normalize the direction vector

    // Calculate the perpendicular vector to determine the radius of the line
    sf::Vector2f perpendicular(-direction.y, direction.x);
    perpendicular *= radius / 2.0f;

    // Define the vertices for the triangle strip
    sf::VertexArray line(sf::PrimitiveType::TriangleStrip, 4);
    line[0].position = start + perpendicular;
    line[0].color = color;
    line[1].position = start - perpendicular;
    line[1].color = color;
    line[2].position = end + perpendicular;
    line[2].color = color;
    line[3].position = end - perpendicular;
    line[3].color = color;

    // Draw the line
    window.draw(line, worldToScreen());
}

sf::Transform Canvas::worldToScreen() const
{
    auto zeroPosToCenter = sf::Transform::Identity;
    zeroPosToCenter.translate(sf::Vector2f(static_cast<double>(window.getSize().x) / 2,
                                           static_cast<double>(window.getSize().y) / 2));
    return zeroPosToCenter * scaleT * shift * userShift; //translation is from right to left
}

sf::Vector2f Canvas::worldToScreen(const pos3d& worldPos) const
{
    return worldToScreen().transformPoint(sf::Vector2f(worldPos.x, worldPos.y));
}

pos3d Canvas::screenToWorld(const sf::Vector2f& screenPos) const
{
    auto worldPosFlat = worldToScreen().getInverse().transformPoint(screenPos);
    return pos3d{worldPosFlat.x, worldPosFlat.y, 0};
}

int roundToNearestPowerOfTen(int value)
{
    if (value <= 0)
    {
        return 1; // Handle non-positive values as needed
    }

    // Find the nearest power of 10
    int power = static_cast<int>(log10(value) + 0.5);
    int nearestPowerOfTen = static_cast<int>(pow(10, power));

    return std::max(1, nearestPowerOfTen);
}

void Canvas::drawSpiderWeb(positionable referenceObj)
{
    sf::Color gridColor(200, 200, 200, 100); // Light gray with some transparency

    {
        int radius = 5000 / scale;
        int r2 = radius / 500;
        int points = 24;
        auto circle = sf::CircleShape(radius, points);
        auto innerCircle = sf::CircleShape(r2, points);
        for (int i = 0; i < circle.getPointCount(); i++)
        {
            auto point = circle.getPoint(i);
            auto point2 = innerCircle.getPoint(i);
            simpleLineInWorld(referenceObj.position.x + point2.x - r2, referenceObj.position.y + point2.y - r2,
                              referenceObj.position.x + point.x - radius, referenceObj.position.y + point.y - radius,
                              gridColor);
        }
    }
    double step = roundToNearestPowerOfTen(100. / scale) * 2.5;
    for (int i = 1; i < 50; i++)
    {
        double radius = i * step * scale;
        auto circle = sf::CircleShape(radius);
        circle.setPosition(worldToScreen(referenceObj.position));
        circle.setOrigin(sf::Vector2f(radius, radius));
        circle.setOutlineColor(gridColor);
        circle.setFillColor(sf::Color::Transparent);
        circle.setOutlineThickness(1);
        window.draw(circle, sf::Transform::Identity);
    }
}


void Canvas::drawGrid()
{
    //find worldpos of top left edge
    auto topLeft = screenToWorld(sf::Vector2f(0, 0));
    auto bottomRight = screenToWorld(sf::Vector2f(window.getSize().x, window.getSize().y));
    sf::Color gridColor(200, 200, 200, 100); // Light gray with some transparency

    int step = roundToNearestPowerOfTen(100. / scale) * 2.5;

    // Draw vertical lines
    int shiftX = static_cast<int>(topLeft.x) % step;
    for (int x = topLeft.x - shiftX; x < bottomRight.x; x += step)
    {
        simpleLineInWorld(x, topLeft.y, x, bottomRight.y, gridColor);
    }

    // Draw horizontal lines
    int shiftY = static_cast<int>(topLeft.y) % step;
    for (int y = topLeft.y - shiftY; y <= bottomRight.y; y += step)
    {
        simpleLineInWorld(topLeft.x, y, bottomRight.x, y, gridColor);
    }
}


Canvas::Canvas() : window(sf::VideoMode({1920, 1240}), "Clustered Orbit 3.0")
{
    font = sf::Font("MartianMono.ttf"); // Throws sf::Exception if an error occurs

    sf::Texture texture;
    if (!texture.loadFromFile("4-Vesta.png", false, sf::IntRect({0, 0}, {1280, 1280})))
    {
        // error...
        std::cout << "ERROR LOADING TEXTURE";
    }
    marsTexture = texture;
    if (!this->spaceshipTexture.loadFromFile("SpaceShip.png", false, sf::IntRect({0, 0}, {1280, 1280})))
    {
        // error...
        std::cout << "ERROR LOADING TEXTURE";
    }
    if (!this->backgroundTexture.loadFromFile("sternen_himmel.PNG", false, sf::IntRect({0, 0}, {1938, 1293})))
    {
        // error...
        std::cout << "ERROR LOADING TEXTURE";
    }
}

std::string doubleToStr(double d)
{
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2) << d;
    return ss.str();
}

sf::CircleShape getCircle(double radius, sf::Color outline = sf::Color::Red,
                          sf::Color fill = sf::Color::White)
{
    sf::CircleShape circle; // Radius of 20 pixels
    circle.setRadius(radius);
    circle.setFillColor(fill);
    circle.setOutlineColor(outline);
    circle.setOutlineThickness(2); // Set the thickness of the outline
    circle.setOrigin(sf::Vector2f(circle.getRadius(), circle.getRadius()));

    return circle;
}

sf::RectangleShape Canvas::guiSquare(pos3d worldPos, int iconSize, sf::Color fill, sf::Color outline,
                                     float outlineThickness)
{
    auto icon = sf::RectangleShape(sf::Vector2f(iconSize, iconSize));
    icon.setOrigin(sf::Vector2f(iconSize / 2, iconSize / 2));
    icon.setPosition(worldToScreen(worldPos));
    icon.setFillColor(fill);
    icon.setOutlineColor(outline);
    icon.setOutlineThickness(outlineThickness);
    return icon;
}

void Canvas::updateWorldstate()
{
    auto old = current;
    assert(old != nullptr);
    auto updated = old->update(currentTime, currentTime - lastUpdate);
    assert(updated->getSize() != 0);
    worldstate_mutex.lock();
    current = updated;    
    lastUpdate = currentTime;
    worldstate_mutex.unlock();
}

std::shared_ptr<WorldState> Canvas::getCurrentWorldState()
{
    worldstate_mutex.lock();
    auto ptr = current;
    worldstate_mutex.unlock();
    return ptr;
};

std::string formatSpeed(double speed, const std::string& prefix = "")
{
    if (speed < 100)
        return doubleToStr(round(speed)) + prefix+"m/s";
    else
        return doubleToStr(round(speed)/1000.) +prefix+"km/s";
}

void Canvas::drawTacticalIcon(const SpaceObject& obj, const SpaceObject& refObj, bool useRef)
{
    int iconSize = 1;
    std::string displayText;
    sf::Shape* icon = nullptr;
    positionable posi = useRef ? obj.positionable - refObj.positionable : obj.positionable;
    auto screenPosObj = worldToScreen(obj.positionable.position);
    
    sf::Text text(font);
    text.setCharacterSize(15);
    text.setFillColor(colorByFaction(obj.meta.faction));
    text.setPosition(sf::Vector2f(screenPosObj));
    
    displayText = obj.positionable.toCharId(obj.globalObjectId) +
                "\n" + objectTypeToString(obj.meta.type) + "\n";
    if (posi.isAccelerated())
    {
        displayText += doubleToStr(posi.acceleration.norm()/9.81) + (useRef ? "G" : "G") + "\n";
    }
    if (posi.isLinear() || posi.isAccelerated())
        displayText += formatSpeed(posi.velocity.norm()) +"\n";
    
    switch (obj.meta.type)
    {
    case ObjectType::MISSILE:
        {
            iconSize = 16;
            text.setCharacterSize(16);
            icon = new sf::CircleShape(iconSize * .7, 3);
            icon->setRotation(sf::degrees(0));
            icon->setOrigin(sf::Vector2f(iconSize * .7, iconSize * .7));
            break;
        }
    case ObjectType::SHIP:
        {
            iconSize = 32;
            text.setCharacterSize(20);
            icon = new sf::RectangleShape(sf::Vector2f(iconSize, iconSize));
            icon->setOrigin(sf::Vector2f(iconSize / 2, iconSize / 2));
            break;
        }
    case ObjectType::ASTEROID:
        {
            iconSize = 32;
            text.setCharacterSize(12);
            icon = new sf::CircleShape(iconSize * .7, 5);
            icon->setOrigin(sf::Vector2f(iconSize * .7, iconSize * .7));
            break;
        }
    case ObjectType::PROJECTILE:
        {
            iconSize = 4;
            displayText = "";
            icon = new sf::RectangleShape(sf::Vector2f(iconSize, iconSize));
            icon->setOrigin(sf::Vector2f(iconSize/2., iconSize /2.));
            break;
        }
    }
    assert(icon);
    icon->setRotation(sf::degrees(obj.meta.faction == OpFor ? 45 : 0));
    icon->setOutlineColor(colorByFaction(obj.meta.faction));
    icon->setOutlineThickness(2);
    icon->setFillColor(sf::Color::Transparent);
    icon->setPosition(sf::Vector2f(screenPosObj));
    window.draw(*icon, sf::Transform::Identity);
    delete icon;

    text.setOrigin(sf::Vector2f(-iconSize / 2, -iconSize / 2));
    text.setString(displayText);

    window.draw(text, sf::Transform::Identity);
}

void Canvas::drawTacticalRanges(const SpaceObject& obj)
{
    if (obj.meta.type != ObjectType::SHIP ||
        (obj.meta.faction != BluFor && obj.meta.faction != OpFor && obj.meta.faction != Independent))
        return;

    std::vector<int> ranges(3);
    ranges[0] = PD_TURRET_RANGE;
    ranges[1] = RAILGUN_DANGERZONE;
    ranges[2] = RAILGUN_DEADZONE;
    for (auto radius : ranges)
    {
        auto radiusV = scaleT.transformPoint(sf::Vector2f(radius, radius));
        auto icon = sf::CircleShape(radiusV.x);
        auto color = colorByFaction(obj.meta.faction);
        color.a = 128;

        icon.setOutlineThickness(1);
        icon.setFillColor(sf::Color::Transparent);
        icon.setOutlineColor(color);
        icon.setOrigin(radiusV);
        icon.setPosition(worldToScreen(obj.positionable.position));
        window.draw(icon, sf::Transform::Identity);
    }
}

void Canvas::drawPhysicalObject(const SpaceObject& obj, double currentTime)
{
    const sf::Texture& texture = obj.meta.type == ObjectType::ASTEROID ? marsTexture : spaceshipTexture;
    sf::Sprite sprite(texture);

    sprite.scale(sf::Vector2f(1. / texture.getSize().x, 1. / texture.getSize().y));
    sprite.setPosition(sf::Vector2f(obj.positionable.position.x, obj.positionable.position.y));
    sprite.scale(sf::Vector2f(obj.positionable.radius * 2, obj.positionable.radius * 2));
    sprite.setOrigin({sf::Vector2f(texture.getSize().x / 2., texture.getSize().y / 2.)});

    auto axis = pos3d(0, 1, 0);
    auto a = obj.positionable.acceleration.normalized();
    double angle = std::atan2(a.x, a.y) + M_PI;
    sprite.rotate(-sf::radians(angle));
    window.draw(sprite, worldToScreen());

    // Create a circle shape for the object
    sf::CircleShape circle; // Radius of 20 pixels
    double screenRadius = scaleT.transformPoint(sf::Vector2f(obj.positionable.radius, 0)).x;
    circle.setRadius(screenRadius);
    circle.setOrigin(sf::Vector2f(screenRadius, screenRadius));

    circle.setFillColor(sf::Color::Transparent);
    circle.setOutlineColor(sf::Color::White);
    circle.setOutlineThickness(1); // Set the thickness of the outline
    auto worldPos = obj.positionable.position;
    circle.setPosition(worldToScreen(worldPos));

    window.draw(circle, sf::Transform::Identity);
}

void Canvas::drawTextWorld(const std::string& string, pos3d worldPos, int fontSize, sf::Color color)
{
    sf::Text text(font);
    text.setCharacterSize(fontSize);
    text.setString(string);
    text.setCharacterSize(18);
    text.setFillColor(sf::Color::White);
    text.setFillColor(color);
    text.setPosition(worldToScreen(worldPos));
    window.draw(text);
}

void Canvas::drawCollissions(const std::vector<CollissionPoint>& colls, double currentTime)
{
    for (auto& c : colls)
    {
        if (c.time < currentTime)
            continue;
        auto shape = sf::RectangleShape(sf::Vector2f(10, 10));
        shape.setFillColor(sf::Color::Yellow);
        shape.setOrigin(sf::Vector2f(shape.getSize().x / 2, shape.getSize().y / 2));
        shape.setPosition(worldToScreen(c.positions));
        window.draw(shape, sf::Transform::Identity);

        std::stringstream ss;

        ss << std::setprecision(2) << "ETA: " << (c.time - currentTime) << "s";

        drawTextWorld(ss.str(), c.positions);
    }
}

void Canvas::drawPath(const SpaceObject& obj, double currentTime, bool physicalVisible)
{
    //draw path
    if (obj.meta.type == ObjectType::SHIP || obj.meta.type == ObjectType::MISSILE)
    {
        pos3d previous = obj.positionable.posAt(currentTime);
        bool drawBBXPath = physicalVisible;
        const double timeStep = std::max(3, roundToNearestPowerOfTen(1 / scale) / 10) / 3.;
        const int totalSteps = 100;
        for (int t = 0; t < totalSteps; t++)
        {
            double i = std::ceil(currentTime / 5) * 5 + t * timeStep;
            double color = (1. - t / totalSteps) * 255;
            auto sfColor = sf::Color(color, color, color, 128 + 64);
            //draw velocity
            pos3d posNext = obj.positionable.posAt(i);
            if (drawBBXPath)
                lineBetweenRel(previous.x, previous.y, posNext.x, posNext.y, sfColor,
                               obj.positionable.radius);
            simpleLineInWorld(previous.x, previous.y, posNext.x, posNext.y, sfColor);
            window.draw(guiSquare(posNext, 4, sfColor, sfColor), sf::Transform::Identity);

            if (t % 5 == 0)
            {
                std::stringstream ss;
                ss << round(i - currentTime) << "s";
                drawTextWorld(ss.str(), posNext);
            }
            previous = posNext;
        }
    }
}

void Canvas::drawTarget(const SpaceObject& target, const SpaceObject& playerShip, double currentTime)
{
    if (target.globalObjectId == playerShip.globalObjectId)
        return;
    auto targetObj = target.objectAt(currentTime);
    auto circle = getCircle(30, sf::Color::Yellow, sf::Color::Transparent);
    circle.setPosition(worldToScreen(targetObj.positionable.position));
    window.draw(circle, sf::Transform::Identity);

    // DRAW MINIMAL DIST POINT TO TARGET

    double t_min_abs = Collission::nextExtremePointDistanceBetween(
        targetObj.positionable, playerShip.positionable, currentTime);
    if (t_min_abs == -1)
        t_min_abs = currentTime;
    {
        pos3d posMinPlayer = playerShip.positionable.posAt(t_min_abs);
        pos3d posMinTarget = targetObj.positionable.posAt(t_min_abs);
        simpleLineInWorld(posMinPlayer.x, posMinPlayer.y, posMinTarget.x, posMinTarget.y,
                          sf::Color::Yellow);
        pos3d middle = (posMinPlayer + posMinTarget) / 2.;

        std::stringstream ss;
        ss << std::setprecision(2) << "ETA: " << (t_min_abs - currentTime) << "s, d=" <<
            std::setprecision(6) << round((posMinPlayer - posMinTarget).norm()) << "m";
        drawTextWorld(ss.str(), middle);
    }
}

void Canvas::drawPositionableInfo(const SpaceObject& obj, double currentTime)
{
    auto objNow = obj.objectAt(currentTime);
    //draw thrust vector
    if (obj.meta.type == ObjectType::SHIP)
        lineBetweenRel(objNow.positionable.position.x, objNow.positionable.position.y,
                       objNow.positionable.position.x + objNow.positionable.acceleration.x * 5.,
                       objNow.positionable.position.y + objNow.positionable.acceleration.y * 5., sf::Color::Cyan);
}


void Canvas::draw(std::shared_ptr<WorldState> worldState, std::vector<SpaceObject> state, double currentTime)
{
    if (worldState->eventHandler->playerState.anchorGlobalId != 0)
    {
        auto anchorObjMaybe = worldState->getObject(worldState->eventHandler->playerState.anchorGlobalId, currentTime);
        if (anchorObjMaybe && worldState->eventHandler->playerState.anchorGlobalId != 0)
        {
            auto pos = anchorObjMaybe.value().positionable.posAt(currentTime);
            shift = sf::Transform::Identity;
            shift.translate(sf::Vector2f(-pos.x, -pos.y));
        }
    }

    // Clear the window with a black background
    window.clear(sf::Color::Black);

    // DRAW BACKGROUND SPRITE
    sf::Sprite sprite(backgroundTexture);
    sprite.setColor(sf::Color(128, 128, 128, 255));
    window.draw(sprite);

    if (worldState->eventHandler->playerState.isRelativeSpace)
    {
        auto ref = worldState->getObject(worldState->eventHandler->playerState.referenceObjectId, currentTime);
        if (ref)
            drawSpiderWeb(ref.value().positionable.objectAt(currentTime));
    }
    else
        drawGrid();


    // draw collissions
    drawCollissions(worldState->collission_points_, currentTime);

    auto playerMaybe = worldState->getObject(REDSHIP_ID, currentTime);
    auto playerTarget = worldState->eventHandler->playerState.targetGlobalId != 0 ? worldState->getObject(worldState->eventHandler->playerState.targetGlobalId, currentTime) : std::nullopt;
    if (playerTarget && playerMaybe)
    {
        drawTarget(playerTarget.value(), playerMaybe.value(), currentTime);
    }
    
    // Draw each positionable object
    for (const auto& obj : worldState->objects)
    {
        if (!obj.lifetime.existsAt(currentTime))
            continue;
        auto radiusScreen = scaleT.transformPoint(sf::Vector2f(obj.positionable.radius, 0)).x;
        bool phyiscallyVisisble =  (radiusScreen > 1);
        
        drawPath(obj, currentTime, phyiscallyVisisble);

        auto objNow = obj.objectAt(currentTime);
        drawPositionableInfo(obj, currentTime);
        if (phyiscallyVisisble)
            drawPhysicalObject(objNow, currentTime);
        drawTacticalIcon(objNow, playerMaybe.has_value() ?  playerMaybe.value().objectAt(currentTime) : objNow, worldState->eventHandler->playerState.isRelativeSpace);
        drawTacticalRanges(objNow);
    }

    sf::Text text(font);
    double number = currentTime;
    std::stringstream ss;
    // TIME

    ss << std::fixed << std::setprecision(2) << number << "ms\n";
    // SCALE
    ss << 1. / scale << "scale\n";

    // GRID SIZE
    int gridSize = roundToNearestPowerOfTen(100 / scale) * 2.5;
    std::string unit = " m";
    if (gridSize >= 1000)
    {
        gridSize /= 1000;
        unit = " km";
    }
    ss.imbue(std::locale(""));
    ss << "1:" << gridSize << unit << "\n";

    // SELECTED WEAPON
    ss << "selected weapon: " << worldState->eventHandler->playerState.selectedWeapon << "\n";
    // TARGET INFO
    {
        auto self = worldState->getObject(REDSHIP_ID, currentTime);
        auto target =worldState->eventHandler->playerState.targetGlobalId != 0 ? worldState->getObject(worldState->eventHandler->playerState.targetGlobalId, currentTime) : std::nullopt;
        if (self && target)
        {
            double dist = round(self.value().positionable.distanceToObjectAt(currentTime, target.value().positionable));
            ss << "target distance=" << dist << "m\n";
        }
    }


    std::string displayText = ss.str();
    text.setString(displayText);
    text.setCharacterSize(32);
    text.setFillColor(sf::Color::White);
    text.setPosition(sf::Vector2f(0, 0));
    window.draw(text);

    // DRAW USER MOUSE
    auto mouseScreen = sf::Mouse::getPosition(window);
    auto mouseWorld = screenToWorld(sf::Vector2f(mouseScreen.x, mouseScreen.y));
    window.draw(guiSquare(pos3d(mouseWorld.x, mouseWorld.y, 0), guiCursorSize, sf::Color::Transparent,
                          sf::Color::Yellow),
                sf::Transform::Identity);

    // Display the updated window
    window.display();
}


std::vector<SpaceObject> ShipDefendAgainstMissile(SpaceObject gunShip, SpaceObject missile, double bulletSpeed = 200,
                                                  double flightTime = 1)
{
    double startTime = missile.lifetime.start;
    gunShip = gunShip.objectAt(startTime);
    missile = missile.objectAt(startTime);
    //find time t, where the position of gunShip at t and position of missile in t+1 is exactly the gunspeed
    // that way, a bullet fired at t, would fly 1 second and then impact the missile
    auto distanceSqEq = Collission::distanceSquaredBetweenObjectsByTime(
        gunShip.positionable, missile.positionable.objectAt(flightTime));
    double desiredDistanceSq = bulletSpeed * flightTime * bulletSpeed * flightTime;
    distanceSqEq.e -= desiredDistanceSq;

    auto solutionTimes = distanceSqEq.findRoots();
    std::vector<SpaceObject> bullets;
    bullets.reserve(solutionTimes.size());
    for (auto time : solutionTimes)
    {
        assert(
            (gunShip.positionable.posAt(time)-missile.positionable.posAt(time+flightTime)).norm() <= bulletSpeed *
            flightTime + 0.1);
        if (time < 0)
            continue;
        SpaceObject bullet(
            positionable{
                gunShip.positionable.posAt(time),
                missile.positionable.posAt(time + flightTime) - gunShip.positionable.posAt(time),
                acc3d{}, 1
            },
            lifeTime{0, 1000000},
            ObjectMetaInfo{ObjectType::PROJECTILE, gunShip.meta.faction},
            SpaceObject::nextId++
        );
        //normalize bullet to t = timeStart
        bullet = bullet.objectAt(-time);

        assert(bullet.positionable.distanceToObjectAt(time + 0,gunShip.positionable) <= 0.1);
        assert(
            bullet.positionable.distanceToObjectAt(time + 0,missile.positionable.objectAt(flightTime)) <= bulletSpeed *
            flightTime + 0.1);
        //distance between startpoint and where missile will be in one seconds, is exactly the distance the bullet can travel in one seconds
        //    assert((bullet.positionable.posAt(time+ flightTime) - missile.positionable.posAt(time + flightTime)).norm() <= 0.1);


        bullet = bullet.objectAt(-startTime);
        bullets.push_back(bullet);
    }
    return bullets;
}


std::shared_ptr<WorldState> spawnDemo()
{
    auto initial = std::make_shared<WorldState>(20);
    for (int i = 0; i < 1; i++)
    {
        double radius = 1000 * MagicMissile::random(1., 20.);
        auto asteroid = SpaceObject{
            positionable{
                pos3d{-75000 + 2. * i * radius, radius * MagicMissile::random(-5., 5.), 0},
                vel3d{0, 0, 0},
                acc3d{0, 0, 0},
                static_cast<int>(radius)
            },
            lifeTime{0, 1000000},
            ObjectMetaInfo{ObjectType::ASTEROID, None},
            SpaceObject::nextId++
        };
        initial->putObject(asteroid, 0);
    }
    /*   auto asteroid = SpaceObject{
           positionable{pos3d{-75000, 0, 0}, vel3d{0, 0, 0}, acc3d{0, 0, 0}, 20000},
           lifeTime{0, 1000000},
           ObjectMetaInfo{ObjectType::ASTEROID, None},
           ASTEROID_ID
       }; 
       initial->putObject(asteroid);*/

    auto civShip = SpaceObject(
        positionable{pos3d{-5000, -500, 0}, vel3d{100, 100, 0}, acc3d{-100 / 3., -100 / 5., 0}, 10},
        lifeTime{0, 1000000},
        ObjectMetaInfo{ObjectType::SHIP, Civilian},
        CIVSHIP_ID);
    initial->putObject(civShip, 0);

    auto redShip = SpaceObject(
        positionable{pos3d{-2500, 3000, 0}, vel3d{100, 100, 0}, acc3d{8, -16, 0}, 25},
        lifeTime{0, 1000000},
        ObjectMetaInfo{ObjectType::SHIP, OpFor},
        REDSHIP_ID
    );
    initial->putObject(redShip, 0);
    auto bluShip = SpaceObject(
        positionable{pos3d{1500, -1500, 1}, vel3d{30, 0, 0}, acc3d{0, 10, 0}, 15},
        lifeTime{0, 1000000},
        ObjectMetaInfo{ObjectType::SHIP, BluFor},
        BLUSHIP_ID
    );
    bluShip.updater = std::make_unique<PointDefenseAI>(bluShip.meta);

    initial->putObject(bluShip, 0);

    initial->calculateCollissions(0);
    initial->eventHandler = new BasicPuppetMaster();
    return initial;
}

void Canvas::display()
{
    auto start = std::chrono::system_clock::now();
    auto millisStart = std::chrono::duration_cast<std::chrono::milliseconds>(start.time_since_epoch());

    double drawTimeout = 0.01;
    long counter = 0;
    long updateCounter = 0;
    scale = scale * 0.1;
    scaleT = scaleT.scale(sf::Vector2f(.1, .1));
    double animationSpeed = 1;
    current = spawnDemo();
    bool pause = false;
    // Keep the window open and handle events
    while (window.isOpen())
    {
        // check all the window's events that were triggered since the last iteration of the loop
        while (const std::optional event = window.pollEvent())
        {
            // "close requested" event: we close the window
            if (event->is<sf::Event::Closed>())
                window.close();
            else if (const auto* wheelEvent = event->getIf<sf::Event::MouseWheelScrolled>())
            {
                double multi = (wheelEvent->delta < 0) ? 1 / 1.5 : 1.5;
                //zoom out
                if (1 / (scale * multi) >= .1 && 1 / (scale * multi) < 1000000)
                {
                    scale *= multi;
                    scaleT = scaleT.scale(sf::Vector2f(multi, multi));
                }
            }
            else if (const auto* keyPressed = event->getIf<sf::Event::MouseButtonPressed>())
            {
                auto clickedScreenPos = keyPressed->position;

                bool selected = false;
                if (keyPressed->button == sf::Mouse::Button::Left || keyPressed->button == sf::Mouse::Button::Middle)
                {
                    double closestDistScreen = 0.;
                    SpaceObject* closestObj = nullptr;
                    //TEST IF ANY OBJECT WAS CLICKED
                    for (auto& obj : getCurrentWorldState()->objects)
                    {
                        auto objectWorldPos = obj.objectAt(counter / 1000).positionable.position;
                        auto objectScreenPos = worldToScreen(objectWorldPos);
                        objectScreenPos.x -= clickedScreenPos.x;
                        objectScreenPos.y -= clickedScreenPos.y;
                        if (closestObj == nullptr || objectScreenPos.length() < closestDistScreen)
                        {
                            closestObj = &obj;
                            closestDistScreen = objectScreenPos.length();
                        }
                    }
                    if (closestObj != nullptr)
                    {
                        if (keyPressed->button == sf::Mouse::Button::Middle)
                        {
                            selected = true;
                            // center on target
                            if (closestDistScreen > guiCursorSize)
                            {
                                getCurrentWorldState()->eventHandler->playerState.anchorGlobalId = 0; //RESET
                            }
                            else
                            {
                                getCurrentWorldState()->eventHandler->playerState.anchorGlobalId = closestObj->globalObjectId;
                                userShift = sf::Transform::Identity;
                            }
                        }
                        else if (keyPressed->button == sf::Mouse::Button::Left && closestDistScreen < guiCursorSize)
                        {
                            selected = true;
                            getCurrentWorldState()->eventHandler->onPlayerSelectObject(closestObj->globalObjectId);
                        }
                    }
                }
                if (!selected)
                {
                    auto worldPos = screenToWorld(sf::Vector2f(clickedScreenPos));
                    if (keyPressed->button == sf::Mouse::Button::Left)
                    {
                        // THRUST TOWARDS CLICKED POS
                        auto playerNow = getCurrentWorldState()->getObject(REDSHIP_ID, counter / 1000).value().positionable.objectAt(counter / 1000);
                        auto thrustDir = (worldPos - playerNow.position).normalized();
                        getCurrentWorldState()->eventHandler->onPlayerInputThrustTowards(thrustDir.x, thrustDir.y);
                        clickedPos = sf::Vector2(worldPos.x, worldPos.y);
                    }
                    else if (keyPressed->button == sf::Mouse::Button::Right)
                    {
                        auto player = getCurrentWorldState()->getObject(REDSHIP_ID, counter / 1000).value().objectAt(counter / 1000.);
                        player.positionable.position = pos3d(worldPos.x, worldPos.y, 0);
                        getCurrentWorldState()->putObject(player.objectAt(-counter / 1000.), counter/1000);
                    }
                }
            }
            else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
            {
                if (keyPressed->scancode == sf::Keyboard::Scancode::Space)
                    pause = !pause;
                if (keyPressed->scancode == sf::Keyboard::Scancode::Left)
                    counter -= drawTimeout * 1000;
                if (keyPressed->scancode == sf::Keyboard::Scancode::Right)
                    counter += drawTimeout * 1000;

                int movementSpeed = 150 / scale;
                if (keyPressed->scancode == sf::Keyboard::Scancode::A)
                    userShift = userShift.translate(sf::Vector2f(movementSpeed, 0));
                if (keyPressed->scancode == sf::Keyboard::Scancode::D)
                    userShift = userShift.translate(sf::Vector2f(-movementSpeed, 0));
                if (keyPressed->scancode == sf::Keyboard::Scancode::S)
                    userShift = userShift.translate(sf::Vector2f(0, -movementSpeed));
                if (keyPressed->scancode == sf::Keyboard::Scancode::W)
                    userShift = userShift.translate(sf::Vector2f(0, movementSpeed));

                if (keyPressed->scancode == sf::Keyboard::Scancode::R) // RELOAD SIMULATION
                {
                    current = spawnDemo();
                    counter = 0;
                }

                if (keyPressed->scancode == sf::Keyboard::Scancode::Num1)
                    getCurrentWorldState()->eventHandler->onPlayerSelectWeapon(Weapon::POINTDEFENSE);
                if (keyPressed->scancode == sf::Keyboard::Scancode::Num2)
                    getCurrentWorldState()->eventHandler->onPlayerSelectWeapon(Weapon::RAILGUN);
                if (keyPressed->scancode == sf::Keyboard::Scancode::Num3)
                    getCurrentWorldState()->eventHandler->onPlayerSelectWeapon(Weapon::MISSILE);
                if (keyPressed->scancode == sf::Keyboard::Scancode::F) // FIRE MISSILE
                    getCurrentWorldState()->eventHandler->onPlayerInputFireMissile();

                if (keyPressed->scancode == sf::Keyboard::Scancode::Q)
                {
                    getCurrentWorldState()->eventHandler->playerState.isRelativeSpace = !getCurrentWorldState()->eventHandler->playerState.
                        isRelativeSpace;
                    getCurrentWorldState()->eventHandler->playerState.referenceObjectId = REDSHIP_ID;
                }
            }
        }
        draw(getCurrentWorldState(), getCurrentWorldState()->aliveObjectsAt((counter / 1000.) / animationSpeed), counter / 1000.);

        if (!pause)
        {
            auto end = std::chrono::system_clock::now();
            auto millisEnd = std::chrono::duration_cast<std::chrono::milliseconds>(end.time_since_epoch());
            long delta = millisEnd.count() - millisStart.count() - counter ;
            counter = millisEnd.count() - millisStart.count();
            updateCounter += delta;
            if (updateCounter >= FIXED_UPDATE_DELTA * 1000)
            {
                updateCounter = 0;
                currentTime = counter/1000.;
                std::cout << "UPDATE current ptr =" << getCurrentWorldState() << std::endl;
                updateWorldstate();
            }
        }
    }
}
