#pragma once
#include "AstarTile.h"
#include <vector>
#include <memory>

class Pathfinder {
private:
    std::vector<std::vector<std::shared_ptr<AstarTile>>> tiles;
    bool first = true;
    int xFinish = 0;
    int yFinish = 0;
    int xStart = 0;
    int yStart = 0;
    int mapHeight;
    int mapWidth;
    int entityWidth;
    int entityHeight;
    std::vector<std::shared_ptr<AstarTile>> path;
    std::vector<std::vector<std::shared_ptr<AstarTile>>> aTiles;
    std::vector<std::shared_ptr<AstarTile>> markedATiles;

    std::vector<std::shared_ptr<AstarTile>> searchTiles();
    std::shared_ptr<AstarTile> closestFinish(int lowestScore);
    int findLowestScore();
    bool turnOver(std::shared_ptr<AstarTile>& tile);
    bool diagonalDir(int xParent, int yParent, int xNext, int yNext);
    std::shared_ptr<AstarTile> getTile(int x, int y);
    std::vector<std::shared_ptr<AstarTile>> backtrackPath(std::shared_ptr<AstarTile>& finishTile);
    int phyt(int xStart, int yStart, int xPos, int yPos);
    bool hasCollision(int x, int y);
    bool hasLineOfSight(const AstarTile& from, const AstarTile& to);
    static bool isCollinear(
        const AstarTile& a,
        const AstarTile& b,
        const AstarTile& c)
    {
        int dx1 = b.getX() - a.getX();
        int dy1 = b.getY() - a.getY();
        int dx2 = c.getX() - b.getX();
        int dy2 = c.getY() - b.getY();

        return dx1 * dy2 == dy1 * dx2;
    }

public:
    Pathfinder(int mapWidth, int mapHeight, int entityWidth, int entityHeight);
    ~Pathfinder() = default;

    void setTileMap(const std::vector<std::vector<std::shared_ptr<AstarTile>>>& tileMap) { tiles = tileMap;  }
    std::vector<std::shared_ptr<AstarTile>> newPath(int xStart, int yStart, int xFinish, int yFinish);
};