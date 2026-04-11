#pragma once

#include <cstdint>
#include <string>

#ifdef _DEBUG
#pragma comment(lib, "debug/DirectXTK12.lib")
#else
#pragma comment(lib, "release/DirectXTK12.lib")
#endif

class PhysicsActor {
public:
    enum class PhysicsActorFlags : std::uint32_t {
        None = 0U,
        Static = 1U << 0U,
        Kinematic = 1U << 1U,
        Trigger = 1U << 2U
    };

    enum class PhysicsActorType : std::uint32_t {
        Dynamic = 0U,
        Terrain = 1U
    };

public:
    PhysicsActor();
    virtual ~PhysicsActor();
    PhysicsActor(const PhysicsActor& Other);
    PhysicsActor& operator=(const PhysicsActor& Other);
    PhysicsActor(PhysicsActor&& Other) noexcept;
    PhysicsActor& operator=(PhysicsActor&& Other) noexcept;

    explicit PhysicsActor(std::string Name);

public:
    void SetName(std::string Name);
    const std::string& GetName() const;

    void SetIsActive(bool IsActive);
    bool GetIsActive() const;

    void SetMass(float Mass);
    float GetMass() const;

    void SetFlags(PhysicsActorFlags Flags);
    PhysicsActorFlags GetFlags() const;
    bool HasFlag(PhysicsActorFlags Flag) const;

    void SetActorType(PhysicsActorType ActorType);
    PhysicsActorType GetActorType() const;

private:
    std::string mName;
    bool mIsActive;
    float mMass;
    PhysicsActorFlags mFlags;
    PhysicsActorType mActorType;
};

PhysicsActor::PhysicsActorFlags operator|(PhysicsActor::PhysicsActorFlags Left, PhysicsActor::PhysicsActorFlags Right);
PhysicsActor::PhysicsActorFlags operator&(PhysicsActor::PhysicsActorFlags Left, PhysicsActor::PhysicsActorFlags Right);
