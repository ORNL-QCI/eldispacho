#include "parser.hpp"
#include "endpoint.hpp"
#include "qswitch.hpp"
#include "debugger.hpp"
#include <numeric>
#include <type_traits>
#include <vector>

/**
 * \brief Short alias of std::to_string()
 */
#define TOS(x) std::to_string(x)

namespace model {
namespace network {
namespace parser {
	using ::model::network::node;
	
	/**
	 * \brief Exception for when type is wrong.
	 */
	class bad_type : public std::exception {
	 public:
		bad_type(const std::string& msg) : msg(msg) {}
		bad_type(std::string&& msg) : msg(msg) {}
		virtual ~bad_type() = default;
		
		virtual const char* what() const noexcept { return msg.c_str(); }
	 protected:
		std::string msg;
	};
	
	/**
	 * \brief Exception for when value is wrong.
	 */
	class bad_value : public std::exception {
	 public:
		bad_value(const std::string& msg) : msg(msg) {}
		bad_value(std::string&& msg) : msg(msg) {}
		virtual ~bad_value() = default;
		
		virtual const char* what() const noexcept { return msg.c_str(); }
	 protected:
		std::string msg;
	};
	
	/**
	 * \brief Raise an exception with the concatenated parts as a message.
	 */
	template <typename T> void raise(std::vector<std::string>&& parts) {
		static_assert(std::is_base_of<std::exception, T>::value,
				"Type T is not child of std::exception");
		throw T(std::accumulate(parts.begin(), parts.end(), std::string()));
	}
	
	/**
	 * \brief Raise an exception with the given message.
	 */
	template <typename T> void raise(const char* const msg) {
		static_assert(std::is_base_of<std::exception, T>::value,
				"Type T is not child of std::exception");
		throw T(std::string(msg));
	}
	
	void parse_description_node(
			std::map<node::id_t, node*>& _nodeList,
			node& parent,
			rapidjson::Value& it,
			std::vector<std::pair<rapidjson::Value*, qswitch*>>& switches) {
		const auto& object = it.GetObject();
		
		if(!object.HasMember("id")) raise<bad_value>("no id in node");
		if(!object["id"].IsUint64()) raise<bad_type>("id in node");
		
		const node::id_t id = object["id"].GetUint64();
		
		// Check if id is already in use
		if(_nodeList.find(id) != _nodeList.end()) {
			std::string msg = "Duplicate node id ";
			msg += std::to_string(id);
			throw std::invalid_argument(msg);
		}
		
		if(!object.HasMember("model")) {
			throw std::invalid_argument("Missing 'model' in node");
		}
		if(!object["model"].IsString()) {
			throw std::invalid_argument("Wrong type for 'model' in node");
		}
		const char* const model = object["model"].GetString();
		
		node& newNode= *node::factory::instantiate(model, id);
		
		switch(newNode.type()) {
		 case node::type_t::qswitch:
			if(!object.HasMember("portCount")) {
				raise<bad_value>({"no 'portcount' in ", model});
			}
			if(!object["portCount"].IsUint64()) {
				raise<bad_type>({"'portcount' in ", model});
			}
			const qswitch::port_id_t portCount = object["portCount"].GetUint64();
			
			static_cast<qswitch&>(newNode).resize(portCount);
			switches.push_back({&it, static_cast<qswitch*>(&newNode)});
			break;
		}
		
		if(!parent.add_child(newNode)) {
			std::string msg = "Cannot add child id ";
			msg += std::to_string(newNode.id());
			throw std::runtime_error(msg);
		}
		_nodeList[newNode.id()] = &newNode;
		
		if(it.HasMember("nodes")) {
			if(!it["nodes"].IsArray()) raise<bad_type>("'nodes'");
			for(auto& dit : it["nodes"].GetArray()) {
				parse_description_node(_nodeList, newNode, dit, switches);
			}
		}
	}
	
	void parse_description(const char* const str,
			system& base,
			system::node_list_t& _nodeList) {
		::rapidjson::Document dom;
		dom.Parse(str);
		
		if(!dom.HasMember("nodes")) raise<bad_value>("no 'nodes' root");
		if(!dom["nodes"].IsArray()) raise<bad_type>("'nodes' root");
		if(!dom.HasMember("connections")) raise<bad_value>("no 'connections' root");
		if(!dom["connections"].IsArray()) raise<bad_value>("'connections' root");
		
		// We have to connect nodes to switches after all nodes are initialized,
		std::vector<std::pair<rapidjson::Value*, qswitch*>> switches;
		
		// Recursively parse nodes
		for(auto& it : dom["nodes"].GetArray()) {
			parse_description_node(_nodeList, base, it, switches);
		}
		
		for(auto s : switches) {
			auto& jobj = *s.first;
			auto& nobj = *s.second;
			
			// No specific port connections, assuming all are null
			if(!jobj.HasMember("ports")) continue;
			if(!jobj["ports"].IsArray()) {
				raise<bad_type>({"ports in node ", TOS(nobj.id())});
			}
			
			const auto& ports = jobj["ports"].GetArray();
			if(ports.Size() > nobj.port_count()) {
				raise<bad_value>({"ports in node ", TOS(nobj.id())});
			}
			
			for(uint64_t i = 0; i < ports.Size(); i++) {
				if(!ports[i].IsUint64()) {
					if(ports[i].IsString() &&
							strcmp("null", ports[i].GetString()) == 0) {
						// Port is not connected
						continue;
					}
					raise<bad_type>({"port in node ", TOS(nobj.id())});
				}
				
				const auto nit = _nodeList.find(ports[i].GetUint64());
				if(nit == _nodeList.end()) {
					raise<bad_value>({"port endpoint ", TOS(ports[i].GetUint64())});
				}
				
				if(!nobj.is_connected(nit->second->id())) {
					if(!nobj.add_connection(*nit->second)) {
						std::string msg = "Cannot connect node ";
						msg += std::to_string(nobj.id());
						msg += " to node ";
						msg += std::to_string(nit->second->id());
						throw std::runtime_error(msg);
					}
				}
				
				if(!nobj.connect_port(i, nit->second->id())) {
					std::string msg = "Cannot connect port ";
					msg += std::to_string(i);
					msg += " to node ";
					msg += std::to_string(nobj.id());
					throw std::runtime_error(msg);
				}
			}
		}
		
		for(auto& it : dom["connections"].GetArray()) {
			const auto& object = it.GetObject();
			
			if(!object.HasMember("endpoints")) {
				raise<bad_value>("no endpoints in connection");
			}
			if(!object["endpoints"].IsArray()) {
				raise<bad_type>("endpoints in connection");
			}
			const auto& array = object["endpoints"].GetArray();
			
			// Perhaps in another universe this may be different...
			if(array.Size() != 2) {
				raise<bad_value>("size of endpoints in connection");
			}
			
			if(!array[0].IsUint64()) {
				raise<bad_type>("endpoint in endpoints in connection");
			}
			if(!array[1].IsUint64()) {
			}
			const node::id_t id1 = array[0].GetUint64();
			const node::id_t id2 = array[1].GetUint64();
			const auto nit1 = _nodeList.find(id1);
			const auto nit2 = _nodeList.find(id2);
			if(nit1 == _nodeList.end()) {
				std::string msg = "Connection endpoint node id ";
				msg += std::to_string(id1);
				msg += " is invalid";
				throw std::invalid_argument(msg);
			}
			if(nit2 == _nodeList.end()) {
				std::string msg = "Connection endpoint node id ";
				msg += std::to_string(id2);
				msg += " is invalid";
				throw std::invalid_argument(msg);
			}
			
			if(!nit1->second->add_connection(*(nit2->second))) {
				/** \FIXME: This may hide mistakes in input (e.g. duplicates */
				if(nit1->second->is_connected(nit2->second->id())) {
					continue;
				}
				std::string msg = "Add connection failed for node id ";
				msg += std::to_string(id1);
				msg += " and node id ";
				msg += std::to_string(id2);
				throw std::invalid_argument(msg);
			}
		}
	}
}
}
}
