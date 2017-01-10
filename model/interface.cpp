#include "interface.hpp"

namespace model {
	namespace interface {
		transmitter::transmitter(base::id_t id)
				: base(id) {
		}
		
		transmitter::transmitter(base::id_t id,
				simulator::unit&& simulationUnit)
				: base(id, std::move(simulationUnit)) {
		}
		
		transmitter::transmitter(transmitter&& old)
				: base(std::move(old)) {
		}
		
		transmitter& transmitter::operator=(transmitter&& old) {
			base::operator=(std::move(old));
			
			return *this;
		}
		
		receiver::receiver(base::id_t id)
				: base(id) {
		}
		
		receiver::receiver(base::id_t id,
				simulator::unit&& simulationUnit)
				: base(id, std::move(simulationUnit)) {
		}
		
		receiver::receiver(receiver&& old)
				: base(std::move(old)) {
		}
		
		receiver& receiver::operator=(receiver&& old) {
			base::operator=(std::move(old));
			
			return *this;
		}
	}
}
