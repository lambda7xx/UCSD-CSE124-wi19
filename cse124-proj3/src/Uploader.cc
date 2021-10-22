#include <sysexits.h>
#include <string>
#include <vector>
#include <iostream>
#include <assert.h>
#include <errno.h>

#include "rpc/server.h"
#include "rpc/rpc_error.h"
#include "picosha2/picosha2.h"

#include "logger.hpp"
#include "Uploader.hpp"

using namespace std;

Uploader::Uploader(INIReader& t_config)
    : config(t_config)
{
    auto log = logger();

	// Read in the uploader's base directory
	base_dir = config.Get("uploader", "base_dir", "");
	if (base_dir == "") {
		log->error("Invalid base directory: {}", base_dir);
		exit(EX_CONFIG);
	}
	log->info("Using base_dir {}", base_dir);

	// Read in the block size
	blocksize = (int) config.GetInteger("uploader", "blocksize", -1);
	if (blocksize <= 0) {
		log->error("Invalid block size: {}", blocksize);
		exit(EX_CONFIG);
	}
	log->info("Using a block size of {}", blocksize);

	// Read in the uploader's block placement policy
	policy = config.Get("uploader", "policy", "");
	if (policy == "") {
		log->error("Invalid placement policy: {}", policy);
		exit(EX_CONFIG);
	}
	log->info("Using a block placement policy of {}", policy);

	num_servers = (int) config.GetInteger("ssd", "num_servers", -1);
	if (num_servers <= 0) {
		log->error("num_servers {} is invalid", num_servers);
		exit(EX_CONFIG);
	}
	log->info("Number of servers: {}", num_servers);

	for (int i = 0; i < num_servers; ++i) {
		string servconf = config.Get("ssd", "server"+std::to_string(i), "");
		if (servconf == "") {
			log->error("Server {} not found in config file", i);
			exit(EX_CONFIG);
		}
		size_t idx = servconf.find(":");
		if (idx == string::npos) {
			log->error("Config line {} is invalid", servconf);
			exit(EX_CONFIG);
		}
		string host = servconf.substr(0, idx);
		int port = (int) strtol(servconf.substr(idx+1).c_str(), nullptr, 0);
		if (port <= 0 || port > 65535) {
			log->error("Invalid port number: {}", servconf);
			exit(EX_CONFIG);
		}

		log->info("  Server {}= {}:{}", i, host, port);
		ssdhosts.push_back(host);
		ssdports.push_back(port);
	}

	log->info("Uploader initalized");
}

void Uploader::upload()
{
    auto log = logger();

	vector<rpc::client*> clients;

	// Connect to all of the servers
	for (int i = 0; i < num_servers; ++i)
	{
		log->info("Connecting to server {}", i);
		try {
			clients.push_back(new rpc::client(ssdhosts[i], ssdports[i]));
			clients[i]->set_timeout(RPC_TIMEOUT);
		} catch (rpc::timeout &t) {
			log->error("Unable to connect to server {}: {}", i, t.what());
			exit(-1);
		}
	}

	// Issue a ping to each server
	for (int i = 0; i < num_servers; ++i)
	{
		log->info("Pinging server {}", i);
		try {
			clients[i]->call("ping");
			log->info("  success");
		} catch (rpc::timeout &t) {
			log->error("Error pinging server {}: {}", i, t.what());
			exit(-1);
		}
	}

	// Delete the clients
	for (int i = 0; i < num_servers; ++i)
	{
		log->info("Tearing down client {}", i);
		delete clients[i];
	}
}
