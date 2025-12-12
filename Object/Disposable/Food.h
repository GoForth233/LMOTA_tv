#ifndef FOOD_H
#define FOOD_H

//治疗类道具（一次性道具）
class Food : public GameObject {
    int energy;

public:
    Food(const string& _name, int _weight, int _energy):GameObject(_name, _weight, "v"),energy(_energy){}

    int getEnergy() const {
        return energy;
    }
};

#endif //FOOD_H
