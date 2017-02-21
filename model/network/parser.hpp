#ifndef _MODEL_NETWORK_PARSER_HPP
#define _MODEL_NETWORK_PARSER_HPP

#include <common.hpp>
#include "node.hpp"
#include "system.hpp"
#include <rapidjson/document.h>

namespace model {
namespace network {
	// Forward declarations
	class qswitch;
	
	namespace parser {
		/**
		 * \brief \todo
		 */
		void parse_description(const char* const str,
				system& base,
				system::node_list_t& _nodeList);
	}
}
}

#endif
