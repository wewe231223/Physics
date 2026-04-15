#pragma once

#include "PhysicsLib/Actors/PhysicsActor.h"

using PhysicsDynamicActor = PhysicsActor<DynamicSolver, DynamicIntegrater, DefaultConstraintSolver, PhysicsActorBase::PhysicsActorType::Dynamic>;
