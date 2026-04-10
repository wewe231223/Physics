#pragma once

#include <string>

class GameObject final {
public:
    GameObject();
    ~GameObject();
    GameObject(const GameObject& Other);
    GameObject& operator=(const GameObject& Other);
    GameObject(GameObject&& Other) noexcept;
    GameObject& operator=(GameObject&& Other) noexcept;

public:
    explicit GameObject(std::string Name);

    void SetName(std::string Name);
    const std::string& GetName() const;

    void SetIsActive(bool IsActive);
    bool GetIsActive() const;

private:
    std::string mName;
    bool mIsActive;
};
