#pragma once

#include "Mesh.h"

#include <string>

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
    static Mesh CreateCylinder(float Radius, float Height, unsigned int SegmentCount);
    static Mesh CreateTriangularPyramid(float Size);
    static Mesh CreateSquarePyramid(float Size);
    static Mesh CreateGrid(float HalfExtent, unsigned int DivisionCount);
    static Mesh CreateBoundingBox();
    static Mesh CreateTerrainFromHeightMapPng(const std::string& FilePath, float MaxHeight, float CellSpacing);
};
