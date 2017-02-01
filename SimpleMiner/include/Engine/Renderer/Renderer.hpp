#pragma once

#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/AABB3.hpp"
#include "Engine/Math/Vertex_PCT.hpp"
#include "Engine/Math/EulerAngles.hpp"
#include "Engine/Math/IntVector3.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/BitmapFont.hpp"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <vector>

//-----------------------------------------------------
class Renderer;

//-----------------------------------------------------
extern Renderer* g_Renderer;

enum PrimitiveType
{
	PRIMITIVE_POINTS,
	PRIMITIVE_LINES,
	PRIMITIVE_TRIANGLES,
	PRIMITIVE_QUADS,
	NUM_PRIMITIVE_TYPES
};

class Renderer
{
public:
	//constructor
	static const Rgba s_White;
	static const Rgba s_Black;
	static const Rgba s_Blue;
	static const Rgba s_Gray;
	static const Rgba s_Red;
	static const Rgba s_Green;
	static const Rgba s_Yellow;
	static const AABB2 s_WholeTextureCoords;
	Renderer();
	~Renderer();

	//Render shape
	void DrawQuad(const Vector2& position0, const Vector2& position1, const Vector2& position2, const Vector2& position3, const Rgba& color);
	void DrawAABB(const AABB2& bounds, const Rgba& color);
	void DrawQuad3D(const Vector3& position0, Vector3& position1, const Vector3& position2, const Vector3& position3, const Rgba& color);
	void DrawTexturedQuad3D(const Vector3& position0, Vector3& position1, const Vector3& position2, const Vector3& position3, Texture* texture, AABB2 texCoords, const Rgba& tint);
	void DrawTexturedAABB(const AABB2& bounds, const Texture* texture, const AABB2& texCoords, const Rgba& tint);
	void DrawLinedBox(const AABB2& bounds, const Rgba& tint, const float& lineThickness);
	void RenderPolygon(const Vector2& center, float radius, int numSides, float degreesOffSet, Rgba color, bool isDotted = false);
	void RenderLine(const Vector2& startPoint, const Vector2& endPoint, Rgba color, float lineThickness);

	//Text related
	void DrawText2D(const Vector2& startBottomLeft, const std::string& asciiText, float cellHeight, const Rgba& tint = s_White, const BitmapFont* font = nullptr, const float& cellAspect = 1.f);

	//VBOs
	void GenerateOrUpdateVBO_ID(const std::vector<Vertex_PCT>& vertices, unsigned int& id);
	void DeleteVBO_ID(unsigned int& id);
	void DrawVBO_PCT(const unsigned int& vboID, const int& numVerts, const PrimitiveType& primType, const bool& hasTexture = true) const;
	void BindTexture(Texture* toBind) const;
	void DisableTexture() const;

	//3D related
	void DrawPoint(const Vector3& position, const float& dotSize = 1.f, const Rgba& color = s_White) const;
	void DrawLine(const Vector3& position, const Vector3& endPosition, const float& lineSize = 1.f, const Rgba& color = s_White) const;
	void DrawSquaresIn3D(const std::vector<Vector3>& squareBounds, const std::vector<AABB2>& indexBounds, const Texture* texture = nullptr, const Rgba tint = s_White) const;
	void DrawLinedRectangularPrism(const AABB3& bounds, const float& lineThickness = 1.f, const Rgba& color = s_White, const bool& drawCrossedLines = false) const;
	void RenderDebugPoints(const float& dotSize = 1.f);
	void DrawVertexArray(const std::vector<Vertex_PCT>& verts, const PrimitiveType& mode, const Texture* spriteSheetTexture = nullptr) const;
	void setLineSize(float size) const;
	void setPointSize(float size) const;
 	int GetAmountExpectedForPrimType(const PrimitiveType& drawMode) const;
// 	void renderABunchOfQuadsViaVertexArrays(const std::vector<Vertex_PCT>& quadVertices, const Texture* spriteSheetTexture = nullptr, bool hasTexture = false) const;
// 	void renderABunchOfPointsViaVertexArrays(const std::vector<Vertex_PCT>& pointVertices, const float& dotSize = 1.0f) const;
// 	void renderABunchOfLinesViaVertexArrays(const std::vector<Vertex_PCT>& lineVertices, const float& lineWidth = 1.0f, const bool& isDotted = false) const;
	void DepthTestingTrueFalse(const bool& setActive = true);
	void CullFaceTrueFalse(const bool& setActive = true);
	void turnCameraSoZUpXForward();
	void DrawSimpleCube(const Vector3& bottomSouthCorner, const Vector3& size, const Rgba& tint);
	void DrawSimpleCubeViaCenter(const Vector3& Center, const EulerAngles& orientation, const Vector3& size, const Rgba& tint, const bool& drawLines);
	void DrawLinedSimpleCube(const Vector3& bottomSouthCorner, const Vector3& size, const Rgba& tint);
	void DrawMultiColoredCube(const Vector3& bottomSouthCorner, const Vector3& size, const Rgba& bottomColor, const Rgba& topColor, const Rgba& frontColor,
		const Rgba& backColor, const Rgba& rightSide, const Rgba& leftSide);
	void DrawGridAlongXYPlane(const Vector2& size, const Rgba& color);
	void DrawSingleTexturedCube(const Vector3& bottomSouthCorner, const Vector3& size, const Texture* texture, const AABB2 texCoords, const Rgba& tint);
	void DrawGridLinesNEU(const float& length, const float& width, const float& height, const float& lineThickness = 1.f);

	//Window and screen
	void CreateOpenGLWindow(HINSTANCE& applicationInstanceHandle, const char*& APP_NAME, const int& OFFSET_FROM_WINDOWS_DESKTOP, const int& WINDOW_PHYSICAL_WIDTH,
		const int& WINDOW_PHYSICAL_HEIGHT, HWND& g_hWnd, HDC& g_displayDeviceContext, HGLRC g_openGLRenderingContext, const double& VIEW_LEFT, const double& VIEW_RIGHT, 
		const double& VIEW_TOP, const double& VIEW_BOTTOM, WNDCLASSEX& windowClassDescription);
	void SetOrtho(const Vector2& bottomLeft, const Vector2& topRight);
	void SetPerspective(const float& fovDegreesY, const float& aspect, const float& nearDist, const float& farDist);
	void clearScreen(const Rgba& color, const bool& clearDepth = false);
	void Translate(const Vector2& translateBy);
	void Translate(const Vector3& translateBy);
	void Rotate(const float& degrees);
	void Rotate(const float& degrees, const EulerAngles& axis);
	void Push();
	void Pop();
	void screenShake(const float& shakeMagnitude);
	void updateCameraPosition(const double& VIEW_LEFT, const double& VIEW_RIGHT, const double& VIEW_BOTTOM, const double& VIEW_TOP);
	void enableAlphaBlend();
	void disableAlphaBlend();

	//3D specific
	std::vector<Vertex_PCT> m_debugPoints;
};