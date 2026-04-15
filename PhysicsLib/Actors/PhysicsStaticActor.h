#pragma once

#include "PhysicsLib/Actors/PhysicsActor.h"

using PhysicsStaticActor = PhysicsActor<StaticSolver, StaticIntegrater, DefaultConstraintSolver, PhysicsActorBase::PhysicsActorType::Static>;
