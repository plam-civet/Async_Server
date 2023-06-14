#pragma once
#include <string>
#include <iostream>
#include <exception>
#include <map>
#include <thread>
#include <mutex>
#include "zmq_helpler.h"
#include "HeatBeat.pb.h"

enum CppStatus
{
	PROTECTED = 0,
	STOP
};

struct CppProgramInfo
{
	std::string process_name;
	int start_counts;    //被启动次数
	CppStatus process_status;
};

class RegisterCore
{
public:
	RegisterCore();
	void loadConfig(const std::string& file = "./conf/config.json");
	void choiceNumber();
private:
	void printMap();
	void recvHeartbeat();
	void setCommand();
private:
	std::map<std::string, std::vector<CppProgramInfo>> info_map; //IP - INFO_LIST

	//std::map<std::string, CppProgramInfo> info_map; //IP - INFO
	ZmqSub suber;
	std::string port;//端口
	std::thread* recv_thread;//接收心跳线程
	std::mutex m_mutex;
	zmqPush pusher;
};

