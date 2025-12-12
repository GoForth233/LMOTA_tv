#ifndef ENEMYOBJECT_H
#define ENEMYOBJECT_H
#include <string>
using namespace std;

class EnemyObject {
protected:
	string name;
	string keyword;
	//生命值
	int strength;
	//攻击力
	int harm;

public:
	EnemyObject(const string& _name, const string& _keyword, int _strength,
		int _harm) : name(_name), keyword(_keyword), strength(_strength), harm(_harm) {}

	virtual ~EnemyObject() {}

	string getName() const {
		return name;
	}

	string getKeyword() const {
		return keyword;
	}

	//获取敌人的生命值
	int getStrength() const {
		return strength;
	}

	//获取敌人的伤害值
	int getHarm() const {
		return harm;
	}

	void setName(const string &name) {
		this->name = name;
	}

	void setStrength(int strength) {
		this->strength = strength;
	}

	void setHarm(int harm) {
		this->harm = harm;
	}

	//受到伤害
	virtual void takeDamage(int amount) = 0;

};

#endif //ENEMYOBJECT_H
