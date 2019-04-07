#pragma once

#include <iostream>
#include <unordered_map>
#include "Room.hpp"

#define ROOM_ID 1024

using namespace std;

class RoomManager{
    private:
        unordered_map<uint32_t, Room> room_set;
        uint32_t assign_id;
    public:
        RoomManager():assign_id(ROOM_ID)
        {}
        void CreateRoom(uint32_t &one, uint32_t &two)
        {
            Room r(one, two);
            uint32_t id = assign_id++;
            room_set.insert({id, r});
        }
        ~RoomManager()
        {}
};













