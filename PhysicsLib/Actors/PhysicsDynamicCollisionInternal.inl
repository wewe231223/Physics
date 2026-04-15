namespace {
    enum class DynamicSatAxisType : std::uint32_t {
        FaceA = 0U,
        FaceB = 1U,
        Edge = 2U
    };

    struct DynamicObb {
        DirectX::SimpleMath::Vector3 mCenter;
        DirectX::SimpleMath::Vector3 mExtents;
        DirectX::XMFLOAT4 mOrientation;
        std::array<DirectX::SimpleMath::Vector3, 3U> mAxes;
    };

    struct DynamicSatResult {
        bool mIntersect;
        DirectX::SimpleMath::Vector3 mNormal;
        float mPenetration;
        DynamicSatAxisType mAxisType;
        std::uint32_t mAxisIndexA;
        std::uint32_t mAxisIndexB;
    };

    struct DynamicContactPoint {
        DirectX::SimpleMath::Vector3 mWorldPoint;
        DirectX::SimpleMath::Vector3 mLocalPointA;
        DirectX::SimpleMath::Vector3 mLocalPointB;
        DirectX::SimpleMath::Vector3 mAccumulatedTangentImpulse;
        float mPenetration;
        float mAccumulatedNormalImpulse;
        std::uint64_t mFeatureKey;
    };

    struct DynamicContactManifold {
        DirectX::SimpleMath::Vector3 mNormal;
        float mPenetration;
        DynamicSatAxisType mAxisType;
        std::uint32_t mAxisIndexA;
        std::uint32_t mAxisIndexB;
        std::size_t mContactCount;
        std::array<DynamicContactPoint, 4U> mContacts;
    };

    struct DynamicPairCacheKey {
        std::uintptr_t mFirstPointer;
        std::uintptr_t mSecondPointer;

        bool operator==(const DynamicPairCacheKey& Other) const {
            return mFirstPointer == Other.mFirstPointer && mSecondPointer == Other.mSecondPointer;
        }
    };

    struct DynamicPairCacheKeyHasher {
        std::size_t operator()(const DynamicPairCacheKey& Key) const {
            std::uint64_t FirstHash{ static_cast<std::uint64_t>(Key.mFirstPointer) * 11400714819323198485ULL };
            std::uint64_t SecondHash{ static_cast<std::uint64_t>(Key.mSecondPointer) * 14029467366897019727ULL };
            std::uint64_t CombinedHash{ FirstHash ^ (SecondHash + 0x9E3779B97F4A7C15ULL + (FirstHash << 6U) + (FirstHash >> 2U)) };
            return static_cast<std::size_t>(CombinedHash);
        }
    };

    struct DynamicPersistentContactPoint {
        DirectX::SimpleMath::Vector3 mLocalPointA;
        DirectX::SimpleMath::Vector3 mLocalPointB;
        DirectX::SimpleMath::Vector3 mAccumulatedTangentImpulse;
        float mAccumulatedNormalImpulse;
        std::uint64_t mFeatureKey;
    };

    struct DynamicPersistentManifoldCache {
        DirectX::SimpleMath::Vector3 mNormal;
        DirectX::SimpleMath::Vector3 mCenterA;
        DirectX::SimpleMath::Vector3 mCenterB;
        DirectX::XMFLOAT4 mOrientationA;
        DirectX::XMFLOAT4 mOrientationB;
        float mPenetration;
        DynamicSatAxisType mAxisType;
        std::uint32_t mAxisIndexA;
        std::uint32_t mAxisIndexB;
        std::uint64_t mLastFrameIndex;
        std::size_t mContactCount;
        std::array<DynamicPersistentContactPoint, 4U> mContacts;
    };

    constexpr float DynamicSatAxisEpsilon{ 0.00001F };
    constexpr float DynamicClipPlaneEpsilon{ 0.0001F };
    constexpr float DynamicContactPenetrationEpsilon{ 0.0005F };
    constexpr float DynamicContactMergeDistanceSquared{ 0.0004F };
    constexpr float DynamicCacheCenterDeltaThresholdSquared{ 0.0025F };
    constexpr float DynamicCacheOrientationDotThreshold{ 0.9995F };
    constexpr float DynamicCacheContactMatchDistanceSquared{ 0.0009F };
    constexpr float DynamicBaumgarteFactor{ 0.08F };
    constexpr float DynamicBaumgarteSlop{ 0.005F };
    constexpr float DynamicRestitutionThreshold{ 1.25F };
    constexpr std::size_t DynamicPgsIterationCount{ 8U };
    constexpr float DynamicPositionCorrectionFactor{ 0.1F };
    constexpr float DynamicPositionCorrectionSlop{ 0.004F };
    constexpr std::uint64_t DynamicPairCacheMaximumFrameAge{ 12U };

    std::unordered_map<DynamicPairCacheKey, DynamicPersistentManifoldCache, DynamicPairCacheKeyHasher> DynamicPersistentPairCache{};
    std::uint64_t DynamicPersistentPairCacheFrameIndex{};

    float GetVectorComponent(const DirectX::SimpleMath::Vector3& Value, std::uint32_t AxisIndex) {
        if (AxisIndex == 0U) {
            return Value.x;
        }

        if (AxisIndex == 1U) {
            return Value.y;
        }

        return Value.z;
    }

    bool IsNearlyZeroVector(const DirectX::SimpleMath::Vector3& Value, float Epsilon) {
        return Value.LengthSquared() <= (Epsilon * Epsilon);
    }

    DirectX::SimpleMath::Vector3 NormalizeOrZero(const DirectX::SimpleMath::Vector3& Value) {
        float LengthSquared{ Value.LengthSquared() };
        if (LengthSquared <= (DynamicSatAxisEpsilon * DynamicSatAxisEpsilon)) {
            return DirectX::SimpleMath::Vector3{};
        }

        return Value / std::sqrt(LengthSquared);
    }

    DirectX::SimpleMath::Vector3 RotateVectorByOrientation(const DirectX::XMFLOAT4& Orientation, const DirectX::SimpleMath::Vector3& Direction) {
        DirectX::XMVECTOR DirectionVector{ DirectX::XMVectorSet(Direction.x, Direction.y, Direction.z, 0.0F) };
        DirectX::XMVECTOR OrientationQuaternion{ DirectX::XMLoadFloat4(&Orientation) };
        DirectX::XMVECTOR RotatedVector{ DirectX::XMVector3Rotate(DirectionVector, OrientationQuaternion) };
        DirectX::XMFLOAT3 RotatedValue{};
        DirectX::XMStoreFloat3(&RotatedValue, RotatedVector);
        return DirectX::SimpleMath::Vector3{ RotatedValue.x, RotatedValue.y, RotatedValue.z };
    }

    void GetPerpendicularAxisIndices(std::uint32_t AxisIndex, std::uint32_t& OutFirstAxisIndex, std::uint32_t& OutSecondAxisIndex) {
        if (AxisIndex == 0U) {
            OutFirstAxisIndex = 1U;
            OutSecondAxisIndex = 2U;
            return;
        }

        if (AxisIndex == 1U) {
            OutFirstAxisIndex = 2U;
            OutSecondAxisIndex = 0U;
            return;
        }

        OutFirstAxisIndex = 0U;
        OutSecondAxisIndex = 1U;
    }

    DynamicObb CreateDynamicObb(const DirectX::BoundingOrientedBox& BoundingBox) {
        DynamicObb ObbValue{};
        ObbValue.mCenter = DirectX::SimpleMath::Vector3{ BoundingBox.Center.x, BoundingBox.Center.y, BoundingBox.Center.z };
        ObbValue.mExtents = DirectX::SimpleMath::Vector3{ BoundingBox.Extents.x, BoundingBox.Extents.y, BoundingBox.Extents.z };
        ObbValue.mOrientation = BoundingBox.Orientation;
        ObbValue.mAxes[0U] = NormalizeOrZero(RotateVectorByOrientation(ObbValue.mOrientation, DirectX::SimpleMath::Vector3{ 1.0F, 0.0F, 0.0F }));
        ObbValue.mAxes[1U] = NormalizeOrZero(RotateVectorByOrientation(ObbValue.mOrientation, DirectX::SimpleMath::Vector3{ 0.0F, 1.0F, 0.0F }));
        ObbValue.mAxes[2U] = NormalizeOrZero(RotateVectorByOrientation(ObbValue.mOrientation, DirectX::SimpleMath::Vector3{ 0.0F, 0.0F, 1.0F }));
        return ObbValue;
    }

    DynamicPairCacheKey CreateDynamicPairCacheKey(const PhysicsActorBase& FirstActor, const PhysicsActorBase& SecondActor) {
        std::uintptr_t FirstPointer{ reinterpret_cast<std::uintptr_t>(&FirstActor) };
        std::uintptr_t SecondPointer{ reinterpret_cast<std::uintptr_t>(&SecondActor) };
        if (FirstPointer <= SecondPointer) {
            return DynamicPairCacheKey{ FirstPointer, SecondPointer };
        }

        return DynamicPairCacheKey{ SecondPointer, FirstPointer };
    }

    std::uint64_t CombineFeatureHash(std::uint64_t Seed, std::uint64_t Value) {
        std::uint64_t CombinedHash{ Seed ^ (Value + 0x9E3779B97F4A7C15ULL + (Seed << 6U) + (Seed >> 2U)) };
        return CombinedHash;
    }

    std::uint32_t QuantizeContactCoordinate(float CoordinateValue, float ExtentValue) {
        float NormalizedValue{ ExtentValue > DynamicSatAxisEpsilon ? (CoordinateValue / ExtentValue) : CoordinateValue };
        float ClampedValue{ std::clamp(NormalizedValue, -2.0F, 2.0F) };
        int QuantizedValue{ static_cast<int>(std::round((ClampedValue + 2.0F) * 1023.75F)) };
        int ClampedQuantizedValue{ std::clamp(QuantizedValue, 0, 4095) };
        return static_cast<std::uint32_t>(ClampedQuantizedValue);
    }

    std::uint64_t BuildContactFeatureKey(const DirectX::SimpleMath::Vector3& LocalPointA, const DirectX::SimpleMath::Vector3& ExtentsA, const DirectX::SimpleMath::Vector3& LocalPointB, const DirectX::SimpleMath::Vector3& ExtentsB, DynamicSatAxisType AxisType, std::uint32_t AxisIndexA, std::uint32_t AxisIndexB) {
        std::uint64_t FeatureHash{ 1469598103934665603ULL };
        FeatureHash = CombineFeatureHash(FeatureHash, static_cast<std::uint64_t>(AxisType));
        FeatureHash = CombineFeatureHash(FeatureHash, static_cast<std::uint64_t>(AxisIndexA));
        FeatureHash = CombineFeatureHash(FeatureHash, static_cast<std::uint64_t>(AxisIndexB));
        FeatureHash = CombineFeatureHash(FeatureHash, static_cast<std::uint64_t>(QuantizeContactCoordinate(LocalPointA.x, ExtentsA.x)));
        FeatureHash = CombineFeatureHash(FeatureHash, static_cast<std::uint64_t>(QuantizeContactCoordinate(LocalPointA.y, ExtentsA.y)));
        FeatureHash = CombineFeatureHash(FeatureHash, static_cast<std::uint64_t>(QuantizeContactCoordinate(LocalPointA.z, ExtentsA.z)));
        FeatureHash = CombineFeatureHash(FeatureHash, static_cast<std::uint64_t>(QuantizeContactCoordinate(LocalPointB.x, ExtentsB.x)));
        FeatureHash = CombineFeatureHash(FeatureHash, static_cast<std::uint64_t>(QuantizeContactCoordinate(LocalPointB.y, ExtentsB.y)));
        FeatureHash = CombineFeatureHash(FeatureHash, static_cast<std::uint64_t>(QuantizeContactCoordinate(LocalPointB.z, ExtentsB.z)));
        return FeatureHash;
    }

    DirectX::SimpleMath::Vector3 ConvertWorldPointToObbLocalPoint(const DynamicObb& ObbValue, const DirectX::SimpleMath::Vector3& WorldPoint) {
        DirectX::SimpleMath::Vector3 RelativePoint{ WorldPoint - ObbValue.mCenter };
        DirectX::SimpleMath::Vector3 LocalPoint{};
        LocalPoint.x = RelativePoint.Dot(ObbValue.mAxes[0U]);
        LocalPoint.y = RelativePoint.Dot(ObbValue.mAxes[1U]);
        LocalPoint.z = RelativePoint.Dot(ObbValue.mAxes[2U]);
        return LocalPoint;
    }

    DirectX::SimpleMath::Vector3 ConvertObbLocalPointToWorldPoint(const DynamicObb& ObbValue, const DirectX::SimpleMath::Vector3& LocalPoint) {
        DirectX::SimpleMath::Vector3 WorldPoint{ ObbValue.mCenter + (ObbValue.mAxes[0U] * LocalPoint.x) + (ObbValue.mAxes[1U] * LocalPoint.y) + (ObbValue.mAxes[2U] * LocalPoint.z) };
        return WorldPoint;
    }

    float CalculateProjectedRadiusOnAxis(const DynamicObb& ObbValue, const DirectX::SimpleMath::Vector3& Axis) {
        float ProjectedRadiusX{ ObbValue.mExtents.x * std::abs(Axis.Dot(ObbValue.mAxes[0U])) };
        float ProjectedRadiusY{ ObbValue.mExtents.y * std::abs(Axis.Dot(ObbValue.mAxes[1U])) };
        float ProjectedRadiusZ{ ObbValue.mExtents.z * std::abs(Axis.Dot(ObbValue.mAxes[2U])) };
        float ProjectedRadius{ ProjectedRadiusX + ProjectedRadiusY + ProjectedRadiusZ };
        return ProjectedRadius;
    }

    float CalculateAxisOverlap(const DynamicObb& FirstObb, const DynamicObb& SecondObb, const DirectX::SimpleMath::Vector3& Axis) {
        if (IsNearlyZeroVector(Axis, DynamicSatAxisEpsilon)) {
            return 0.0F;
        }

        DirectX::SimpleMath::Vector3 NormalizedAxis{ NormalizeOrZero(Axis) };
        float FirstProjectedRadius{ CalculateProjectedRadiusOnAxis(FirstObb, NormalizedAxis) };
        float SecondProjectedRadius{ CalculateProjectedRadiusOnAxis(SecondObb, NormalizedAxis) };
        float CenterDistance{ std::abs((SecondObb.mCenter - FirstObb.mCenter).Dot(NormalizedAxis)) };
        float Overlap{ (FirstProjectedRadius + SecondProjectedRadius) - CenterDistance };
        return Overlap;
    }

    bool TryGetAxisFromPersistentCache(const DynamicPersistentManifoldCache& PersistentCache, const DynamicObb& FirstObb, const DynamicObb& SecondObb, DirectX::SimpleMath::Vector3& OutAxis) {
        if (PersistentCache.mAxisType == DynamicSatAxisType::FaceA) {
            OutAxis = FirstObb.mAxes[PersistentCache.mAxisIndexA];
        } else if (PersistentCache.mAxisType == DynamicSatAxisType::FaceB) {
            OutAxis = SecondObb.mAxes[PersistentCache.mAxisIndexB];
        } else {
            DirectX::SimpleMath::Vector3 EdgeAxis{ FirstObb.mAxes[PersistentCache.mAxisIndexA].Cross(SecondObb.mAxes[PersistentCache.mAxisIndexB]) };
            OutAxis = NormalizeOrZero(EdgeAxis);
        }

        if (IsNearlyZeroVector(OutAxis, DynamicSatAxisEpsilon)) {
            return false;
        }

        if (OutAxis.Dot(PersistentCache.mNormal) < 0.0F) {
            OutAxis = -OutAxis;
        }

        OutAxis = NormalizeOrZero(OutAxis);
        return !IsNearlyZeroVector(OutAxis, DynamicSatAxisEpsilon);
    }

    bool ComputeObbSatResult(const DynamicObb& FirstObb, const DynamicObb& SecondObb, DynamicSatResult& OutSatResult) {
        std::array<std::array<float, 3U>, 3U> Rotation{};
        std::array<std::array<float, 3U>, 3U> AbsoluteRotation{};
        for (std::uint32_t AxisAIndex{ 0U }; AxisAIndex < 3U; ++AxisAIndex) {
            for (std::uint32_t AxisBIndex{ 0U }; AxisBIndex < 3U; ++AxisBIndex) {
                float DotValue{ FirstObb.mAxes[AxisAIndex].Dot(SecondObb.mAxes[AxisBIndex]) };
                Rotation[AxisAIndex][AxisBIndex] = DotValue;
                AbsoluteRotation[AxisAIndex][AxisBIndex] = std::abs(DotValue) + DynamicSatAxisEpsilon;
            }
        }

        DirectX::SimpleMath::Vector3 CenterDelta{ SecondObb.mCenter - FirstObb.mCenter };
        std::array<float, 3U> TranslationInFirstBasis{};
        TranslationInFirstBasis[0U] = CenterDelta.Dot(FirstObb.mAxes[0U]);
        TranslationInFirstBasis[1U] = CenterDelta.Dot(FirstObb.mAxes[1U]);
        TranslationInFirstBasis[2U] = CenterDelta.Dot(FirstObb.mAxes[2U]);

        DynamicSatResult BestResult{};
        BestResult.mIntersect = true;
        BestResult.mNormal = DirectX::SimpleMath::Vector3{ 1.0F, 0.0F, 0.0F };
        BestResult.mPenetration = std::numeric_limits<float>::max();
        BestResult.mAxisType = DynamicSatAxisType::FaceA;
        BestResult.mAxisIndexA = 0U;
        BestResult.mAxisIndexB = 0U;

        for (std::uint32_t AxisIndex{ 0U }; AxisIndex < 3U; ++AxisIndex) {
            float RadiusFirst{ GetVectorComponent(FirstObb.mExtents, AxisIndex) };
            float RadiusSecond{ (SecondObb.mExtents.x * AbsoluteRotation[AxisIndex][0U]) + (SecondObb.mExtents.y * AbsoluteRotation[AxisIndex][1U]) + (SecondObb.mExtents.z * AbsoluteRotation[AxisIndex][2U]) };
            float Projection{ std::abs(TranslationInFirstBasis[AxisIndex]) };
            float Overlap{ (RadiusFirst + RadiusSecond) - Projection };
            if (Overlap < 0.0F) {
                OutSatResult = DynamicSatResult{};
                OutSatResult.mIntersect = false;
                return false;
            }

            if (Overlap < BestResult.mPenetration) {
                DirectX::SimpleMath::Vector3 AxisValue{ FirstObb.mAxes[AxisIndex] };
                if (AxisValue.Dot(CenterDelta) < 0.0F) {
                    AxisValue = -AxisValue;
                }

                BestResult.mPenetration = Overlap;
                BestResult.mNormal = NormalizeOrZero(AxisValue);
                BestResult.mAxisType = DynamicSatAxisType::FaceA;
                BestResult.mAxisIndexA = AxisIndex;
                BestResult.mAxisIndexB = 0U;
            }
        }

        for (std::uint32_t AxisIndex{ 0U }; AxisIndex < 3U; ++AxisIndex) {
            float RadiusFirst{ (FirstObb.mExtents.x * AbsoluteRotation[0U][AxisIndex]) + (FirstObb.mExtents.y * AbsoluteRotation[1U][AxisIndex]) + (FirstObb.mExtents.z * AbsoluteRotation[2U][AxisIndex]) };
            float RadiusSecond{ GetVectorComponent(SecondObb.mExtents, AxisIndex) };
            float Projection{ std::abs((TranslationInFirstBasis[0U] * Rotation[0U][AxisIndex]) + (TranslationInFirstBasis[1U] * Rotation[1U][AxisIndex]) + (TranslationInFirstBasis[2U] * Rotation[2U][AxisIndex])) };
            float Overlap{ (RadiusFirst + RadiusSecond) - Projection };
            if (Overlap < 0.0F) {
                OutSatResult = DynamicSatResult{};
                OutSatResult.mIntersect = false;
                return false;
            }

            if (Overlap < BestResult.mPenetration) {
                DirectX::SimpleMath::Vector3 AxisValue{ SecondObb.mAxes[AxisIndex] };
                if (AxisValue.Dot(CenterDelta) < 0.0F) {
                    AxisValue = -AxisValue;
                }

                BestResult.mPenetration = Overlap;
                BestResult.mNormal = NormalizeOrZero(AxisValue);
                BestResult.mAxisType = DynamicSatAxisType::FaceB;
                BestResult.mAxisIndexA = 0U;
                BestResult.mAxisIndexB = AxisIndex;
            }
        }

        for (std::uint32_t AxisAIndex{ 0U }; AxisAIndex < 3U; ++AxisAIndex) {
            std::uint32_t AxisAFirstPerpendicular{ (AxisAIndex + 1U) % 3U };
            std::uint32_t AxisASecondPerpendicular{ (AxisAIndex + 2U) % 3U };
            for (std::uint32_t AxisBIndex{ 0U }; AxisBIndex < 3U; ++AxisBIndex) {
                std::uint32_t AxisBFirstPerpendicular{ (AxisBIndex + 1U) % 3U };
                std::uint32_t AxisBSecondPerpendicular{ (AxisBIndex + 2U) % 3U };
                float RadiusFirst{ (GetVectorComponent(FirstObb.mExtents, AxisAFirstPerpendicular) * AbsoluteRotation[AxisASecondPerpendicular][AxisBIndex]) + (GetVectorComponent(FirstObb.mExtents, AxisASecondPerpendicular) * AbsoluteRotation[AxisAFirstPerpendicular][AxisBIndex]) };
                float RadiusSecond{ (GetVectorComponent(SecondObb.mExtents, AxisBFirstPerpendicular) * AbsoluteRotation[AxisAIndex][AxisBSecondPerpendicular]) + (GetVectorComponent(SecondObb.mExtents, AxisBSecondPerpendicular) * AbsoluteRotation[AxisAIndex][AxisBFirstPerpendicular]) };
                float Projection{ std::abs((TranslationInFirstBasis[AxisASecondPerpendicular] * Rotation[AxisAFirstPerpendicular][AxisBIndex]) - (TranslationInFirstBasis[AxisAFirstPerpendicular] * Rotation[AxisASecondPerpendicular][AxisBIndex])) };
                float Overlap{ (RadiusFirst + RadiusSecond) - Projection };
                if (Overlap < 0.0F) {
                    OutSatResult = DynamicSatResult{};
                    OutSatResult.mIntersect = false;
                    return false;
                }

                DirectX::SimpleMath::Vector3 AxisValue{ FirstObb.mAxes[AxisAIndex].Cross(SecondObb.mAxes[AxisBIndex]) };
                float AxisLengthSquared{ AxisValue.LengthSquared() };
                if (AxisLengthSquared <= (DynamicSatAxisEpsilon * DynamicSatAxisEpsilon)) {
                    continue;
                }

                AxisValue /= std::sqrt(AxisLengthSquared);
                if (AxisValue.Dot(CenterDelta) < 0.0F) {
                    AxisValue = -AxisValue;
                }

                if (Overlap < BestResult.mPenetration) {
                    BestResult.mPenetration = Overlap;
                    BestResult.mNormal = AxisValue;
                    BestResult.mAxisType = DynamicSatAxisType::Edge;
                    BestResult.mAxisIndexA = AxisAIndex;
                    BestResult.mAxisIndexB = AxisBIndex;
                }
            }
        }

        BestResult.mNormal = NormalizeOrZero(BestResult.mNormal);
        OutSatResult = BestResult;
        return true;
    }

    std::size_t ClipPolygonAgainstPlane(const std::array<DirectX::SimpleMath::Vector3, 8U>& InputVertices, std::size_t InputCount, const DirectX::SimpleMath::Vector3& PlaneNormal, float PlaneOffset, std::array<DirectX::SimpleMath::Vector3, 8U>& OutputVertices) {
        if (InputCount == 0U) {
            return 0U;
        }

        std::size_t OutputCount{};
        DirectX::SimpleMath::Vector3 StartVertex{ InputVertices[InputCount - 1U] };
        float StartDistance{ StartVertex.Dot(PlaneNormal) - PlaneOffset };
        bool IsStartInside{ StartDistance <= DynamicClipPlaneEpsilon };

        for (std::size_t VertexIndex{ 0U }; VertexIndex < InputCount; ++VertexIndex) {
            DirectX::SimpleMath::Vector3 EndVertex{ InputVertices[VertexIndex] };
            float EndDistance{ EndVertex.Dot(PlaneNormal) - PlaneOffset };
            bool IsEndInside{ EndDistance <= DynamicClipPlaneEpsilon };

            if (IsStartInside && IsEndInside) {
                if (OutputCount < OutputVertices.size()) {
                    OutputVertices[OutputCount] = EndVertex;
                    ++OutputCount;
                }
            } else if (IsStartInside != IsEndInside) {
                DirectX::SimpleMath::Vector3 EdgeVector{ EndVertex - StartVertex };
                float Denominator{ EdgeVector.Dot(PlaneNormal) };
                if (std::abs(Denominator) > DynamicSatAxisEpsilon) {
                    float InterpolationFactor{ -StartDistance / Denominator };
                    InterpolationFactor = std::clamp(InterpolationFactor, 0.0F, 1.0F);
                    DirectX::SimpleMath::Vector3 IntersectionPoint{ StartVertex + (EdgeVector * InterpolationFactor) };
                    if (OutputCount < OutputVertices.size()) {
                        OutputVertices[OutputCount] = IntersectionPoint;
                        ++OutputCount;
                    }
                }

                if (IsEndInside && OutputCount < OutputVertices.size()) {
                    OutputVertices[OutputCount] = EndVertex;
                    ++OutputCount;
                }
            }

            StartVertex = EndVertex;
            StartDistance = EndDistance;
            IsStartInside = IsEndInside;
        }

        return OutputCount;
    }

    void AddContactPointToManifold(DynamicContactManifold& InOutManifold, const DynamicContactPoint& ContactPointValue) {
        for (std::size_t ContactIndex{ 0U }; ContactIndex < InOutManifold.mContactCount; ++ContactIndex) {
            DirectX::SimpleMath::Vector3 DeltaPoint{ InOutManifold.mContacts[ContactIndex].mWorldPoint - ContactPointValue.mWorldPoint };
            if (DeltaPoint.LengthSquared() <= DynamicContactMergeDistanceSquared) {
                if (ContactPointValue.mPenetration > InOutManifold.mContacts[ContactIndex].mPenetration) {
                    InOutManifold.mContacts[ContactIndex] = ContactPointValue;
                }

                return;
            }
        }

        if (InOutManifold.mContactCount < InOutManifold.mContacts.size()) {
            InOutManifold.mContacts[InOutManifold.mContactCount] = ContactPointValue;
            ++InOutManifold.mContactCount;
            return;
        }

        std::size_t MinimumPenetrationIndex{};
        float MinimumPenetration{ InOutManifold.mContacts[0U].mPenetration };
        for (std::size_t ContactIndex{ 1U }; ContactIndex < InOutManifold.mContacts.size(); ++ContactIndex) {
            if (InOutManifold.mContacts[ContactIndex].mPenetration < MinimumPenetration) {
                MinimumPenetration = InOutManifold.mContacts[ContactIndex].mPenetration;
                MinimumPenetrationIndex = ContactIndex;
            }
        }

        if (ContactPointValue.mPenetration > MinimumPenetration) {
            InOutManifold.mContacts[MinimumPenetrationIndex] = ContactPointValue;
        }
    }

    void BuildFaceVertices(const DynamicObb& ObbValue, std::uint32_t FaceAxisIndex, float FaceSign, std::array<DirectX::SimpleMath::Vector3, 4U>& OutFaceVertices, DirectX::SimpleMath::Vector3& OutFaceCenter, std::uint32_t& OutAxisUIndex, std::uint32_t& OutAxisVIndex) {
        GetPerpendicularAxisIndices(FaceAxisIndex, OutAxisUIndex, OutAxisVIndex);
        float FaceExtent{ GetVectorComponent(ObbValue.mExtents, FaceAxisIndex) };
        float ExtentU{ GetVectorComponent(ObbValue.mExtents, OutAxisUIndex) };
        float ExtentV{ GetVectorComponent(ObbValue.mExtents, OutAxisVIndex) };
        DirectX::SimpleMath::Vector3 AxisN{ ObbValue.mAxes[FaceAxisIndex] };
        DirectX::SimpleMath::Vector3 AxisU{ ObbValue.mAxes[OutAxisUIndex] };
        DirectX::SimpleMath::Vector3 AxisV{ ObbValue.mAxes[OutAxisVIndex] };
        OutFaceCenter = ObbValue.mCenter + (AxisN * (FaceExtent * FaceSign));
        OutFaceVertices[0U] = OutFaceCenter + (AxisU * ExtentU) + (AxisV * ExtentV);
        OutFaceVertices[1U] = OutFaceCenter - (AxisU * ExtentU) + (AxisV * ExtentV);
        OutFaceVertices[2U] = OutFaceCenter - (AxisU * ExtentU) - (AxisV * ExtentV);
        OutFaceVertices[3U] = OutFaceCenter + (AxisU * ExtentU) - (AxisV * ExtentV);
    }

    void BuildIncidentFaceVertices(const DynamicObb& IncidentObb, const DirectX::SimpleMath::Vector3& ReferenceNormal, std::array<DirectX::SimpleMath::Vector3, 4U>& OutFaceVertices, std::uint32_t& OutIncidentFaceAxisIndex) {
        float MinimumDotValue{ std::numeric_limits<float>::max() };
        std::uint32_t MinimumDotAxisIndex{};
        for (std::uint32_t AxisIndex{ 0U }; AxisIndex < 3U; ++AxisIndex) {
            float DotValue{ IncidentObb.mAxes[AxisIndex].Dot(ReferenceNormal) };
            if (DotValue < MinimumDotValue) {
                MinimumDotValue = DotValue;
                MinimumDotAxisIndex = AxisIndex;
            }
        }

        float FaceSign{ MinimumDotValue < 0.0F ? 1.0F : -1.0F };
        DirectX::SimpleMath::Vector3 FaceCenter{};
        std::uint32_t AxisUIndex{};
        std::uint32_t AxisVIndex{};
        BuildFaceVertices(IncidentObb, MinimumDotAxisIndex, FaceSign, OutFaceVertices, FaceCenter, AxisUIndex, AxisVIndex);
        OutIncidentFaceAxisIndex = MinimumDotAxisIndex;
    }

    bool GenerateFaceContactManifold(const DynamicObb& FirstObb, const DynamicObb& SecondObb, const DynamicSatResult& SatResult, DynamicContactManifold& OutManifold) {
        const DynamicObb* ReferenceObb{ &FirstObb };
        const DynamicObb* IncidentObb{ &SecondObb };
        DirectX::SimpleMath::Vector3 ReferenceNormal{ SatResult.mNormal };
        std::uint32_t ReferenceFaceAxisIndex{ SatResult.mAxisIndexA };
        if (SatResult.mAxisType == DynamicSatAxisType::FaceB) {
            ReferenceObb = &SecondObb;
            IncidentObb = &FirstObb;
            ReferenceNormal = -SatResult.mNormal;
            ReferenceFaceAxisIndex = SatResult.mAxisIndexB;
        }

        std::array<DirectX::SimpleMath::Vector3, 4U> ReferenceFaceVertices{};
        DirectX::SimpleMath::Vector3 ReferenceFaceCenter{};
        std::uint32_t ReferenceAxisUIndex{};
        std::uint32_t ReferenceAxisVIndex{};
        float ReferenceFaceSign{ ReferenceObb->mAxes[ReferenceFaceAxisIndex].Dot(ReferenceNormal) >= 0.0F ? 1.0F : -1.0F };
        BuildFaceVertices(*ReferenceObb, ReferenceFaceAxisIndex, ReferenceFaceSign, ReferenceFaceVertices, ReferenceFaceCenter, ReferenceAxisUIndex, ReferenceAxisVIndex);

        std::array<DirectX::SimpleMath::Vector3, 4U> IncidentFaceVertices{};
        std::uint32_t IncidentFaceAxisIndex{};
        BuildIncidentFaceVertices(*IncidentObb, ReferenceNormal, IncidentFaceVertices, IncidentFaceAxisIndex);

        std::array<DirectX::SimpleMath::Vector3, 8U> ClipBufferA{};
        std::array<DirectX::SimpleMath::Vector3, 8U> ClipBufferB{};
        for (std::size_t VertexIndex{ 0U }; VertexIndex < IncidentFaceVertices.size(); ++VertexIndex) {
            ClipBufferA[VertexIndex] = IncidentFaceVertices[VertexIndex];
        }

        std::size_t ClippedVertexCount{ IncidentFaceVertices.size() };
        DirectX::SimpleMath::Vector3 ReferenceAxisU{ ReferenceObb->mAxes[ReferenceAxisUIndex] };
        DirectX::SimpleMath::Vector3 ReferenceAxisV{ ReferenceObb->mAxes[ReferenceAxisVIndex] };
        float ReferenceExtentU{ GetVectorComponent(ReferenceObb->mExtents, ReferenceAxisUIndex) };
        float ReferenceExtentV{ GetVectorComponent(ReferenceObb->mExtents, ReferenceAxisVIndex) };
        ClippedVertexCount = ClipPolygonAgainstPlane(ClipBufferA, ClippedVertexCount, ReferenceAxisU, ReferenceFaceCenter.Dot(ReferenceAxisU) + ReferenceExtentU, ClipBufferB);
        ClippedVertexCount = ClipPolygonAgainstPlane(ClipBufferB, ClippedVertexCount, -ReferenceAxisU, (-ReferenceFaceCenter).Dot(ReferenceAxisU) + ReferenceExtentU, ClipBufferA);
        ClippedVertexCount = ClipPolygonAgainstPlane(ClipBufferA, ClippedVertexCount, ReferenceAxisV, ReferenceFaceCenter.Dot(ReferenceAxisV) + ReferenceExtentV, ClipBufferB);
        ClippedVertexCount = ClipPolygonAgainstPlane(ClipBufferB, ClippedVertexCount, -ReferenceAxisV, (-ReferenceFaceCenter).Dot(ReferenceAxisV) + ReferenceExtentV, ClipBufferA);
        if (ClippedVertexCount == 0U) {
            return false;
        }

        OutManifold.mNormal = SatResult.mNormal;
        OutManifold.mPenetration = SatResult.mPenetration;
        OutManifold.mAxisType = SatResult.mAxisType;
        OutManifold.mAxisIndexA = SatResult.mAxisIndexA;
        OutManifold.mAxisIndexB = SatResult.mAxisIndexB;
        OutManifold.mContactCount = 0U;

        for (std::size_t VertexIndex{ 0U }; VertexIndex < ClippedVertexCount; ++VertexIndex) {
            DirectX::SimpleMath::Vector3 IncidentVertex{ ClipBufferA[VertexIndex] };
            float SignedDistanceToReferenceFace{ (IncidentVertex - ReferenceFaceCenter).Dot(ReferenceNormal) };
            float PenetrationDepth{ -SignedDistanceToReferenceFace };
            if (PenetrationDepth < -DynamicContactPenetrationEpsilon) {
                continue;
            }

            DynamicContactPoint ContactPointValue{};
            float ClampedPenetrationDepth{ std::max(0.0F, PenetrationDepth) };
            ContactPointValue.mPenetration = ClampedPenetrationDepth;
            ContactPointValue.mWorldPoint = IncidentVertex + (ReferenceNormal * (0.5F * ClampedPenetrationDepth));
            ContactPointValue.mLocalPointA = ConvertWorldPointToObbLocalPoint(FirstObb, ContactPointValue.mWorldPoint);
            ContactPointValue.mLocalPointB = ConvertWorldPointToObbLocalPoint(SecondObb, ContactPointValue.mWorldPoint);
            ContactPointValue.mAccumulatedNormalImpulse = 0.0F;
            ContactPointValue.mAccumulatedTangentImpulse = DirectX::SimpleMath::Vector3{};
            ContactPointValue.mFeatureKey = BuildContactFeatureKey(ContactPointValue.mLocalPointA, FirstObb.mExtents, ContactPointValue.mLocalPointB, SecondObb.mExtents, SatResult.mAxisType, SatResult.mAxisIndexA, SatResult.mAxisIndexB);
            AddContactPointToManifold(OutManifold, ContactPointValue);
        }

        return OutManifold.mContactCount > 0U;
    }

    void BuildSupportEdgeSegment(const DynamicObb& ObbValue, std::uint32_t EdgeAxisIndex, const DirectX::SimpleMath::Vector3& SupportDirection, DirectX::SimpleMath::Vector3& OutEdgeStart, DirectX::SimpleMath::Vector3& OutEdgeEnd) {
        std::uint32_t FirstPerpendicularAxisIndex{};
        std::uint32_t SecondPerpendicularAxisIndex{};
        GetPerpendicularAxisIndices(EdgeAxisIndex, FirstPerpendicularAxisIndex, SecondPerpendicularAxisIndex);
        float FirstPerpendicularSign{ ObbValue.mAxes[FirstPerpendicularAxisIndex].Dot(SupportDirection) >= 0.0F ? 1.0F : -1.0F };
        float SecondPerpendicularSign{ ObbValue.mAxes[SecondPerpendicularAxisIndex].Dot(SupportDirection) >= 0.0F ? 1.0F : -1.0F };
        float FirstPerpendicularExtent{ GetVectorComponent(ObbValue.mExtents, FirstPerpendicularAxisIndex) };
        float SecondPerpendicularExtent{ GetVectorComponent(ObbValue.mExtents, SecondPerpendicularAxisIndex) };
        float EdgeExtent{ GetVectorComponent(ObbValue.mExtents, EdgeAxisIndex) };
        DirectX::SimpleMath::Vector3 BasePoint{ ObbValue.mCenter + (ObbValue.mAxes[FirstPerpendicularAxisIndex] * (FirstPerpendicularExtent * FirstPerpendicularSign)) + (ObbValue.mAxes[SecondPerpendicularAxisIndex] * (SecondPerpendicularExtent * SecondPerpendicularSign)) };
        DirectX::SimpleMath::Vector3 EdgeAxis{ ObbValue.mAxes[EdgeAxisIndex] };
        OutEdgeStart = BasePoint + (EdgeAxis * EdgeExtent);
        OutEdgeEnd = BasePoint - (EdgeAxis * EdgeExtent);
    }

    void CalculateClosestPointsOnSegments(const DirectX::SimpleMath::Vector3& FirstSegmentStart, const DirectX::SimpleMath::Vector3& FirstSegmentEnd, const DirectX::SimpleMath::Vector3& SecondSegmentStart, const DirectX::SimpleMath::Vector3& SecondSegmentEnd, DirectX::SimpleMath::Vector3& OutClosestPointOnFirstSegment, DirectX::SimpleMath::Vector3& OutClosestPointOnSecondSegment) {
        DirectX::SimpleMath::Vector3 FirstSegmentVector{ FirstSegmentEnd - FirstSegmentStart };
        DirectX::SimpleMath::Vector3 SecondSegmentVector{ SecondSegmentEnd - SecondSegmentStart };
        DirectX::SimpleMath::Vector3 StartDelta{ FirstSegmentStart - SecondSegmentStart };
        float FirstSegmentLengthSquared{ FirstSegmentVector.Dot(FirstSegmentVector) };
        float SecondSegmentLengthSquared{ SecondSegmentVector.Dot(SecondSegmentVector) };
        float FirstSegmentParameter{};
        float SecondSegmentParameter{};

        if (FirstSegmentLengthSquared <= DynamicSatAxisEpsilon && SecondSegmentLengthSquared <= DynamicSatAxisEpsilon) {
            OutClosestPointOnFirstSegment = FirstSegmentStart;
            OutClosestPointOnSecondSegment = SecondSegmentStart;
            return;
        }

        if (FirstSegmentLengthSquared <= DynamicSatAxisEpsilon) {
            SecondSegmentParameter = std::clamp(SecondSegmentVector.Dot(StartDelta) / SecondSegmentLengthSquared, 0.0F, 1.0F);
        } else {
            float DotWithStartDelta{ FirstSegmentVector.Dot(StartDelta) };
            if (SecondSegmentLengthSquared <= DynamicSatAxisEpsilon) {
                FirstSegmentParameter = std::clamp(-DotWithStartDelta / FirstSegmentLengthSquared, 0.0F, 1.0F);
            } else {
                float SegmentDotProduct{ FirstSegmentVector.Dot(SecondSegmentVector) };
                float Denominator{ (FirstSegmentLengthSquared * SecondSegmentLengthSquared) - (SegmentDotProduct * SegmentDotProduct) };
                if (std::abs(Denominator) > DynamicSatAxisEpsilon) {
                    FirstSegmentParameter = std::clamp(((SegmentDotProduct * SecondSegmentVector.Dot(StartDelta)) - (DotWithStartDelta * SecondSegmentLengthSquared)) / Denominator, 0.0F, 1.0F);
                }

                float SecondParameterNumerator{ (SegmentDotProduct * FirstSegmentParameter) + SecondSegmentVector.Dot(StartDelta) };
                if (SecondParameterNumerator < 0.0F) {
                    SecondSegmentParameter = 0.0F;
                    FirstSegmentParameter = std::clamp(-DotWithStartDelta / FirstSegmentLengthSquared, 0.0F, 1.0F);
                } else if (SecondParameterNumerator > SecondSegmentLengthSquared) {
                    SecondSegmentParameter = 1.0F;
                    FirstSegmentParameter = std::clamp((SegmentDotProduct - DotWithStartDelta) / FirstSegmentLengthSquared, 0.0F, 1.0F);
                } else {
                    SecondSegmentParameter = SecondParameterNumerator / SecondSegmentLengthSquared;
                }
            }
        }

        OutClosestPointOnFirstSegment = FirstSegmentStart + (FirstSegmentVector * FirstSegmentParameter);
        OutClosestPointOnSecondSegment = SecondSegmentStart + (SecondSegmentVector * SecondSegmentParameter);
    }

    bool GenerateEdgeContactManifold(const DynamicObb& FirstObb, const DynamicObb& SecondObb, const DynamicSatResult& SatResult, DynamicContactManifold& OutManifold) {
        DirectX::SimpleMath::Vector3 FirstEdgeStart{};
        DirectX::SimpleMath::Vector3 FirstEdgeEnd{};
        DirectX::SimpleMath::Vector3 SecondEdgeStart{};
        DirectX::SimpleMath::Vector3 SecondEdgeEnd{};
        BuildSupportEdgeSegment(FirstObb, SatResult.mAxisIndexA, SatResult.mNormal, FirstEdgeStart, FirstEdgeEnd);
        BuildSupportEdgeSegment(SecondObb, SatResult.mAxisIndexB, -SatResult.mNormal, SecondEdgeStart, SecondEdgeEnd);

        DirectX::SimpleMath::Vector3 ClosestPointOnFirstEdge{};
        DirectX::SimpleMath::Vector3 ClosestPointOnSecondEdge{};
        CalculateClosestPointsOnSegments(FirstEdgeStart, FirstEdgeEnd, SecondEdgeStart, SecondEdgeEnd, ClosestPointOnFirstEdge, ClosestPointOnSecondEdge);

        OutManifold.mNormal = SatResult.mNormal;
        OutManifold.mPenetration = SatResult.mPenetration;
        OutManifold.mAxisType = SatResult.mAxisType;
        OutManifold.mAxisIndexA = SatResult.mAxisIndexA;
        OutManifold.mAxisIndexB = SatResult.mAxisIndexB;
        OutManifold.mContactCount = 1U;

        DynamicContactPoint ContactPointValue{};
        ContactPointValue.mWorldPoint = (ClosestPointOnFirstEdge + ClosestPointOnSecondEdge) * 0.5F;
        ContactPointValue.mPenetration = SatResult.mPenetration;
        ContactPointValue.mLocalPointA = ConvertWorldPointToObbLocalPoint(FirstObb, ContactPointValue.mWorldPoint);
        ContactPointValue.mLocalPointB = ConvertWorldPointToObbLocalPoint(SecondObb, ContactPointValue.mWorldPoint);
        ContactPointValue.mAccumulatedNormalImpulse = 0.0F;
        ContactPointValue.mAccumulatedTangentImpulse = DirectX::SimpleMath::Vector3{};
        ContactPointValue.mFeatureKey = BuildContactFeatureKey(ContactPointValue.mLocalPointA, FirstObb.mExtents, ContactPointValue.mLocalPointB, SecondObb.mExtents, SatResult.mAxisType, SatResult.mAxisIndexA, SatResult.mAxisIndexB);
        OutManifold.mContacts[0U] = ContactPointValue;
        return true;
    }

    bool BuildContactManifoldFromSatResult(const DynamicObb& FirstObb, const DynamicObb& SecondObb, const DynamicSatResult& SatResult, DynamicContactManifold& OutManifold) {
        if (SatResult.mAxisType == DynamicSatAxisType::Edge) {
            return GenerateEdgeContactManifold(FirstObb, SecondObb, SatResult, OutManifold);
        }

        return GenerateFaceContactManifold(FirstObb, SecondObb, SatResult, OutManifold);
    }

    void SeedManifoldWithPersistentCache(const DynamicPersistentManifoldCache& PersistentCache, DynamicContactManifold& InOutManifold, bool ApplyWarmStart) {
        std::array<bool, 4U> UsedPersistentContacts{};
        for (std::size_t ContactIndex{ 0U }; ContactIndex < InOutManifold.mContactCount; ++ContactIndex) {
            DynamicContactPoint& ContactPointValue{ InOutManifold.mContacts[ContactIndex] };
            ContactPointValue.mAccumulatedNormalImpulse = 0.0F;
            ContactPointValue.mAccumulatedTangentImpulse = DirectX::SimpleMath::Vector3{};

            std::size_t BestPersistentIndex{ PersistentCache.mContactCount };
            for (std::size_t PersistentIndex{ 0U }; PersistentIndex < PersistentCache.mContactCount; ++PersistentIndex) {
                if (UsedPersistentContacts[PersistentIndex]) {
                    continue;
                }

                if (PersistentCache.mContacts[PersistentIndex].mFeatureKey == ContactPointValue.mFeatureKey) {
                    BestPersistentIndex = PersistentIndex;
                    break;
                }
            }

            if (BestPersistentIndex == PersistentCache.mContactCount) {
                float MinimumDistanceSquared{ std::numeric_limits<float>::max() };
                for (std::size_t PersistentIndex{ 0U }; PersistentIndex < PersistentCache.mContactCount; ++PersistentIndex) {
                    if (UsedPersistentContacts[PersistentIndex]) {
                        continue;
                    }

                    DirectX::SimpleMath::Vector3 LocalDeltaA{ ContactPointValue.mLocalPointA - PersistentCache.mContacts[PersistentIndex].mLocalPointA };
                    DirectX::SimpleMath::Vector3 LocalDeltaB{ ContactPointValue.mLocalPointB - PersistentCache.mContacts[PersistentIndex].mLocalPointB };
                    float DistanceSquared{ LocalDeltaA.LengthSquared() + LocalDeltaB.LengthSquared() };
                    if (DistanceSquared < MinimumDistanceSquared) {
                        MinimumDistanceSquared = DistanceSquared;
                        BestPersistentIndex = PersistentIndex;
                    }
                }

                if (BestPersistentIndex < PersistentCache.mContactCount) {
                    DirectX::SimpleMath::Vector3 LocalDeltaA{ ContactPointValue.mLocalPointA - PersistentCache.mContacts[BestPersistentIndex].mLocalPointA };
                    DirectX::SimpleMath::Vector3 LocalDeltaB{ ContactPointValue.mLocalPointB - PersistentCache.mContacts[BestPersistentIndex].mLocalPointB };
                    if ((LocalDeltaA.LengthSquared() + LocalDeltaB.LengthSquared()) > DynamicCacheContactMatchDistanceSquared) {
                        BestPersistentIndex = PersistentCache.mContactCount;
                    }
                }
            }

            if (ApplyWarmStart && BestPersistentIndex < PersistentCache.mContactCount) {
                ContactPointValue.mAccumulatedNormalImpulse = PersistentCache.mContacts[BestPersistentIndex].mAccumulatedNormalImpulse;
                ContactPointValue.mAccumulatedTangentImpulse = PersistentCache.mContacts[BestPersistentIndex].mAccumulatedTangentImpulse;
                UsedPersistentContacts[BestPersistentIndex] = true;
            }
        }
    }

    bool HasSmallTransformDeltaForCacheReuse(const DynamicPersistentManifoldCache& PersistentCache, const DynamicObb& FirstObb, const DynamicObb& SecondObb) {
        DirectX::SimpleMath::Vector3 CenterDeltaA{ FirstObb.mCenter - PersistentCache.mCenterA };
        DirectX::SimpleMath::Vector3 CenterDeltaB{ SecondObb.mCenter - PersistentCache.mCenterB };
        if (CenterDeltaA.LengthSquared() > DynamicCacheCenterDeltaThresholdSquared || CenterDeltaB.LengthSquared() > DynamicCacheCenterDeltaThresholdSquared) {
            return false;
        }

        float OrientationDotA{ std::abs((FirstObb.mOrientation.x * PersistentCache.mOrientationA.x) + (FirstObb.mOrientation.y * PersistentCache.mOrientationA.y) + (FirstObb.mOrientation.z * PersistentCache.mOrientationA.z) + (FirstObb.mOrientation.w * PersistentCache.mOrientationA.w)) };
        float OrientationDotB{ std::abs((SecondObb.mOrientation.x * PersistentCache.mOrientationB.x) + (SecondObb.mOrientation.y * PersistentCache.mOrientationB.y) + (SecondObb.mOrientation.z * PersistentCache.mOrientationB.z) + (SecondObb.mOrientation.w * PersistentCache.mOrientationB.w)) };
        return OrientationDotA >= DynamicCacheOrientationDotThreshold && OrientationDotB >= DynamicCacheOrientationDotThreshold;
    }

    bool TryBuildContactManifoldFromPersistentCache(const DynamicPersistentManifoldCache& PersistentCache, const DynamicObb& FirstObb, const DynamicObb& SecondObb, DynamicContactManifold& OutManifold) {
        if (PersistentCache.mContactCount == 0U) {
            return false;
        }

        if (!HasSmallTransformDeltaForCacheReuse(PersistentCache, FirstObb, SecondObb)) {
            return false;
        }

        DirectX::SimpleMath::Vector3 CachedAxis{};
        bool HasAxis{ TryGetAxisFromPersistentCache(PersistentCache, FirstObb, SecondObb, CachedAxis) };
        if (!HasAxis) {
            return false;
        }

        float Overlap{ CalculateAxisOverlap(FirstObb, SecondObb, CachedAxis) };
        if (Overlap <= DynamicContactPenetrationEpsilon) {
            return false;
        }

        OutManifold.mNormal = CachedAxis;
        OutManifold.mPenetration = Overlap;
        OutManifold.mAxisType = PersistentCache.mAxisType;
        OutManifold.mAxisIndexA = PersistentCache.mAxisIndexA;
        OutManifold.mAxisIndexB = PersistentCache.mAxisIndexB;
        OutManifold.mContactCount = 0U;
        for (std::size_t ContactIndex{ 0U }; ContactIndex < PersistentCache.mContactCount; ++ContactIndex) {
            const DynamicPersistentContactPoint& PersistentContact{ PersistentCache.mContacts[ContactIndex] };
            DynamicContactPoint ContactPointValue{};
            DirectX::SimpleMath::Vector3 WorldPointOnFirstObb{ ConvertObbLocalPointToWorldPoint(FirstObb, PersistentContact.mLocalPointA) };
            DirectX::SimpleMath::Vector3 WorldPointOnSecondObb{ ConvertObbLocalPointToWorldPoint(SecondObb, PersistentContact.mLocalPointB) };
            DirectX::SimpleMath::Vector3 ContactDelta{ WorldPointOnSecondObb - WorldPointOnFirstObb };
            ContactPointValue.mWorldPoint = (WorldPointOnFirstObb + WorldPointOnSecondObb) * 0.5F;
            ContactPointValue.mLocalPointA = PersistentContact.mLocalPointA;
            ContactPointValue.mLocalPointB = PersistentContact.mLocalPointB;
            ContactPointValue.mPenetration = std::max(0.0F, -ContactDelta.Dot(CachedAxis));
            if (ContactPointValue.mPenetration <= DynamicContactPenetrationEpsilon) {
                continue;
            }

            ContactPointValue.mAccumulatedNormalImpulse = PersistentContact.mAccumulatedNormalImpulse;
            ContactPointValue.mAccumulatedTangentImpulse = PersistentContact.mAccumulatedTangentImpulse;
            ContactPointValue.mFeatureKey = PersistentContact.mFeatureKey;
            AddContactPointToManifold(OutManifold, ContactPointValue);
        }

        return OutManifold.mContactCount > 0U;
    }

    void ApplyImpulseToVelocityPair(DirectX::SimpleMath::Vector3& InOutFirstVelocity, DirectX::SimpleMath::Vector3& InOutSecondVelocity, float FirstInverseMass, float SecondInverseMass, const DirectX::SimpleMath::Vector3& ImpulseValue) {
        InOutFirstVelocity -= ImpulseValue * FirstInverseMass;
        InOutSecondVelocity += ImpulseValue * SecondInverseMass;
    }

    void ApplyWarmStartingToManifold(const DynamicContactManifold& ManifoldValue, float FirstInverseMass, float SecondInverseMass, DirectX::SimpleMath::Vector3& InOutFirstVelocity, DirectX::SimpleMath::Vector3& InOutSecondVelocity) {
        for (std::size_t ContactIndex{ 0U }; ContactIndex < ManifoldValue.mContactCount; ++ContactIndex) {
            const DynamicContactPoint& ContactPointValue{ ManifoldValue.mContacts[ContactIndex] };
            DirectX::SimpleMath::Vector3 TotalImpulse{ (ManifoldValue.mNormal * ContactPointValue.mAccumulatedNormalImpulse) + ContactPointValue.mAccumulatedTangentImpulse };
            ApplyImpulseToVelocityPair(InOutFirstVelocity, InOutSecondVelocity, FirstInverseMass, SecondInverseMass, TotalImpulse);
        }
    }

    void SolveContactManifoldWithPgs(DynamicContactManifold& InOutManifold, PhysicsActorBase& FirstActor, PhysicsActorBase& SecondActor, float DeltaTime) {
        float FirstInverseMass{ FirstActor.GetInverseMass() };
        float SecondInverseMass{ SecondActor.GetInverseMass() };
        float CombinedInverseMass{ FirstInverseMass + SecondInverseMass };
        if (CombinedInverseMass <= 0.0F) {
            return;
        }

        DirectX::SimpleMath::Vector3 FirstVelocity{ FirstActor.GetVelocity() };
        DirectX::SimpleMath::Vector3 SecondVelocity{ SecondActor.GetVelocity() };
        float EffectiveRestitution{ std::min(FirstActor.GetRestitution(), SecondActor.GetRestitution()) };
        float EffectiveFriction{ std::sqrt(std::max(0.0F, FirstActor.GetFriction() * SecondActor.GetFriction())) };
        ApplyWarmStartingToManifold(InOutManifold, FirstInverseMass, SecondInverseMass, FirstVelocity, SecondVelocity);

        float InverseDeltaTime{ DeltaTime > DynamicSatAxisEpsilon ? (1.0F / DeltaTime) : 0.0F };
        for (std::size_t IterationIndex{ 0U }; IterationIndex < DynamicPgsIterationCount; ++IterationIndex) {
            for (std::size_t ContactIndex{ 0U }; ContactIndex < InOutManifold.mContactCount; ++ContactIndex) {
                DynamicContactPoint& ContactPointValue{ InOutManifold.mContacts[ContactIndex] };
                DirectX::SimpleMath::Vector3 RelativeVelocity{ SecondVelocity - FirstVelocity };
                float RelativeNormalVelocity{ RelativeVelocity.Dot(InOutManifold.mNormal) };
                float PositionBiasVelocity{ std::max(0.0F, ContactPointValue.mPenetration - DynamicBaumgarteSlop) * DynamicBaumgarteFactor * InverseDeltaTime };
                float RestitutionVelocity{};
                if (ContactPointValue.mPenetration <= DynamicBaumgarteSlop && RelativeNormalVelocity < -DynamicRestitutionThreshold) {
                    RestitutionVelocity = -EffectiveRestitution * RelativeNormalVelocity;
                }

                float NormalImpulseDelta{ (-RelativeNormalVelocity + RestitutionVelocity + PositionBiasVelocity) / CombinedInverseMass };
                float PreviousNormalImpulse{ ContactPointValue.mAccumulatedNormalImpulse };
                ContactPointValue.mAccumulatedNormalImpulse = std::max(0.0F, PreviousNormalImpulse + NormalImpulseDelta);
                float AppliedNormalImpulseDelta{ ContactPointValue.mAccumulatedNormalImpulse - PreviousNormalImpulse };
                if (AppliedNormalImpulseDelta > 0.0F) {
                    DirectX::SimpleMath::Vector3 NormalImpulse{ InOutManifold.mNormal * AppliedNormalImpulseDelta };
                    ApplyImpulseToVelocityPair(FirstVelocity, SecondVelocity, FirstInverseMass, SecondInverseMass, NormalImpulse);
                }

                RelativeVelocity = SecondVelocity - FirstVelocity;
                float VelocityAlongNormal{ RelativeVelocity.Dot(InOutManifold.mNormal) };
                DirectX::SimpleMath::Vector3 TangentialVelocity{ RelativeVelocity - (InOutManifold.mNormal * VelocityAlongNormal) };
                float TangentialVelocityLengthSquared{ TangentialVelocity.LengthSquared() };
                if (TangentialVelocityLengthSquared <= (DynamicSatAxisEpsilon * DynamicSatAxisEpsilon)) {
                    continue;
                }

                DirectX::SimpleMath::Vector3 Tangent{ TangentialVelocity / std::sqrt(TangentialVelocityLengthSquared) };
                float TangentImpulseDeltaMagnitude{ -RelativeVelocity.Dot(Tangent) / CombinedInverseMass };
                DirectX::SimpleMath::Vector3 CandidateTangentImpulse{ ContactPointValue.mAccumulatedTangentImpulse + (Tangent * TangentImpulseDeltaMagnitude) };
                float MaximumFrictionImpulse{ EffectiveFriction * ContactPointValue.mAccumulatedNormalImpulse };
                float CandidateTangentLengthSquared{ CandidateTangentImpulse.LengthSquared() };
                if (CandidateTangentLengthSquared > (MaximumFrictionImpulse * MaximumFrictionImpulse) && CandidateTangentLengthSquared > (DynamicSatAxisEpsilon * DynamicSatAxisEpsilon)) {
                    CandidateTangentImpulse *= MaximumFrictionImpulse / std::sqrt(CandidateTangentLengthSquared);
                }

                DirectX::SimpleMath::Vector3 TangentImpulseDelta{ CandidateTangentImpulse - ContactPointValue.mAccumulatedTangentImpulse };
                ContactPointValue.mAccumulatedTangentImpulse = CandidateTangentImpulse;
                ApplyImpulseToVelocityPair(FirstVelocity, SecondVelocity, FirstInverseMass, SecondInverseMass, TangentImpulseDelta);
            }
        }

        FirstActor.SetVelocity(FirstVelocity);
        SecondActor.SetVelocity(SecondVelocity);
    }

    void ApplyPositionCorrectionFromManifold(const DynamicContactManifold& ManifoldValue, PhysicsActorBase& FirstActor, PhysicsActorBase& SecondActor) {
        float FirstInverseMass{ FirstActor.GetInverseMass() };
        float SecondInverseMass{ SecondActor.GetInverseMass() };
        float CombinedInverseMass{ FirstInverseMass + SecondInverseMass };
        if (CombinedInverseMass <= 0.0F) {
            return;
        }

        if (ManifoldValue.mPenetration <= (DynamicBaumgarteSlop * 2.0F)) {
            return;
        }

        float CorrectedPenetration{ std::max(0.0F, ManifoldValue.mPenetration - DynamicPositionCorrectionSlop) };
        if (CorrectedPenetration <= 0.0F) {
            return;
        }

        float CorrectionMagnitude{ (CorrectedPenetration / CombinedInverseMass) * DynamicPositionCorrectionFactor };
        DirectX::SimpleMath::Vector3 CorrectionVector{ ManifoldValue.mNormal * CorrectionMagnitude };
        FirstActor.SetPosition(FirstActor.GetPosition() - (CorrectionVector * FirstInverseMass));
        SecondActor.SetPosition(SecondActor.GetPosition() + (CorrectionVector * SecondInverseMass));
    }

    void UpdatePersistentCacheFromManifold(const DynamicObb& FirstObb, const DynamicObb& SecondObb, const DynamicContactManifold& ManifoldValue, DynamicPersistentManifoldCache& OutPersistentCache) {
        OutPersistentCache.mNormal = ManifoldValue.mNormal;
        OutPersistentCache.mCenterA = FirstObb.mCenter;
        OutPersistentCache.mCenterB = SecondObb.mCenter;
        OutPersistentCache.mOrientationA = FirstObb.mOrientation;
        OutPersistentCache.mOrientationB = SecondObb.mOrientation;
        OutPersistentCache.mPenetration = ManifoldValue.mPenetration;
        OutPersistentCache.mAxisType = ManifoldValue.mAxisType;
        OutPersistentCache.mAxisIndexA = ManifoldValue.mAxisIndexA;
        OutPersistentCache.mAxisIndexB = ManifoldValue.mAxisIndexB;
        OutPersistentCache.mLastFrameIndex = DynamicPersistentPairCacheFrameIndex;
        OutPersistentCache.mContactCount = ManifoldValue.mContactCount;
        for (std::size_t ContactIndex{ 0U }; ContactIndex < ManifoldValue.mContactCount; ++ContactIndex) {
            OutPersistentCache.mContacts[ContactIndex].mLocalPointA = ManifoldValue.mContacts[ContactIndex].mLocalPointA;
            OutPersistentCache.mContacts[ContactIndex].mLocalPointB = ManifoldValue.mContacts[ContactIndex].mLocalPointB;
            OutPersistentCache.mContacts[ContactIndex].mAccumulatedNormalImpulse = ManifoldValue.mContacts[ContactIndex].mAccumulatedNormalImpulse;
            OutPersistentCache.mContacts[ContactIndex].mAccumulatedTangentImpulse = ManifoldValue.mContacts[ContactIndex].mAccumulatedTangentImpulse;
            OutPersistentCache.mContacts[ContactIndex].mFeatureKey = ManifoldValue.mContacts[ContactIndex].mFeatureKey;
        }
    }

    void BeginDynamicCollisionPairCacheFrame(std::size_t PairCandidateCount) {
        ++DynamicPersistentPairCacheFrameIndex;
        std::size_t ExpectedCacheSize{ PairCandidateCount * 2U };
        if (ExpectedCacheSize > DynamicPersistentPairCache.bucket_count()) {
            DynamicPersistentPairCache.reserve(ExpectedCacheSize);
        }
    }

    void EndDynamicCollisionPairCacheFrame() {
        for (auto CacheIterator{ DynamicPersistentPairCache.begin() }; CacheIterator != DynamicPersistentPairCache.end();) {
            std::uint64_t FrameAge{ DynamicPersistentPairCacheFrameIndex > CacheIterator->second.mLastFrameIndex ? (DynamicPersistentPairCacheFrameIndex - CacheIterator->second.mLastFrameIndex) : 0U };
            if (FrameAge > DynamicPairCacheMaximumFrameAge) {
                CacheIterator = DynamicPersistentPairCache.erase(CacheIterator);
                continue;
            }

            ++CacheIterator;
        }
    }
}
