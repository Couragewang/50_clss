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
        void MatchPoolClear()
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
        }
        int IsPlayerReady(uint32_t &id)
        {
            return pm.Ready(id);
        }
        void GamePrepare(uint32_t &one, uint32_t &two)
        {
            pm.SetPlayStatus(one, two);
            rm.CreateRoom(one, two);
            //pm.SignalPlayer(one, two);
        }
        static void *MatchService(void *arg)
        {
            pthread_detach(pthread_self());
            Hall *hp = (Hall*)arg;

            while(1){
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
                num &= (~1);
                for(auto i = 0; i <= num; i+=2){
                    uint32_t play_one = id_list[i];
                    uint32_t play_two = id_list[i+1];
                    hp->GamePrepare(play_one, play_two);
                }
                hp->MatchPoolClear();
                hp->UnlockMatchPool();
            }
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






