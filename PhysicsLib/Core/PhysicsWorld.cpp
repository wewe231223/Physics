#include "PhysicsWorld.h"

#include "Renderer/MathConversion.h"

#include <utility>

PhysicsWorld::PhysicsWorld()
    : mSettings{ 1.0F / 60.0F, DirectX::SimpleMath::Vector3{ 0.0F, -9.8F, 0.0F } },
      mAccumulator{},
      mActors{} {
}

PhysicsWorld::~PhysicsWorld() {
}

PhysicsWorld::PhysicsWorld(const PhysicsWorld& Other)
    : mSettings{ Other.mSettings },
      mAccumulator{ Other.mAccumulator },
      mActors{ Other.mActors } {
}

PhysicsWorld& PhysicsWorld::operator=(const PhysicsWorld& Other) {
    if (this == &Other) {
        return *this;
    }

    mSettings = Other.mSettings;
    mAccumulator = Other.mAccumulator;
    mActors = Other.mActors;

    return *this;
}

PhysicsWorld::PhysicsWorld(PhysicsWorld&& Other) noexcept
    : mSettings{ Other.mSettings },
      mAccumulator{ Other.mAccumulator },
      mActors{ std::move(Other.mActors) } {
    Other.mSettings = WorldSettings{ 1.0F / 60.0F, DirectX::SimpleMath::Vector3{} };
    Other.mAccumulator = 0.0F;
}

PhysicsWorld& PhysicsWorld::operator=(PhysicsWorld&& Other) noexcept {
    if (this == &Other) {
        return *this;
    }

    mSettings = Other.mSettings;
    mAccumulator = Other.mAccumulator;
    mActors = std::move(Other.mActors);

    Other.mSettings = WorldSettings{ 1.0F / 60.0F, DirectX::SimpleMath::Vector3{} };
    Other.mAccumulator = 0.0F;

    return *this;
}

PhysicsWorld::PhysicsWorld(const WorldSettings& Settings)
    : mSettings{ Settings },
      mAccumulator{},
      mActors{} {
}

void PhysicsWorld::Initialize(const WorldSettings& Settings) {
    mSettings = Settings;
    mAccumulator = 0.0F;
}

void PhysicsWorld::AddActor(const PhysicsActor& Actor) {
    mActors.push_back(Actor);
}

void PhysicsWorld::AddActor(PhysicsActor&& Actor) {
    mActors.push_back(std::move(Actor));
}

void PhysicsWorld::ClearActors() {
    mActors.clear();
}

void PhysicsWorld::BuildActorsFromScene(const Scene& CurrentScene) {
    mActors.clear();

    std::size_t GameObjectCount{ CurrentScene.GetGameObjectCount() };
    for (std::size_t ObjectIndex{ 0U }; ObjectIndex < GameObjectCount; ++ObjectIndex) {
        const GameObject* CurrentObject{ CurrentScene.GetGameObject(ObjectIndex) };

        if (CurrentObject == nullptr) {
            continue;
        }

        PhysicsActor NewActor{ CurrentObject->GetName() };
        NewActor.SetIsActive(CurrentObject->GetIsActive());

        glm::vec3 ObjectPosition{ CurrentObject->GetTransform().GetPosition() };
        glm::vec3 ObjectRotation{ CurrentObject->GetTransform().GetRotation() };
        glm::vec3 ObjectScale{ CurrentObject->GetTransform().GetScale() };

        NewActor.SetPosition(RendererMathConversion::ToSimpleMathVector3(ObjectPosition));
        NewActor.SetRotation(RendererMathConversion::ToSimpleMathVector3(ObjectRotation));
        NewActor.SetScale(RendererMathConversion::ToSimpleMathVector3(ObjectScale));

        bool IsStaticActor{ CurrentObject->GetMesh() == nullptr || CurrentObject->GetName() == "Grid" };
        if (IsStaticActor) {
            NewActor.SetMass(0.0F);
            NewActor.SetFlags(PhysicsActor::PhysicsActorFlags::Static);
        }

        AddActor(std::move(NewActor));
    }
}

PhysicsActor* PhysicsWorld::GetActor(std::size_t Index) {
    if (Index >= mActors.size()) {
        return nullptr;
    }

    return &mActors[Index];
}

const PhysicsActor* PhysicsWorld::GetActor(std::size_t Index) const {
    if (Index >= mActors.size()) {
        return nullptr;
    }

    return &mActors[Index];
}

std::size_t PhysicsWorld::GetActorCount() const {
    std::size_t ActorCount{ mActors.size() };
    return ActorCount;
}

const PhysicsWorld::WorldSettings& PhysicsWorld::GetSettings() const {
    return mSettings;
}

float PhysicsWorld::GetAccumulator() const {
    return mAccumulator;
}

void PhysicsWorld::StepSimulation() {
    std::size_t ActorCount{ mActors.size() };
    for (std::size_t ActorIndex{ 0U }; ActorIndex < ActorCount; ++ActorIndex) {
        IntegrateActor(mActors[ActorIndex], mSettings.FixedTimeStep);
    }
}

void PhysicsWorld::Update(float DeltaTime) {
    mAccumulator += DeltaTime;

    while (mAccumulator >= mSettings.FixedTimeStep) {
        StepSimulation();
        mAccumulator -= mSettings.FixedTimeStep;
    }
}

void PhysicsWorld::IntegrateActor(PhysicsActor& Actor, float DeltaTime) const {
    if (!Actor.GetIsActive()) {
        return;
    }

    if (Actor.HasFlag(PhysicsActor::PhysicsActorFlags::Static)) {
        return;
    }

    if (Actor.GetMass() <= 0.0F) {
        return;
    }

    DirectX::SimpleMath::Vector3 NextPosition{ Actor.GetPosition() + (mSettings.Gravity * DeltaTime) };
    Actor.SetPosition(NextPosition);
}
