#ifndef _MODEL_INTERFACE_HPP
#define _MODEL_INTERFACE_HPP

#include <common.hpp>
#include <simulator/unit.hpp>

namespace model {
	namespace interface {
		/**
		 * \brief \todo
		 */
		enum class type {
			rx,
			tx
		};
		
		/**
		 * \brief \todo
		 */
		template <::model::interface::type T> struct base {
		 public:
			typedef std::uint_fast64_t id_t;
			
			/**
			 * \brief Null simulation unit constructor takes only an id for the interface.
			 */
			base<T>(id_t id)
					: _id(id),
					_simulationUnit(0, 0, 0) {
			}
			
			/**
			 * \brief Constructor that takes both an interface id and the simulation unit.
			 */
			base<T>(id_t id,
					::simulator::unit&& simulationUnit)
					: _id(id),
					_simulationUnit(std::move(simulationUnit)) {
			}
			
			/**
			 * \brief Copy constructor is disabled.
			 */
			base<T>(const base<T>&) = delete;
			
			/**
			 * \brief Move constructor.
			 */
			base<T>(base<T>&& old)
					: _type(old._type),
					_id(old._id),
					_simulationUnit(std::move(old._simulationUnit)) {
			}
			
			/**
			 * \brief Assignment operator is disabled.
			 */
			base<T>& operator=(const base<T>&) = delete;
			
			/**
			 * \brief Move assignment operator.
			 */
			base<T>& operator=(base<T>&& old) {
				_type = old._type;
				_id = old._id;
				_simulationUnit = std::move(old._simulationUnit);
				
				return *this;
			}
			
			/**
			 * \brief Virtual destructor.
			 */
			virtual ~base<T>() {
			}
			
			/**
			 * \brief \todo
			 */
			::model::interface::type type() const {
				return _type;
			}
			
			/**
			 * \brief Return the interface id.
			 */
			id_t id() const {
				return _id;
			}
			
			/**
			 * \brief Return an immutable reference to the interface's simulation unit.
			 */
			const ::simulator::unit& simulation_unit() const {
				return _simulationUnit;
			}
		
		 private:
			/**
			 * \brief \todo
			 */
			::model::interface::type _type = T;
			
			/**
			 * \brief The id of the interface.
			 */
			id_t _id;
			
			/**
			 * \brief The simulation unit associated with the interface that is used to
			 * modify a state interacting with the interface.
			 */
			::simulator::unit _simulationUnit;
		};
		
		/**
		 * \brief \todo
		 */
		struct transmitter : public base<interface::type::tx> {
		 public:
			/**
			 * \brief Null simulation unit constructor takes only an id for the interface.
			 */
			transmitter(base::id_t id);
			
			/**
			 * \brief Constructor that takes both an interface id and the simulation unit.
			 */
			transmitter(base::id_t id,
					simulator::unit&& simulationUnit);
			
			/**
			 * \brief Move constructor.
			 */
			transmitter(transmitter&& old);
			
			/**
			 * \brief Move assignment operator.
			 */
			transmitter& operator=(transmitter&& old);
		};
		
		/**
		 * \brief \todo
		 */
		struct receiver : public base<interface::type::rx> {
		 public:
			/**
			 * \brief Null simulation unit constructor takes only an id for the interface.
			 */
			receiver(base::id_t id);
			
			/**
			 * \brief Constructor that takes both an interface id and the simulation unit.
			 */
			receiver(base::id_t id,
					simulator::unit&& simulationUnit);
			
			/**
			 * \brief Move constructor.
			 */
			receiver(receiver&& old);
			
			/**
			 * \brief Move assignment operator.
			 */
			receiver& operator=(receiver&& old);
		};
	}
}

#endif
