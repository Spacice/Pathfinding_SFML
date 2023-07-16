#pragma once
#include <SFML/Graphics.hpp>

class cell
{
public:
	enum states {
		NOTALLOWED,
		OPENED,
		CLOSED,
		PATH,
		BLOCKED
	};

	static struct cellPosition {
		int x = 0, y = 0;
		cellPosition operator=(sf::Vector2i value) {
			return { value.x, value.y };
		}
	} targetPosition, startPosition;

public:
	cell();
	cell(int x, int y);

	void setNewPosition(sf::Vector2i newPosition);
	void setNewPosition(int newX, int newY);
	void setStartPosition(sf::Vector2i pos) { startPosition = pos; }
	void setStartPosition(int x, int y) { startPosition.x = x; startPosition.y = y; }
	void setTargetPosition(sf::Vector2i pos) { targetPosition = pos; }
	void setTargetPosition(int x, int y) { targetPosition.x = x; targetPosition.y = y; }

	void setState(unsigned short state);
	unsigned short getState() { return state; }
	bool isTravelable() { return travelable; }

	void open(cellPosition parentCell, float parentA_cost);

	float getA_cost() { return toA_cost; }
	float getB_cost() { return toB_cost; }
	float getSumCost() { return toA_cost + toB_cost; }

	sf::Vector2i getPosition() { return sf::Vector2i(thisPosition.x, thisPosition.y); };

	void updateCost();
	void draw(sf::RenderWindow& window, bool drawCosts);

	void setFont(const sf::Font& font) { text.setFont(font); }

private:
	sf::Text text;
	inline void setBoxPosition(const int& x, const int& y) { box.setPosition((float)(x * 64), (float)(y * 64)); }
	inline float getSqrDistance(const cellPosition& a, const cellPosition& b) { return sqrt((b.x - a.x) * (b.x - a.x) + (b.y - a.y) * (b.y - a.y)); }
	cellPosition thisPosition;
	float toA_cost = 0.f;
	float toB_cost = 0.f;
	bool travelable = true;
	unsigned short state = NOTALLOWED;

	bool selfParent = false;
		cellPosition parentPosition;
		float parentA_cost;

	sf::RectangleShape box;
};

