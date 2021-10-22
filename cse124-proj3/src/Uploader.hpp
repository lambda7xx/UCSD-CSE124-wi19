#ifndef UPLOADER_HPP
#define UPLOADER_HPP

#include <string>
#include <vector>

#include "inih/INIReader.h"
#include "rpc/client.h"

#include "SurfStoreTypes.hpp"
#include "logger.hpp"

using namespace std;

class Uploader {
public:
    Uploader(INIReader& t_config);

	void upload();

	const uint64_t RPC_TIMEOUT = 500; // milliseconds

protected:

    INIReader& config;

	string base_dir;
	int blocksize;
	string policy;

	int num_servers;
	vector<string> ssdhosts;
	vector<int> ssdports;
};

#endif // UPLOADER_HPP
