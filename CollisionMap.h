#pragma once

#include "Pathfinder.h"
#include "Vector2.h"
#include "Collider.h"
#include "AstarTile.h"
#include <memory>
#include <list>
#include <vector>
#include <algorithm>

class CollisionMap {
public:
	CollisionMap();
	~CollisionMap() = default;

	std::vector<std::shared_ptr<Vector2>> GetPath(const std::shared_ptr<Vector2>& start, const std::shared_ptr<Vector2>& end);
	void RefreshMap(std::list<std::shared_ptr<Collider>>& colliders);

private:
	std::shared_ptr<Pathfinder> pathfinder_;
	
	float accuracy_ = 1.0f;
	float smallestEntitySize_ = 1.0f;

	float worldStartX_ = 0.0f;
	float worldStartY_ = 0.0f;
	float worldEndX_ = 100.0f;
	float worldEndY_ = 100.0f;

	int worldWidth_ = 100;
	int worldHeight_ = 100;

	std::vector<std::vector<std::shared_ptr<AstarTile>>> GenerateTileMap(std::list<std::shared_ptr<Collider>>& colliders);
	std::vector<std::vector<std::shared_ptr<AstarTile>>> tileMap_;

	inline int ClampInt(int v, int lo, int hi) {
		return (v < lo) ? lo : (v > hi) ? hi : v;
	}

	void FindMapData(std::list<std::shared_ptr<Collider>>& colliders); // find properties of agent size and world size, based on GameObject distributions


};
