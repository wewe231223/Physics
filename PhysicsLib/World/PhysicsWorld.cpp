#include "PhysicsWorld.h"

#include <algorithm>
#include <chrono>
#include <utility>

#include "PhysicsLib/Simulation/Logic/IPhysicsSimulationLogic.h"
#include "PhysicsLib/Simulation/Logic/PhysicsDynamicCollisionLogic.h"
#include "PhysicsLib/Simulation/Logic/PhysicsDynamicIntegrationLogic.h"
#include "PhysicsLib/Simulation/Repository/IPhysicsActorRepository.h"
#include "PhysicsLib/Simulation/Repository/PhysicsActorRepository.h"
#include "PhysicsLib/Simulation/SpatialQuery/BruteForcePhysicsSpatialQuery.h"
#include "PhysicsLib/Simulation/SpatialQuery/IPhysicsSpatialQuery.h"

namespace {
DirectX::SimpleMath::Vector3 InterpolateVector3(const DirectX::SimpleMath::Vector3& StartValue, const DirectX::SimpleMath::Vector3& EndValue, float Alpha) {
    DirectX::SimpleMath::Vector3 InterpolatedValue{ StartValue + ((EndValue - StartValue) * Alpha) };
    return InterpolatedValue;
}

PhysicsFrameAccumulator::ActorState CreateActorStateFromActor(const PhysicsActorBase& Actor) {
    PhysicsFrameAccumulator::ActorState ActorStateValue{ &Actor, Actor.GetActorType(), Actor.GetPosition(), Actor.GetRotation(), Actor.GetScale() };
    return ActorStateValue;
}
}

PhysicsFrameAccumulator::PhysicsFrameAccumulator()
    : mFixedTimeStep{ 1.0F / 60.0F },
      mAccumulatedTime{},
      mPreviousStates{},
      mCurrentStates{} {
}

PhysicsFrameAccumulator::~PhysicsFrameAccumulator() {
}

PhysicsFrameAccumulator::PhysicsFrameAccumulator(const PhysicsFrameAccumulator& Other)
    : mFixedTimeStep{ Other.mFixedTimeStep },
      mAccumulatedTime{ Other.mAccumulatedTime },
      mPreviousStates{ Other.mPreviousStates },
      mCurrentStates{ Other.mCurrentStates } {
}

PhysicsFrameAccumulator& PhysicsFrameAccumulator::operator=(const PhysicsFrameAccumulator& Other) {
    if (this == &Other) {
        return *this;
    }

    mFixedTimeStep = Other.mFixedTimeStep;
    mAccumulatedTime = Other.mAccumulatedTime;
    mPreviousStates = Other.mPreviousStates;
    mCurrentStates = Other.mCurrentStates;

    return *this;
}

PhysicsFrameAccumulator::PhysicsFrameAccumulator(PhysicsFrameAccumulator&& Other) noexcept
    : mFixedTimeStep{ Other.mFixedTimeStep },
      mAccumulatedTime{ Other.mAccumulatedTime },
      mPreviousStates{ std::move(Other.mPreviousStates) },
      mCurrentStates{ std::move(Other.mCurrentStates) } {
    Other.mFixedTimeStep = 1.0F / 60.0F;
    Other.mAccumulatedTime = 0.0F;
}

PhysicsFrameAccumulator& PhysicsFrameAccumulator::operator=(PhysicsFrameAccumulator&& Other) noexcept {
    if (this == &Other) {
        return *this;
    }

    mFixedTimeStep = Other.mFixedTimeStep;
    mAccumulatedTime = Other.mAccumulatedTime;
    mPreviousStates = std::move(Other.mPreviousStates);
    mCurrentStates = std::move(Other.mCurrentStates);

    Other.mFixedTimeStep = 1.0F / 60.0F;
    Other.mAccumulatedTime = 0.0F;

    return *this;
}

PhysicsFrameAccumulator::PhysicsFrameAccumulator(float FixedTimeStep)
    : mFixedTimeStep{ FixedTimeStep > 0.0F ? FixedTimeStep : (1.0F / 60.0F) },
      mAccumulatedTime{},
      mPreviousStates{},
      mCurrentStates{} {
}

void PhysicsFrameAccumulator::Initialize(float FixedTimeStep) {
    mFixedTimeStep = FixedTimeStep > 0.0F ? FixedTimeStep : (1.0F / 60.0F);
    mAccumulatedTime = 0.0F;
    mPreviousStates.clear();
    mCurrentStates.clear();
}

void PhysicsFrameAccumulator::AddDeltaTime(float DeltaTime) {
    if (DeltaTime <= 0.0F) {
        return;
    }

    mAccumulatedTime += DeltaTime;
}

bool PhysicsFrameAccumulator::TryConsumeFixedStep() {
    if (mFixedTimeStep <= 0.0F) {
        return false;
    }

    if (mAccumulatedTime < mFixedTimeStep) {
        return false;
    }

    mAccumulatedTime -= mFixedTimeStep;
    return true;
}

float PhysicsFrameAccumulator::GetAccumulatedTime() const {
    return mAccumulatedTime;
}

float PhysicsFrameAccumulator::GetInterpolationAlpha() const {
    if (mFixedTimeStep <= 0.0F) {
        return 0.0F;
    }

    float RawAlpha{ mAccumulatedTime / mFixedTimeStep };
    float InterpolationAlpha{ std::clamp(RawAlpha, 0.0F, 1.0F) };
    return InterpolationAlpha;
}

void PhysicsFrameAccumulator::SynchronizeStatePair(const IPhysicsActorRepository& ActorRepository) {
    CaptureState(mCurrentStates, ActorRepository);
    mPreviousStates = mCurrentStates;
}

void PhysicsFrameAccumulator::CapturePreviousState(const IPhysicsActorRepository& ActorRepository) {
    CaptureState(mPreviousStates, ActorRepository);
}

void PhysicsFrameAccumulator::CaptureCurrentState(const IPhysicsActorRepository& ActorRepository) {
    CaptureState(mCurrentStates, ActorRepository);
}

bool PhysicsFrameAccumulator::TryGetInterpolatedState(const PhysicsActorBase& Actor, DirectX::SimpleMath::Vector3& OutPosition, DirectX::SimpleMath::Vector3& OutRotation, DirectX::SimpleMath::Vector3& OutScale) const {
    ActorState PreviousState{};
    ActorState CurrentState{};
    bool HasPreviousState{ TryGetActorState(mPreviousStates, Actor, PreviousState) };
    bool HasCurrentState{ TryGetActorState(mCurrentStates, Actor, CurrentState) };
    if (!HasPreviousState && !HasCurrentState) {
        return false;
    }

    if (!HasPreviousState) {
        OutPosition = CurrentState.mPosition;
        OutRotation = CurrentState.mRotation;
        OutScale = CurrentState.mScale;
        return true;
    }

    if (!HasCurrentState) {
        OutPosition = PreviousState.mPosition;
        OutRotation = PreviousState.mRotation;
        OutScale = PreviousState.mScale;
        return true;
    }

    float InterpolationAlpha{ GetInterpolationAlpha() };
    OutPosition = InterpolateVector3(PreviousState.mPosition, CurrentState.mPosition, InterpolationAlpha);
    OutRotation = InterpolateVector3(PreviousState.mRotation, CurrentState.mRotation, InterpolationAlpha);
    OutScale = InterpolateVector3(PreviousState.mScale, CurrentState.mScale, InterpolationAlpha);
    return true;
}

void PhysicsFrameAccumulator::CaptureState(std::vector<ActorState>& OutStates, const IPhysicsActorRepository& ActorRepository) const {
    OutStates.clear();

    std::size_t ActorCount{ ActorRepository.GetActorCount() };
    OutStates.reserve(ActorCount);
    for (std::size_t ActorIndex{ 0U }; ActorIndex < ActorCount; ++ActorIndex) {
        const PhysicsActorBase* ActorPointer{ ActorRepository.GetActor(ActorIndex) };
        if (ActorPointer == nullptr) {
            continue;
        }

        ActorState CapturedState{ CreateActorStateFromActor(*ActorPointer) };
        OutStates.push_back(CapturedState);
    }
}

bool PhysicsFrameAccumulator::TryGetActorState(const std::vector<ActorState>& States, const PhysicsActorBase& Actor, ActorState& OutActorState) const {
    std::size_t StateCount{ States.size() };
    for (std::size_t StateIndex{ 0U }; StateIndex < StateCount; ++StateIndex) {
        const ActorState& CurrentState{ States[StateIndex] };
        if (CurrentState.mActorPointer != &Actor) {
            continue;
        }

        OutActorState = CurrentState;
        return true;
    }

    return false;
}

PhysicsWorld::PhysicsWorld()
    : mSettings{ 1.0F / 60.0F, DirectX::SimpleMath::Vector3{ 0.0F, -9.8F, 0.0F } },
      mFrameAccumulator{ 1.0F / 60.0F },
      mLastUpdateStepCount{},
      mLastUpdateStepElapsedMilliseconds{},
      mLastStepElapsedMilliseconds{},
      mActorRepository{},
      mSpatialQuery{},
      mSimulationLogics{},
      mPublishedEvents{} {
    InitializeDependencies();
    InitializeSimulationLogics();
    mFrameAccumulator.SynchronizeStatePair(*mActorRepository);
}

PhysicsWorld::~PhysicsWorld() {
}

PhysicsWorld::PhysicsWorld(const PhysicsWorld& Other)
    : mSettings{ Other.mSettings },
      mFrameAccumulator{ Other.mSettings.FixedTimeStep },
      mLastUpdateStepCount{ Other.mLastUpdateStepCount },
      mLastUpdateStepElapsedMilliseconds{ Other.mLastUpdateStepElapsedMilliseconds },
      mLastStepElapsedMilliseconds{ Other.mLastStepElapsedMilliseconds },
      mActorRepository{ Other.mActorRepository != nullptr ? Other.mActorRepository->Clone() : nullptr },
      mSpatialQuery{ Other.mSpatialQuery != nullptr ? Other.mSpatialQuery->Clone() : nullptr },
      mSimulationLogics{},
      mPublishedEvents{} {
    InitializeDependencies();
    InitializeSimulationLogics();
    mFrameAccumulator.Initialize(mSettings.FixedTimeStep);
    mFrameAccumulator.SynchronizeStatePair(*mActorRepository);
}

PhysicsWorld& PhysicsWorld::operator=(const PhysicsWorld& Other) {
    if (this == &Other) {
        return *this;
    }

    mSettings = Other.mSettings;
    mLastUpdateStepCount = Other.mLastUpdateStepCount;
    mLastUpdateStepElapsedMilliseconds = Other.mLastUpdateStepElapsedMilliseconds;
    mLastStepElapsedMilliseconds = Other.mLastStepElapsedMilliseconds;
    mActorRepository = Other.mActorRepository != nullptr ? Other.mActorRepository->Clone() : nullptr;
    mSpatialQuery = Other.mSpatialQuery != nullptr ? Other.mSpatialQuery->Clone() : nullptr;
    mPublishedEvents.clear();
    InitializeDependencies();
    InitializeSimulationLogics();
    mFrameAccumulator.Initialize(mSettings.FixedTimeStep);
    mFrameAccumulator.SynchronizeStatePair(*mActorRepository);

    return *this;
}

PhysicsWorld::PhysicsWorld(PhysicsWorld&& Other) noexcept
    : mSettings{ Other.mSettings },
      mFrameAccumulator{ std::move(Other.mFrameAccumulator) },
      mLastUpdateStepCount{ Other.mLastUpdateStepCount },
      mLastUpdateStepElapsedMilliseconds{ Other.mLastUpdateStepElapsedMilliseconds },
      mLastStepElapsedMilliseconds{ Other.mLastStepElapsedMilliseconds },
      mActorRepository{ std::move(Other.mActorRepository) },
      mSpatialQuery{ std::move(Other.mSpatialQuery) },
      mSimulationLogics{ std::move(Other.mSimulationLogics) },
      mPublishedEvents{ std::move(Other.mPublishedEvents) } {
    if (mActorRepository != nullptr) {
        mFrameAccumulator.SynchronizeStatePair(*mActorRepository);
    }

    Other.mSettings = WorldSettings{ 1.0F / 60.0F, DirectX::SimpleMath::Vector3{} };
    Other.mLastUpdateStepCount = 0U;
    Other.mLastUpdateStepElapsedMilliseconds = 0.0;
    Other.mLastStepElapsedMilliseconds = 0.0;
    Other.InitializeDependencies();
    Other.InitializeSimulationLogics();
    Other.mFrameAccumulator.Initialize(Other.mSettings.FixedTimeStep);
    Other.mFrameAccumulator.SynchronizeStatePair(*Other.mActorRepository);
    Other.ClearPublishedEvents();
}

PhysicsWorld& PhysicsWorld::operator=(PhysicsWorld&& Other) noexcept {
    if (this == &Other) {
        return *this;
    }

    mSettings = Other.mSettings;
    mFrameAccumulator = std::move(Other.mFrameAccumulator);
    mLastUpdateStepCount = Other.mLastUpdateStepCount;
    mLastUpdateStepElapsedMilliseconds = Other.mLastUpdateStepElapsedMilliseconds;
    mLastStepElapsedMilliseconds = Other.mLastStepElapsedMilliseconds;
    mActorRepository = std::move(Other.mActorRepository);
    mSpatialQuery = std::move(Other.mSpatialQuery);
    mSimulationLogics = std::move(Other.mSimulationLogics);
    mPublishedEvents = std::move(Other.mPublishedEvents);
    if (mActorRepository != nullptr) {
        mFrameAccumulator.SynchronizeStatePair(*mActorRepository);
    }

    Other.mSettings = WorldSettings{ 1.0F / 60.0F, DirectX::SimpleMath::Vector3{} };
    Other.mLastUpdateStepCount = 0U;
    Other.mLastUpdateStepElapsedMilliseconds = 0.0;
    Other.mLastStepElapsedMilliseconds = 0.0;
    Other.InitializeDependencies();
    Other.InitializeSimulationLogics();
    Other.mFrameAccumulator.Initialize(Other.mSettings.FixedTimeStep);
    Other.mFrameAccumulator.SynchronizeStatePair(*Other.mActorRepository);
    Other.ClearPublishedEvents();

    return *this;
}

PhysicsWorld::PhysicsWorld(const WorldSettings& Settings)
    : mSettings{ Settings },
      mFrameAccumulator{ Settings.FixedTimeStep },
      mLastUpdateStepCount{},
      mLastUpdateStepElapsedMilliseconds{},
      mLastStepElapsedMilliseconds{},
      mActorRepository{},
      mSpatialQuery{},
      mSimulationLogics{},
      mPublishedEvents{} {
    InitializeDependencies();
    InitializeSimulationLogics();
    mFrameAccumulator.SynchronizeStatePair(*mActorRepository);
}

void PhysicsWorld::Initialize(const WorldSettings& Settings) {
    mSettings = Settings;
    mFrameAccumulator.Initialize(mSettings.FixedTimeStep);
    mLastUpdateStepCount = 0U;
    mLastUpdateStepElapsedMilliseconds = 0.0;
    mLastStepElapsedMilliseconds = 0.0;
    if (mActorRepository != nullptr) {
        mActorRepository->ClearActors();
    }

    if (mActorRepository != nullptr) {
        mFrameAccumulator.SynchronizeStatePair(*mActorRepository);
    }

    ClearPublishedEvents();
}

PhysicsDynamicActor* PhysicsWorld::CreateDynamicActor(const PhysicsDynamicActor::ActorDesc& Desc) {
    PhysicsDynamicActor* CreatedActor{ mActorRepository->CreateDynamicActor(Desc) };
    mFrameAccumulator.SynchronizeStatePair(*mActorRepository);
    return CreatedActor;
}

PhysicsKinematicActor* PhysicsWorld::CreateKinematicActor(const PhysicsKinematicActor::ActorDesc& Desc) {
    PhysicsKinematicActor* CreatedActor{ mActorRepository->CreateKinematicActor(Desc) };
    mFrameAccumulator.SynchronizeStatePair(*mActorRepository);
    return CreatedActor;
}

PhysicsTerrainActor* PhysicsWorld::CreateTerrainActor(const PhysicsTerrainActor::ActorDesc& Desc) {
    PhysicsTerrainActor* CreatedActor{ mActorRepository->CreateTerrainActor(Desc) };
    mFrameAccumulator.SynchronizeStatePair(*mActorRepository);
    return CreatedActor;
}

void PhysicsWorld::AddActor(std::unique_ptr<PhysicsActorBase> Actor) {
    mActorRepository->AddActor(std::move(Actor));
    mFrameAccumulator.SynchronizeStatePair(*mActorRepository);
}

void PhysicsWorld::ClearActors() {
    mActorRepository->ClearActors();
    mFrameAccumulator.SynchronizeStatePair(*mActorRepository);
}

PhysicsActorBase* PhysicsWorld::GetActor(std::size_t Index) {
    return mActorRepository->GetActor(Index);
}

const PhysicsActorBase* PhysicsWorld::GetActor(std::size_t Index) const {
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
    return mFrameAccumulator.GetAccumulatedTime();
}

float PhysicsWorld::GetInterpolationAlpha() const {
    return mFrameAccumulator.GetInterpolationAlpha();
}

std::size_t PhysicsWorld::GetLastUpdateStepCount() const {
    return mLastUpdateStepCount;
}

double PhysicsWorld::GetLastUpdateStepElapsedMilliseconds() const {
    return mLastUpdateStepElapsedMilliseconds;
}

double PhysicsWorld::GetLastStepElapsedMilliseconds() const {
    return mLastStepElapsedMilliseconds;
}

bool PhysicsWorld::TryGetInterpolatedActorTransform(const PhysicsActorBase& Actor, DirectX::SimpleMath::Vector3& OutPosition, DirectX::SimpleMath::Vector3& OutRotation, DirectX::SimpleMath::Vector3& OutScale) const {
    bool HasInterpolatedState{ mFrameAccumulator.TryGetInterpolatedState(Actor, OutPosition, OutRotation, OutScale) };
    return HasInterpolatedState;
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
    mLastUpdateStepCount = 0U;
    mLastUpdateStepElapsedMilliseconds = 0.0;
    mLastStepElapsedMilliseconds = 0.0;
    mFrameAccumulator.AddDeltaTime(DeltaTime);

    while (mFrameAccumulator.TryConsumeFixedStep()) {
        mFrameAccumulator.CapturePreviousState(*mActorRepository);
        std::chrono::steady_clock::time_point StepStartTime{ std::chrono::steady_clock::now() };
        StepSimulation();
        std::chrono::steady_clock::time_point StepEndTime{ std::chrono::steady_clock::now() };
        mFrameAccumulator.CaptureCurrentState(*mActorRepository);
        std::chrono::duration<double, std::milli> StepElapsedTime{ StepEndTime - StepStartTime };
        mLastUpdateStepElapsedMilliseconds += StepElapsedTime.count();
        mLastStepElapsedMilliseconds = StepElapsedTime.count();
        ++mLastUpdateStepCount;
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

void PhysicsWorld::PublishEvent(PhysicsSimulationEventType EventType, const PhysicsActorBase* FirstActor, const PhysicsActorBase* SecondActor) {
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
    mSimulationLogics.push_back(std::make_unique<PhysicsDynamicCollisionLogic>());
    mSimulationLogics.push_back(std::make_unique<PhysicsDynamicIntegrationLogic>());
}
