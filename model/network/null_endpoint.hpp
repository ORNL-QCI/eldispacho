#ifndef _MODEL_NETWORK_NULL_ENDPOINT_HPP
#define _MODEL_NETWORK_NULL_ENDPOINT_HPP

#include <common.hpp>
#include "node.hpp"

namespace model {
namespace network {
	/**
	 * \brief A null endpoint drops everything sent to it.
	 */
	class null_endpoint : public network::node {
	 public:
		/**
		 * \brief Constructor takes an id and the number of initial conenctions (which by
		 * default is 0).
		 */
		null_endpoint(const node::id_t id);
		
		/**
		 * \brief Virtual destructor.
		 */
		virtual ~null_endpoint();
		
		/**
		 * \brief Initialize a new instance of the class.
		 */
		static node* create(const node::id_t id) {
			return new null_endpoint(id);
		}
		
	 protected:
		/**
		 * \brief \TODO
		 */
		inline const char* name() const noexcept {
			return _name;
		}
		
	 private:
		/**
		 * \brief Register node.
		 */
		static const node::registry<null_endpoint> register_node;
		
		static constexpr const char* const _name = "null_endpoint";
	};
}
}
#endif
