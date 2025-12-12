cmake_minimum_required(VERSION 3.29)
project(CHC5028_Coursework_2)

set(CMAKE_CXX_STANDARD 20)

add_executable(CHC5028_Coursework_2 main.cpp
        Room.cpp
        State.cpp
        wordwrap.cpp
        Enemy/Boss.h
        Enemy/Clowns.h
        Enemy/EnemyObject.h
        Object/GameObject.h
        Object/Equipment/Weapon.h
        Object/Disposable/Food.h
)
