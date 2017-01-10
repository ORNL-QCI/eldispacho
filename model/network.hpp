#ifndef _MODEL_NETWORK_HPP
#define _MODEL_NETWORK_HPP

#include <common.hpp>
#include "adjacency.hpp"
#include "node.hpp"
#include "endpoint.hpp"
#include "circulator_switch.hpp"
#include <rapidjson/document.h>

namespace model {
	/**
	 * \brief \todo
	 */
	class network {
	 public:
		/**
		 * \brief Constructor
		 */
		network(const char* const topology);
		
		/**
		 * \brief Destructor.
		 */
		~network();
		
		/**
		 * \brief Return a pointer to the node with the given id.
		 * 
		 * \throws std::out_of_range if node with the given id is not found.
		 */
		node& find_node(const node::id_t id);
		
		/**
		 * \brief Return a pointer to the next neighbor connected to a node.
		 * 
		 * \throws std::out_of_range if node with the given id is not found.
		 */
		node& find_connecting_node(const node::id_t id);
	
	 private:
	 	node** _nodes;
		std::size_t _nodeSize;
		
		adjacency<node> _connections;
		
		/**
		 * \brief
		 */
		void parse_desc(const char* const str);
		
		/**
		 * \brief Add a node to the network.
		 */
		bool add_node(node* newNode);
		
		/**
		 * \brief Add a connection between two nodes.
		 * 
		 * The bidirectional flag adds a connection from a |-> b and b |-> a if true and
		 * only adds a connection from a |-> b if false.
		 * 
		 * \throws std::out_of_range if either of given ids do not correspond to nodes.
		 */
		void add_connection(const node::id_t a,
				const node::id_t b,
				const bool bidirectional = true);
		
		/**
		 * \brief Validate the existance of a json key within an object.
		 */
		inline void _parse_validate_existance(const ::rapidjson::Value& val,
				const char* const name) const {
			#ifdef THROW
			if(UNLIKELY(name == 0)) {
				throw std::invalid_argument(err_msg::_nllpntr);
			}
			if(UNLIKELY(!val.HasMember(name))) {
				throw std::invalid_argument(err_msg::_tpntfnd);
			}
			#endif
			UNUSED(val);
			UNUSED(name);
		}
		
		/**
		 * \brief Validate both the existance of a json key and the value type T within an
		 * object.
		 */
		template <typename T> inline void _parse_validate(const ::rapidjson::Value& val,
				const char* const name) const;
	};
	
	/**
	 * \brief Validate the existance of a json key and that the value is a string.
	 */
	template <> inline void network::_parse_validate<const char*>(
			const ::rapidjson::Value& val,
			const char* const name) const {
		_parse_validate_existance(val, name);
		#ifdef THROW
		if(UNLIKELY(!val[name].IsString())) {
			throw std::invalid_argument(err_msg::_badtype);
		}
		#endif
	}
	
	/**
	 * \brief Validate the existance of a json key and that the value is an unsigned long
	 * integer.
	 */
	template <> inline void network::_parse_validate<unsigned long int>(
			const ::rapidjson::Value& val,
			const char* const name) const {
		_parse_validate_existance(val, name);
		#ifdef THROW
		if(UNLIKELY(!val[name].IsUint64())) {
			throw std::invalid_argument(err_msg::_badtype);
		}
		#endif
	}
}

#endif
