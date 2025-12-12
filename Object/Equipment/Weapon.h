#ifndef WEAPON_H
#define WEAPON_H

//可装备道具
class Weapon : public GameObject {
    int harm;

public:
    Weapon(const string& _name ,int _weight, int _harm):GameObject(_name,_weight,"e"),harm(_harm) {}

    int getHarm() const {
        return harm;
    }
};
#endif //WEAPON_H
