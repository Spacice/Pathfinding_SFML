#include <SFML/Graphics.hpp>
#include "cell.h"
#include <Stack>
#include <iostream>

sf::Font font;

cell::cellPosition cell::startPosition = { 2, 2 };
cell::cellPosition cell::targetPosition = { 28, 28 };

cell* targetCellPtr;

const int mapWidth = 32;
const int mapHeight = 32;

const int screenWidth = 1600;
const int screenHeight = 900;

cell cellMap[mapWidth][mapHeight];
std::stack<cell*> openedCells;

void tick() {
	if (openedCells.empty())return;
	cell* current = openedCells.top();
	if (current != targetCellPtr) {
		openedCells.pop();
		if (cellMap[current->getPosition().y][current->getPosition().x].isTravelable() && cellMap[current->getPosition().y][current->getPosition().x].getState() != cell::CLOSED) {
			if (cellMap[current->getPosition().y][current->getPosition().x].getState() != cell::PATH)
				cellMap[current->getPosition().y][current->getPosition().x].setState(cell::CLOSED);
			for (int i = -1; i < 2; i++)
				for (int j = -1; j < 2; j++) {
					if (i == 0 && j == 0) continue;
					if (current->getPosition().y + i >= 0 && current->getPosition().y + i < mapWidth && current->getPosition().x + j >= 0 && current->getPosition().x + j < mapHeight
						&& cellMap[current->getPosition().y + i][current->getPosition().x + j].getState() != cell::BLOCKED
						&& cellMap[current->getPosition().y + i][current->getPosition().x + j].getState() != cell::CLOSED) {
						cellMap[current->getPosition().y + i][current->getPosition().x + j].open({ current->getPosition().x, current->getPosition().y }, cellMap[current->getPosition().y][current->getPosition().x].getA_cost());
						std::stack<cell*> tempStack;
						while (!openedCells.empty()) {
							if (openedCells.top() == &cellMap[current->getPosition().y + i][current->getPosition().x + j])
								break;
							tempStack.push(openedCells.top());
							openedCells.pop();
						}
						if (openedCells.empty() && !tempStack.empty())
							while (true) {
								if (cellMap[current->getPosition().y + i][current->getPosition().x + j].getSumCost() > tempStack.top()->getSumCost()) {
									openedCells.push(&cellMap[current->getPosition().y + i][current->getPosition().x + j]);
									break;
								}
								else if (cellMap[current->getPosition().y + i][current->getPosition().x + j].getSumCost() == tempStack.top()->getSumCost()
									&& cellMap[current->getPosition().y + i][current->getPosition().x + j].getA_cost() > tempStack.top()->getA_cost()) {
									openedCells.push(&cellMap[current->getPosition().y + i][current->getPosition().x + j]);
									break;
								}
								else {
									openedCells.push(tempStack.top());
									tempStack.pop();
									if (tempStack.empty()) {
										openedCells.push(&cellMap[current->getPosition().y + i][current->getPosition().x + j]);
										break;
									}
									continue;
								}
							}
						else {
							if (!openedCells.empty()) openedCells.pop();
							openedCells.push(&cellMap[current->getPosition().y + i][current->getPosition().x + j]);
						}
						while (!tempStack.empty()) {
							openedCells.push(tempStack.top());
							tempStack.pop();
						}
					}
				}
		}
	}
	else {
		current = &cellMap[cell::startPosition.x][cell::startPosition.y];
		while (current != targetCellPtr) {
			cell* next = nullptr;
			for (int i = -1; i < 2; i++)
				for (int j = -1; j < 2; j++) {
					if (i == 0 && j == 0) continue;
					if (current->getPosition().y + i >= 0 && current->getPosition().y + i < mapWidth && current->getPosition().x + j >= 0 && current->getPosition().x + j < mapHeight
						&& cellMap[current->getPosition().y + i][current->getPosition().x + j].getState() == cell::CLOSED) {
						if ((next == nullptr) || (cellMap[current->getPosition().y + i][current->getPosition().x + j].getSumCost() < next->getSumCost())) {
							next = &cellMap[current->getPosition().y + i][current->getPosition().x + j];
						}
					}
				}
			if (next == nullptr) throw std::exception("Проложенный путь не найден");
			current = next;
			next->setState(cell::PATH);
		}
	}
}

int main()
{
	font.loadFromFile("font.ttf");
	sf::RenderWindow window(sf::VideoMode(screenWidth, screenHeight), "SFML");
	//window.setFramerateLimit(60);

	const float Zoom[3] = { 1.f, 2.f, 3.f };
	int tempZoom = 0;
	sf::View v[3];
	for (int i = 0; i < 3; i++) {
		v[i].setCenter(64 * 16, 64 * 16);
		v[i].setSize(screenWidth, screenHeight);
		v[i].zoom(Zoom[i]);
	}
	sf::Vector2f viewCenter(v[0].getCenter());

	sf::Clock clk;
	bool drawCosts = false;

	for (int y = 0; y < mapHeight; y++)
		for (int x = 0; x < mapWidth; x++) {
			cellMap[y][x].setNewPosition(x, y);
			cellMap[y][x].setFont(font);
			//cellMap[y][x].updateCost();
		}
	cellMap[cell::startPosition.x][cell::startPosition.y].setState(cell::OPENED);
	cellMap[cell::startPosition.x][cell::startPosition.y].updateCost();
	cellMap[cell::targetPosition.x][cell::targetPosition.y].setState(cell::PATH);
	cellMap[cell::targetPosition.x][cell::targetPosition.y].updateCost();

	openedCells.push(&cellMap[cell::startPosition.x][cell::startPosition.y]);
	targetCellPtr = &cellMap[cell::targetPosition.x][cell::targetPosition.y];

	sf::Mouse ms;

	while (window.isOpen())
	{
		float elepTime = clk.getElapsedTime().asMicroseconds();
		clk.restart();
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
			if (event.type == sf::Event::MouseButtonPressed) {
				if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
					sf::Vector2i mousePos = ms.getPosition(window);
					sf::Vector2i index((mousePos.x + (window.getView().getCenter().x - window.getView().getSize().x / 2) / Zoom[tempZoom]) / 64.f * Zoom[tempZoom], (mousePos.y + (window.getView().getCenter().y - window.getView().getSize().y / 2) / Zoom[tempZoom]) / 64.f * Zoom[tempZoom]);
					if (index.x >= 0 && index.y >= 0 && index.x < mapWidth && index.y < mapHeight) {
						if (cellMap[index.y][index.x].isTravelable() && cellMap[index.y][index.x].getState() != cell::CLOSED) {
							if (cellMap[index.y][index.x].getState() != cell::PATH) {
								cellMap[index.y][index.x].updateCost();
								cellMap[index.y][index.x].setState(cell::CLOSED);
							}
							for(int i = -1; i < 2; i++)
								for (int j = -1; j < 2; j++) {
									if (i == 0 && j == 0) continue;
									if(index.y + i >= 0 && index.y + i < mapHeight && index.x + j >= 0 && index.x + j < mapWidth
										&& cellMap[index.y + i][index.x + j].getState() != cell::BLOCKED
										&& cellMap[index.y + i][index.x + j].getState() != cell::CLOSED
										&& cellMap[index.y + i][index.x + j].getState() != cell::PATH) {
											cellMap[index.y + i][index.x + j].open({ index.x, index.y }, cellMap[index.y][index.x].getA_cost());
									}
								}
						}
					}
				}
			}
			if (event.type == sf::Event::KeyPressed) {
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
					tick();
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1)) {
					tempZoom = 0;
					v[0].setCenter(window.getView().getCenter());
				}
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2)) {
					tempZoom = 1;
					v[1].setCenter(window.getView().getCenter());
				}
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num3)) {
					tempZoom = 2;
					v[2].setCenter(window.getView().getCenter());
				}
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z)) {
					drawCosts = !drawCosts;
				}
			}

		}

		if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
			sf::Vector2i mousePos = ms.getPosition(window);
			sf::Vector2i index((mousePos.x + (window.getView().getCenter().x - window.getView().getSize().x / 2) / Zoom[tempZoom]) / 64.f * Zoom[tempZoom], (mousePos.y + (window.getView().getCenter().y - window.getView().getSize().y / 2) / Zoom[tempZoom]) / 64.f * Zoom[tempZoom]);
			std::cout << "X: " << index.x << " Y: " << index.y << std::endl;
			if (index.x >= 0 && index.y >= 0 && index.x < mapWidth && index.y < mapHeight) {
				cellMap[index.y][index.x].setState(cell::BLOCKED);
			}
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
			viewCenter.y -= 0.001 * elepTime;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
			viewCenter.y += 0.001 * elepTime;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
			viewCenter.x -= 0.001 * elepTime;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
			viewCenter.x += 0.001 * elepTime;

		v[tempZoom].setCenter(viewCenter);
		window.setView(v[tempZoom]);

		window.clear(sf::Color(127, 127, 127));

		for (int y = 0; y < mapHeight; y++)
			for (int x = 0; x < mapWidth; x++) {
				cellMap[y][x].draw(window, drawCosts);
			}

		window.display();
	}

	return 0;
}