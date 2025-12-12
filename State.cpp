#include "State.h"
#include <iostream>
#include <ostream>

#include "strings.h"
#include "wordwrap.h"
#include "Object/Disposable/Food.h"
#include "Object/Equipment/Weapon.h"

/**
 * Current state of the game.
 */

/**
 * Display the description of the room the player is in. */

void State::announceLoc() const {
    this->currentRoom->describe();
}

/**
 * Constructor.
 * @param startRoom Pointer to the room to start in.
 */
State::State(Room *startRoom) : currentRoom(startRoom), strength(100), harm(20), maxWeight(20) {}

int State::getStrength() const {
	return this->strength;
}

int State::getHarm() const {
	return this->harm;
}

void State::setStrength(int _strength) {
	this->strength = _strength;
	if (this->strength < 0) {
		this->strength = 0;
	}
}

void State::setHarm(int _harm) {
	this->harm = _harm;
}
/**
 * Move to a specified room and print its description.
 * @param target Pointer to the room to move to.
 */
void State::goTo(Room *target) {
    this->currentRoom = target;
    this->announceLoc();
}

/**
 * Return a pointer to the current room.
 * @return Pointer to the current room.
 */
Room* State::getCurrentRoom() const {
    return this->currentRoom;
}

list<GameObject *> &State::getBagList() {
	return this->bag;
}

list<GameObject*>& State::getEquipmentList() {
	return this->equipment;
}

//背包有关的东西
void State::showBag() {
	if (!bag.empty()) {
		string objStr = "Bag: ";
		for (auto it = bag.begin(); it != bag.end(); ++it) {
			if (it != bag.begin()) {
				objStr += ", ";
			}
			objStr += (*it)->getName();
		}
		objStr += ".";
		wrapOut(&objStr);
		wrapEndPara();
	}else {
		wrapOut(&emptyBag);
		wrapEndPara();
	}
}

//查看装备栏有的道具
void State::showEquipment() {
	if (!equipment.empty()) {
		string objStr = "Equipment: ";
		for (auto it = equipment.begin(); it != equipment.end(); ++it) {
			if (it != equipment.begin()) {
				objStr += ", ";
			}
			objStr += (*it)->getName();
		}
		objStr += ".";
		wrapOut(&objStr);
		wrapEndPara();
		/*//提示关闭背包或者操作道具
		wrapOut(&choiceInEquip);
		wrapEndPara();*/
	}else {
		wrapOut(&emptyEquip);
		wrapEndPara();
	}
}

//TODO如何才能取到这个特定位置的指针(解决批量删除的问题)
GameObject *State::findInBag(string thing) const{
	for (auto obj : bag) {
		if (obj->getName()==thing) {
			return obj;
		}
	}
	return nullptr;
}

//TODO如何才能取到这个特定位置的指针(解决批量删除的问题)
GameObject *State::findInEquipment(string thing) const {
	for (auto obj : equipment) {
		if (obj->getName()==thing) {
			return obj;
		}
	}
	return nullptr;
}


void State::getThing(GameObject* &thing) {
	//计算现有背包的总重量
	int currentTotalWeight = 0;
	for (auto& obj : bag) {
		currentTotalWeight+=obj->getWeight();
	}
	//自动拾取（不需要再次确认是否要拾取）
	if (currentTotalWeight + thing->getWeight() <= maxWeight){
		//将道具从房间移动到背包
		//先添加再移除，防止内存泄漏
		bag.push_back(thing);
		currentRoom->getObject().remove(thing);
		string msg = "You picked up " + thing->getName() + ".";
		wrapOut(&msg);
		wrapEndPara();
	}else {
		string msg = "Your bag is too heavy! You need a bigger bag or drop some thing.";
		wrapOut(&msg);
		wrapEndPara();
	}
}

void State::dropThing(GameObject* &thing) {
	//提示语句
	string msg = "Are you sure to drop the " + thing->getName() + " Y or N";
	wrapOut(&msg);
	wrapEndPara();
	string flag = "N";
	cout << "> "; // 打印提示符
	//输入是否要丢弃
	getline(cin, flag);
	if (flag == "Y") {
		//将道具背包移动到房间
		//先添加再移除，防止内存泄漏
		currentRoom->addObject(thing);
		bag.remove(thing);
		string msg = "You dropped the " + thing->getName();
		wrapOut(&msg);
		wrapEndPara();
	}
}

//缺陷：装备相应的武器后这个武器将会内存泄漏！对象还在堆内存里占用空间，但你再也找不到它的地址了（无法 delete 它了）。
//方法：使用一个粗糙的容器暂时存放使用中的装备
void State::equipThing(GameObject* &thing) {
	equipment.push_back(thing);
	bag.remove(thing);
	string msg = "You equipped with " + thing->getName() + ".";
	wrapOut(&msg);
	wrapEndPara();
	this->harm+=dynamic_cast<Weapon*>(thing)->getHarm();
}

void State::takeOffThing(GameObject* &thing) {
	string msg = "You take off the " + thing->getName();
	wrapOut(&msg);
	wrapEndPara();

	int currentTotalWeight = 0;
	for (auto& obj : bag) {
		currentTotalWeight+=obj->getWeight();
	}
	//检查背包现在可以装下我脱下的装备吗
	if (currentTotalWeight + thing->getWeight() <= maxWeight){
		//将道具从已装备移动到背包
		//先添加再移除，防止内存泄漏
		bag.push_back(thing);
		equipment.remove(thing);
		this->harm-=dynamic_cast<Weapon*>(thing)->getHarm();
	}else {
		string msg = "Your bag is too heavy! You need a bigger bag or drop some thing.";
		wrapOut(&msg);
		wrapEndPara();
	}
}

void State::eatThing(GameObject* &thing) {
	string msg = "You eat the " + thing->getName();
	wrapOut(&msg);
	wrapEndPara();
	this->strength+=dynamic_cast<Food*>(thing)->getEnergy();
	bag.remove(thing);
	delete thing;
}



void State::upgradeBackpack(int extraWeight) {
	this->maxWeight += extraWeight;
	cout << "You found a larger backpack! Max weight increased." << endl;
}



