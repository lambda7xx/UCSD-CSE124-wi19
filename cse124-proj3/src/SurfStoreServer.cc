#include <sysexits.h>
#include <string>

#include "rpc/server.h"

#include "logger.hpp"
#include "SurfStoreTypes.hpp"
#include "SurfStoreServer.hpp"

SurfStoreServer::SurfStoreServer(INIReader& t_config, int t_servernum)
    : config(t_config), servernum(t_servernum)
{
    auto log = logger();

	// pull our address and port
	string serverid = "server" + std::to_string(servernum);
	string servconf = config.Get("ssd", serverid, "");
	if (servconf == "") {
		log->error("{} not found in config file", serverid);
		exit(EX_CONFIG);
	}
	size_t idx = servconf.find(":");
	if (idx == string::npos) {
		log->error("Config line {} is invalid", servconf);
		exit(EX_CONFIG);
	}
	port = (int) strtol(servconf.substr(idx+1).c_str(), nullptr, 0);
	if (port <= 0 || port > 65535) {
		log->error("The port provided is invalid: {}", servconf);
		exit(EX_CONFIG);
	}
}

void SurfStoreServer::launch()
{
    auto log = logger();

    log->info("Launching SurfStore server");
    log->info("My ID is: {}", servernum);
    log->info("Port: {}", port);

	rpc::server srv(port);

	srv.bind("ping", []() {
		auto log = logger();

		log->info("ping()");
		return;
	});

	srv.run();
}
