#pragma once

#include "Mesh.h"

class MeshFactory final {
public:
    MeshFactory() = delete;
    ~MeshFactory() = delete;
    MeshFactory(const MeshFactory& Other) = delete;
    MeshFactory& operator=(const MeshFactory& Other) = delete;
    MeshFactory(MeshFactory&& Other) = delete;
    MeshFactory& operator=(MeshFactory&& Other) = delete;

public:
    static Mesh CreateCube(float Size);
    static Mesh CreateSphere(float Radius, unsigned int SectorCount, unsigned int StackCount);
    static Mesh CreateTriangularPyramid(float Size);
    static Mesh CreateSquarePyramid(float Size);
    static Mesh CreateGrid(float HalfExtent, unsigned int DivisionCount);
};
