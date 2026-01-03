#pragma once

#include <memory>

class AstarTile {
private:
    bool collision;
    int x = 0;
    int y = 0;
    bool finish = false;
    int startCost = 0;
    int finishCost = 0;
    int totalCost = 0;
    bool checked = false;
    std::shared_ptr<AstarTile> parentTile = nullptr;
    int weight;

public:
    AstarTile(int xPos, int yPos, bool collision, int weight = 1) {
		x = xPos;
		y = yPos;
		this->collision = collision;
		this->weight = weight;
    }

	~AstarTile() = default;

    // Getters
	int getX() const { return x; }
	int getY() const { return y; }
	bool isFinish() const { return finish; }
	bool hasCollision() const { return collision; }
	int getTotalCost() const { return totalCost; }
	int getStartCost() const { return startCost; }
	int getFinishCost() const { return finishCost; }
	bool getChecked() const { return checked; }
	std::shared_ptr<AstarTile> getParentTile() const { return parentTile; }
    int getWeight() const { return weight; }

    // Setters
    void setStartDiff(const int i) { startCost = i; }
    void setFinishDiff(const int i) { finishCost = i; }
    void setTotalCost() { totalCost = startCost + finishCost; }
	void setChecked(const bool b) { checked = b; }
	void setParent(const std::shared_ptr<AstarTile> parent) { parentTile = parent; }
	void setFinish(const bool b) { finish = b; }
	void setHasCollision(const bool b) { collision = b; }
};
