#include "Room.h"
#include "wordwrap.h"
#include "Enemy/EnemyObject.h"
#include "Object/GameObject.h"

/**
 * Stores a static list of all rooms.
 */
std::list<Room*> Room::rooms;

/**
 * Room default constructor.
 * @param _name Room's name.
 * @param _desc Room's description.
 */
Room::Room(const string* _name, const string *_desc) :
        name(_name), description(_desc), north(nullptr), south(nullptr), east(nullptr), west(nullptr) {};

/**
 * Remove destroyed rooms from the static list.
 */
Room::~Room() {
    Room::rooms.remove(this);

	for (GameObject* obj : objects) {
		delete obj;
	}
	objects.clear();
	for (EnemyObject* enemy : enemies) {
		delete enemy;
	}
	enemies.clear();
}

void Room::clearContent() {
	// 释放物品内存并清空列表
	for (GameObject* obj : objects) {
		delete obj;
	}
	objects.clear();

	// 释放敌人内存并清空列表
	for (EnemyObject* enemy : enemies) {
		delete enemy;
	}
	enemies.clear();
}

/**
 * Prints the description of a room (the name and long description)
 */
void Room::describe() const {
    wrapOut(this->name);
    wrapEndPara();
    wrapOut(this->description);
    wrapEndPara();

	//击败所有敌人才能知道这个房间里有什么物品
	if (!enemies.empty()) {
		std::string enemyStr = "Carefully! There are enemies: ";
		for (auto it = enemies.begin(); it != enemies.end(); ++it) {
			if (it != enemies.begin()) {
				enemyStr += ", ";
			}
			enemyStr += (*it)->getName();
		}
		enemyStr += "!";
		wrapOut(&enemyStr);
		wrapEndPara();
	}else {
		std::string objStr = "Things: ";
		for (auto it = objects.begin(); it != objects.end(); ++it) {
			if (it != objects.begin()) {
				objStr += ", ";
			}
			objStr += (*it)->getName();
		}
		objStr += ".";
		wrapOut(&objStr);
		wrapEndPara();
	}
}

/**
 * Statically creates a room and then adds it to the global list.
 * @param _name Name for the new room.
 * @param _desc Description for the new room.
 * @return A pointer to the newly created room.
 */
Room* Room::addRoom(const string* _name, const string *_desc) {
    auto *newRoom = new Room(_name, _desc);
    Room::rooms.push_back(newRoom);
    return newRoom;
}

/**
 * Adds an existing room to the static list.
 * @param room Pointer to the room to add.
 * @return
 */
void Room::addRoom(Room* room) {
    Room::rooms.push_back(room);
}

//判断房间里有没有这个东西
GameObject *Room::findInRoom(string name) const {
	for (auto obj : objects) {
		if (obj->getName()==name) {
			return obj;
		}
	}
	return nullptr;
}

EnemyObject* Room::findEnemy(list<EnemyObject*>& enemyList, string name) const{
	// 1. 使用迭代器遍历列表
	for (auto it = enemyList.begin(); it != enemyList.end(); ++it) {
		// 2. 检查名字
		if ((*it)->getName() == name) {
			EnemyObject* foundEnemy = *it;
			// 3. 返回对象指针
			return foundEnemy;
		}
	}
	// 4. 找了一圈没找到
	return nullptr;
}

void Room::addObject(GameObject *object) {
	Room::objects.push_back(object);
}

void Room::removeObject(GameObject *object) {
	Room::objects.remove(object);
}

void Room::addEnemy(EnemyObject* enemy) {
	Room::enemies.push_back(enemy);
}

/**
 * Gets the north exit of a room.
 * @return The room that is north of this one, or NULL if there is no exit.
 */
Room* Room::getNorth() const {
    return this->north;
}

/**
 * Sets the north exit of a room.
 * @param _north Pointer to the room to be north of this one.
 */
void Room::setNorth(Room* _north) {
    this->north = _north;
}

Room * Room::getSouth() const {
    return this->south;
}

void Room::setSouth(Room *_south) {
    this->south = _south;
}

Room * Room::getEast() const {
    return this->east;
}

void Room::setEast(Room *_east) {
    this->east = _east;
}

Room * Room::getWest() const {
    return this->west;
}

void Room::setWest(Room *_wast) {
    this->west = _wast;
}


