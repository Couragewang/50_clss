#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <pthread.h>
#include "PlayerManager.hpp"
#include "RoomManager.hpp"

#define MATCH_LEVEL 101

using namespace std;

class Hall{
    private:
        PlayerManager pm;
        RoomManager rm;
        vector<vector<uint32_t> > match_pool;
        int match_num;
        pthread_mutex_t match_lock;
        pthread_cond_t  match_cond;
    public:
        Hall():match_pool(MATCH_LEVEL),match_num(0)
        {
        }
        int MatchNum()
        {
            return match_num;
        }
        void IncMatchNum()
        {
            match_num++;
        }
        void DecMatchNum()
        {
            match_num--;
        }
        void ResetMatchNum()
        {
            match_num = 0;
        }
        void GetAllMatchId(vector<uint32_t> &id_list)
        {
            for(auto i = MATCH_LEVEL-1; i >= 0; i--){
                auto &v = match_pool[i];
                if(v.empty()){
                    continue;
                }
                for(auto it = v.begin(); it != v.end(); it++){
                    id_list.push_back(*it);
                }
            }
        }
        void LockMatchPool()
        {
            pthread_mutex_lock(&match_lock);
        }
        void UnlockMatchPool()
        {
            pthread_mutex_unlock(&match_lock);
        }
        void ServiceWait()
        {
            pthread_cond_wait(&match_cond, &match_lock);
        }
        void ServiceWakeup()
        {
            pthread_cond_signal(&match_cond);
        }
        uint32_t Register(string &name, string &passwd)
        {
            return pm.InsertPlayer(name, passwd);
        }
        uint32_t Login(uint32_t &id, string &passwd)
        {
            return pm.SearchPlayer(id, passwd);
        }
        bool PushIdInMatchPool(uint32_t &id)
        {
            LOG(INFO, "Push Player Into Match Pool...");
            pm.SetMatching(id);
            int rate = pm.GetRate(id);
            cout << "rate : " << rate << endl;
            LockMatchPool();
            auto &v = match_pool[rate];
            auto it = v.begin();
            for(; it != v.end(); it++){
                if(*it == id){
                    return false;
                }
            }
            v.push_back(id);
            IncMatchNum();
            UnlockMatchPool();
            ServiceWakeup();
            return true;
            //return pm.PlayerWait(id);
        }
        bool PopIdMatchPool(uint32_t &id)
        {
            LockMatchPool();
            //MatchPoolClear(id);
            int rate = pm.GetRate(id);
            auto &v = match_pool[rate];
            for(auto it = v.begin(); it != v.end(); it++){
                if(*it == id){
                    v.erase(it);
                    break;
                }
            }
            DecMatchNum();
            UnlockMatchPool();
            pm.SetOnline(id);
            return true;
        }
        void MatchPoolClear(uint32_t &id)
        {
            LOG(INFO, "match pool clear...");
            for(auto i = MATCH_LEVEL-1; i >= 0; i--){
                auto &v = match_pool[i];
                if(v.empty()){
                    continue;
                }
                vector<uint32_t>().swap(v);
            }
            ResetMatchNum();
            if(id >= 10000){
                pm.SetOnline(id);
            }
        }
        int IsPlayerReady(uint32_t &id)
        {
            return pm.Ready(id);
        }
        void GamePrepare(uint32_t &one, uint32_t &two)
        {
            pm.SetPlayStatus(one, two);
            uint32_t room_id = rm.CreateRoom(one, two);
            pm.SetPlayRoom(room_id, one, two);
            cout << "debug: room_id: " << room_id << endl;
            //pm.SignalPlayer(one, two);
        }
        static void *MatchService(void *arg)
        {
            pthread_detach(pthread_self());
            Hall *hp = (Hall*)arg;

            while(1){
                uint32_t last = 0;
                hp->LockMatchPool();
                while(hp->MatchNum() < 2){
                    LOG(INFO, "service thread wait...");
                    //wait
                    hp->ServiceWait();
                }
                LOG(INFO, "service thread wakeup...");
                vector<uint32_t> id_list;
                hp->GetAllMatchId(id_list);
                int num = id_list.size();
                if(num & 1){
                    last = id_list[id_list.size()-1];
                    num &= (~1);
                }else{
                    last = 0;
                }
                for(auto i = 0; i < num; i+=2){
                    uint32_t play_one = id_list[i];
                    uint32_t play_two = id_list[i+1];
                    hp->GamePrepare(play_one, play_two);
                }
                hp->MatchPoolClear(last);
                hp->UnlockMatchPool();
            }
        }
        string GetPlayerBoard(uint32_t &room_id)
        {
            string board;
            //uint32_t room_id = pm.GetPlayerRoomId(id);
            rm.GetBoard(room_id, board);
            return board;
        }
        uint32_t GetPlayerRoomId(uint32_t &id)
        {
            return pm.GetPlayerRoomId(id);
        }
        char GetPlayerPiece(uint32_t &room_id, uint32_t &id)
        {
            return rm.GetPlayerPiece(room_id, id);
        }
        bool IsMyTurn(uint32_t &room_id, uint32_t &id)
        {
            return rm.IsMyTurn(room_id, id);
        }
        void Step(uint32_t &room_id, uint32_t &id, int x, int y)
        {
            return rm.Step(room_id, id, x, y);
        }
        char Judge(uint32_t &room_id, uint32_t &id)
        {
            return rm.Judge(room_id, id);
        }
        void InitHall()
        {
            pthread_mutex_init(&match_lock, NULL);
            pthread_cond_init(&match_cond, NULL);

            pthread_t tid;
            pthread_create(&tid, NULL, MatchService, this);
        }
        ~Hall()
        {
            pthread_mutex_destroy(&match_lock);
            pthread_cond_destroy(&match_cond);
        }
};






