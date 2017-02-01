#pragma once
#include "Engine/Core/Time.hpp"
#include "Engine/Math/ExtendedMath.hpp"
#include "Game/World.hpp"



#ifndef THEGAME_HPP
#define THEGAME_HPP
//-----------------------------------------------------
class TheGame;

//-----------------------------------------------------
extern TheGame* g_TheGame;


//-----------------------------------------------
class TheGame
{
public:
	TheGame();
	~TheGame();
	void Update(const float& deltaSeconds, const bool& focus);
	void Render() const;
	const bool isQuitting() const {return m_quitNow;};
protected:

	void SetUp3DView() const;
	void SetUp2DView() const;
	void RenderJetPackFuelIndicators() const;

	void DrawDebugAxisLines() const;
	std::vector<Vertex_PCT> RenderCrossHairs() const;
	void RenderBlockSelectionGrid() const;

	float WindowHeight;
	float WindowWidth;
	float m_timeSinceStart;
	int totalNumOfBlocksToDraw;
	float amountToShiftLeft;
	bool m_quitNow;
	AABB2 m_rocketFuelMaxPositions;
	IntVector2 m_dirtBottomIndex;
	IntVector2 m_dirtSideIndex;
	IntVector2 m_dirtTopIndex;
	SpriteSheet* m_spriteSheet;
	std::string m_exactuableFolderLocation;
	std::vector<BlockTextureContainer> m_blockInfoContainers;
	BitmapFont* m_font;
	AudioSystem* m_audio;
	World* m_world;
};

#endif //THEGAME_HPP