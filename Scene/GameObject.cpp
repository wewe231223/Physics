#include "GameObject.h"

#include <algorithm>
#include <cstdint>
#include <utility>

#include <glm/ext/matrix_transform.hpp>

GameObject::GameObject()
    : mName{ "GameObject" },
      mIsActive{ true },
      mTransform{},
      mMesh{},
      mWorldMatrix{ 1.0F },
      mBoundingBoxMesh{},
      mBoundingBoxVisible{},
      mBoundingBoxWorldMatrix{ 1.0F },
      mActorId{ InvalidActorId },
      mPhysicsMass{ 1.0F },
      mHasInitialImpulse{},
      mInitialImpulse{} {
}

GameObject::~GameObject() {
}

GameObject::GameObject(const GameObject& Other)
    : mName{ Other.mName },
      mIsActive{ Other.mIsActive },
      mTransform{ Other.mTransform },
      mMesh{ Other.mMesh },
      mWorldMatrix{ Other.mWorldMatrix },
      mBoundingBoxMesh{ Other.mBoundingBoxMesh },
      mBoundingBoxVisible{ Other.mBoundingBoxVisible },
      mBoundingBoxWorldMatrix{ Other.mBoundingBoxWorldMatrix },
      mActorId{ Other.mActorId },
      mPhysicsMass{ Other.mPhysicsMass },
      mHasInitialImpulse{ Other.mHasInitialImpulse },
      mInitialImpulse{ Other.mInitialImpulse } {
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
    mBoundingBoxMesh = Other.mBoundingBoxMesh;
    mBoundingBoxVisible = Other.mBoundingBoxVisible;
    mBoundingBoxWorldMatrix = Other.mBoundingBoxWorldMatrix;
    mActorId = Other.mActorId;
    mPhysicsMass = Other.mPhysicsMass;
    mHasInitialImpulse = Other.mHasInitialImpulse;
    mInitialImpulse = Other.mInitialImpulse;

    return *this;
}

GameObject::GameObject(GameObject&& Other) noexcept
    : mName{ std::move(Other.mName) },
      mIsActive{ Other.mIsActive },
      mTransform{ std::move(Other.mTransform) },
      mMesh{ std::move(Other.mMesh) },
      mWorldMatrix{ Other.mWorldMatrix },
      mBoundingBoxMesh{ std::move(Other.mBoundingBoxMesh) },
      mBoundingBoxVisible{ Other.mBoundingBoxVisible },
      mBoundingBoxWorldMatrix{ Other.mBoundingBoxWorldMatrix },
      mActorId{ Other.mActorId },
      mPhysicsMass{ Other.mPhysicsMass },
      mHasInitialImpulse{ Other.mHasInitialImpulse },
      mInitialImpulse{ Other.mInitialImpulse } {
    Other.mName = "";
    Other.mIsActive = false;
    Other.mWorldMatrix = glm::mat4{ 1.0F };
    Other.mBoundingBoxVisible = false;
    Other.mBoundingBoxWorldMatrix = glm::mat4{ 1.0F };
    Other.mActorId = InvalidActorId;
    Other.mPhysicsMass = 1.0F;
    Other.mHasInitialImpulse = false;
    Other.mInitialImpulse = DirectX::SimpleMath::Vector3{};
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
    mBoundingBoxMesh = std::move(Other.mBoundingBoxMesh);
    mBoundingBoxVisible = Other.mBoundingBoxVisible;
    mBoundingBoxWorldMatrix = Other.mBoundingBoxWorldMatrix;
    mActorId = Other.mActorId;
    mPhysicsMass = Other.mPhysicsMass;
    mHasInitialImpulse = Other.mHasInitialImpulse;
    mInitialImpulse = Other.mInitialImpulse;

    Other.mName = "";
    Other.mIsActive = false;
    Other.mWorldMatrix = glm::mat4{ 1.0F };
    Other.mBoundingBoxVisible = false;
    Other.mBoundingBoxWorldMatrix = glm::mat4{ 1.0F };
    Other.mActorId = InvalidActorId;
    Other.mPhysicsMass = 1.0F;
    Other.mHasInitialImpulse = false;
    Other.mInitialImpulse = DirectX::SimpleMath::Vector3{};

    return *this;
}

GameObject::GameObject(std::string Name)
    : mName{ std::move(Name) },
      mIsActive{ true },
      mTransform{},
      mMesh{},
      mWorldMatrix{ 1.0F },
      mBoundingBoxMesh{},
      mBoundingBoxVisible{},
      mBoundingBoxWorldMatrix{ 1.0F },
      mActorId{ InvalidActorId },
      mPhysicsMass{ 1.0F },
      mHasInitialImpulse{},
      mInitialImpulse{} {
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

void GameObject::SetBoundingBoxMesh(const std::shared_ptr<Mesh>& MeshData) {
    mBoundingBoxMesh = MeshData;
}

const std::shared_ptr<Mesh>& GameObject::GetBoundingBoxMesh() const {
    return mBoundingBoxMesh;
}

void GameObject::SetBoundingBoxVisible(bool IsVisible) {
    mBoundingBoxVisible = IsVisible;
}

bool GameObject::GetBoundingBoxVisible() const {
    return mBoundingBoxVisible;
}

void GameObject::UpdateWorldMatrix() {
    mWorldMatrix = mTransform.GetWorldMatrix();
}

const glm::mat4& GameObject::GetWorldMatrix() const {
    return mWorldMatrix;
}

const glm::mat4& GameObject::GetBoundingBoxWorldMatrix() const {
    return mBoundingBoxWorldMatrix;
}

bool GameObject::IsTerrainObject() const {
    bool HasTerrainSampleDesc{ mMesh != nullptr && mMesh->HasTerrainSampleDesc() };
    bool IsTerrain{ mMesh == nullptr || mName == "Grid" || HasTerrainSampleDesc };
    return IsTerrain;
}

void GameObject::SetActorId(ActorId ActorIdValue) {
    mActorId = ActorIdValue;
}

ActorId GameObject::GetActorId() const {
    return mActorId;
}

bool GameObject::HasActorId() const {
    return mActorId != InvalidActorId;
}

void GameObject::SetPhysicsMass(float PhysicsMass) {
    mPhysicsMass = std::max(PhysicsMass, 0.0001F);
}

float GameObject::GetPhysicsMass() const {
    return mPhysicsMass;
}

void GameObject::SetInitialImpulse(const DirectX::SimpleMath::Vector3& InitialImpulse) {
    mHasInitialImpulse = true;
    mInitialImpulse = InitialImpulse;
}

void GameObject::ClearInitialImpulse() {
    mHasInitialImpulse = false;
    mInitialImpulse = DirectX::SimpleMath::Vector3{};
}

bool GameObject::HasInitialImpulse() const {
    return mHasInitialImpulse;
}

const DirectX::SimpleMath::Vector3& GameObject::GetInitialImpulse() const {
    return mInitialImpulse;
}

PhysicsDynamicActor::ActorDesc GameObject::GetPhysicsDynamicActorDesc() const {
    glm::vec3 Position{ mTransform.GetPosition() };
    glm::vec3 Rotation{ mTransform.GetRotation() };
    glm::vec3 Scale{ mTransform.GetScale() };

    DirectX::BoundingOrientedBox ActorBoundingBox{};
    if (mMesh == nullptr) {
        ActorBoundingBox.Center = DirectX::XMFLOAT3{ 0.0F, 0.0F, 0.0F };
        ActorBoundingBox.Extents = DirectX::XMFLOAT3{ 0.5F, 0.5F, 0.5F };
        ActorBoundingBox.Orientation = DirectX::XMFLOAT4{ 0.0F, 0.0F, 0.0F, 1.0F };
    } else {
        ActorBoundingBox = mMesh->GetBoundingBox();
    }

    PhysicsDynamicActor::ActorDesc ActorDesc{ mName, mIsActive, mPhysicsMass, PhysicsActorBase::PhysicsActorFlags::None, PhysicsActorBase::PhysicsActorType::Dynamic, ActorBoundingBox, DirectX::SimpleMath::Vector3{ Position.x, Position.y, Position.z }, DirectX::SimpleMath::Vector3{ Rotation.x, Rotation.y, Rotation.z }, DirectX::SimpleMath::Vector3{ Scale.x, Scale.y, Scale.z }, DirectX::SimpleMath::Vector3{}, DirectX::SimpleMath::Vector3{}, 0.6F, 0.1F, 0.03F, 0.03F, false, 0.05F, 0.1F };
    return ActorDesc;
}

PhysicsTerrainActor::ActorDesc GameObject::GetPhysicsTerrainActorDesc() const {
    glm::vec3 Position{ mTransform.GetPosition() };
    glm::vec3 Rotation{ mTransform.GetRotation() };
    glm::vec3 Scale{ mTransform.GetScale() };

    float HalfExtentX{ 0.5F };
    float HalfExtentZ{ 0.5F };
    std::uint32_t HeightFieldWidth{};
    std::uint32_t HeightFieldHeight{};
    float HeightFieldCellSpacing{ 1.0F };
    float HeightFieldMaxHeight{ 1.0F };
    bool HeightFieldCenterOrigin{ true };
    std::vector<float> HeightFieldValues{};

    if (mMesh != nullptr) {
        DirectX::BoundingOrientedBox LocalBoundingBox{ mMesh->GetBoundingBox() };
        HalfExtentX = LocalBoundingBox.Extents.x;
        HalfExtentZ = LocalBoundingBox.Extents.z;
        if (mMesh->HasTerrainSampleDesc()) {
            const Mesh::TerrainSampleDesc& TerrainSampleDescValue{ mMesh->GetTerrainSampleDesc() };
            HeightFieldWidth = TerrainSampleDescValue.Width;
            HeightFieldHeight = TerrainSampleDescValue.Height;
            HeightFieldCellSpacing = TerrainSampleDescValue.CellSpacing;
            HeightFieldMaxHeight = TerrainSampleDescValue.MaxHeight;
            HeightFieldCenterOrigin = TerrainSampleDescValue.CenterOrigin;
            HeightFieldValues = TerrainSampleDescValue.HeightValues;
        }
    }

    PhysicsTerrainActor::ActorDesc ActorDesc{ DirectX::SimpleMath::Vector3{ Position.x, Position.y, Position.z }, DirectX::SimpleMath::Vector3{ Rotation.x, Rotation.y, Rotation.z }, DirectX::SimpleMath::Vector3{ Scale.x, Scale.y, Scale.z }, HalfExtentX, HalfExtentZ, HeightFieldWidth, HeightFieldHeight, HeightFieldCellSpacing, HeightFieldMaxHeight, HeightFieldCenterOrigin, HeightFieldValues };
    return ActorDesc;
}

void GameObject::ApplyPhysicsState(const DirectX::SimpleMath::Vector3& Position, const DirectX::SimpleMath::Vector3& Rotation, const DirectX::SimpleMath::Vector3& Scale) {
    mTransform.SetPosition(glm::vec3{ Position.x, Position.y, Position.z });
    mTransform.SetRotation(glm::vec3{ Rotation.x, Rotation.y, Rotation.z });
    mTransform.SetScale(glm::vec3{ Scale.x, Scale.y, Scale.z });
}

void GameObject::SetBoundingBoxFromPhysicsState(const DirectX::BoundingOrientedBox& WorldBoundingBox, const DirectX::SimpleMath::Vector3& Rotation) {
    DirectX::XMFLOAT3 Center{ WorldBoundingBox.Center };
    DirectX::XMFLOAT3 Extents{ WorldBoundingBox.Extents };
    glm::vec3 BoundingBoxPosition{ Center.x, Center.y, Center.z };
    glm::vec3 BoundingBoxRotation{ Rotation.x, Rotation.y, Rotation.z };
    glm::vec3 BoundingBoxScale{ Extents.x * 2.0F, Extents.y * 2.0F, Extents.z * 2.0F };
    glm::mat4 TranslationMatrix{ glm::translate(glm::mat4{ 1.0F }, BoundingBoxPosition) };
    glm::mat4 RotationXMatrix{ glm::rotate(glm::mat4{ 1.0F }, BoundingBoxRotation.x, glm::vec3{ 1.0F, 0.0F, 0.0F }) };
    glm::mat4 RotationYMatrix{ glm::rotate(glm::mat4{ 1.0F }, BoundingBoxRotation.y, glm::vec3{ 0.0F, 1.0F, 0.0F }) };
    glm::mat4 RotationZMatrix{ glm::rotate(glm::mat4{ 1.0F }, BoundingBoxRotation.z, glm::vec3{ 0.0F, 0.0F, 1.0F }) };
    glm::mat4 ScaleMatrix{ glm::scale(glm::mat4{ 1.0F }, BoundingBoxScale) };
    mBoundingBoxWorldMatrix = TranslationMatrix * RotationZMatrix * RotationYMatrix * RotationXMatrix * ScaleMatrix;
}

void GameObject::ClearBoundingBoxWorldMatrix() {
    mBoundingBoxWorldMatrix = glm::mat4{ 1.0F };
}

