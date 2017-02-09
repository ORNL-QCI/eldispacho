#include "network.hpp"

namespace model {
	network::network(const char* const topology)
			: _nodes(0), _nodeSize(0), _connections(0) {
		parse_desc(topology);
	}
	
	network::~network() {
		if(_nodes != 0) {
			for(std::size_t i = 0; i < _nodeSize; i++) {
				delete _nodes[i];
			}
			delete[] _nodes;
		}
	}
	
	node& network::find_node(const node::id_t id) {
		for(std::size_t i = 0; i < _nodeSize; i++) {
			if(_nodes[i]->id() == id) {
				return *_nodes[i];
			}
		}
		
		// A node with id was not found
		throw std::out_of_range("node not found");
	}
	
	node& network::find_connecting_node(const node::id_t id) {
		for(std::size_t i = 0; i < _nodeSize; i++) {
			if(_nodes[i]->id() == id) {
				return *_connections.next_neighbor(i);
			}
		}
		
		// A node with id was not found
		throw std::out_of_range("node not found");
	}
		
	void network::parse_desc(const char* const str) {
		assert(str != 0);
		
		::rapidjson::Document dom;
		::rapidjson::StringStream ss(str);
		dom.ParseStream(ss);
		
		// We must process connections after all nodes are added and configured
		auto connItr = dom.MemberBegin();
		bool connectionFound = false;
		
		// We must process switch connections at the end, this contains a pointer to each
		// switch dom object
		std::vector<rapidjson::Value*> switches;
		
		for(auto itr = dom.MemberBegin(); itr != dom.MemberEnd(); itr++) {
			if(!connectionFound && strcmp(itr->name.GetString(), "connection") == 0) {
				connItr = itr;
				connectionFound = true;
			} else if(strcmp(itr->name.GetString(), "switches") == 0) {
				// Iterate over switch objects
				for(auto vitr = itr->value.Begin(); vitr != itr->value.End(); vitr++) {
					_parse_validate<const char*>(*vitr, "model");
					_parse_validate<unsigned long int>(*vitr, "id");
					
					auto qswitch = node::factory::instantiate((*vitr)["model"].GetString(),
							(*vitr)["id"].GetUint64());
					
					if((*vitr).HasMember("ports")) {
						_parse_validate<unsigned long int>(*vitr, "ports");
						
						static_cast<base_node_qswitch*>(qswitch)->resize((*vitr)["ports"].GetUint64());
					}
					
					switches.push_back(&(*vitr));
					
					add_node(qswitch);
				}
			} else if(strcmp(itr->name.GetString(), "clients") == 0) {
				// Iterate over host objects
				for(auto vitr = itr->value.Begin(); vitr != itr->value.End(); vitr++) {
					
					add_node(node::factory::instantiate("client", vitr->GetUint64()));	
				}
			} else {
				throw std::invalid_argument(err_msg::_undhcse);
			}
		}
		
		if(!connectionFound) {
			throw std::invalid_argument("no connection data");
		}
		
		for(auto itr = connItr->value.Begin(); itr != connItr->value.End(); itr++) {
			add_connection((*itr)[0].GetUint(), (*itr)[1].GetUint(), true);
		}
		
		for(auto item : switches) {
			auto nswitch = static_cast<base_node_qswitch*>(&find_node((*item)["id"].GetUint64()));
			
			auto i = 0;
			for(auto itr = (*item)["connections"].Begin(); itr != (*item)["connections"].End(); itr++) {
				nswitch->connect_node(i++, &find_node(itr->GetUint64()));
			}
			nswitch->update_routing_table();
		}
	}
	
	bool network::add_node(node* newNode) {	
		// Resize
		node** newNodes = new node*[_nodeSize+1];
		memcpy(newNodes, _nodes, _nodeSize*sizeof(node*));
		delete[] _nodes;
		_nodes = newNodes;
		// Add
		_nodes[_nodeSize++] = newNode;
		
		// Update adjacency
		_connections.resize(_connections.size() + 1);
		
		return true;
	}
	
	void network::add_connection(const node::id_t a,
			const node::id_t b,
			const bool bidirectional) {
		std::size_t aIndex = 0;
		bool aFound = false;
		std::size_t bIndex = 0;
		bool bFound = false;
		for(std::size_t i = 0; i < _nodeSize; i++) {
			if(_nodes[i]->id() == a) {
				aFound = true;
				aIndex = i;
				if(bFound) {
					break;
				}
			} else if(_nodes[i]->id() == b) {
				bFound = true;
				bIndex = i;
				if(aFound) {
					break;
				}
			}
		}
		
		if(!aFound || !bFound) {
			// A node with id was not found
			throw std::out_of_range("node not found");
		}
		
		_connections.add<false>(aIndex, bIndex, _nodes[bIndex]);
		if(bidirectional) {
			_connections.add<false>(bIndex, aIndex, _nodes[aIndex]);
		}
	}
}
