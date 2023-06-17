#pragma once

#include "UUID.h"
#include <glm/glm.hpp>

#include <string>

struct Vertex
{
	glm::vec3 m_Position;
	glm::vec3 m_Normal;
};

struct Mesh
{
	Vertex* m_Vertices = nullptr;
};

struct Material
{
	glm::vec3 m_Albedo{ 1.0f };
	float m_Roughness = 1.0f;
	float m_Metallic = 0.0f;

	float m_EmissionPower = 0.0f;

	Material() = default;
	Material(const Material&) = default;
	Material(const glm::vec3& albedo, float roughness, float metallic, float emissionPower)
		: m_Albedo(albedo), m_Roughness(roughness), m_Metallic(metallic), m_EmissionPower(emissionPower) {}

	glm::vec3 GetEmission() const { return m_Albedo * m_EmissionPower; }
};

#pragma region Components
struct IDComponent
{
	UUID m_UUID;

	IDComponent() = default;
	IDComponent(const IDComponent&) = default;
	IDComponent(const UUID& uuid)
		: m_UUID(uuid) {}
};

struct TagComponent
{
	std::string m_Tag;

	TagComponent() = default;
	TagComponent(const TagComponent&) = default;
	TagComponent(const std::string& tag)
		: m_Tag(tag) {}
};

struct SphereComponent
{
	glm::vec3 m_Position;
	float m_Radius;

	SphereComponent() = default;
	SphereComponent(const SphereComponent&) = default;
	SphereComponent(const glm::vec3& position, float radius)
		: m_Position(position), m_Radius(radius) {}
};

struct MeshComponent
{
	Mesh* m_Mesh;

	MeshComponent() = default;
	MeshComponent(const MeshComponent&) = default;
	MeshComponent(Mesh* mesh)
		: m_Mesh(mesh) {}
};

struct MaterialComponent
{
	Material* m_Material;

	MaterialComponent() = default;
	MaterialComponent(const MaterialComponent&) = default;
	MaterialComponent(Material* mat)
		: m_Material(mat) {}
};
#pragma endregion