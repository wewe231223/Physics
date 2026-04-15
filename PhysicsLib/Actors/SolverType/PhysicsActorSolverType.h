#pragma once

#include "PhysicsLib/Actors/CollisionSolver/PhysicsDynamicCollisionSolver.h"
#include "PhysicsLib/Actors/CollisionSolver/PhysicsKinematicCollisionSolver.h"
#include "PhysicsLib/Actors/CollisionSolver/PhysicsStaticCollisionSolver.h"
#include "PhysicsLib/Actors/ConstraintSolver/PhysicsNoConstraintSolver.h"
#include "PhysicsLib/Actors/Integrater/PhysicsDynamicIntegrater.h"
#include "PhysicsLib/Actors/Integrater/PhysicsKinematicIntegrater.h"
#include "PhysicsLib/Actors/Integrater/PhysicsStaticIntegrater.h"

using DynamicSolver = PhysicsDynamicCollisionSolver;
using StaticSolver = PhysicsStaticCollisionSolver;
using KinematicSolver = PhysicsKinematicCollisionSolver;
using DynamicIntegrater = PhysicsDynamicIntegrater;
using StaticIntegrater = PhysicsStaticIntegrater;
using KinematicIntegrater = PhysicsKinematicIntegrater;
using DefaultConstraintSolver = PhysicsNoConstraintSolver;
