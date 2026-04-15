#pragma once

#include "PhysicsLib/Actors/PhysicsActor.h"

using PhysicsKinematicActor = PhysicsActor<KinematicSolver, KinematicIntegrater, DefaultConstraintSolver, PhysicsActorBase::PhysicsActorType::Kinematic>;
