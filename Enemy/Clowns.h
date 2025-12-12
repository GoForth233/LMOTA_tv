#ifndef CLOWNS_H
#define CLOWNS_H
#include "EnemyObject.h"

//小怪
class Clowns : public EnemyObject {
public:
	Clowns(const string& _name, const string& _keyword,int _strength,
		int _harm) : EnemyObject(_name, _keyword, _strength, _harm) {}

	//受到伤害
	void takeDamage(int amount) override{
		strength -= amount;
		if (strength < 0) strength = 0;
	}

};
#endif //CLOWNS_H
