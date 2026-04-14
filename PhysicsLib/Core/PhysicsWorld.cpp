#include "PhysicsWorld.h"

#include <utility>

#include "SpatialQuery/BruteForcePhysicsSpatialQuery.h"
#include "Repository/IPhysicsActorRepository.h"
#include "Logic/IPhysicsSimulationLogic.h"
#include "SpatialQuery/IPhysicsSpatialQuery.h"
#include "Repository/PhysicsActorRepository.h"
#include "Logic/PhysicsDynamicCollisionLogic.h"
#include "Logic/PhysicsDynamicIntegrationLogic.h"

PhysicsWorld::PhysicsWorld()
    : mSettings{ 1.0F / 60.0F, DirectX::SimpleMath::Vector3{ 0.0F, -9.8F, 0.0F } },
      mAccumulator{},
      mActorRepository{},
      mSpatialQuery{},
      mSimulationLogics{},
      mPublishedEvents{} {
    InitializeDependencies();
    InitializeSimulationLogics();
}

PhysicsWorld::~PhysicsWorld() {
}

PhysicsWorld::PhysicsWorld(const PhysicsWorld& Other)
    : mSettings{ Other.mSettings },
      mAccumulator{ Other.mAccumulator },
      mActorRepository{ Other.mActorRepository != nullptr ? Other.mActorRepository->Clone() : nullptr },
      mSpatialQuery{ Other.mSpatialQuery != nullptr ? Other.mSpatialQuery->Clone() : nullptr },
      mSimulationLogics{},
      mPublishedEvents{} {
    InitializeDependencies();
    InitializeSimulationLogics();
}

PhysicsWorld& PhysicsWorld::operator=(const PhysicsWorld& Other) {
    if (this == &Other) {
        return *this;
    }

    mSettings = Other.mSettings;
    mAccumulator = Other.mAccumulator;
    mActorRepository = Other.mActorRepository != nullptr ? Other.mActorRepository->Clone() : nullptr;
    mSpatialQuery = Other.mSpatialQuery != nullptr ? Other.mSpatialQuery->Clone() : nullptr;
    mPublishedEvents.clear();
    InitializeDependencies();
    InitializeSimulationLogics();

    return *this;
}

PhysicsWorld::PhysicsWorld(PhysicsWorld&& Other) noexcept
    : mSettings{ Other.mSettings },
      mAccumulator{ Other.mAccumulator },
      mActorRepository{ std::move(Other.mActorRepository) },
      mSpatialQuery{ std::move(Other.mSpatialQuery) },
      mSimulationLogics{ std::move(Other.mSimulationLogics) },
      mPublishedEvents{ std::move(Other.mPublishedEvents) } {
    Other.mSettings = WorldSettings{ 1.0F / 60.0F, DirectX::SimpleMath::Vector3{} };
    Other.mAccumulator = 0.0F;
    Other.InitializeDependencies();
    Other.InitializeSimulationLogics();
    Other.ClearPublishedEvents();
}

PhysicsWorld& PhysicsWorld::operator=(PhysicsWorld&& Other) noexcept {
    if (this == &Other) {
        return *this;
    }

    mSettings = Other.mSettings;
    mAccumulator = Other.mAccumulator;
    mActorRepository = std::move(Other.mActorRepository);
    mSpatialQuery = std::move(Other.mSpatialQuery);
    mSimulationLogics = std::move(Other.mSimulationLogics);
    mPublishedEvents = std::move(Other.mPublishedEvents);

    Other.mSettings = WorldSettings{ 1.0F / 60.0F, DirectX::SimpleMath::Vector3{} };
    Other.mAccumulator = 0.0F;
    Other.InitializeDependencies();
    Other.InitializeSimulationLogics();
    Other.ClearPublishedEvents();

    return *this;
}

PhysicsWorld::PhysicsWorld(const WorldSettings& Settings)
    : mSettings{ Settings },
      mAccumulator{},
      mActorRepository{},
      mSpatialQuery{},
      mSimulationLogics{},
      mPublishedEvents{} {
    InitializeDependencies();
    InitializeSimulationLogics();
}

void PhysicsWorld::Initialize(const WorldSettings& Settings) {
    mSettings = Settings;
    mAccumulator = 0.0F;
    if (mActorRepository != nullptr) {
        mActorRepository->ClearActors();
    }
    ClearPublishedEvents();
}

PhysicsDynamicActor* PhysicsWorld::CreateDynamicActor(const PhysicsDynamicActor::ActorDesc& Desc) {
    return mActorRepository->CreateDynamicActor(Desc);
}

PhysicsKinematicActor* PhysicsWorld::CreateKinematicActor(const PhysicsKinematicActor::ActorDesc& Desc) {
    return mActorRepository->CreateKinematicActor(Desc);
}

PhysicsTerrainActor* PhysicsWorld::CreateTerrainActor(const PhysicsTerrainActor::ActorDesc& Desc) {
    return mActorRepository->CreateTerrainActor(Desc);
}

void PhysicsWorld::AddActor(std::unique_ptr<PhysicsActor> Actor) {
    mActorRepository->AddActor(std::move(Actor));
}

void PhysicsWorld::ClearActors() {
    mActorRepository->ClearActors();
}

PhysicsActor* PhysicsWorld::GetActor(std::size_t Index) {
    return mActorRepository->GetActor(Index);
}

const PhysicsActor* PhysicsWorld::GetActor(std::size_t Index) const {
    return mActorRepository->GetActor(Index);
}

std::size_t PhysicsWorld::GetActorCount() const {
    std::size_t ActorCount{ mActorRepository->GetActorCount() };
    return ActorCount;
}

const PhysicsWorld::WorldSettings& PhysicsWorld::GetSettings() const {
    return mSettings;
}

float PhysicsWorld::GetAccumulator() const {
    return mAccumulator;
}

void PhysicsWorld::StepSimulation() {
    ClearPublishedEvents();

    std::size_t LogicCount{ mSimulationLogics.size() };
    for (std::size_t LogicIndex{ 0U }; LogicIndex < LogicCount; ++LogicIndex) {
        IPhysicsSimulationLogic* CurrentLogic{ mSimulationLogics[LogicIndex].get() };
        if (CurrentLogic == nullptr) {
            continue;
        }

        CurrentLogic->Execute(*this, mSettings.FixedTimeStep);
    }
}

void PhysicsWorld::Update(float DeltaTime) {
    mAccumulator += DeltaTime;

    while (mAccumulator >= mSettings.FixedTimeStep) {
        StepSimulation();
        mAccumulator -= mSettings.FixedTimeStep;
    }
}

const DirectX::SimpleMath::Vector3& PhysicsWorld::GetGravity() const {
    return mSettings.Gravity;
}

IPhysicsActorRepository& PhysicsWorld::GetActorRepository() {
    return *mActorRepository;
}

const IPhysicsActorRepository& PhysicsWorld::GetActorRepository() const {
    return *mActorRepository;
}

IPhysicsSpatialQuery& PhysicsWorld::GetSpatialQuery() {
    return *mSpatialQuery;
}

const IPhysicsSpatialQuery& PhysicsWorld::GetSpatialQuery() const {
    return *mSpatialQuery;
}

void PhysicsWorld::PublishEvent(PhysicsSimulationEventType EventType, const PhysicsActor* FirstActor, const PhysicsActor* SecondActor) {
    mPublishedEvents.push_back(PhysicsSimulationEvent{ EventType, FirstActor, SecondActor });
}

void PhysicsWorld::ClearPublishedEvents() {
    mPublishedEvents.clear();
}

const std::vector<PhysicsSimulationEvent>& PhysicsWorld::GetPublishedEvents() const {
    return mPublishedEvents;
}

void PhysicsWorld::InitializeDependencies() {
    if (mActorRepository == nullptr) {
        mActorRepository = std::make_unique<PhysicsActorRepository>();
    }

    if (mSpatialQuery == nullptr) {
        mSpatialQuery = std::make_unique<BruteForcePhysicsSpatialQuery>();
    }
}

void PhysicsWorld::InitializeSimulationLogics() {
    mSimulationLogics.clear();
    mSimulationLogics.push_back(std::make_unique<PhysicsDynamicIntegrationLogic>());
    mSimulationLogics.push_back(std::make_unique<PhysicsDynamicCollisionLogic>());
}
