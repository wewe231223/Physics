#include <algorithm>
#include <cmath>
#include <limits>
#include <utility>

#include "PhysicsTerrainActor.h"

#undef min
#undef max

PhysicsTerrainActor::PhysicsTerrainActor()
    : PhysicsStaticActor{},
      mPosition{},
      mRotation{},
      mScale{ 1.0F, 1.0F, 1.0F },
      mHalfExtentX{ 0.5F },
      mHalfExtentZ{ 0.5F },
      mHeightFieldWidth{},
      mHeightFieldHeight{},
      mHeightFieldCellSpacing{ 1.0F },
      mHeightFieldMaxHeight{ 1.0F },
      mHeightFieldCenterOrigin{ true },
      mHeightFieldValues{} {
}

PhysicsTerrainActor::~PhysicsTerrainActor() {
}

PhysicsTerrainActor::PhysicsTerrainActor(const PhysicsTerrainActor& Other)
    : PhysicsStaticActor{ Other },
      mPosition{ Other.mPosition },
      mRotation{ Other.mRotation },
      mScale{ Other.mScale },
      mHalfExtentX{ Other.mHalfExtentX },
      mHalfExtentZ{ Other.mHalfExtentZ },
      mHeightFieldWidth{ Other.mHeightFieldWidth },
      mHeightFieldHeight{ Other.mHeightFieldHeight },
      mHeightFieldCellSpacing{ Other.mHeightFieldCellSpacing },
      mHeightFieldMaxHeight{ Other.mHeightFieldMaxHeight },
      mHeightFieldCenterOrigin{ Other.mHeightFieldCenterOrigin },
      mHeightFieldValues{ Other.mHeightFieldValues } {
}

PhysicsTerrainActor& PhysicsTerrainActor::operator=(const PhysicsTerrainActor& Other) {
    if (this == &Other) {
        return *this;
    }

    PhysicsStaticActor::operator=(Other);
    mPosition = Other.mPosition;
    mRotation = Other.mRotation;
    mScale = Other.mScale;
    mHalfExtentX = Other.mHalfExtentX;
    mHalfExtentZ = Other.mHalfExtentZ;
    mHeightFieldWidth = Other.mHeightFieldWidth;
    mHeightFieldHeight = Other.mHeightFieldHeight;
    mHeightFieldCellSpacing = Other.mHeightFieldCellSpacing;
    mHeightFieldMaxHeight = Other.mHeightFieldMaxHeight;
    mHeightFieldCenterOrigin = Other.mHeightFieldCenterOrigin;
    mHeightFieldValues = Other.mHeightFieldValues;

    return *this;
}

PhysicsTerrainActor::PhysicsTerrainActor(PhysicsTerrainActor&& Other) noexcept
    : PhysicsStaticActor{ std::move(Other) },
      mPosition{ Other.mPosition },
      mRotation{ Other.mRotation },
      mScale{ Other.mScale },
      mHalfExtentX{ Other.mHalfExtentX },
      mHalfExtentZ{ Other.mHalfExtentZ },
      mHeightFieldWidth{ Other.mHeightFieldWidth },
      mHeightFieldHeight{ Other.mHeightFieldHeight },
      mHeightFieldCellSpacing{ Other.mHeightFieldCellSpacing },
      mHeightFieldMaxHeight{ Other.mHeightFieldMaxHeight },
      mHeightFieldCenterOrigin{ Other.mHeightFieldCenterOrigin },
      mHeightFieldValues{ std::move(Other.mHeightFieldValues) } {
}

PhysicsTerrainActor& PhysicsTerrainActor::operator=(PhysicsTerrainActor&& Other) noexcept {
    if (this == &Other) {
        return *this;
    }

    PhysicsStaticActor::operator=(std::move(Other));
    mPosition = Other.mPosition;
    mRotation = Other.mRotation;
    mScale = Other.mScale;
    mHalfExtentX = Other.mHalfExtentX;
    mHalfExtentZ = Other.mHalfExtentZ;
    mHeightFieldWidth = Other.mHeightFieldWidth;
    mHeightFieldHeight = Other.mHeightFieldHeight;
    mHeightFieldCellSpacing = Other.mHeightFieldCellSpacing;
    mHeightFieldMaxHeight = Other.mHeightFieldMaxHeight;
    mHeightFieldCenterOrigin = Other.mHeightFieldCenterOrigin;
    mHeightFieldValues = std::move(Other.mHeightFieldValues);

    return *this;
}

PhysicsTerrainActor::PhysicsTerrainActor(const ActorDesc& Desc)
    : PhysicsStaticActor{},
      mPosition{ Desc.Position },
      mRotation{ Desc.Rotation },
      mScale{ Desc.Scale },
      mHalfExtentX{ Desc.HalfExtentX },
      mHalfExtentZ{ Desc.HalfExtentZ },
      mHeightFieldWidth{ Desc.HeightFieldWidth },
      mHeightFieldHeight{ Desc.HeightFieldHeight },
      mHeightFieldCellSpacing{ Desc.HeightFieldCellSpacing },
      mHeightFieldMaxHeight{ Desc.HeightFieldMaxHeight },
      mHeightFieldCenterOrigin{ Desc.HeightFieldCenterOrigin },
      mHeightFieldValues{ Desc.HeightFieldValues } {
}

void PhysicsTerrainActor::SetActorDesc(const ActorDesc& Desc) {
    mPosition = Desc.Position;
    mRotation = Desc.Rotation;
    mScale = Desc.Scale;
    mHalfExtentX = Desc.HalfExtentX;
    mHalfExtentZ = Desc.HalfExtentZ;
    mHeightFieldWidth = Desc.HeightFieldWidth;
    mHeightFieldHeight = Desc.HeightFieldHeight;
    mHeightFieldCellSpacing = Desc.HeightFieldCellSpacing;
    mHeightFieldMaxHeight = Desc.HeightFieldMaxHeight;
    mHeightFieldCenterOrigin = Desc.HeightFieldCenterOrigin;
    mHeightFieldValues = Desc.HeightFieldValues;
}

PhysicsTerrainActor::ActorDesc PhysicsTerrainActor::GetActorDesc() const {
    ActorDesc Desc{ mPosition, mRotation, mScale, mHalfExtentX, mHalfExtentZ, mHeightFieldWidth, mHeightFieldHeight, mHeightFieldCellSpacing, mHeightFieldMaxHeight, mHeightFieldCenterOrigin, mHeightFieldValues };
    return Desc;
}

bool PhysicsTerrainActor::TryGetSurfaceHeightAtWorldPosition(float WorldX, float WorldZ, float& OutWorldHeight) const {
    DirectX::SimpleMath::Matrix ScalingMatrix{ DirectX::SimpleMath::Matrix::CreateScale(mScale) };
    DirectX::SimpleMath::Matrix RotationMatrix{ DirectX::SimpleMath::Matrix::CreateFromYawPitchRoll(mRotation.y, mRotation.x, mRotation.z) };
    DirectX::SimpleMath::Matrix TranslationMatrix{ DirectX::SimpleMath::Matrix::CreateTranslation(mPosition) };
    DirectX::SimpleMath::Matrix WorldMatrix{ ScalingMatrix * RotationMatrix * TranslationMatrix };
    DirectX::SimpleMath::Matrix InverseWorldMatrix{ WorldMatrix.Invert() };
    DirectX::SimpleMath::Vector3 LocalPoint{ DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3{ WorldX, 0.0F, WorldZ }, InverseWorldMatrix) };

    float LocalHeight{};
    bool HasLocalHeight{ TryGetSurfaceHeightAtLocalPosition(LocalPoint.x, LocalPoint.z, LocalHeight) };
    if (!HasLocalHeight) {
        return false;
    }

    DirectX::SimpleMath::Vector3 WorldPoint{ DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3{ LocalPoint.x, LocalHeight, LocalPoint.z }, WorldMatrix) };
    OutWorldHeight = WorldPoint.y;
    return true;
}

bool PhysicsTerrainActor::ResolveDynamicCollision(const DirectX::BoundingOrientedBox& PredictedWorldBoundingBox, DirectX::SimpleMath::Vector3& CorrectedPosition, DirectX::SimpleMath::Vector3& CorrectedVelocity) const {
    float TerrainHalfExtentX{ mHalfExtentX * std::abs(mScale.x) };
    float TerrainHalfExtentZ{ mHalfExtentZ * std::abs(mScale.z) };

    if (mHeightFieldWidth > 1U && mHeightFieldHeight > 1U && mHeightFieldCellSpacing > 0.0F) {
        float HeightFieldHalfExtentX{ (static_cast<float>(mHeightFieldWidth - 1U) * mHeightFieldCellSpacing * 0.5F) * std::abs(mScale.x) };
        float HeightFieldHalfExtentZ{ (static_cast<float>(mHeightFieldHeight - 1U) * mHeightFieldCellSpacing * 0.5F) * std::abs(mScale.z) };
        TerrainHalfExtentX = std::max(TerrainHalfExtentX, HeightFieldHalfExtentX);
        TerrainHalfExtentZ = std::max(TerrainHalfExtentZ, HeightFieldHalfExtentZ);
    }

    float TerrainHalfExtentY{ mHeightFieldMaxHeight * std::abs(mScale.y) + PredictedWorldBoundingBox.Extents.y };
    DirectX::BoundingOrientedBox TerrainBoundingBox{};
    TerrainBoundingBox.Center = DirectX::XMFLOAT3{ mPosition.x, mPosition.y + (TerrainHalfExtentY * 0.5F), mPosition.z };
    TerrainBoundingBox.Extents = DirectX::XMFLOAT3{ TerrainHalfExtentX, TerrainHalfExtentY, TerrainHalfExtentZ };
    TerrainBoundingBox.Orientation = DirectX::XMFLOAT4{ 0.0F, 0.0F, 0.0F, 1.0F };
    DirectX::SimpleMath::Quaternion TerrainRotationQuaternion{ DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(mRotation.y, mRotation.x, mRotation.z) };
    TerrainBoundingBox.Orientation = DirectX::XMFLOAT4{ TerrainRotationQuaternion.x, TerrainRotationQuaternion.y, TerrainRotationQuaternion.z, TerrainRotationQuaternion.w };

    if (!TerrainBoundingBox.Intersects(PredictedWorldBoundingBox)) {
        return false;
    }

    DirectX::SimpleMath::Matrix TerrainScalingMatrix{ DirectX::SimpleMath::Matrix::CreateScale(mScale) };
    DirectX::SimpleMath::Matrix TerrainRotationMatrix{ DirectX::SimpleMath::Matrix::CreateFromYawPitchRoll(mRotation.y, mRotation.x, mRotation.z) };
    DirectX::SimpleMath::Matrix TerrainTranslationMatrix{ DirectX::SimpleMath::Matrix::CreateTranslation(mPosition) };
    DirectX::SimpleMath::Matrix TerrainWorldMatrix{ TerrainScalingMatrix * TerrainRotationMatrix * TerrainTranslationMatrix };
    DirectX::SimpleMath::Matrix InverseTerrainWorldMatrix{ TerrainWorldMatrix.Invert() };
    DirectX::SimpleMath::Matrix InverseTransposeTerrainWorldMatrix{ InverseTerrainWorldMatrix.Transpose() };
    DirectX::XMFLOAT3 DynamicCorners[8]{};
    PredictedWorldBoundingBox.GetCorners(DynamicCorners);
    float MaximumPenetrationDepth{};
    DirectX::SimpleMath::Vector3 ContactNormal{};
    bool HasContact{};

    for (std::size_t CornerIndex{ 0U }; CornerIndex < 8U; ++CornerIndex) {
        DirectX::SimpleMath::Vector3 CornerWorldPosition{ DynamicCorners[CornerIndex].x, DynamicCorners[CornerIndex].y, DynamicCorners[CornerIndex].z };
        DirectX::SimpleMath::Vector3 CornerLocalPosition{ DirectX::SimpleMath::Vector3::Transform(CornerWorldPosition, InverseTerrainWorldMatrix) };
        float SurfaceLocalHeight{};
        if (!TryGetSurfaceHeightAtLocalPosition(CornerLocalPosition.x, CornerLocalPosition.z, SurfaceLocalHeight)) {
            continue;
        }

        DirectX::SimpleMath::Vector3 SurfaceLocalNormal{};
        if (!TryGetSurfaceNormalAtLocalPosition(CornerLocalPosition.x, CornerLocalPosition.z, SurfaceLocalNormal)) {
            continue;
        }

        DirectX::SimpleMath::Vector3 SurfaceWorldPosition{ DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3{ CornerLocalPosition.x, SurfaceLocalHeight, CornerLocalPosition.z }, TerrainWorldMatrix) };
        DirectX::SimpleMath::Vector3 SurfaceWorldNormal{ DirectX::SimpleMath::Vector3::TransformNormal(SurfaceLocalNormal, InverseTransposeTerrainWorldMatrix) };
        float SurfaceWorldNormalLengthSquared{ SurfaceWorldNormal.LengthSquared() };
        if (SurfaceWorldNormalLengthSquared <= std::numeric_limits<float>::epsilon()) {
            continue;
        }

        SurfaceWorldNormal /= std::sqrt(SurfaceWorldNormalLengthSquared);
        DirectX::SimpleMath::Vector3 PenetrationVector{ SurfaceWorldPosition - CornerWorldPosition };
        float PenetrationDepth{ PenetrationVector.Dot(SurfaceWorldNormal) };
        if (PenetrationDepth <= 0.0F) {
            continue;
        }

        if (PenetrationDepth > MaximumPenetrationDepth) {
            MaximumPenetrationDepth = PenetrationDepth;
            ContactNormal = SurfaceWorldNormal;
            HasContact = true;
        }
    }

    if (!HasContact) {
        return false;
    }

    CorrectedPosition += ContactNormal * MaximumPenetrationDepth;
    float VelocityProjection{ CorrectedVelocity.Dot(ContactNormal) };
    if (VelocityProjection < 0.0F) {
        CorrectedVelocity -= ContactNormal * VelocityProjection;
    }

    return true;
}

bool PhysicsTerrainActor::TryGetSurfaceHeightAtLocalPosition(float LocalX, float LocalZ, float& OutLocalHeight) const {
    if (mHeightFieldWidth < 2U || mHeightFieldHeight < 2U || mHeightFieldValues.empty()) {
        return false;
    }

    if (mHeightFieldCellSpacing <= 0.0F || mHeightFieldMaxHeight <= 0.0F) {
        return false;
    }

    float OffsetX{};
    float OffsetZ{};
    if (mHeightFieldCenterOrigin) {
        OffsetX = (static_cast<float>(mHeightFieldWidth) - 1.0F) * mHeightFieldCellSpacing * 0.5F;
        OffsetZ = (static_cast<float>(mHeightFieldHeight) - 1.0F) * mHeightFieldCellSpacing * 0.5F;
    }

    float GridX{ (LocalX + OffsetX) / mHeightFieldCellSpacing };
    float GridZ{ (LocalZ + OffsetZ) / mHeightFieldCellSpacing };

    if (GridX < 0.0F || GridZ < 0.0F) {
        return false;
    }

    float MaxGridX{ static_cast<float>(mHeightFieldWidth - 1U) };
    float MaxGridZ{ static_cast<float>(mHeightFieldHeight - 1U) };
    if (GridX > MaxGridX || GridZ > MaxGridZ) {
        return false;
    }

    std::uint32_t X0{ static_cast<std::uint32_t>(std::floor(GridX)) };
    std::uint32_t Z0{ static_cast<std::uint32_t>(std::floor(GridZ)) };
    std::uint32_t X1{ std::min(X0 + 1U, mHeightFieldWidth - 1U) };
    std::uint32_t Z1{ std::min(Z0 + 1U, mHeightFieldHeight - 1U) };
    float FractionX{ GridX - static_cast<float>(X0) };
    float FractionZ{ GridZ - static_cast<float>(Z0) };

    float Height00{ mHeightFieldValues[CalculateHeightFieldIndex(X0, Z0)] * mHeightFieldMaxHeight };
    float Height10{ mHeightFieldValues[CalculateHeightFieldIndex(X1, Z0)] * mHeightFieldMaxHeight };
    float Height01{ mHeightFieldValues[CalculateHeightFieldIndex(X0, Z1)] * mHeightFieldMaxHeight };
    float Height11{ mHeightFieldValues[CalculateHeightFieldIndex(X1, Z1)] * mHeightFieldMaxHeight };
    float InterpolatedHeight0{ Height00 + ((Height10 - Height00) * FractionX) };
    float InterpolatedHeight1{ Height01 + ((Height11 - Height01) * FractionX) };
    OutLocalHeight = InterpolatedHeight0 + ((InterpolatedHeight1 - InterpolatedHeight0) * FractionZ);

    return true;
}

bool PhysicsTerrainActor::TryGetSurfaceNormalAtLocalPosition(float LocalX, float LocalZ, DirectX::SimpleMath::Vector3& OutLocalNormal) const {
    if (mHeightFieldWidth < 2U || mHeightFieldHeight < 2U || mHeightFieldValues.empty()) {
        return false;
    }

    float SampleStep{ std::max(mHeightFieldCellSpacing * 0.5F, 0.001F) };
    float LeftHeight{};
    float RightHeight{};
    float DownHeight{};
    float UpHeight{};
    bool HasLeftHeight{ TryGetSurfaceHeightAtLocalPosition(LocalX - SampleStep, LocalZ, LeftHeight) };
    bool HasRightHeight{ TryGetSurfaceHeightAtLocalPosition(LocalX + SampleStep, LocalZ, RightHeight) };
    bool HasDownHeight{ TryGetSurfaceHeightAtLocalPosition(LocalX, LocalZ - SampleStep, DownHeight) };
    bool HasUpHeight{ TryGetSurfaceHeightAtLocalPosition(LocalX, LocalZ + SampleStep, UpHeight) };
    if (!HasLeftHeight || !HasRightHeight || !HasDownHeight || !HasUpHeight) {
        return false;
    }

    float HeightDeltaX{ RightHeight - LeftHeight };
    float HeightDeltaZ{ UpHeight - DownHeight };
    DirectX::SimpleMath::Vector3 TangentX{ 2.0F * SampleStep, HeightDeltaX, 0.0F };
    DirectX::SimpleMath::Vector3 TangentZ{ 0.0F, HeightDeltaZ, 2.0F * SampleStep };
    DirectX::SimpleMath::Vector3 LocalNormal{ TangentZ.Cross(TangentX) };
    float NormalLengthSquared{ LocalNormal.LengthSquared() };
    if (NormalLengthSquared <= std::numeric_limits<float>::epsilon()) {
        return false;
    }

    LocalNormal /= std::sqrt(NormalLengthSquared);
    if (LocalNormal.y < 0.0F) {
        LocalNormal = -LocalNormal;
    }

    OutLocalNormal = LocalNormal;
    return true;
}

std::size_t PhysicsTerrainActor::CalculateHeightFieldIndex(std::uint32_t X, std::uint32_t Z) const {
    std::size_t Index{ static_cast<std::size_t>(Z) * static_cast<std::size_t>(mHeightFieldWidth) + static_cast<std::size_t>(X) };
    return Index;
}
