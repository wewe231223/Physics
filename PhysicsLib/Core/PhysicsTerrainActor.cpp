#include <utility>
#include <cmath>
#include <algorithm>

#include "PhysicsTerrainActor.h"

#undef min 
#undef max 

PhysicsTerrainActor::PhysicsTerrainActor()
    : PhysicsActor{},
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
    SetActorType(PhysicsActorType::Terrain);
    SetFlags(GetFlags() | PhysicsActorFlags::Static);
    SetMass(0.0F);
}

PhysicsTerrainActor::~PhysicsTerrainActor() {
}

PhysicsTerrainActor::PhysicsTerrainActor(const PhysicsTerrainActor& Other)
    : PhysicsActor{ Other },
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

    PhysicsActor::operator=(Other);
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
    : PhysicsActor{ std::move(Other) },
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
    Other.mPosition = DirectX::SimpleMath::Vector3{};
    Other.mRotation = DirectX::SimpleMath::Vector3{};
    Other.mScale = DirectX::SimpleMath::Vector3{};
    Other.mHalfExtentX = 0.0F;
    Other.mHalfExtentZ = 0.0F;
    Other.mHeightFieldWidth = 0U;
    Other.mHeightFieldHeight = 0U;
    Other.mHeightFieldCellSpacing = 1.0F;
    Other.mHeightFieldMaxHeight = 1.0F;
    Other.mHeightFieldCenterOrigin = true;
    Other.mHeightFieldValues.clear();
}

PhysicsTerrainActor& PhysicsTerrainActor::operator=(PhysicsTerrainActor&& Other) noexcept {
    if (this == &Other) {
        return *this;
    }

    PhysicsActor::operator=(std::move(Other));
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

    Other.mPosition = DirectX::SimpleMath::Vector3{};
    Other.mRotation = DirectX::SimpleMath::Vector3{};
    Other.mScale = DirectX::SimpleMath::Vector3{};
    Other.mHalfExtentX = 0.0F;
    Other.mHalfExtentZ = 0.0F;
    Other.mHeightFieldWidth = 0U;
    Other.mHeightFieldHeight = 0U;
    Other.mHeightFieldCellSpacing = 1.0F;
    Other.mHeightFieldMaxHeight = 1.0F;
    Other.mHeightFieldCenterOrigin = true;
    Other.mHeightFieldValues.clear();

    return *this;
}

PhysicsTerrainActor::PhysicsTerrainActor(std::string Name)
    : PhysicsActor{ std::move(Name) },
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
    SetActorType(PhysicsActorType::Terrain);
    SetFlags(GetFlags() | PhysicsActorFlags::Static);
    SetMass(0.0F);
}

PhysicsTerrainActor::PhysicsTerrainActor(const ActorDesc& Desc)
    : PhysicsActor{},
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
    SetActorType(PhysicsActorType::Terrain);
    SetFlags(GetFlags() | PhysicsActorFlags::Static);
    SetMass(0.0F);
    SetActorDesc(Desc);
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
    ActorDesc Desc{
        mPosition,
        mRotation,
        mScale,
        mHalfExtentX,
        mHalfExtentZ,
        mHeightFieldWidth,
        mHeightFieldHeight,
        mHeightFieldCellSpacing,
        mHeightFieldMaxHeight,
        mHeightFieldCenterOrigin,
        mHeightFieldValues
    };

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

std::size_t PhysicsTerrainActor::CalculateHeightFieldIndex(std::uint32_t X, std::uint32_t Z) const {
    std::size_t Index{ static_cast<std::size_t>(Z) * static_cast<std::size_t>(mHeightFieldWidth) + static_cast<std::size_t>(X) };
    return Index;
}
