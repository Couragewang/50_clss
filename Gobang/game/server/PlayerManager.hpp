#pragma once

#include <iostream>
#include <string>
#include <unordered_map>
#include <utility>      // std::pair
#include "Log.hpp"
#include "Player.hpp"

#define DEFAULT_ID 10000

using namespace std;

class PlayerManager{
    private:
        std::unordered_map<uint32_t, Player> player_set;
        uint32_t assign_id;
    public:
        PlayerManager():assign_id(DEFAULT_ID)
        {}
        uint32_t InsertPlayer(string &name, string &passwd)
        {
            uint32_t id_ = assign_id++;
            Player p(name, passwd, id_);
            //player_set.insert(std::make_pair<uint32_t,Player>(id_, p));
            player_set.insert({id_, p});
            LOG(INFO, "insert play done...");
            return id_;
        }
        uint32_t SearchPlayer(uint32_t &id, string &passwd)
        {
            auto iter = player_set.find(id);
            if(iter == player_set.end()){
                LOG(WARNING, "Search Player Error...");
                return 1;
            }

            Player &p = iter->second;
            if(p.Passwd() != passwd){
                LOG(WARNING, "Player Passwd Error...");
                return 2;
            }
            p.Online();
            LOG(INFO, "Play Online Success...");
            return id;
        }
        
        void SignalPlayer(uint32_t &one, uint32_t &two)
        {
            player_set[one].Signal();
            player_set[two].Signal();
        }
        void SetPlayStatus(uint32_t &one, uint32_t &two)
        {
            player_set[one].Playing();
            player_set[two].Playing();
        }
        void SetMatching(uint32_t &id)
        {
            player_set[id].Matching();
        }
        bool PlayerWait(uint32_t &id)
        {
            if(ETIMEDOUT == player_set[id].Wait()){
                cout << "Play time out..." << endl;
                return false;
            }
            cout << "Play wakeup ..." << endl;
            return true;
        }
        int GetRate(uint32_t &id)
        {
            return player_set[id].Rate();
        }
        int Ready(uint32_t &id)
        {
            return player_set[id].Status();
        }
        ~PlayerManager()
        {}
};





