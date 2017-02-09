#ifndef _MODEL_ENDPOINT_HPP
#define _MODEL_ENDPOINT_HPP

#include <common.hpp>
#include "node.hpp"

namespace model {
	/**
	 * \brief A null endpoint drops everything sent to it.
	 */
	class null_endpoint : public node {
	 public:
		/**
		 * \brief Constructor takes an id and the number of initial conenctions (which by
		 * default is 0).
		 */
		null_endpoint(const node::id_t id, const std::size_t connectionCount = 0);
		
		/**
		 * \brief Virtual destructor.
		 */
		virtual ~null_endpoint();
		
		/**
		 * \brief Initialize a new instance of the class.
		 */
		static node* create(const node::id_t id) {
			return new null_endpoint(id, 0);
		}
	
	 private:
		/**
		 * \brief Register node.
		 */
		static const node::registry<null_endpoint> name;
	};
	
	/**
	 * \brief Base class for endpoint nodes.
	 * 
	 * These nodes have a detector.
	 * 
	 * We do not override node's create() function here as this is treated as a virtual
	 * class. All children of this class MUST implement a create() function (see node).
	 */
	class base_node_endpoint : public node {
	 public:
		typedef interface::receiver receiver;
		
		/**
		 * \brief Constructor takes an id, number of connections, and detector.
		 */
		base_node_endpoint(const node::id_t id,
				const std::size_t connectionSize,
				interface::receiver&& _detector)
				: node(node_type::endpoint, id, connectionSize),
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
				receiver&& detector,
				const std::size_t connectionCount = 0);
		
		/**
		 * \brief Initialize a new instance of the class.
		 */
		static node* create(const node::id_t id) {
			return new client(id, receiver(1), 0);
		}
	
	 private:
		/**
		 * \brief Register node.
		 */
		static const node::registry<client> name;
	};
}

#endif
