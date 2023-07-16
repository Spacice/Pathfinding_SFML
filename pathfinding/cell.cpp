#include "cell.h"

cell::cell()
{
	box.setFillColor(sf::Color::White);
	box.setOutlineColor(sf::Color::Black);
	box.setOutlineThickness(-2);
	box.setSize(sf::Vector2f(64.f, 64.f));
	text.setCharacterSize(16);
	text.setFillColor(sf::Color::Black);
	//text.setStyle(sf::Text::Bold);
}

cell::cell(int x, int y)
	:cell()
{
	setBoxPosition(x, y);
}

void cell::setNewPosition(sf::Vector2i newPosition)
{
	thisPosition = newPosition;
	setBoxPosition(newPosition.x, newPosition.y);
}

void cell::setNewPosition(int newX, int newY)
{
	thisPosition.x = newX; thisPosition.y = newY;
	setBoxPosition(newX, newY);
}

void cell::setState(unsigned short state)
{
	switch (state) {
		case OPENED:
			box.setFillColor(sf::Color::Green);
			break;
		case CLOSED:
			box.setFillColor(sf::Color::Red);
			travelable = false;
			break;
		case PATH:
			box.setFillColor(sf::Color::Cyan);
			break;
		case BLOCKED:
			box.setFillColor(sf::Color(0x777777FF));
			travelable = false;
			break;
		default:
			box.setFillColor(sf::Color::White);
			break;
	}
	if(state < BLOCKED + 1)
		this->state = state;
}

void cell::open(cellPosition parentCell, float parentA_cost)
{
	if (state == NOTALLOWED) {
		selfParent = true;
		parentPosition = parentCell;
		this->parentA_cost = parentA_cost;
		updateCost();
	}else if (toA_cost > getSqrDistance(thisPosition, parentPosition) + parentA_cost) {
		selfParent = true;
		parentPosition = parentCell;
		this->parentA_cost = parentA_cost;
		updateCost();
	}
	setState(OPENED);
}

void cell::updateCost()
{
	toA_cost = selfParent ? getSqrDistance(thisPosition, parentPosition) + parentA_cost
							: getSqrDistance(thisPosition, startPosition);
	toB_cost = getSqrDistance(thisPosition, targetPosition);
}

void cell::draw(sf::RenderWindow& window, bool drawCosts)
{
	window.draw(box);
	if (state != BLOCKED && state != NOTALLOWED && drawCosts) {
		text.setPosition(box.getPosition().x + 5.f, box.getPosition().y + 2.f);
		text.setString(std::to_string(toA_cost) + '\n' + std::to_string(toB_cost) + '\n' + std::to_string((toA_cost + toB_cost)));
		window.draw(text);
	}
}