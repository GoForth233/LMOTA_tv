#ifndef TEXTADV_STATE_H
#define TEXTADV_STATE_H
#include "Room.h"
#include "Object/GameObject.h"

class State {
    Room *currentRoom;
	//生命值
	int strength;
	//攻击力
	int harm;
	//1.创建一个背包
	list<GameObject*> inventory;
	//2. 添加一个变量控制背包容量（后期可以获得更大的背包）
	int maxWeight;
	//TODO装备栏（缺点可以无限装备道具,应该通过类型来限制装备数，而不是重量！以后改进），(有的装备有伤害，有的装备只增加防御或者生命值，这样就没法区分了)
	list<GameObject*> equipment;

public:
    explicit State(Room *startRoom);
    void goTo(Room *target);
    void announceLoc() const;
    Room* getCurrentRoom() const;

    list<GameObject *> &getInventoryList();
	list<GameObject*>& getEquipmentList();

	int getStrength() const;
	int getHarm() const;
	void setStrength(int _strength);
	void setHarm(int _Harm);
	int getMaxWeight() const { return maxWeight; }
	void setMaxWeight(int mw) { maxWeight = mw; }


	//1、寻找道具
	GameObject* findInInventory(string thing) const;

	//2、查看道具
	void showInventory();

	//3、添加道具
	void getThing(GameObject* &thing);

	//4、丢弃道具
	void dropThing(GameObject* &thing);

	//1、寻找已装备的道具
	GameObject* findInEquipment(string thing) const;

	//2、查看装备
	void showEquipment();

	//3、装备相应道具，只针对于增加攻击力的装备
	void equipThing(GameObject* &thing);

	//4、脱掉装备
	void takeOffThing(GameObject* &thing);

	//吃东西(治疗)
	void eatThing(GameObject* &thing);

	//挑战敌人


	// 捡起更大的背包时调用这个函数
	void upgradeBackpack(int extraWeight);
};


#endif //TEXTADV_STATE_H
