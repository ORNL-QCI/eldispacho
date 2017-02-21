#ifndef _MODEL_NETWORK_ENDPOINT_HPP
#define _MODEL_NETWORK_ENDPOINT_HPP

#include <common.hpp>
#include "node.hpp"

namespace model {
namespace network {	
	/**
	 * \brief Base class for endpoint nodes.
	 * 
	 * These nodes have a detector.
	 * 
	 * We do not override node's create() function here as this is treated as a virtual
	 * class. All children of this class MUST implement a create() function (see node).
	 */
	class base_node_endpoint : public network::node {
	 public:
		typedef interface::receiver receiver;
		
		/**
		 * \brief Constructor takes an id, number of connections, and detector.
		 */
		base_node_endpoint(const node::id_t id,
				interface::receiver&& _detector)
				: node(type_t::endpoint, id),
				_detector(std::move(_detector)) {
		}
		
		/**
		 * \brief Virtual destructor.
		 */
		virtual ~base_node_endpoint() {
		}
		
		/**
		 * \brief Return a non-mutable reference to the detector.
		 */
		inline const interface::receiver& get_detector() const {
			return _detector;
		}
		
		/**
		 * \brief Reconfigure the client's detector.
		 */
		inline void configure_detector(receiver&& newDetector) {
			_detector = std::move(newDetector);
		}
		
	 private:
		/**
		 * \brief The detector.
		 */
		receiver _detector;
	};
	
	/**
	 * \brief A client is a node that contains a detector configuration and is associated
	 * with a host.
	 */
	struct client : public base_node_endpoint {
	 public:
		/**
		 * \brief Constructor takes node id, initial client_detector object, and the
		 * initial number of connections (which is by default 0).
		 */
		client(const node::id_t id,
				receiver&& detector);
		
		/**
		 * \brief Initialize a new instance of the class.
		 */
		static node* create(const node::id_t id) {
			return new client(id, receiver(1));
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
		static const node::registry<client> register_node;
		
		static constexpr const char* const _name = "client";
	};
}
}

#endif
