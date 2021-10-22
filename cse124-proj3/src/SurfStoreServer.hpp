#ifndef SURFSTORESERVER_HPP
#define SURFSTORESERVER_HPP

#include "inih/INIReader.h"
#include "logger.hpp"

using namespace std;

class SurfStoreServer {
public:
    SurfStoreServer(INIReader& t_config, int t_servernum);

    void launch();

	const uint64_t RPC_TIMEOUT = 500; // milliseconds

protected:
    INIReader& config;
	const int servernum;
	int port;
};

#endif // SURFSTORESERVER_HPP
