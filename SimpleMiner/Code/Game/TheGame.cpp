#include "Game/TheGame.hpp"

//--------------------------------------------
TheGame* g_TheGame = nullptr;



//--------------------------------------------
TheGame::TheGame()
:WindowWidth(16.f),
WindowHeight(9.f),
m_quitNow(false),
m_timeSinceStart(0.f),
totalNumOfBlocksToDraw(10),
m_rocketFuelMaxPositions(Vector2(0.1f, WindowHeight - (WindowHeight * 0.05f) - 0.1f), Vector2(4.1f, (WindowHeight ) - 0.1f))
{
	m_audio = new AudioSystem();
	amountToShiftLeft = (1.1f / 2.f) * totalNumOfBlocksToDraw;
	m_spriteSheet = new SpriteSheet("Data/Images/SimpleMinerAtlas.png", 16, 16);
	m_font = BitmapFont::CreateOrGetFont("SquirrelFixedFont");
	m_world = new World(m_spriteSheet, m_audio);
}
TheGame::~TheGame()
{
	delete m_audio;
	delete m_spriteSheet;
	delete m_world;
}

void TheGame::Update(const float& deltaSeconds, const bool& focus)
{
	m_world->Update(deltaSeconds, focus);
	m_timeSinceStart += deltaSeconds;
	if (g_input->isKeyDown(VK_ESCAPE) == true)
	{
		m_quitNow = true;
	}
}


void TheGame::Render() const
{
	g_Renderer->clearScreen(g_Renderer->s_Black, true);
	SetUp3DView();
	g_Renderer->CullFaceTrueFalse();
	g_Renderer->DepthTestingTrueFalse();

	g_Renderer->turnCameraSoZUpXForward();
	//now facing east with Z up.


	m_world->Render();
	//DrawDebugAxisLines();
	g_Renderer->RenderDebugPoints(20.f);
	SetUp2DView();
	if (m_world->IsPlayerInWater() == true)
	{
		std::vector<Vertex_PCT> quad;
		IntVector2 texCoordsIndex = g_blockInfoContainers[WATER_BLOCK].sideTextureCoords;
		AABB2 waterTexCoords = m_spriteSheet->GetTexCoordsForSpriteCoords(texCoordsIndex);
		quad.push_back(Vertex_PCT(Vector3(WindowWidth, 0.f),				g_Renderer->s_White, Vector2(waterTexCoords.maxs.x, waterTexCoords.maxs.y)));
		quad.push_back(Vertex_PCT(Vector3(WindowWidth, WindowHeight),		g_Renderer->s_White, Vector2(waterTexCoords.maxs.x, waterTexCoords.mins.y)));
		quad.push_back(Vertex_PCT(Vector3(0.f, WindowHeight),				g_Renderer->s_White, Vector2(waterTexCoords.mins.x, waterTexCoords.mins.y)));
		quad.push_back(Vertex_PCT(Vector3(0.f, 0.f),						g_Renderer->s_White, Vector2(waterTexCoords.mins.x, waterTexCoords.maxs.y)));
		g_Renderer->DrawVertexArray(quad, PRIMITIVE_QUADS, m_spriteSheet->getSpriteSheetTexture());
	}

	RenderJetPackFuelIndicators();

	RenderBlockSelectionGrid();
	RenderCrossHairs();
	Vector3 playerPosition = m_world->GetPlayerPosition();
	std::string DebugText = Stringf("Position x:%0.f, y: %0.f, z: %0.f", playerPosition.x, playerPosition.y, playerPosition.z);
	std::string gameModeText = "";
	GAME_MODE mode = m_world->GetGameMode();
	if (mode == NO_CLIP)
	{
		gameModeText += "NO CLIP";
	}
	else if (mode == FLYING)
	{
		gameModeText += "FLYING";
	}
	else if (mode == WALKING)
	{
		gameModeText += "WALKING";
	}
	std::string debugPointsCount = Stringf("DebugPoints: %i", g_Renderer->m_debugPoints.size());
	g_Renderer->DrawText2D(Vector2(0.f, 0.f), DebugText, 0.5f, g_Renderer->s_White, m_font);
	g_Renderer->DrawText2D(Vector2(12.f, 8.f), gameModeText, 0.5f, g_Renderer->s_White, m_font);
	g_Renderer->DrawText2D(Vector2(3.f, 3.f), m_world->toShowInDebug, 0.5f, g_Renderer->s_White, m_font);

}
std::vector<Vertex_PCT> TheGame::RenderCrossHairs() const
{
	Vector2 middle = Vector2(WindowWidth, WindowHeight) / 2.f;
	std::vector<Vertex_PCT> lines;
	lines.push_back(Vertex_PCT(Vector3(middle.x - 0.25f, middle.y)));
	lines.push_back(Vertex_PCT(Vector3(middle.x + 0.25f, middle.y)));
	lines.push_back(Vertex_PCT(Vector3(middle.x, middle.y - 0.25f)));
	lines.push_back(Vertex_PCT(Vector3(middle.x, middle.y + 0.25f)));
	return lines;
}

void TheGame::RenderBlockSelectionGrid() const
{
	Vector2 centerPosition = Vector2(WindowWidth, WindowHeight) / 2.f;
	centerPosition.y -= 3.f;
	Vector2 blockSize = Vector2(1.f, 1.f);
	Vector2 trueStartPosition = Vector2(centerPosition.x - amountToShiftLeft, centerPosition.y - 0.5f);
	AABB2 startPostion = AABB2(trueStartPosition.x, trueStartPosition.y, trueStartPosition.x + blockSize.x, trueStartPosition.y + blockSize.y);
	BLOCK_TYPE endBlock = (BLOCK_TYPE)((int)GRASS_BLOCK + totalNumOfBlocksToDraw - 1);
	Texture* spriteTex = m_spriteSheet->getSpriteSheetTexture();
	std::vector<Vertex_PCT> quads;
	std::vector<Vertex_PCT> lines;
	for (int i = GRASS_BLOCK; i <= endBlock; i++)
	{
		if (i != AIR_BLOCK && i != WATER_BLOCK)
		{
			BLOCK_TYPE type = (BLOCK_TYPE)i;
			IntVector2 spriteSheetCoords = m_world->GetBlockSideTexCoordsForIndex(type);
			AABB2 texCoords = m_spriteSheet->GetTexCoordsForSpriteCoords(spriteSheetCoords);
			quads.push_back(Vertex_PCT(Vector3(startPostion.maxs.x, startPostion.mins.y), g_Renderer->s_White, Vector2(texCoords.maxs.x, texCoords.maxs.y)));
			quads.push_back(Vertex_PCT(Vector3(startPostion.maxs.x, startPostion.maxs.y), g_Renderer->s_White, Vector2(texCoords.maxs.x, texCoords.mins.y)));
			quads.push_back(Vertex_PCT(Vector3(startPostion.mins.x, startPostion.maxs.y), g_Renderer->s_White, Vector2(texCoords.mins.x, texCoords.mins.y)));
			quads.push_back(Vertex_PCT(Vector3(startPostion.mins.x, startPostion.mins.y), g_Renderer->s_White, Vector2(texCoords.mins.x, texCoords.maxs.y)));

			//g_Renderer->DrawTexturedAABB(startPostion, spriteTex, m_spriteSheet->GetTexCoordsForSpriteCoords(spriteSheetCoords), g_Renderer->s_White);

			Rgba boxLineColor = g_Renderer->s_Black;
			if (m_world->GetBlockPlacementIndex() == i)
			{
				boxLineColor = g_Renderer->s_White;
			}
			//g_Renderer->DrawLinedBox(startPostion, boxLineColor, 3.f);

			lines.push_back(Vertex_PCT(Vector3(startPostion.mins.x, startPostion.mins.y), boxLineColor)); //bottom
			lines.push_back(Vertex_PCT(Vector3(startPostion.maxs.x, startPostion.mins.y), boxLineColor));

			lines.push_back(Vertex_PCT(Vector3(startPostion.maxs.x, startPostion.mins.y), boxLineColor)); //right
			lines.push_back(Vertex_PCT(Vector3(startPostion.maxs.x, startPostion.maxs.y), boxLineColor));

			lines.push_back(Vertex_PCT(Vector3(startPostion.maxs.x, startPostion.maxs.y), boxLineColor)); //top
			lines.push_back(Vertex_PCT(Vector3(startPostion.mins.x, startPostion.maxs.y), boxLineColor));

			lines.push_back(Vertex_PCT(Vector3(startPostion.mins.x, startPostion.maxs.y), boxLineColor)); //left
			lines.push_back(Vertex_PCT(Vector3(startPostion.mins.x, startPostion.mins.y), boxLineColor));

			startPostion.mins.x += blockSize.x + 0.1f;
			startPostion.maxs.x += blockSize.x + 0.1f;
		}
	}
	std::vector<Vertex_PCT> crossHair = RenderCrossHairs();
	for (size_t i = 0; i < crossHair.size(); i++)
	{
		lines.push_back(crossHair[i]);
	}
	g_Renderer->setLineSize(3.f);
	g_Renderer->DrawVertexArray(quads, PRIMITIVE_QUADS, spriteTex);
	g_Renderer->DrawVertexArray(lines, PRIMITIVE_LINES, nullptr);
}

void TheGame::RenderJetPackFuelIndicators() const
{
	float maxFuel = m_world->GetMaximumPlayerRocketFuel();
	if (maxFuel != 0.f)
	{
		std::vector<Vertex_PCT> fuelIndicatorVertices;


		fuelIndicatorVertices.push_back(Vertex_PCT(Vector3(m_rocketFuelMaxPositions.maxs.x, m_rocketFuelMaxPositions.mins.y, 0.f), Rgba(1.f, 0.f, 0.f, 1.f)));
		fuelIndicatorVertices.push_back(Vertex_PCT(Vector3(m_rocketFuelMaxPositions.maxs.x, m_rocketFuelMaxPositions.maxs.y, 0.f), Rgba(1.f, 0.f, 0.f, 1.f)));
		fuelIndicatorVertices.push_back(Vertex_PCT(Vector3(m_rocketFuelMaxPositions.mins.x, m_rocketFuelMaxPositions.maxs.y, 0.f), Rgba(1.f, 0.f, 0.f, 1.f)));
		fuelIndicatorVertices.push_back(Vertex_PCT(Vector3(m_rocketFuelMaxPositions.mins.x, m_rocketFuelMaxPositions.mins.y, 0.f), Rgba(1.f, 0.f, 0.f, 1.f)));


		float remainingFuel = m_world->GetRemainingPlayerRocketFuel();
		if (remainingFuel != 0.f)
		{
			float percentageFuelLeft = remainingFuel / maxFuel;
			float yDiff = m_rocketFuelMaxPositions.maxs.y - m_rocketFuelMaxPositions.mins.y;
			float yStart = m_rocketFuelMaxPositions.mins.y + (yDiff * 0.1f);
			yDiff -= (yDiff * 0.2f);
			float xDiff = m_rocketFuelMaxPositions.maxs.x - m_rocketFuelMaxPositions.mins.x;
			float xStart = m_rocketFuelMaxPositions.mins.x + (xDiff * 0.02f);
			xDiff -= (xDiff * 0.04f);
			xDiff *= percentageFuelLeft;

			fuelIndicatorVertices.push_back(Vertex_PCT(Vector3(xStart + xDiff, yStart), Rgba(0.f, 1.f, 0.f, 1.f)));
			fuelIndicatorVertices.push_back(Vertex_PCT(Vector3(xStart + xDiff, yStart + yDiff), Rgba(0.f, 1.f, 0.f, 1.f)));
			fuelIndicatorVertices.push_back(Vertex_PCT(Vector3(xStart, yStart + yDiff), Rgba(0.f, 1.f, 0.f, 1.f)));
			fuelIndicatorVertices.push_back(Vertex_PCT(Vector3(xStart, yStart), Rgba(0.f, 1.f, 0.f, 1.f)));
		}

		g_Renderer->DrawVertexArray(fuelIndicatorVertices, PRIMITIVE_QUADS, nullptr);
	}
}
void TheGame::SetUp3DView() const
{
	float aspect = (16.f / 9.f);
	//float fovDegreesHorizontal = 90.f;
	//float fovDegreesVertical = (fovDegreesHorizontal / aspect);
	float fovDegreesVertical = 70.f;
	float zNear = 0.1f;
	float zFar = 1000.f;

	g_Renderer->SetPerspective(fovDegreesVertical, aspect, zNear, zFar);
}
void TheGame::SetUp2DView() const
{
	g_Renderer->SetOrtho(Vector2(0.f, 0.f), Vector2(WindowWidth, WindowHeight)); 
}

void TheGame::DrawDebugAxisLines() const
{
	g_Renderer->DepthTestingTrueFalse(true);
	g_Renderer->DrawGridLinesNEU(2.f, 2.f, 2.f, 3.f);
	g_Renderer->DepthTestingTrueFalse(false);
	g_Renderer->DrawGridLinesNEU(2.f, 2.f, 2.f, 1.f);
}