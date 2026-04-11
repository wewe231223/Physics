#include "GameObject.h"

#include <utility>

GameObject::GameObject()
    : mName{ "GameObject" },
      mIsActive{ true },
      mTransform{},
      mMesh{},
      mWorldMatrix{ 1.0F },
      mPhysicsActor{} {
}

GameObject::~GameObject() {
}

GameObject::GameObject(const GameObject& Other)
    : mName{ Other.mName },
      mIsActive{ Other.mIsActive },
      mTransform{ Other.mTransform },
      mMesh{ Other.mMesh },
      mWorldMatrix{ Other.mWorldMatrix },
      mPhysicsActor{} {
}

GameObject& GameObject::operator=(const GameObject& Other) {
    if (this == &Other) {
        return *this;
    }

    mName = Other.mName;
    mIsActive = Other.mIsActive;
    mTransform = Other.mTransform;
    mMesh = Other.mMesh;
    mWorldMatrix = Other.mWorldMatrix;
    mPhysicsActor = nullptr;

    return *this;
}

GameObject::GameObject(GameObject&& Other) noexcept
    : mName{ std::move(Other.mName) },
      mIsActive{ Other.mIsActive },
      mTransform{ std::move(Other.mTransform) },
      mMesh{ std::move(Other.mMesh) },
      mWorldMatrix{ Other.mWorldMatrix },
      mPhysicsActor{ Other.mPhysicsActor } {
    Other.mName = "";
    Other.mIsActive = false;
    Other.mWorldMatrix = glm::mat4{ 1.0F };
    Other.mPhysicsActor = nullptr;
}

GameObject& GameObject::operator=(GameObject&& Other) noexcept {
    if (this == &Other) {
        return *this;
    }

    mName = std::move(Other.mName);
    mIsActive = Other.mIsActive;
    mTransform = std::move(Other.mTransform);
    mMesh = std::move(Other.mMesh);
    mWorldMatrix = Other.mWorldMatrix;
    mPhysicsActor = Other.mPhysicsActor;

    Other.mName = "";
    Other.mIsActive = false;
    Other.mWorldMatrix = glm::mat4{ 1.0F };
    Other.mPhysicsActor = nullptr;

    return *this;
}

GameObject::GameObject(std::string Name)
    : mName{ std::move(Name) },
      mIsActive{ true },
      mTransform{},
      mMesh{},
      mWorldMatrix{ 1.0F },
      mPhysicsActor{} {
}

void GameObject::SetName(std::string Name) {
    mName = std::move(Name);
}

const std::string& GameObject::GetName() const {
    return mName;
}

void GameObject::SetIsActive(bool IsActive) {
    mIsActive = IsActive;
}

bool GameObject::GetIsActive() const {
    return mIsActive;
}

Transform& GameObject::GetTransform() {
    return mTransform;
}

const Transform& GameObject::GetTransform() const {
    return mTransform;
}

void GameObject::SetMesh(const std::shared_ptr<Mesh>& MeshData) {
    mMesh = MeshData;
}

const std::shared_ptr<Mesh>& GameObject::GetMesh() const {
    return mMesh;
}

void GameObject::UpdateWorldMatrix() {
    mWorldMatrix = mTransform.GetWorldMatrix();
}

const glm::mat4& GameObject::GetWorldMatrix() const {
    return mWorldMatrix;
}

PhysicsActor::ActorDesc GameObject::GetPhysicsActorDesc() const {
    glm::vec3 Position{ mTransform.GetPosition() };
    glm::vec3 Rotation{ mTransform.GetRotation() };
    glm::vec3 Scale{ mTransform.GetScale() };

    bool IsStaticActor{ mMesh == nullptr || mName == "Grid" };
    PhysicsActor::PhysicsActorFlags ActorFlags{ PhysicsActor::PhysicsActorFlags::None };
    float ActorMass{ 1.0F };

    if (IsStaticActor) {
        ActorFlags = PhysicsActor::PhysicsActorFlags::Static;
        ActorMass = 0.0F;
    }

    PhysicsActor::BoundingBox ActorBoundingBox{
        DirectX::SimpleMath::Vector3{ -0.5F, -0.5F, -0.5F },
        DirectX::SimpleMath::Vector3{ 0.5F, 0.5F, 0.5F }
    };

    PhysicsActor::ActorDesc ActorDesc{
        mName,
        mIsActive,
        ActorMass,
        ActorFlags,
        ActorBoundingBox,
        DirectX::SimpleMath::Vector3{ Position.x, Position.y, Position.z },
        DirectX::SimpleMath::Vector3{ Rotation.x, Rotation.y, Rotation.z },
        DirectX::SimpleMath::Vector3{ Scale.x, Scale.y, Scale.z }
    };

    return ActorDesc;
}

void GameObject::SetPhysicsActor(PhysicsActor* PhysicsActorPointer) {
    mPhysicsActor = PhysicsActorPointer;
}

PhysicsActor* GameObject::GetPhysicsActor() {
    return mPhysicsActor;
}

const PhysicsActor* GameObject::GetPhysicsActor() const {
    return mPhysicsActor;
}

void GameObject::PullTransformFromPhysicsActor() {
    if (mPhysicsActor == nullptr) {
        return;
    }

    DirectX::SimpleMath::Vector3 Position{ mPhysicsActor->GetPosition() };
    DirectX::SimpleMath::Vector3 Rotation{ mPhysicsActor->GetRotation() };
    DirectX::SimpleMath::Vector3 Scale{ mPhysicsActor->GetScale() };

    mTransform.SetPosition(glm::vec3{ Position.x, Position.y, Position.z });
    mTransform.SetRotation(glm::vec3{ Rotation.x, Rotation.y, Rotation.z });
    mTransform.SetScale(glm::vec3{ Scale.x, Scale.y, Scale.z });
}
