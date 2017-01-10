#include <common.hpp>
#include "diagnostics/server.hpp"
#include "buffer.hpp"
#include "net/server.hpp"
#include "processor.hpp"
#include <csignal>
#include <atomic>
#include <chrono>
#include <fstream>
#include <iostream>
#include <thread>
#include "boost/program_options.hpp"

#define DEFAULT_RX_SERVER_THREAD_COUNT 1
#define DEFAULT_TX_SERVER_THREAD_COUNT 1
#define DEFAULT_SABOT_CLIENT_THREAD_COUNT 1

std::sig_atomic_t signal_code = 0;

void term_handler(int code) {
	signal_code = code;
}

int main(int argc, char *argv[]) {
	// Program parameters
	std::string topology;
	std::string loggerEndpoint;
	std::string rxServerEndpoint;
	std::size_t rxServerThreadCount(DEFAULT_RX_SERVER_THREAD_COUNT);
	std::string txServerEndpoint;
	std::size_t txServerThreadCount(DEFAULT_TX_SERVER_THREAD_COUNT);
	std::string sabotLocation;
	std::size_t sabotClientThreadCount(DEFAULT_SABOT_CLIENT_THREAD_COUNT);
	
	try {
		namespace po = boost::program_options;
		
		po::options_description desc("Options");
		desc.add_options()
			("help,h", "Print help")
			("topology,t", po::value<std::string>(&topology), "Topology string or file")
			("logger,l", po::value<std::string>(&loggerEndpoint), "Logger Server Endpoint")
			("rs", po::value<std::string>(&rxServerEndpoint)->required(), "Rx Server Endpoint")
			("rt", po::value<std::size_t>(&rxServerThreadCount), "Rx Server Thread Count")
			("ts", po::value<std::string>(&txServerEndpoint)->required(), "Tx Server Endpoint")
			("tt", po::value<std::size_t>(&txServerThreadCount), "Tx Server Thread Count")
			("s", po::value<std::string>(&sabotLocation)->required(), "Sabot Location")
			("st", po::value<std::size_t>(&sabotClientThreadCount), "Sabot client Thread Count");
		
		po::variables_map vm;
		po::store(po::parse_command_line(argc, argv, desc), vm);
		
		if(vm.count("help")) {
			std::cout << desc << std::endl;
			exit(0);
		}
		
		po::notify(vm);
		
	} catch(boost::program_options::required_option &e) {
		std::cerr << e.what() << std::endl;
		exit(-1);
	} catch(boost::program_options::error &e) {
		std::cerr << e.what() << std::endl;
		exit(-1);
	}
	
	// We try to be smart here: if the topology looks like JSON, we send it directly to be
	// parsed. If it doesn't look like JSON, we see if it is a file that could contain
	// JSON.
	if(topology.find('{') == std::string::npos) {
		try {
			std::ifstream topoFile;
			topoFile.exceptions(std::ios::badbit | std::ios::failbit);
			topoFile.open(topology);
			
			topology = std::string(std::istreambuf_iterator<char>(topoFile),
					std::istreambuf_iterator<char>());
			
			if(topoFile.tellg() == 0) {
				throw std::ios_base::failure("Zero length file");
			}
			
			if(topology.find('{') == std::string::npos) {
				throw std::ios_base::failure("No JSON in file.");
			}
			
		} catch(const std::ios_base::failure& e) {
			std::cerr << "Topology was invalid JSON string and invalid file: " <<
					e.what() << std::endl;
			exit(-1);
		}
	}
	
	// This context object is what our sockets are associated with and it takes care of
	// our network io. It's most efficient to have just one context for all the different
	// components that use zmq. The argument is the size of the zmq thread pool to handle
	// I/O operations. It is suggested to use a size of 1 unless processing more than 3-6
	// million messages per second. See zmq documentation for more details.
	::zmq::context_t context(1);
	
	// Diagnostics Logger
	diagnostics::logger* logger(nullptr);
	
	if(loggerEndpoint.size() != 0) {
		logger = new diagnostics::server(context, loggerEndpoint.c_str());
	} else {
		logger = new diagnostics::server();
	}
	
	logger->start();
	
	// topology
	model::state state(topology.c_str());
	
	// Processor
	processor worker(logger, sabotLocation.c_str(), state, context);
	worker.start(sabotClientThreadCount);
	
	// Client facing server
	net::rpc_server frontend(context,
							logger, 
							worker,
							rxServerEndpoint.c_str(), 
							txServerEndpoint.c_str());
	frontend.listen(rxServerThreadCount, txServerThreadCount);
	
	// Handle term signal
	struct sigaction sigIntHandler;
	sigIntHandler.sa_handler = term_handler;
	sigemptyset(&sigIntHandler.sa_mask);
	sigIntHandler.sa_flags = 0;
	sigaction(SIGINT, &sigIntHandler, 0);
	sigaction(SIGTERM, &sigIntHandler, 0);
	
	// Wait for signal
	pause();
	std::cout << "Caught signal " << signal_code << "." << std::endl;
	
	// Cleanup
	frontend.stop();
	worker.stop();
	
	if(logger) {
		delete logger;
	}
	
	return 0;
}
