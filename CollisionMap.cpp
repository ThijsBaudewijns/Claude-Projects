#include "../Headers/CollisionMap.h"
#include "../Headers/Transform.h"
#include "../Headers/GameObject.h"
#include "../Headers/Collider.h"
#include "../Headers/BoxCollider.h"
#include "../Headers/CircleCollider.h"
#include "../Headers/Vector2.h"
#include "../Headers/AIAgent.h"
#include "../Headers/Engine.h"

CollisionMap::CollisionMap() {
	pathfinder_ = nullptr;
}

std::vector<std::shared_ptr<Vector2>> CollisionMap::GetPath(const std::shared_ptr<Vector2>& start, const std::shared_ptr<Vector2>& end) {
	if (!pathfinder_) {
		return std::vector<std::shared_ptr<Vector2>>();
	}

	// Convert world coordinates to tile indices
	const float cellSize = smallestEntitySize_ / accuracy_;
	if (cellSize <= 0.0f)
		return {};

	int xStart = static_cast<int>(std::floor((start->getX() - worldStartX_) / cellSize));
	int yStart = static_cast<int>(std::floor((start->getY() - worldStartY_) / cellSize));
	int xEnd = static_cast<int>(std::floor((end->getX() - worldStartX_) / cellSize));
	int yEnd = static_cast<int>(std::floor((end->getY() - worldStartY_) / cellSize));

	// Get actual map dimensions
	const int mapWidth = static_cast<int>(std::ceil(worldWidth_));
	const int mapHeight = static_cast<int>(std::ceil(worldHeight_));

	if (mapWidth <= 0 || mapHeight <= 0)
		return {};

	xStart = ClampInt(xStart, 0, mapWidth - 1);
	yStart = ClampInt(yStart, 0, mapHeight - 1);
	xEnd = ClampInt(xEnd, 0, mapWidth - 1);
	yEnd = ClampInt(yEnd, 0, mapHeight - 1);

	std::vector<std::shared_ptr<AstarTile>> astarPath = pathfinder_->newPath(xStart, yStart, xEnd, yEnd);
	std::vector<std::shared_ptr<Vector2>> path;


	for (auto tile : astarPath) {
		auto vec = std::make_shared<Vector2>();
		// Convert tile coordinates back to world coordinates (CENTER of tile)
		vec->setX(tile->getX() * cellSize + worldStartX_ + cellSize * 0.5f);
		vec->setY(tile->getY() * cellSize + worldStartY_ + cellSize * 0.5f);
		path.push_back(vec);
	}

	// Debug path with RenderSystem
	Engine& engine = Engine::instance();
	auto renderSystem = engine.GetSystem<RenderSystem>();
	if (renderSystem) {
		renderSystem->SetDebugPath(path, mapWidth, mapHeight, cellSize, worldStartX_, worldStartY_);
	}

	return path;
}

void CollisionMap::RefreshMap(std::list<std::shared_ptr<Collider>>& colliders) {
	FindMapData(colliders);
	tileMap_ = GenerateTileMap(colliders);

	// Get actual tile dimensions
	const int mapWidthInTiles = static_cast<int>(std::ceil(worldWidth_));
	const int mapHeightInTiles = static_cast<int>(std::ceil(worldHeight_));

	// Entity size in tiles should be 1 for simple pathfinding
	// Or calculate based on actual agent size if you want larger agents
	const int entitySizeInTiles = 1;

	if (!pathfinder_) {
		pathfinder_ = std::make_shared<Pathfinder>(
			mapWidthInTiles,
			mapHeightInTiles,
			entitySizeInTiles,  // width in tiles
			entitySizeInTiles   // height in tiles
		);
	}
	pathfinder_->setTileMap(tileMap_);
}

std::vector<std::vector<std::shared_ptr<AstarTile>>> CollisionMap::GenerateTileMap(std::list<std::shared_ptr<Collider>>& colliders) {

	const int mapWidth = static_cast<int>(std::ceil(worldWidth_));
	const int mapHeight = static_cast<int>(std::ceil(worldHeight_));

	const float cellSize = smallestEntitySize_ / accuracy_;
	if (cellSize <= 0.0f) {
		throw std::runtime_error("Invalid CollisionMap cell size");
	}

	// Initialize empty tile map
	std::vector<std::vector<std::shared_ptr<AstarTile>>> tileMap;
	tileMap.resize(mapWidth);
	for (int i = 0; i < mapWidth; i++) {
		tileMap[i].resize(mapHeight, nullptr);
	}

	for (int y = 0; y < mapHeight; y++) {
		for (int x = 0; x < mapWidth; x++) {
			tileMap[x][y] = std::make_shared<AstarTile>(x, y, false);
		}
	}

	// Mark tiles with collisions
	for (auto collider : colliders) {
		auto gameObject = collider->GetGameObject();
		if (!gameObject) continue;
		auto& transform = gameObject->transform;
		auto box = gameObject->GetComponent<BoxCollider>();
		auto circle = gameObject->GetComponent<CircleCollider>();

		if (auto agentComp = gameObject->GetComponent<AIAgent>()) {
			continue; // skip AI agents
		}

		float startX = transform.GetWorldPosition().getX();
		float startY = transform.GetWorldPosition().getY();
		float endX = startX;
		float endY = startY;
		if (box) {
			float halfWidth = box->GetWidth() * 0.5f;
			float halfHeight = box->GetHeight() * 0.5f;
			startX -= halfWidth;
			startY -= halfHeight;
			endX += halfWidth;
			endY += halfHeight;
		}
		else if (circle) {
			float radius = circle->GetRadius();
			startX -= radius;
			startY -= radius;
			endX += radius;
			endY += radius;
		}
		else {
			continue; // skip invalid collider
		}

		// Convert world coordinates to tile indices
		int tileStartX = static_cast<int>(std::floor((startX - worldStartX_) / cellSize));
		int tileStartY = static_cast<int>(std::floor((startY - worldStartY_) / cellSize));
		int tileEndX = static_cast<int>(std::floor((endX - worldStartX_) / cellSize));
		int tileEndY = static_cast<int>(std::floor((endY - worldStartY_) / cellSize));

		// Clamp to map bounds
		tileStartX = (std::max)(0, (std::min)(tileStartX, mapWidth - 1));
		tileStartY = (std::max)(0, (std::min)(tileStartY, mapHeight - 1));
		tileEndX = (std::max)(0, (std::min)(tileEndX, mapWidth - 1));
		tileEndY = (std::max)(0, (std::min)(tileEndY, mapHeight - 1));


		// Mark tiles as having collision
		for (int y = tileStartY; y <= tileEndY; y++) {
			for (int x = tileStartX; x <= tileEndX; x++) {
				if (tileMap[x][y]) {
					tileMap[x][y]->setHasCollision(true);
				}
			}
		}
	}

	return tileMap;
}

void CollisionMap::FindMapData(std::list<std::shared_ptr<Collider>>& colliders) {

	// For each collider, determine the smallest entity size and world size
	smallestEntitySize_ = 999999.0f;

	float worldMinX = 999999.0f;
	float worldMinY = 999999.0f;
	float worldMaxX = std::numeric_limits<float>::lowest();
	float worldMaxY = std::numeric_limits<float>::lowest();

	bool foundAny = false;

	for (const auto& collider : colliders) {
		auto gameObject = collider->GetGameObject();
		if (!gameObject) continue;

		auto& transform = gameObject->transform;

		auto box = gameObject->GetComponent<BoxCollider>();
		auto circle = gameObject->GetComponent<CircleCollider>();

		float startX = transform.GetWorldPosition().getX();
		float startY = transform.GetWorldPosition().getY();
		float endX = startX;
		float endY = startY;

		float entitySize;

		if (box) {
			entitySize = (std::min)(box->GetWidth(), box->GetHeight());
			float halfWidth = box->GetWidth() * 0.5f;
			float halfHeight = box->GetHeight() * 0.5f;
			startX -= halfWidth;
			startY -= halfHeight;
			endX += halfWidth;
			endY += halfHeight;
		}
		else if (circle) {
			entitySize = circle->GetRadius() * 2.0f;
			float radius = circle->GetRadius();
			startX -= radius;
			startY -= radius;
			endX += radius;
			endY += radius;
		}
		else {
			continue; // skip invalid collider
		}

		foundAny = true;

		smallestEntitySize_ = (std::min)(smallestEntitySize_, entitySize);

		worldMinX = (std::min)(worldMinX, startX);
		worldMinY = (std::min)(worldMinY, startY);
		worldMaxX = (std::max)(worldMaxX, endX);
		worldMaxY = (std::max)(worldMaxY, endY);
	}

	if (!foundAny) {
		return;
	}

	worldStartX_ = worldMinX;
	worldStartY_ = worldMinY;
	worldEndX_ = worldMaxX;
	worldEndY_ = worldMaxY;

	float deltaX = std::abs(worldEndX_ - worldStartX_);
	float deltaY = std::abs(worldEndY_ - worldStartY_);

	if (smallestEntitySize_ < 1.0f) smallestEntitySize_ = 1.0f;

	worldWidth_ = deltaX / (smallestEntitySize_ / accuracy_);
	worldHeight_ = deltaY / (smallestEntitySize_ / accuracy_);
}