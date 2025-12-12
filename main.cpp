#include <iostream>
#include <iomanip>
#include <memory>
#include <iterator>
#include <vector>
#include <forward_list>
#include <fstream>

#include "Room.h"
#include "wordwrap.h"
#include "State.h"
#include "strings.h"
#include "Enemy/Boss.h"
#include "Enemy/Clowns.h"
#include "Object/Disposable/Food.h"
#include "Object/Equipment/Weapon.h"

using std::string;
using std::unique_ptr;

string commandBuffer;
State *currentState;

// 辅助函数：保存物品列表
void saveObjectList(ofstream& file, const list<GameObject*>& list) {
    file << list.size() << endl; // 先写入数量
    for (GameObject* obj : list) {
        // 使用 dynamic_cast 判断具体类型
        if (auto* w = dynamic_cast<Weapon*>(obj)) {
            file << "WEAPON " << w->getName() << " " << w->getWeight() << " " << w->getKeyword() << " " << w->getHarm() << endl;
        } else if (auto* f = dynamic_cast<Food*>(obj)) {
            file << "FOOD " << f->getName() << " " << f->getWeight() << " " << f->getKeyword() << " " << f->getEnergy() << endl;
        } else {
            // 普通物品
            file << "NORMAL " << obj->getName() << " " << obj->getWeight() << " " << obj->getKeyword() << endl;
        }
    }
}

// 存档函数
void saveGame() {
    ofstream file("savegame.txt");
    if (!file.is_open()) {
        cout << "Failed to open save file!" << endl;
        return;
    }

    // 1. 保存玩家基础状态
    // 保存当前房间的名字
    file << currentState->getCurrentRoom()->getNameStr() << endl;
    // 保存属性
    file << currentState->getStrength() << " " << currentState->getHarm() << " " << currentState->getMaxWeight() << endl;

    // 2. 保存背包
    saveObjectList(file, currentState->getBagList());

    // 3. 保存装备
    saveObjectList(file, currentState->getEquipmentList()); // 需要在State.h中添加此访问器

    // 4. 保存每个房间的状态（物品和敌人）
    file << Room::rooms.size() << endl;
    for (Room* room : Room::rooms) {
        file << room->getNameStr() << endl;

        // 保存房间内物品
        saveObjectList(file, room->getObject()); // Room::getObject() 返回的是 list<GameObject*>&

        // 保存房间内敌人
        auto& enemies = room->getEnemies();
        file << enemies.size() << endl;
        for (EnemyObject* enemy : enemies) {
            // 这里假设通过名字或简单的类型区分，简化起见统一保存属性
            // 如果有 Boss 和 Clowns 的特殊区别，可以用 typeid 或 dynamic_cast
            string type = "CLOWN";
            if (dynamic_cast<Boss*>(enemy)) type = "BOSS";

            file << type << " " << enemy->getName() << " " << enemy->getKeyword() << " "
                 << enemy->getStrength() << " " << enemy->getHarm() << endl;
        }
    }

    file.close();
    cout << "Game Saved Successfully!" << endl;
}

// 辅助函数：读取并重建物品列表
void loadObjectList(ifstream& file, list<GameObject*>& list) {
    int count;
    file >> count;
    for (int i = 0; i < count; ++i) {
        string type, name, keyword;
        int weight, val;
        file >> type >> name >> weight >> keyword;

        GameObject* newObj = nullptr;
        if (type == "WEAPON") {
            file >> val; // 读取伤害
            newObj = new Weapon(name, weight, val); // 假设 Weapon 构造函数支持 (name, weight, harm)
            // 注意：你的 Weapon 构造函数可能是 (name, weight, harm)，请核对 Weapon.h
        } else if (type == "FOOD") {
            file >> val; // 读取能量
            newObj = new Food(name, weight, val);
        } else {
            newObj = new GameObject(name, weight, keyword);
        }

        if (newObj) list.push_back(newObj);
    }
}

// 读档函数
void loadGame() {
    ifstream file("savegame.txt");
    if (!file.is_open()) {
        cout << "No save file found!" << endl;
        return;
    }

    string roomName;
    int str, harm, maxW;

    // 1. 读取玩家位置和属性
    if (getline(file, roomName) && file >> str >> harm >> maxW) {
        // 恢复属性
        currentState->setStrength(str);
        currentState->setHarm(harm);
        currentState->setMaxWeight(maxW);

        // 恢复位置：遍历所有房间寻找名字匹配的
        for (Room* room : Room::rooms) {
            if (room->getNameStr() == roomName) {
                currentState->goTo(room);
                break;
            }
        }
    } else {
        cout << "Save file corrupted!" << endl; return;
    }

    // 清除玩家当前的物品和装备，防止内存泄漏或重复
    for(auto o : currentState->getBagList()) delete o;
    currentState->getBagList();

    for(auto o : currentState->getEquipmentList()) delete o; // 注意：如果装备在State里是单独管理的
    currentState->getEquipmentList().clear(); // 需要在State.h中确认 equipment 是 list<GameObject*>

    // 2. 恢复背包
    loadObjectList(file, currentState->getBagList());

    // 3. 恢复装备
    loadObjectList(file, currentState->getEquipmentList());

    // 4. 恢复所有房间状态
    int roomCount;
    file >> roomCount;
    string tempName;

    // 注意：这里需要一种机制匹配文件中的房间顺序和内存中的 Room::rooms 顺序
    // 或者我们每次读一个名字，然后在 list 中找对应的房间

    // 消耗掉上一行的换行符
    getline(file, tempName);

    for (int i = 0; i < roomCount; ++i) {
        getline(file, roomName);
        if (roomName.empty()) getline(file, roomName); // 处理可能的空行

        Room* targetRoom = nullptr;
        for (Room* r : Room::rooms) {
            if (r->getNameStr() == roomName) {
                targetRoom = r;
                break;
            }
        }

        if (targetRoom) {
            targetRoom->clearContent(); // 清空当前房间内容

            // 恢复物品
            loadObjectList(file, targetRoom->getObject());

            // 恢复敌人
            int enemyCount;
            file >> enemyCount;
            for (int j = 0; j < enemyCount; ++j) {
                string type, eName, eKey;
                int eStr, eHarm;
                file >> type >> eName >> eKey >> eStr >> eHarm;

                EnemyObject* newEnemy = nullptr;
                if (type == "BOSS") {
                    newEnemy = new Boss(eName, eKey, eStr, eHarm);
                } else {
                    newEnemy = new Clowns(eName, eKey, eStr, eHarm);
                }
                targetRoom->addEnemy(newEnemy);
            }
        }
    }

    file.close();
    cout << "Game Loaded Successfully!" << endl;
    currentState->announceLoc(); // 刷新当前房间描述
}

/**
 * Print out the command prompt then read a command into the provided string buffer.
 * @param buffer Pointer to the string buffer to use.
 */
void inputCommand(string *buffer) {
    buffer->clear();
    std::cout << "> ";
    std::getline(std::cin, *buffer);
}

/**
 * Sets up the map.
 */
void initRooms() {
    auto * r1 = new Room(&r1name, &r1desc);
    auto * r2 = new Room(&r2name, &r2desc);
    auto * r3 = new Room(&r3name, &r3desc);
    auto * r4 = new Room(&r4name, &r4desc);
    auto * r5 = new Room(&r5name, &r5desc);
    auto * rw = new Room(&rwname, &rwdesc);

    Room::addRoom(r1);
    Room::addRoom(r2);
    Room::addRoom(r3);
    Room::addRoom(r4);
    Room::addRoom(r5);
    Room::addRoom(rw);

    r3->setNorth(rw);
    r3->setWest(r2);
    r3->setEast(r4);

    r1->setNorth(r2);
    r2->setSouth(r1);
    r2->setEast(r3);

    r4->setEast(r5);
    r4->setWest(r3);
    r5->setWest(r4);


	/**
	 *"r"使用装备，"v"使用食物，治疗
	 */
	//TODO可以给每一样东西加相应的描述
	auto * sword = new Weapon("sword",5,10);
	r2->addObject(sword);
	auto * axe = new Weapon("axe",7,20);
	r5->addObject(axe);
	auto * apple = new Food("apple",2,50);
	r5->addObject(apple);
	auto * banana = new Food("banana",3,50);
	r2->addObject(banana);

	auto * goblin = new Clowns("goblin","goblin",30,10);
	r3->addEnemy(goblin);
	auto * dragon = new Boss("dragon","goblin",100,30);
	r5->addEnemy(dragon);
}

/**
 * Sets up the game state.
 * 谁在列表的最前面谁就是出身点
 */
void initState() {
	currentState = new State(Room::rooms.front());
	//测试版：可以在此处自定义角色属性
}

/**
 * The main game loop.
 * 游戏循环
 */
void gameLoop() {
    bool gameOver=false;
    while (!gameOver) {
        /* Ask for a command. */
        bool commandOk = false;
    	if (currentState->getStrength() <= 0) {
    		std::string msg = "You lose all strength, GAME OVER!";
    		wrapOut(&msg);
    		wrapEndPara();
    		break;
    	}
        inputCommand(&commandBuffer);

        /* The first word of a command would normally be the verb. The first word is the text before the first
         * space, or if there is no space, the whole string. */
        auto endOfVerb = static_cast<uint8_t>(commandBuffer.find(' '));

    	// 存档
    	if ((commandBuffer.compare(0,endOfVerb,"save") == 0)) {
    		commandOk = true;
    		saveGame();
    		currentState->getCurrentRoom()->describe();
    	}

    	// 读档
    	if ((commandBuffer.compare(0,endOfVerb,"load") == 0)) {
    		commandOk = true;
    		loadGame();
    		// 读档后可能需要跳过本轮循环的后续逻辑，直接开始新状态
    		continue;
    	}

        /* We could copy the verb to another string but there's no reason to, we'll just compare it in place. */
        /* Command to go north. */
        if ((commandBuffer.compare(0,endOfVerb,"north") == 0) || (commandBuffer.compare(0,endOfVerb,"n") == 0)) {
            commandOk = true; /* Confirm command has been handled */
            /* See if there's a north exit */
            Room *northRoom = currentState->getCurrentRoom()->getNorth();
            if (northRoom == nullptr) { /* there isn't */
                wrapOut(&badExit);      /* Output the "can't go there" message */
                wrapEndPara();
            } else {                    /* There is */
                currentState->goTo(northRoom); /* Update state to that room - this will also describe it */
            }
        }

        if ((commandBuffer.compare(0,endOfVerb,"south") == 0) || (commandBuffer.compare(0,endOfVerb,"s") == 0)) {
            commandOk = true; /* Confirm command has been handled */
            /* See if there's a south exit */
            Room *southRoom = currentState->getCurrentRoom()->getSouth();
            if (southRoom == nullptr) { /* there isn't */
                wrapOut(&badExit);      /* Output the "can't go there" message */
                wrapEndPara();
            } else {                    /* There is */
                currentState->goTo(southRoom); /* Update state to that room - this will also describe it */
            }
        }

        if ((commandBuffer.compare(0,endOfVerb,"east") == 0) || (commandBuffer.compare(0,endOfVerb,"e") == 0)) {
            commandOk = true; /* Confirm command has been handled */
            /* See if there's a east exit */
            Room *eastRoom = currentState->getCurrentRoom()->getEast();
            if (eastRoom == nullptr) { /* there isn't */
                wrapOut(&badExit);      /* Output the "can't go there" message */
                wrapEndPara();
            } else {                    /* There is */
                currentState->goTo(eastRoom); /* Update state to that room - this will also describe it */
            }
        }

        if ((commandBuffer.compare(0,endOfVerb,"west") == 0) || (commandBuffer.compare(0,endOfVerb,"w") == 0)) {
            commandOk = true; /* Confirm command has been handled */
            /* See if there's a west exit */
            Room *westRoom = currentState->getCurrentRoom()->getWest();
            if (westRoom == nullptr) { /* there isn't */
                wrapOut(&badExit);      /* Output the "can't go there" message */
                wrapEndPara();
            } else {                    /* There is */
                currentState->goTo(westRoom); /* Update state to that room - this will also describe it */
            }
        }

    	//查看个人属性
    	if ((commandBuffer.compare(0,endOfVerb,"state") == 0)) {
    		//1.这两句必须要写
    		commandOk = true;
    		int strength = currentState->getStrength();
    		int harm = currentState->getHarm();
    		string msg1 = "Strength: " + to_string(strength);
    		string msg2 = "Harm: " + to_string(harm);
    		wrapOut(&msg1);
    		wrapOut(&msg2);
    		wrapEndPara();
    		//2、这两句必须要写
    		currentState->getCurrentRoom()->describe();
    	}

    	//打开背包
        if ((commandBuffer.compare(0,endOfVerb,"bag") == 0) || (commandBuffer.compare(0,endOfVerb,"b") == 0)) {
        	commandOk = true; /* Confirm command has been handled */
        	while(commandOk) {
        		//查看背包中已有道具
        		currentState->showBag();
        		//查看背包中已有装备
        		currentState->showEquipment();
        		//提示语句
        		wrapOut(&choiceInBag);
        		wrapEndPara();
        		//输入想进行操作的道具
        		inputCommand(&commandBuffer);
        		//判读该道具是否在背包中
        		if (GameObject* obj = currentState->findInBag(commandBuffer)) {
        			//输入想进行的操作
        			string msg = "You can use 'drop' to dorp the thing, or use 'equip' to equip the equipment, or "
						"'eat' some food to cure, or use 'close' to exit bag.";
        			wrapOut(&msg);
        			wrapEndPara();
        			inputCommand(&commandBuffer);
        			//丢弃道具
        			if (commandBuffer.compare(0,endOfVerb,"drop") == 0){
        				currentState->dropThing(obj);
        			}
        			//装备武器(使用可装备道具)
        			else if ((commandBuffer.compare(0,endOfVerb,"equip") == 0) and (obj->getKeyword()=="e")) {
						currentState->equipThing(obj);
        			}
        			//使用道具(使用治疗类道具)
        			else if ((commandBuffer.compare(0,endOfVerb,"use") == 0) and (obj->getKeyword()=="v")) {
        				currentState->eatThing(obj);
        			}
        			//TODO出售功能
        			//关闭背包
        			else if ((commandBuffer.compare(0,endOfVerb,"close") == 0)) {
        				//展示房间信息
        				currentState->getCurrentRoom()->describe();
        				break;
        			}
        			else {
        				wrapOut(&badOption);
        				wrapEndPara();
        			}
        		}
        		//判读该道具是否在装备栏中
        		else if (GameObject* obj = currentState->findInEquipment(commandBuffer)) {
        			//输入想进行的操作
        			string msg = "You can use 'takeoff' to take off the equipment.";
        			wrapOut(&msg);
        			wrapEndPara();
        			inputCommand(&commandBuffer);
        			if(commandBuffer.compare(0,endOfVerb,"takeoff") == 0) {
        				currentState->takeOffThing(obj);
        			//TODO出售功能
        			}else {
        				wrapOut(&badOption);
        				wrapEndPara();
        			}
        		}else if ((commandBuffer.compare(0,endOfVerb,"close") == 0) || (commandBuffer.compare(0,endOfVerb,"b")==0)){
        			//展示房间信息
					currentState->getCurrentRoom()->describe();
					break;
        		}else {
        			wrapOut(&notInBagAndEquipment);
        			wrapEndPara();
        		}
        	}
        }

    	//拾取道具
    	if ((commandBuffer.compare(0, endOfVerb, "get") == 0) || (commandBuffer.compare(0, endOfVerb, "g") == 0)) {
    		commandOk = true;
    		if (!(currentState->getCurrentRoom()->getEnemies().empty())) {
    			string msg = "You must defeat all the enemies in the room before you can pick up things.";
    			wrapOut(&msg);
    			wrapEndPara();
    		}else {
    			string msg = "Take what?";
    			wrapOut(&msg);
    			wrapEndPara();
    			//输入你想拾取的道具
    			inputCommand(&commandBuffer);
    			if (GameObject* obj = currentState->getCurrentRoom()->findInRoom(commandBuffer)) {
    				//放入背包
    				currentState->getThing(obj);
    			}else {
    				wrapOut(&notInRoom);
    				wrapEndPara();
    			}
    		}
    		currentState->getCurrentRoom()->describe();
    	}

    	//查看敌人的数值
    	if ((commandBuffer.compare(0, endOfVerb, "enemy") == 0)) {
    		commandOk = true;
    		// 1. 获取当前房间的敌人列表引用
    		auto& enemies = currentState->getCurrentRoom()->getEnemies();

    		// 2. 判断是否有敌人
    		if (enemies.empty()) {
    			string msg = "There are no enemies in this room.";
    			wrapOut(&msg);
    			wrapEndPara();
    		} else {
    			string title = "Enemy Status:";
    			wrapOut(&title);
    			wrapEndPara();

    			// 3. 遍历列表并打印每个敌人的详细数值
    			for (EnemyObject* enemy : enemies) {
    				string info = "Name: " + enemy->getName() +
								  ", HP: " + to_string(enemy->getStrength()) +
								  ", ATK: " + to_string(enemy->getHarm());
    				wrapOut(&info);
    				wrapEndPara();
    			}
    		}
    		currentState->getCurrentRoom()->describe();
    	}

    	//和敌人对战(TODO每个房间只有一只怪)
    	if ((commandBuffer.compare(0, endOfVerb, "fight") == 0) || (commandBuffer.compare(0, endOfVerb, "f") == 0)){
    		commandOk = true; /* Confirm command has been handled */
    		if (!(currentState->getCurrentRoom()->getEnemies().empty())){
    			string msg1 = "Which enemy you want to fight?";
    			wrapOut(&msg1);
    			wrapEndPara();
    			inputCommand(&commandBuffer);
    			//TODO enemy_object为什么这里无法获得迭代器对象
    			//为什么智能指针无法使用
    			if (EnemyObject* enemy_object = currentState->getCurrentRoom()->findEnemy(currentState->getCurrentRoom()->getEnemies(),commandBuffer)) {
					//计算玩家挑战后的血量
    				if (currentState->getHarm()>=enemy_object->getStrength()) {
    					//一击毙命
    					string msg2 = "Defeated "+enemy_object->getName();
    					wrapOut(&msg2);
    					wrapEndPara();
    					currentState->getCurrentRoom()->getEnemies().remove(enemy_object);
						delete(enemy_object);
    				}else {
    					//回合制
    					int playerStrength = currentState->getStrength()-enemy_object->getHarm();
    					currentState->setStrength(playerStrength);
    					int enemyStrength = enemy_object->getStrength()-currentState->getHarm();
    					enemy_object->setStrength(enemyStrength);
    				}
    			}else {
    				string msg3 = "This enemy don't in this room.";
    				wrapOut(&msg3);
    				wrapEndPara();
    			}
    		}else {
    			wrapOut(&notEnemy);
    			wrapEndPara();
    		}
    		currentState->getCurrentRoom()->describe();
    	}

        /* Quit command */
        if ((commandBuffer.compare(0,endOfVerb,"quit") == 0)) {
            commandOk = true;
            gameOver = true;
        }

        /* If commandOk hasn't been set, command wasn't understood, display error message */
        if(!commandOk) {
            wrapOut(&badCommand);
            wrapEndPara();
        }
    }
}

/**
 * 主程序入口
 */

int main() {
	//TODO 快捷键介绍
    initWordWrap();
    initRooms();
    initState();
    currentState->announceLoc();
    gameLoop();

	for (auto r : Room::rooms) {
		delete r;
	}
    return 0;
}