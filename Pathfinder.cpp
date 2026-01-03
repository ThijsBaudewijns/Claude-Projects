#include "../Headers/Pathfinder.h"
#include <algorithm>
#include <cmath>

Pathfinder::Pathfinder(int mapWidth, int mapHeight, int entityWidth, int entityHeight)
{
    this->mapWidth = mapWidth;
    this->mapHeight = mapHeight;
    this->entityWidth = entityWidth;
    this->entityHeight = entityHeight;

    if (mapWidth <= 0 || mapHeight <= 0)
        return;

    aTiles.resize(mapWidth,
        std::vector<std::shared_ptr<AstarTile>>(mapHeight, nullptr));
}

std::vector<std::shared_ptr<AstarTile>>
Pathfinder::newPath(int xStart, int yStart, int xFinish, int yFinish)
{
    if (tiles.empty())
        return {};

    if (hasCollision(xFinish, yFinish))
        return {};

    if (xStart != this->xStart || yStart != this->yStart ||
        xFinish != this->xFinish || yFinish != this->yFinish)
    {
        this->xStart = xStart;
        this->yStart = yStart;
        this->xFinish = xFinish;
        this->yFinish = yFinish;

        if (xStart == xFinish && yStart == yFinish) {
            path.clear();
            path.push_back(std::make_shared<AstarTile>(xStart, yStart, false));
            return path;
        }

        return searchTiles();
    }
    return path;
}


std::vector<std::shared_ptr<AstarTile>> Pathfinder::searchTiles()
{
    first = true;
    std::shared_ptr<AstarTile> tile = nullptr;

    while (true) {
        tile = nullptr;

        if (first) {
            first = false;

            for (int x = 0; x < mapWidth; ++x)
                for (int y = 0; y < mapHeight; ++y)
                    aTiles[x][y] = nullptr;

            markedATiles.clear();

            for (int y = 0; y < mapHeight; ++y) {
                for (int x = 0; x < mapWidth; ++x) {

                    if (x >= tiles.size() || y >= tiles[x].size())
                        continue;

                    // Null pointer safety
                    if (!tiles[x][y])
                        continue;

                    auto t = std::make_shared<AstarTile>(
                        x, y,
                        hasCollision(x, y),
                        tiles[x][y]->getWeight()
                    );

                    aTiles[x][y] = t;

                    if (!t->hasCollision()) {
                        t->setFinishDiff(phyt(xFinish, yFinish, x, y));
                        t->setTotalCost();
                    }

                    if (x == xFinish && y == yFinish)
                        t->setFinish(true);
                }
            }
            auto startTile = getTile(xStart, yStart);
            if (turnOver(startTile))
                return backtrackPath(tile);
        }
        else {
            int lowestScore = findLowestScore();
            tile = closestFinish(lowestScore);

            if (!tile)
                return {};

            if (turnOver(tile))
                return backtrackPath(tile);
        }
    }
}

std::shared_ptr<AstarTile> Pathfinder::closestFinish(int lowestScore)
{
    std::shared_ptr<AstarTile> tile = nullptr;
    int closestFinish = 0;

    for (auto& t : markedATiles) {
        int tCost = t->getTotalCost();
        if (tCost == lowestScore) {
            int fCost = t->getFinishCost();
            if (!tile || fCost < closestFinish) {
                closestFinish = fCost;
                tile = t;
            }
        }
    }
    return tile;
}

int Pathfinder::findLowestScore()
{
    int lowestScore = 0;
    std::shared_ptr<AstarTile> tile = nullptr;

    for (auto& t : markedATiles) {
        int tCost = t->getTotalCost();
        if (!tile || tCost < lowestScore) {
            lowestScore = tCost;
            tile = t;
        }
    }
    return lowestScore;
}

bool Pathfinder::turnOver(std::shared_ptr<AstarTile>& tile)
{
    if (!tile)
        return false;

    int x1 = tile->getX();
    int y1 = tile->getY();

    for (int y = y1 - 1; y <= y1 + 1; ++y) {
        for (int x = x1 - 1; x <= x1 + 1; ++x) {

            auto t = getTile(x, y);
            if (!t || t->hasCollision())
                continue;

            if (x == x1 && y == y1) {
                t->setChecked(true);
                markedATiles.erase(
                    std::remove(markedATiles.begin(), markedATiles.end(), t),
                    markedATiles.end()
                );
                if (t->isFinish())
                    return true;
            }

            if (!t->getChecked() &&
                diagonalDir(x1, y1, x, y))
            {
                int moveCost = phyt(x, y, x1, y1) * t->getWeight();
                int newCost = moveCost + tile->getStartCost();

                auto it = std::find(markedATiles.begin(), markedATiles.end(), t);
                bool contains = it != markedATiles.end();

                if (!contains || t->getStartCost() > newCost) {
                    t->setStartDiff(newCost);
                    t->setTotalCost();
                    t->setParent(tile);
                }

                if (!contains)
                    markedATiles.push_back(t);
            }
        }
    }
    return false;
}

bool Pathfinder::diagonalDir(int xParent, int yParent, int xNext, int yNext)
{
    int dx = xParent - xNext;
    int dy = yParent - yNext;

    if (dx != 0 && dy != 0) {
        auto t1 = getTile(xNext + dx, yNext);
        auto t2 = getTile(xNext, yNext + dy);

        if ((t1 && t1->hasCollision()) ||
            (t2 && t2->hasCollision()))
            return false;
    }
    return true;
}

std::shared_ptr<AstarTile> Pathfinder::getTile(int x, int y)
{
    if (x >= 0 && x < mapWidth && y >= 0 && y < mapHeight)
        return aTiles[x][y];
    return nullptr;
}

std::vector<std::shared_ptr<AstarTile>>
Pathfinder::backtrackPath(std::shared_ptr<AstarTile>& finishTile)
{
    path.clear();

    // 1) backtrack
    for (auto t = finishTile; t; t = t->getParentTile())
        path.push_back(t);

    // 2) reverse
    std::reverse(path.begin(), path.end());

    if (path.size() < 3)
        return path;

    // 3) remove collinear points
    std::vector<std::shared_ptr<AstarTile>> filtered;
    filtered.push_back(path[0]);

    for (size_t i = 1; i + 1 < path.size(); ++i) {
        if (!isCollinear(*filtered.back(), *path[i], *path[i + 1]))
            filtered.push_back(path[i]);
    }
    filtered.push_back(path.back());

    // 4) line-of-sight pruning
    std::vector<std::shared_ptr<AstarTile>> optimized;
    optimized.push_back(filtered[0]);

    size_t anchor = 0;
    for (size_t i = 2; i < filtered.size(); ++i) {
        if (!hasLineOfSight(*filtered[anchor], *filtered[i])) {
            optimized.push_back(filtered[i - 1]);
            anchor = i - 1;
        }
    }
    optimized.push_back(filtered.back());

    path = std::move(optimized);
    return path;
}


int Pathfinder::phyt(int xStart, int yStart, int xPos, int yPos)
{
    int dx = std::abs(xStart - xPos);
    int dy = std::abs(yStart - yPos);

    int cost = 0;
    while (dx > 0 && dy > 0) {
        dx--; dy--;
        cost += 14;
    }
    return cost + dx * 10 + dy * 10;
}

bool Pathfinder::hasCollision(int x, int y)
{
    if (tiles.empty())
        return true;

    for (int i = 0; i < entityWidth; ++i) {
        for (int j = 0; j < entityHeight; ++j) {

            int tx = x + i;
            int ty = y + j;

            if (tx < 0 || ty < 0 || tx >= mapWidth || ty >= mapHeight)
                return true;

            if (!tiles[tx][ty] || tiles[tx][ty]->hasCollision())
                return true;
        }
    }
    return false;
}

bool Pathfinder::hasLineOfSight(
    const AstarTile& from,
    const AstarTile& to)
{
    int x0 = from.getX();
    int y0 = from.getY();
    int x1 = to.getX();
    int y1 = to.getY();

    int dx = std::abs(x1 - x0);
    int dy = std::abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;

    while (true) {
        // entity-sized collision check
        if (hasCollision(x0, y0))
            return false;

        if (x0 == x1 && y0 == y1)
            break;

        int e2 = 2 * err;
        if (e2 > -dy) { err -= dy; x0 += sx; }
        if (e2 < dx) { err += dx; y0 += sy; }
    }
    return true;
}
