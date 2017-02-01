#pragma once
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Core/Audio.hpp"

#ifndef BLOCKTEXTURECONTAINER_HPP
#define BLOCKTEXTURECONTAINER_HPP

struct BlockTextureContainer;

extern std::vector<BlockTextureContainer> g_blockInfoContainers;

enum BLOCK_TYPE
{
	AIR_BLOCK,
	WATER_BLOCK,
	GRASS_BLOCK,
	DIRT_BLOCK,
	STONE_BLOCK,
	COBBLE_STONE_BLOCK,
	SAND_BLOCK,
	LAVA_BLOCK,
	BRICK_BLOCK,
	GOLD_BRICK_BLOCK,
	GLOW_STONE_BLOCK,
	OBSIDIAN_BLOCK,
	SAND_STONE_BLOCK,
	HORROR_STONE_BLOCK,
	NUM_OF_BLOCK_TYPES
};

struct BlockTextureContainer
{
	BlockTextureContainer(const IntVector2& topCoords = IntVector2(0, 0), const IntVector2& sideCoords = IntVector2(0, 0), const IntVector2& bottomCoords = IntVector2(0, 0),
	const bool& opaq = false, const bool& lightSource = false, const int& lightRadius = 0, const bool& solid = false, const float& toughness = 0.f,
	const SoundID& walk = -1, const SoundID& placed = -1, const SoundID& breaking = -1, const SoundID& broken = -1);

	IntVector2 topTextureCoords;
	IntVector2 sideTextureCoords;
	IntVector2 bottomTextureCoords;
	bool opague;
	bool m_lightSource;
	bool m_solid;
	float m_toughness;
	int m_lightRadius;
	SoundID m_walk;
	SoundID m_placed;
	SoundID m_breaking;
	SoundID m_broken;
};

#endif