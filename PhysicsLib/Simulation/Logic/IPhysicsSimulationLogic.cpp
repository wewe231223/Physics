#include "IPhysicsSimulationLogic.h"

IPhysicsSimulationLogic::IPhysicsSimulationLogic() {
}

IPhysicsSimulationLogic::~IPhysicsSimulationLogic() {
}

IPhysicsSimulationLogic::IPhysicsSimulationLogic(const IPhysicsSimulationLogic& Other) {
    (void)Other;
}

IPhysicsSimulationLogic& IPhysicsSimulationLogic::operator=(const IPhysicsSimulationLogic& Other) {
    (void)Other;
    return *this;
}

IPhysicsSimulationLogic::IPhysicsSimulationLogic(IPhysicsSimulationLogic&& Other) noexcept {
    (void)Other;
}

IPhysicsSimulationLogic& IPhysicsSimulationLogic::operator=(IPhysicsSimulationLogic&& Other) noexcept {
    (void)Other;
    return *this;
}
