#include "Game/BlockTextureContainer.hpp"
std::vector<BlockTextureContainer> g_blockInfoContainers = std::vector<BlockTextureContainer>();

BlockTextureContainer::BlockTextureContainer(const IntVector2& topCoords, const IntVector2& sideCoords, const IntVector2& bottomCoords,
	const bool& opaq, const bool& lightSource, const int& lightRadius, const bool& solid, const float& toughness,
	const SoundID& walk, const SoundID& placed, const SoundID& breaking, const SoundID& broken)
: topTextureCoords(topCoords), sideTextureCoords(sideCoords), bottomTextureCoords(bottomCoords), opague(opaq), m_lightSource(lightSource), m_lightRadius(lightRadius),
m_solid(solid), m_toughness(toughness), m_walk(walk), m_placed(placed), m_breaking(breaking), m_broken(broken)
{
}