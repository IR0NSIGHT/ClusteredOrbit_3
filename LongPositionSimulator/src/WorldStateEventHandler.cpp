#include "../include/WorldStateEventHandler.h"

#include "../include/Collission.h"

void WorldStateEventHandler::onNewObject(SpaceObject obj) const
{
}

void WorldStateEventHandler::onDeleteObject(SpaceObject obj) const
{
}

void WorldStateEventHandler::onObjectChangedCourse(SpaceObject old, SpaceObject newObj) const
{
}

void WorldStateEventHandler::onUpdate(const SpaceObject& obj) const
{
}

void WorldStateEventHandler::onNewFutureCollission(CollissionPoint coll) const
{
}

void WorldStateEventHandler::onDeleteFutureCollission(CollissionPoint coll) const
{
}

void WorldStateEventHandler::onCollissionHappened(CollissionPoint coll) const
{
}
