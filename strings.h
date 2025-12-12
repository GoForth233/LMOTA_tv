#ifndef TEXTADV_STRINGS_H
#define TEXTADV_STRINGS_H

#include <string>

const std::string r1name = "Room 1";
const std::string r1desc = "You are in room 1. It's really quite boring, but your The adventure is beginning!. There's a passage to the north.";
const std::string r2name = "Room Blue";
const std::string r2desc = "You are the Room blue. That's about all though. There's a passage to the south and east.";
const std::string r3name = "Room 3";
const std::string r3desc = "You are the Room 3. That's about all though. There's a passage to the north,east and west.";
const std::string r4name = "Room 4";
const std::string r4desc = "You are the Room 4. That's about all though. There's a passage to the east and west.";
const std::string r5name = "Room 5";
const std::string r5desc = "You are the Room 5. That's about all though. There's a passage to the west.";
const std::string rwname = "White Room";
const std::string rwdesc = "You are the white room. You know because it's end of this level. That's about all though. There's a passage to the next level.";


const std::string badExit = "You can't go that way.";
const std::string badCommand = "I don't understand that.";
const std::string badOption = "I can't do that.";

const std::string emptyBag = "There is nothing in your bag.";
const std::string choiceInBag = "You can now choose to 'close' or 'b' the backpack or choice 'one thing' that you want to option.";

const std::string emptyEquip = "There is nothing in your equipment.";
//const std::string notInEquip = "This thing is not in your equipment.";
//const std::string choiceInEquip = "You can now choose to 'close' or 'b' the backpack or choice 'one thing' that you want to option.";
const std::string notInBagAndEquipment = "This thing is not in your bag and equipment.You only can 'close' your bag";

const std::string notInRoom = "This thing is not in this room.";
const std::string notEnemy = "No enemy in this room.";

#endif //TEXTADV_STRINGS_H
