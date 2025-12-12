#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H
#include <string>
using namespace std;

class GameObject {
protected:
	string name;
    int weight;
    string keyword;

public:
    GameObject(const string& _name, int _weight, const string& _keyword):name(_name),weight(_weight),keyword(_keyword){}
    virtual ~GameObject(){};

	string getName() const {
		return name;
	}

    int getWeight() const {
        return weight;
    }

    string getKeyword() const {
        return keyword;
    }

};



#endif //GAMEOBJECT_H
