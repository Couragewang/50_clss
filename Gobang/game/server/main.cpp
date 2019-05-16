#include <rpc_server.h>
#include "Hall.hpp"
using namespace rest_rpc;
using namespace rpc_service;
using namespace std;

Hall GameHall;

uint32_t RpcRegister(connection* conn, string name, string passwd)
{
    return GameHall.Register(name, passwd);
}
uint32_t RpcLogin(connection* conn, uint32_t id, string passwd)
{
    return GameHall.Login(id, passwd);
}
bool RpcMatchAndWait(connection* conn, uint32_t id)
{
    return GameHall.PushIdInMatchPool(id);
}
int RpcPlayerReady(connection* conn, uint32_t id)
{
    return GameHall.IsPlayerReady(id);
}
string RpcBoard(connection* conn, uint32_t room_id)
{
    return GameHall.GetPlayerBoard(room_id);
}
uint32_t RpcPlayerRoomId(connection* conn,uint32_t id)
{
    return GameHall.GetPlayerRoomId(id);
}
char RpcPlayerPiece(connection* conn,uint32_t room_id, uint32_t id)
{
    return GameHall.GetPlayerPiece(room_id, id);
}
bool RpcIsMyTurn(connection* conn,uint32_t room_id, uint32_t id)
{
    return GameHall.IsMyTurn(room_id, id);
}
void RpcStep(connection* conn,uint32_t room_id, uint32_t id, int x, int y)
{
    return GameHall.Step(room_id, id, x, y);
}
char RpcJudge(connection* conn,uint32_t room_id, uint32_t id)
{
    return GameHall.Judge(room_id, id);
}
bool RpcPopMatchPool(connection* conn,uint32_t id)
{
    return GameHall.PopIdMatchPool(id);
}
int main() {
	rpc_server server(9001, 4);
    LOG(INFO, "Init Server .... done");

	server.register_handler("RpcRegister", RpcRegister);
	server.register_handler("RpcLogin", RpcLogin);
	server.register_handler("RpcMatchAndWait", RpcMatchAndWait);
	server.register_handler("RpcPlayerReady", RpcPlayerReady);
	server.register_handler("RpcPlayerRoomId", RpcPlayerRoomId);
	server.register_handler("RpcPlayerPiece", RpcPlayerPiece);
	server.register_handler("RpcBoard", RpcBoard);
	server.register_handler("RpcIsMyTurn", RpcIsMyTurn);
	server.register_handler("RpcStep", RpcStep);
	server.register_handler("RpcJudge", RpcJudge);
	server.register_handler("RpcPopMatchPool", RpcPopMatchPool);

    LOG(INFO, "Register All Function .... done");
    LOG(INFO, "Server start ... done");
    GameHall.InitHall();
//	dummy d;
//	server.register_handler("add", &dummy::add, &d);
//	server.register_handler("hello", hello);
//	server.register_handler("get_person_name", get_person_name);
//	server.register_handler("get_person", get_person);
//	server.register_handler("upload", upload);
//	server.register_handler("download", download);
//	server.register_handler("get_name", get_name);

	server.run();

	std::string str;
	std::cin >> str;
}


