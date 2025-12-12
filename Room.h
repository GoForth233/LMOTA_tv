#ifndef TEXTADV_ROOM_H
#define TEXTADV_ROOM_H

#include <string>
#include <forward_list>
#include <list>

#include "Enemy/EnemyObject.h"
#include "Object/GameObject.h"

using std::string;
using namespace std;

/**
 * Represents a room (accessible location in the game).
 */
class Room {
    /**
     * Short name used as a header.
     */
    const string* name;
    /**
     * Full description of the room.
     */
    const string* description;
    /**
     * Pointer to room that is north of this one.
     */
    Room* north;
    Room* south;
    Room* east;
    Room* west;

	list<GameObject*> objects;
	list<EnemyObject*> enemies;

public:
    /**
     * Constructs a new Room.
     * @param _name Name of the room.
     * @param _desc Description of the room.
     */
    Room(const string *_name, const string *_desc);

    /**
     * Removes a destroyed room from the global list if it's there.
     */
    ~Room();

    /**
     * Outputs the name and description of the room
     * in standard format.
     */
    void describe() const;

    /**
     * List storing all rooms that have been registered via addRoom().
     */
    static std::list<Room*> rooms;

    /**
     * Creates a new Room with the given parameters and register it with the static list.
     * @param _name Name of the room.
     * @param _desc Description of the room.
     */
    static Room* addRoom(const string* _name, const string* _desc);
    static void addRoom(Room* room);
	GameObject* findInRoom(string name) const;
	EnemyObject* findEnemy(list<EnemyObject*>& enemyList, string name) const;

	//用于读档
	string getNameStr() const { return *name; }
	void clearContent();

    Room* getNorth() const;
    void setNorth(Room* _north);

    Room* getSouth() const;
    void setSouth(Room* _south);

    Room* getEast() const;
    void setEast(Room* _east);

    Room* getWest() const;
	void setWest(Room* _wast);

	void addObject(GameObject* object);

	void removeObject(GameObject* object);

	list<GameObject*>& getObject() {
		return objects;
	}

	void addEnemy(EnemyObject* enemy);

	list<EnemyObject*>& getEnemies() {
		return enemies;
	}

};

#endif //TEXTADV_ROOM_H
