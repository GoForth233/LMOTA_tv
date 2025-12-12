#ifndef BOSS_H
#define BOSS_H
#include "EnemyObject.h"

//boss怪
class Boss : public EnemyObject {
public:
	Boss(const string& _name, const string& _keyword,int _strength,
		int _harm) : EnemyObject(_name, _keyword, _strength, _harm) {}

	//受到伤害
	void takeDamage(int amount) override{
		strength -= amount;
		if (strength < 0) strength = 0;
	}
};

#endif //BOSS_H
