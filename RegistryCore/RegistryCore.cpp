#include "RegistryCore.h"

RegisterCore::RegisterCore()
{
	loadConfig();
	port = "5555";
	std::string address = "tcp://*:" + port;
	try
	{
		suber.bindAddress(address);
		suber.setSubscribe("");
		//pusher.bindAddress("tcp://192.168.10.124:8888");
		reper.bindAddress("tcp://192.168.10.124:8888");
		
	}
	catch (std::exception* e)
	{
		std::cout << e->what() << std::endl;
	}
	this->recv_thread = new std::thread([this]() {this->recvHeartbeat(); });

	
}

void RegisterCore::loadConfig(const std::string& file)
{
}

void RegisterCore::recvHeartbeat()
{
	while (true)
	{
		try
		{
			std::string recv_msg = reper.recvMsg();
			HeatBeat heat_beat;
			heat_beat.ParseFromString(recv_msg);
			std::string host_ip = heat_beat.host_ip();
			std::vector<CppProgramInfo> vec;
			for (auto& proto_info : heat_beat.program_info())
			{
				CppProgramInfo m_info;
				m_info.process_name = proto_info.process_name();
				m_info.start_counts = proto_info.start_counts();
				m_info.process_status = static_cast<CppStatus>(proto_info.start_counts());

				vec.push_back(m_info);

				//info_map.insert(std::make_pair(host_ip, m_info));
				//info_map[host_ip] = m_info; //能更新value
				/*using ValueType = std::map <std::string, ProgramInfo>::value_type;
				info_map.insert(ValueType(host_ip, info_map));*/
			}
			m_mutex.lock();
			this->info_map[host_ip] = vec;
			m_mutex.unlock();
			
			//TODO	解析recv_msg到 info_map 中
		}
		catch (std::exception* e)
		{
			std::cout << e->what() << std::endl;
		}
		
	}
}
void RegisterCore::setCommand()
{
	//while (true)
	//{
	Command cmd;
	cmd.set_process_name("all");
	cmd.set_m_action(Action::KILL);

	std::string setcmd;
	cmd.SerializeToString(&setcmd);
	try
	{
		//while (true)
		//{
		reper.sendMsg(setcmd);
		std::this_thread::sleep_for(std::chrono::seconds(1));
		/*	}*/
	}
	catch (std::exception* e)
	{
		std::cout << e->what() << std::endl;
	}

	//}
}
void RegisterCore::choiceNumber()
{
	int number;
	while (true)
	{
		std::cout << "choices:" << std::endl;
		std::cout << "	1.print all ip" << std::endl;
		std::cout << "	2.show all information" << std::endl;
		std::cout << "	3.restart all process" << std::endl;
		std::cin >> number;
		if (number == 1)
		{
			m_mutex.lock();
			for (auto& key_value : this->info_map)
			{
				std::cout << key_value.first << std::endl;
			}
			m_mutex.unlock();
		}
		if (number == 2)
		{
			printMap();
		}
		if (number == 3)
		{
			setCommand();
		}
	}
}
void RegisterCore::printMap()
{
	std::cout << "print info,info size:" << info_map.size() << std::endl;
	m_mutex.lock();
	for (auto& key_value : this->info_map)
	{
		std::cout << "主机地址：\n" << key_value.first << std::endl;
		for (auto& info : key_value.second)
		{
			std::cout << "	process_name:" << info.process_name << std::endl;
			std::cout << "	start_counts:" << info.start_counts << std::endl;
			std::cout << "	process_status:" << info.process_status << std::endl;
			std::cout << "------------------------------" << std::endl;
		}
	}
	m_mutex.unlock();

}


int main()
{
	RegisterCore registry;
	//registry.recvHeartbeat();
	registry.choiceNumber();
	return 0;
}
