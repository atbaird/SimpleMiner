#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Math/ExtendedMath.hpp"

#include "Engine/Renderer/OpenGLExtensions.hpp"

Renderer* g_Renderer = nullptr;

//default colors
const Rgba Renderer::s_White = Rgba(1.f, 1.f, 1.f, 1.f);
const Rgba Renderer::s_Black = Rgba(0.f, 0.f, 0.f, 1.f);
const Rgba Renderer::s_Blue = Rgba(0.f, 0.f, 1.f, 1.f);
const Rgba Renderer::s_Gray = Rgba(0.5f, 0.5f, 0.5f, 1.f);
const Rgba Renderer::s_Red = Rgba(1.f, 0.f, 0.f, 1.f);
const Rgba Renderer::s_Green = Rgba(0.f, 1.f, 0.f, 1.f);
const Rgba Renderer::s_Yellow = Rgba(1.f,1.f,0.f,1.f);
const AABB2 Renderer::s_WholeTextureCoords = AABB2(Vector2(0.f,0.f), Vector2(1.f,1.f));

Renderer::Renderer()
{
	glGenBuffers = (PFNGLGENBUFFERSPROC)wglGetProcAddress("glGenBuffers");
	glBindBuffer = (PFNGLBINDBUFFERPROC)wglGetProcAddress("glBindBuffer");
	glBufferData = (PFNGLBUFFERDATAPROC)wglGetProcAddress("glBufferData");
	glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)wglGetProcAddress("glDeleteBuffers");
	glGenerateMipmap = (PFNGLGENERATEMIPMAPPROC)wglGetProcAddress("glGenerateMipmap");

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.1f);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_LINE_SMOOTH);
	glLineWidth(1.5f);
}
Renderer::~Renderer()
{
	/*
	for (std::map<std::string, std::map<std::string, Texture*>>::iterator it = s_textureRegistry.begin(); it != s_textureRegistry.end(); it++) {
		if (s_textureRegistry[it] != nullptr)
		{
			delete it->second;
			s_textureRegistry[it] = nullptr;
		}
	}
	*/
}



void Renderer::DrawQuad3D(const Vector3& position0, Vector3& position1, const Vector3& position2, const Vector3& position3, const Rgba& color)
{
	glDisable(GL_TEXTURE_2D);
	std::vector<Vertex_PCT> quad;
	{
		quad.push_back(Vertex_PCT(Vector3(position0.x, position0.y, position0.z), color));
		quad.push_back(Vertex_PCT(Vector3(position1.x, position1.y, position1.z), color));
		quad.push_back(Vertex_PCT(Vector3(position2.x, position2.y, position2.z), color));
		quad.push_back(Vertex_PCT(Vector3(position3.x, position3.y, position3.z), color));
	}
	DrawVertexArray(quad, PRIMITIVE_QUADS, nullptr);
}

void Renderer::DrawTexturedQuad3D(const Vector3& position0, Vector3& position1, const Vector3& position2, const Vector3& position3, Texture* texture, AABB2 texCoords, const Rgba& tint)
{
	std::vector<Vertex_PCT> quad;
	{
		quad.push_back(Vertex_PCT(Vector3(position0.x, position0.y, position0.z), tint, Vector2(texCoords.mins.x, texCoords.mins.y)));
		quad.push_back(Vertex_PCT(Vector3(position1.x, position1.y, position1.z), tint, Vector2(texCoords.mins.x, texCoords.maxs.y)));
		quad.push_back(Vertex_PCT(Vector3(position2.x, position2.y, position2.z), tint, Vector2(texCoords.maxs.x, texCoords.maxs.y)));
		quad.push_back(Vertex_PCT(Vector3(position3.x, position3.y, position3.z), tint, Vector2(texCoords.maxs.x, texCoords.mins.y)));
	}
	DrawVertexArray(quad, PRIMITIVE_QUADS, texture);
}

void Renderer::DrawAABB(const AABB2& bounds, const Rgba& color)
{
	std::vector<Vertex_PCT> quad;
	//draws a square.
	{
		quad.push_back(Vertex_PCT(Vector3(bounds.mins.x, bounds.mins.y), color));
		quad.push_back(Vertex_PCT(Vector3(bounds.maxs.x, bounds.mins.y), color));
		quad.push_back(Vertex_PCT(Vector3(bounds.maxs.x, bounds.maxs.y), color));
		quad.push_back(Vertex_PCT(Vector3(bounds.mins.x, bounds.maxs.y), color));
	}
	DrawVertexArray(quad, PRIMITIVE_QUADS, nullptr);
}

void Renderer::DrawTexturedAABB(const AABB2& bounds, const Texture* texture, const AABB2& texCoord, const Rgba& tint)
{
	//draws a textured AABB, aka square,
	std::vector<Vertex_PCT> quad;
	{
		quad.push_back(Vertex_PCT(Vector3(bounds.mins.x, bounds.mins.y), tint, Vector2(texCoord.mins.x, texCoord.maxs.y)));
		quad.push_back(Vertex_PCT(Vector3(bounds.maxs.x, bounds.mins.y), tint, Vector2(texCoord.maxs.x, texCoord.maxs.y)));
		quad.push_back(Vertex_PCT(Vector3(bounds.maxs.x, bounds.maxs.y), tint, Vector2(texCoord.maxs.x, texCoord.mins.y)));
		quad.push_back(Vertex_PCT(Vector3(bounds.mins.x, bounds.maxs.y), tint, Vector2(texCoord.mins.x, texCoord.mins.y)));
	}
	DrawVertexArray(quad, PRIMITIVE_QUADS, texture);
	glDisable(GL_TEXTURE_2D);
}

void Renderer::DrawLinedBox(const AABB2& bounds, const Rgba& tint, const float& lineThickness)
{
	std::vector<Vertex_PCT> lines;
	{
		lines.push_back(Vertex_PCT(Vector3(bounds.mins.x, bounds.mins.y), tint));
		lines.push_back(Vertex_PCT(Vector3(bounds.mins.x, bounds.maxs.y), tint));

		lines.push_back(Vertex_PCT(Vector3(bounds.mins.x, bounds.maxs.y), tint));
		lines.push_back(Vertex_PCT(Vector3(bounds.maxs.x, bounds.maxs.y), tint));

		lines.push_back(Vertex_PCT(Vector3(bounds.maxs.x, bounds.maxs.y), tint));
		lines.push_back(Vertex_PCT(Vector3(bounds.maxs.x, bounds.mins.y), tint));

		lines.push_back(Vertex_PCT(Vector3(bounds.maxs.x, bounds.mins.y), tint));
		lines.push_back(Vertex_PCT(Vector3(bounds.mins.x, bounds.mins.y), tint));
	}
	setLineSize(lineThickness);
	DrawVertexArray(lines, PRIMITIVE_LINES, nullptr);
}

void Renderer::DrawQuad(const Vector2& position0, const Vector2& position1, const Vector2& position2, const Vector2& position3, const Rgba& color)
{
	//Literally just draws a square of color
	std::vector<Vertex_PCT> quad;
	{
		quad.push_back(Vertex_PCT(Vector3(position0.x, position0.y), color));
		quad.push_back(Vertex_PCT(Vector3(position1.x, position1.y), color));
		quad.push_back(Vertex_PCT(Vector3(position2.x, position2.y), color));
		quad.push_back(Vertex_PCT(Vector3(position3.x, position3.y), color));
	}
	DrawVertexArray(quad, PRIMITIVE_QUADS, nullptr);
}

void Renderer::RenderPolygon(const Vector2& center, float radius, int numSides, float degreesOffSet, Rgba color, bool isDotted )
{
	isDotted = isDotted;
	//renders a polygon shape of the given number of sides
	//const float radiansTotal = 2.f * ExtendMath->pi;
	const float radiansTotal = (float)(2.f * PI);
	const float radiansPerSide = radiansTotal / numSides;
	glColor4f(color.Red, color.Green, color.Blue, color.Alpha);
	std::vector<Vertex_PCT> lines;
	{
		for (float radians = 0.f; radians < radiansTotal; radians += radiansPerSide) {
			//float rotatedRadians = (0.5f * pi) + radians;
			float rotatedRadians = radians + DegreesToRadians(degreesOffSet);
			//float rotatedRadians = radians + DegreesToRadians(degreesOffSet);
			float x = (cos(rotatedRadians) * radius) + center.x;
			float y = (sin(rotatedRadians) * radius) + center.y;
			lines.push_back(Vertex_PCT(Vector3(x,y, 0.f), color));
		}
	}
	setLineSize(1.f);
	DrawVertexArray(lines, PRIMITIVE_LINES, nullptr);
}

void Renderer::RenderLine(const Vector2& startPoint, const Vector2& endPoint, Rgba color, float lineThickness)
{
	//draws a straight line from the startPoint to the endPoint
	std::vector<Vertex_PCT> line;
	line.push_back(Vertex_PCT(Vector3(startPoint.x, startPoint.y, 0.f), color));
	line.push_back(Vertex_PCT(Vector3(endPoint.x, endPoint.y, 0.f), color));
	setLineSize(lineThickness);
	DrawVertexArray(line, PRIMITIVE_LINES, nullptr);
}



//Text related
void Renderer::DrawText2D(const Vector2& startBottomLeft, const std::string& asciiText, float cellHeight, const Rgba& tint, const BitmapFont* font, const float& cellAspect)
{
	if (!font)
		return;
	CullFaceTrueFalse(false);
	int numTextChars = (int)asciiText.size();
	Vector2 cellSize = Vector2(cellAspect * cellHeight, cellHeight);
	Vector2 shiftRight = Vector2(cellSize.x, 0.f);
	
	const Texture* texture = font->GetTexture();

	Vector2 currentPosition = startBottomLeft;
	for (int i = 0; i < numTextChars; i++)
	{
		//Vector2 positionMaxes = Vector2(currentPosition.x, currentPosition.y + cellSize.y);
		int val = (int)asciiText[i];
		AABB2 fontCoords = font->GetTexCoordsForGlyph(val);
		DrawTexturedAABB(AABB2(currentPosition, Vector2(currentPosition.x + cellSize.x, currentPosition.y + cellSize.y)), texture, fontCoords, tint);
		currentPosition += shiftRight;
	}
}

GLenum GetGLenumForPrimType(const PrimitiveType& drawMode)
{
	GLenum toReturn;
	if (drawMode == PRIMITIVE_POINTS)
	{
		toReturn = GL_POINTS;
	}
	else if (drawMode == PRIMITIVE_LINES)
	{
		toReturn = GL_LINES;
	}
	else if (drawMode == PRIMITIVE_TRIANGLES)
	{
		toReturn = GL_TRIANGLES;
	}
	else if (drawMode == PRIMITIVE_QUADS)
	{
		toReturn = GL_QUADS;
	}
	return toReturn;
}
//VBOs

void Renderer::GenerateOrUpdateVBO_ID(const std::vector<Vertex_PCT>& vertices, unsigned int& id)
{
	if (id == 0)
	{
		glGenBuffers(1, &(GLuint)id);
	}
	size_t vertexArrayNumBytes = sizeof(Vertex_PCT) * vertices.size();
	glBindBuffer(GL_ARRAY_BUFFER, (GLuint)id);
	glBufferData(GL_ARRAY_BUFFER, vertexArrayNumBytes, vertices.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
void Renderer::DeleteVBO_ID(unsigned int& id)
{
	if (id != 0)
	{
		glDeleteBuffers(1, &(GLuint)id);
		id = 0;
	}
}

void Renderer::DrawVBO_PCT(const unsigned int& vboID, const int& numVerts, const PrimitiveType& primType, const bool& hasTexture) const
{
	glBindBuffer(GL_ARRAY_BUFFER, (GLuint)vboID);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	if (hasTexture == true)
	{
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	}

	glVertexPointer(3, GL_FLOAT, sizeof(Vertex_PCT), (const GLvoid*) offsetof(Vertex_PCT, m_pos));
	glColorPointer(4, GL_FLOAT, sizeof(Vertex_PCT), (const GLvoid*)offsetof(Vertex_PCT, m_color));
	if (hasTexture == true)
	{
		glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex_PCT), (const GLvoid*)offsetof(Vertex_PCT, m_texCoords));
	}

	GLenum type = GetGLenumForPrimType(primType);
	glDrawArrays(type, 0, (GLuint)numVerts);


	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

}

void Renderer::BindTexture(Texture* toBind) const
{
	if (toBind != nullptr)
	{
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, toBind->m_openglTextureID);
	}
}

void Renderer::DisableTexture() const
{
	glDisable(GL_TEXTURE_2D);
}

//3D related

void Renderer::DrawPoint(const Vector3& position, const float& dotSize, const Rgba& color) const
{
	std::vector<Vertex_PCT> point;
	point.push_back(Vertex_PCT(position, color));
	setPointSize(dotSize);
	DrawVertexArray(point, PRIMITIVE_POINTS, nullptr);
}

void Renderer::DrawLine(const Vector3& position, const Vector3& endPosition, const float& lineSize, const Rgba& color) const
{
	std::vector<Vertex_PCT> line;
	line.push_back(Vertex_PCT(position, color));
	line.push_back(Vertex_PCT(endPosition, color));
	setLineSize(lineSize);
	DrawVertexArray(line, PRIMITIVE_LINES, nullptr);
}

void Renderer::DrawSquaresIn3D(const std::vector<Vector3>& squareBounds, const std::vector<AABB2>& indexBounds, const Texture* texture, const Rgba tint) const
{
	//draws a textured AABB, aka square,
	if (squareBounds.size() != 0 && squareBounds.size() % 4 == 0)
	{
		std::vector<Vertex_PCT> quads;
		{
			int index = 0;
			for (size_t i = 0; i < (size_t)(squareBounds.size() / 4); i++)
			{
				quads.push_back(Vertex_PCT(squareBounds[index], tint));
				quads.push_back(Vertex_PCT(squareBounds[index + 1], tint));
				quads.push_back(Vertex_PCT(squareBounds[index + 2], tint));
				quads.push_back(Vertex_PCT(squareBounds[index + 3], tint));
				if (texture != nullptr)
				{
					AABB2 texCoord = indexBounds[i];
					quads[index].m_texCoords = Vector2(texCoord.mins.x, texCoord.maxs.y);
					quads[index + 1].m_texCoords = Vector2(texCoord.maxs.x, texCoord.maxs.y);
					quads[index + 2].m_texCoords = Vector2(texCoord.maxs.x, texCoord.mins.y);
					quads[index + 3].m_texCoords = Vector2(texCoord.mins.x, texCoord.mins.y);
				}
				index += 4;
			}
		}
		DrawVertexArray(quads, PRIMITIVE_QUADS, texture);
	}
}
void Renderer::DrawVertexArray(const std::vector<Vertex_PCT>& verts, const PrimitiveType& mode, const Texture* spriteSheetTexture) const
{
	int numPointsExpected = GetAmountExpectedForPrimType(mode);
	GLenum type = GetGLenumForPrimType(mode);

	if (verts.size() > 0 && numPointsExpected != 0 && verts.size() % numPointsExpected == 0)
	{
		bool hasTexture = false;
		if (spriteSheetTexture != nullptr)
		{
			hasTexture = true;
		}
		if (hasTexture == true)
		{
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, spriteSheetTexture->m_openglTextureID);
		}
		else
		{
			glDisable(GL_TEXTURE_2D);
		}

		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_COLOR_ARRAY);
		if (hasTexture == true)
		{
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		}
		glVertexPointer(3, GL_FLOAT, sizeof(Vertex_PCT), &verts[0].m_pos);
		glColorPointer(4, GL_FLOAT, sizeof(Vertex_PCT), &verts[0].m_color);
		if (hasTexture == true)
		{
			glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex_PCT), &verts[0].m_texCoords);
		}

		glDrawArrays(type, 0, verts.size()); //Dies here.

		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisable(GL_TEXTURE_2D);
	}
}
//VBO
//glPushMatrix();
//glTranslatef(m_worldMins.x, m_worldMins.y, m_worldMins.z);
//glBindBuffer(GL_ARRAY_, m_vboiID);
//glEnableClientState(GL_VERTEX_ARRAY);
//glEnableClientState(GL_COLOR_ARRAY);
//glEnableClientState(GL_TEXTURE_COORD_ARRAY);
//glVertexPointer(3,GL_FLOAT, sizeof(Vertex3D_PCT), (const GLvoid*) offsetof(Vertex3D_PCT), m_position);
//glColorPointer(4, GL_FLOAT, sizeof(vertex3D_PCT), (const GLvoid*) offsetof(Vertex3D_PCT), m_color);
//glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex3D_PCT), (const GLvoid*) offsetof(Vertex3D_PCT), m_texCoords);
//glDrawArrays(GL_QUADS, 0, vertexes.size());
//glDisableClientState(GL_VERTEX_ARRAY);
//glDisableClientState(GL_COLOR_ARRAY);
//glDisableClientState(GL_TEXTURE_COORD_ARRAY);

//DrawAABB2(... texture =nullptr)
//&(((Type*)0)Member)

void Renderer::setLineSize(float size) const
{
	glLineWidth(size);
}
void Renderer::setPointSize(float size) const
{
	glPointSize(size);
}
int Renderer::GetAmountExpectedForPrimType(const PrimitiveType& drawMode) const
{
	int amountOfPointsExpected = 0;
	if (drawMode == PRIMITIVE_POINTS)
	{
		amountOfPointsExpected = 1;
	}
	else if (drawMode == PRIMITIVE_LINES)
	{
		amountOfPointsExpected = 2;
	}
	else if (drawMode == PRIMITIVE_TRIANGLES)
	{
		amountOfPointsExpected = 3;
	}
	else if (drawMode == PRIMITIVE_QUADS)
	{
		amountOfPointsExpected = 4;
	}
	return amountOfPointsExpected;
}
void Renderer::DrawLinedRectangularPrism(const AABB3& bounds, const float& lineThickness, const Rgba& color, const bool& drawCrossedLines) const
{
	std::vector<Vertex_PCT> RectPrism;
	{
		//Bottom
		RectPrism.push_back(Vertex_PCT(Vector3(bounds.mins.x, bounds.mins.y, bounds.mins.z), Rgba(color.Red, color.Green, color.Blue, color.Alpha)));
		RectPrism.push_back(Vertex_PCT(Vector3(bounds.mins.x, bounds.maxs.y, bounds.mins.z), Rgba(color.Red, color.Green, color.Blue, color.Alpha)));


		RectPrism.push_back(Vertex_PCT(Vector3(bounds.mins.x, bounds.maxs.y, bounds.mins.z), Rgba(color.Red, color.Green, color.Blue, color.Alpha)));
		RectPrism.push_back(Vertex_PCT(Vector3(bounds.maxs.x, bounds.maxs.y, bounds.mins.z), Rgba(color.Red, color.Green, color.Blue, color.Alpha)));

		RectPrism.push_back(Vertex_PCT(Vector3(bounds.maxs.x, bounds.maxs.y, bounds.mins.z), Rgba(color.Red, color.Green, color.Blue, color.Alpha)));
		RectPrism.push_back(Vertex_PCT(Vector3(bounds.maxs.x, bounds.mins.y, bounds.mins.z), Rgba(color.Red, color.Green, color.Blue, color.Alpha)));

		RectPrism.push_back(Vertex_PCT(Vector3(bounds.maxs.x, bounds.mins.y, bounds.mins.z), Rgba(color.Red, color.Green, color.Blue, color.Alpha)));
		RectPrism.push_back(Vertex_PCT(Vector3(bounds.mins.x, bounds.mins.y, bounds.mins.z), Rgba(color.Red, color.Green, color.Blue, color.Alpha)));


		//Top
		RectPrism.push_back(Vertex_PCT(Vector3(bounds.mins.x, bounds.mins.y, bounds.maxs.z), Rgba(color.Red, color.Green, color.Blue, color.Alpha)));
		RectPrism.push_back(Vertex_PCT(Vector3(bounds.mins.x, bounds.maxs.y, bounds.maxs.z), Rgba(color.Red, color.Green, color.Blue, color.Alpha)));

		RectPrism.push_back(Vertex_PCT(Vector3(bounds.mins.x, bounds.maxs.y, bounds.maxs.z), Rgba(color.Red, color.Green, color.Blue, color.Alpha)));
		RectPrism.push_back(Vertex_PCT(Vector3(bounds.maxs.x, bounds.maxs.y, bounds.maxs.z), Rgba(color.Red, color.Green, color.Blue, color.Alpha)));

		RectPrism.push_back(Vertex_PCT(Vector3(bounds.maxs.x, bounds.maxs.y, bounds.maxs.z), Rgba(color.Red, color.Green, color.Blue, color.Alpha)));
		RectPrism.push_back(Vertex_PCT(Vector3(bounds.maxs.x, bounds.mins.y, bounds.maxs.z), Rgba(color.Red, color.Green, color.Blue, color.Alpha)));

		RectPrism.push_back(Vertex_PCT(Vector3(bounds.maxs.x, bounds.mins.y, bounds.maxs.z), Rgba(color.Red, color.Green, color.Blue, color.Alpha)));
		RectPrism.push_back(Vertex_PCT(Vector3(bounds.mins.x, bounds.mins.y, bounds.maxs.z), Rgba(color.Red, color.Green, color.Blue, color.Alpha)));

		//Sides
		RectPrism.push_back(Vertex_PCT(Vector3(bounds.mins.x, bounds.mins.y, bounds.mins.z), Rgba(color.Red, color.Green, color.Blue, color.Alpha)));
		RectPrism.push_back(Vertex_PCT(Vector3(bounds.mins.x, bounds.mins.y, bounds.maxs.z), Rgba(color.Red, color.Green, color.Blue, color.Alpha)));

		RectPrism.push_back(Vertex_PCT(Vector3(bounds.mins.x, bounds.maxs.y, bounds.mins.z), Rgba(color.Red, color.Green, color.Blue, color.Alpha)));
		RectPrism.push_back(Vertex_PCT(Vector3(bounds.mins.x, bounds.maxs.y, bounds.maxs.z), Rgba(color.Red, color.Green, color.Blue, color.Alpha)));

		RectPrism.push_back(Vertex_PCT(Vector3(bounds.maxs.x, bounds.mins.y, bounds.mins.z), Rgba(color.Red, color.Green, color.Blue, color.Alpha)));
		RectPrism.push_back(Vertex_PCT(Vector3(bounds.maxs.x, bounds.mins.y, bounds.maxs.z), Rgba(color.Red, color.Green, color.Blue, color.Alpha)));

		RectPrism.push_back(Vertex_PCT(Vector3(bounds.maxs.x, bounds.maxs.y, bounds.mins.z), Rgba(color.Red, color.Green, color.Blue, color.Alpha)));
		RectPrism.push_back(Vertex_PCT(Vector3(bounds.maxs.x, bounds.maxs.y, bounds.maxs.z), Rgba(color.Red, color.Green, color.Blue, color.Alpha)));

		if (drawCrossedLines == true)
		{
			//Crossed Lines
			RectPrism.push_back(Vertex_PCT(Vector3(bounds.mins.x, bounds.mins.y, bounds.mins.z), Rgba(color.Red, color.Green, color.Blue, color.Alpha)));
			RectPrism.push_back(Vertex_PCT(Vector3(bounds.mins.x, bounds.maxs.y, bounds.maxs.z), Rgba(color.Red, color.Green, color.Blue, color.Alpha)));

			RectPrism.push_back(Vertex_PCT(Vector3(bounds.mins.x, bounds.maxs.y, bounds.mins.z), Rgba(color.Red, color.Green, color.Blue, color.Alpha)));
			RectPrism.push_back(Vertex_PCT(Vector3(bounds.mins.x, bounds.mins.y, bounds.maxs.z), Rgba(color.Red, color.Green, color.Blue, color.Alpha)));

			RectPrism.push_back(Vertex_PCT(Vector3(bounds.maxs.x, bounds.mins.y, bounds.mins.z), Rgba(color.Red, color.Green, color.Blue, color.Alpha)));
			RectPrism.push_back(Vertex_PCT(Vector3(bounds.mins.x, bounds.mins.y, bounds.maxs.z), Rgba(color.Red, color.Green, color.Blue, color.Alpha)));

			RectPrism.push_back(Vertex_PCT(Vector3(bounds.mins.x, bounds.mins.y, bounds.mins.z), Rgba(color.Red, color.Green, color.Blue, color.Alpha)));
			RectPrism.push_back(Vertex_PCT(Vector3(bounds.maxs.x, bounds.mins.y, bounds.maxs.z), Rgba(color.Red, color.Green, color.Blue, color.Alpha)));

			RectPrism.push_back(Vertex_PCT(Vector3(bounds.mins.x, bounds.mins.y, bounds.mins.z), Rgba(color.Red, color.Green, color.Blue, color.Alpha)));
			RectPrism.push_back(Vertex_PCT(Vector3(bounds.maxs.x, bounds.maxs.y, bounds.mins.z), Rgba(color.Red, color.Green, color.Blue, color.Alpha)));

			RectPrism.push_back(Vertex_PCT(Vector3(bounds.mins.x, bounds.maxs.y, bounds.mins.z), Rgba(color.Red, color.Green, color.Blue, color.Alpha)));
			RectPrism.push_back(Vertex_PCT(Vector3(bounds.mins.x, bounds.maxs.y, bounds.mins.z), Rgba(color.Red, color.Green, color.Blue, color.Alpha)));

			RectPrism.push_back(Vertex_PCT(Vector3(bounds.mins.x, bounds.mins.y, bounds.maxs.z), Rgba(color.Red, color.Green, color.Blue, color.Alpha)));
			RectPrism.push_back(Vertex_PCT(Vector3(bounds.maxs.x, bounds.maxs.y, bounds.maxs.z), Rgba(color.Red, color.Green, color.Blue, color.Alpha)));

			RectPrism.push_back(Vertex_PCT(Vector3(bounds.mins.x, bounds.maxs.y, bounds.maxs.z), Rgba(color.Red, color.Green, color.Blue, color.Alpha)));
			RectPrism.push_back(Vertex_PCT(Vector3(bounds.mins.x, bounds.maxs.y, bounds.maxs.z), Rgba(color.Red, color.Green, color.Blue, color.Alpha)));
		}
		setLineSize(lineThickness);
		DrawVertexArray(RectPrism, PRIMITIVE_LINES, nullptr);
	}
	
}
void Renderer::RenderDebugPoints(const float& dotSize)
{
	DepthTestingTrueFalse(false);
	setPointSize(dotSize);
	DrawVertexArray(m_debugPoints, PRIMITIVE_POINTS, nullptr);
}
void Renderer::DepthTestingTrueFalse(const bool& setActive)
{
	if (setActive == true)
	{
		glEnable(GL_DEPTH_TEST);
	}
	else
	{
		glDisable(GL_DEPTH_TEST);
	}
}
void Renderer::CullFaceTrueFalse(const bool& setActive)
{
	if (setActive == true)
	{
		glEnable(GL_CULL_FACE);
	}
	else
	{
		glDisable(GL_CULL_FACE);
	}
}

void Renderer::turnCameraSoZUpXForward()
{
	//put +Z-up
	glRotatef(-90.f, 1.f, 0.f, 0.f);
	//put +X forward.
	glRotatef(90.f, 0.f, 0.f, 1.f);
}
void Renderer::DrawSimpleCube(const Vector3& bottomSouthCorner, const Vector3& size, const Rgba& tint)
{
	DrawMultiColoredCube(bottomSouthCorner, size, tint, tint, tint, tint, tint, tint);
}

void Renderer::DrawSimpleCubeViaCenter(const Vector3& Center, const EulerAngles& orientation, const Vector3& size, const Rgba& tint, const bool& drawLinedVersion)
{
	Push();
	Translate(Center);
	Rotate(-orientation.m_xRoll, EulerAngles(1.f, 0.f, 0.f));
	Rotate(-orientation.m_yPitch, EulerAngles(0.f, 1.f, 0.f));
	Rotate(-orientation.m_zYaw, EulerAngles(0.f, 0.f, 1.f));
	Vector3 halfSize = size / 2;
	Vector3 bottomLeft = Vector3(0.f,0.f,0.f) - halfSize;
	DrawMultiColoredCube(bottomLeft, size, tint, tint, tint, tint, tint, tint);
	if (drawLinedVersion == true)
	{
		DrawLinedSimpleCube(bottomLeft, size, Rgba(1.f,0.f,1.f,1.f));
	}

	Pop();
}

void Renderer::DrawLinedSimpleCube(const Vector3& bottomSouthCorner, const Vector3& size, const Rgba& tint)
{
	float xAndLength = bottomSouthCorner.x + size.x;
	float yAndWidth = bottomSouthCorner.y + size.y;
	float zAndHeight = bottomSouthCorner.z + size.z;
	std::vector<Vertex_PCT> lines;
	{
		//bottom
		lines.push_back(Vertex_PCT(Vector3(bottomSouthCorner.x, bottomSouthCorner.y, bottomSouthCorner.z), tint));
		lines.push_back(Vertex_PCT(Vector3(bottomSouthCorner.x, yAndWidth, bottomSouthCorner.z), tint));

		lines.push_back(Vertex_PCT(Vector3(bottomSouthCorner.x, yAndWidth, bottomSouthCorner.z), tint));
		lines.push_back(Vertex_PCT(Vector3(xAndLength, yAndWidth, bottomSouthCorner.z), tint));

		lines.push_back(Vertex_PCT(Vector3(xAndLength, yAndWidth, bottomSouthCorner.z), tint));
		lines.push_back(Vertex_PCT(Vector3(xAndLength, bottomSouthCorner.z, bottomSouthCorner.z), tint));

		lines.push_back(Vertex_PCT(Vector3(xAndLength, bottomSouthCorner.z, bottomSouthCorner.z), tint));
		lines.push_back(Vertex_PCT(Vector3(bottomSouthCorner.x, bottomSouthCorner.y, bottomSouthCorner.z), tint));

		//side
		lines.push_back(Vertex_PCT(Vector3(bottomSouthCorner.x, bottomSouthCorner.y, bottomSouthCorner.z), tint));
		lines.push_back(Vertex_PCT(Vector3(bottomSouthCorner.x, bottomSouthCorner.y, zAndHeight), tint));

		lines.push_back(Vertex_PCT(Vector3(bottomSouthCorner.x, yAndWidth, bottomSouthCorner.z), tint));
		lines.push_back(Vertex_PCT(Vector3(bottomSouthCorner.x, yAndWidth, zAndHeight), tint));

		lines.push_back(Vertex_PCT(Vector3(xAndLength, yAndWidth, bottomSouthCorner.z), tint));
		lines.push_back(Vertex_PCT(Vector3(xAndLength, yAndWidth, zAndHeight), tint));

		lines.push_back(Vertex_PCT(Vector3(xAndLength, bottomSouthCorner.y, bottomSouthCorner.z), tint));
		lines.push_back(Vertex_PCT(Vector3(xAndLength, bottomSouthCorner.y, zAndHeight), tint));

		//top
		lines.push_back(Vertex_PCT(Vector3(bottomSouthCorner.x, bottomSouthCorner.y, zAndHeight), tint));
		lines.push_back(Vertex_PCT(Vector3(bottomSouthCorner.x, yAndWidth, zAndHeight), tint));

		lines.push_back(Vertex_PCT(Vector3(bottomSouthCorner.x, yAndWidth, zAndHeight), tint));
		lines.push_back(Vertex_PCT(Vector3(xAndLength, yAndWidth, zAndHeight), tint));

		lines.push_back(Vertex_PCT(Vector3(xAndLength, yAndWidth, zAndHeight), tint));
		lines.push_back(Vertex_PCT(Vector3(xAndLength, bottomSouthCorner.z, zAndHeight), tint));

		lines.push_back(Vertex_PCT(Vector3(xAndLength, bottomSouthCorner.z, zAndHeight), tint));
		lines.push_back(Vertex_PCT(Vector3(bottomSouthCorner.x, bottomSouthCorner.y, zAndHeight), tint));
	}
	setLineSize(1.0f);
	DrawVertexArray(lines, PRIMITIVE_LINES, nullptr);
}
void Renderer::DrawMultiColoredCube(const Vector3& bottomSouthCorner, const Vector3& size, const Rgba& bottomColor, const Rgba& topColor, const Rgba& frontColor,
	const Rgba& backColor, const Rgba& rightSide, const Rgba& leftSide)
{
	//bottom, top, front, back, right, left
	//assume that Camera facing East direction = front.

	float xAndLength = bottomSouthCorner.x + size.x;
	float yAndWidth = bottomSouthCorner.y + size.y;
	float zAndHeight = bottomSouthCorner.z + size.z;
	std::vector<Vertex_PCT> quads;

	{
		//bottom
		quads.push_back(Vertex_PCT(Vector3(bottomSouthCorner.x, bottomSouthCorner.y, bottomSouthCorner.z), Rgba(bottomColor)));
		quads.push_back(Vertex_PCT(Vector3(bottomSouthCorner.x, yAndWidth, bottomSouthCorner.z), Rgba(bottomColor)));
		quads.push_back(Vertex_PCT(Vector3(xAndLength, yAndWidth, bottomSouthCorner.z), Rgba(bottomColor)));
		quads.push_back(Vertex_PCT(Vector3(xAndLength, bottomSouthCorner.z, bottomSouthCorner.z), Rgba(bottomColor)));

		//top
		quads.push_back(Vertex_PCT(Vector3(bottomSouthCorner.x, bottomSouthCorner.y, zAndHeight), Rgba(topColor)));
		quads.push_back(Vertex_PCT(Vector3(xAndLength, bottomSouthCorner.y, zAndHeight), Rgba(topColor)));
		quads.push_back(Vertex_PCT(Vector3(xAndLength, yAndWidth, zAndHeight), Rgba(topColor)));
		quads.push_back(Vertex_PCT(Vector3(bottomSouthCorner.x, yAndWidth, zAndHeight), Rgba(topColor)));

		//West
		quads.push_back(Vertex_PCT(Vector3(bottomSouthCorner.x, bottomSouthCorner.y, bottomSouthCorner.z), Rgba(frontColor)));
		quads.push_back(Vertex_PCT(Vector3(bottomSouthCorner.x, bottomSouthCorner.y, zAndHeight), Rgba(frontColor)));
		quads.push_back(Vertex_PCT(Vector3(bottomSouthCorner.x, yAndWidth, zAndHeight), Rgba(frontColor)));
		quads.push_back(Vertex_PCT(Vector3(bottomSouthCorner.x, yAndWidth, bottomSouthCorner.z), Rgba(frontColor)));

		//East
		quads.push_back(Vertex_PCT(Vector3(xAndLength, yAndWidth, bottomSouthCorner.z), Rgba(backColor)));
		quads.push_back(Vertex_PCT(Vector3(xAndLength, yAndWidth, zAndHeight), Rgba(backColor)));
		quads.push_back(Vertex_PCT(Vector3(xAndLength, bottomSouthCorner.y, zAndHeight), Rgba(backColor)));
		quads.push_back(Vertex_PCT(Vector3(xAndLength, bottomSouthCorner.y, bottomSouthCorner.z), Rgba(backColor)));

		//South
		quads.push_back(Vertex_PCT(Vector3(xAndLength, bottomSouthCorner.y, bottomSouthCorner.z), Rgba(rightSide)));
		quads.push_back(Vertex_PCT(Vector3(xAndLength, bottomSouthCorner.y, zAndHeight), Rgba(rightSide)));
		quads.push_back(Vertex_PCT(Vector3(bottomSouthCorner.x, bottomSouthCorner.y, zAndHeight), Rgba(rightSide)));
		quads.push_back(Vertex_PCT(Vector3(bottomSouthCorner.x, bottomSouthCorner.y, bottomSouthCorner.z), Rgba(rightSide)));

		//North
		quads.push_back(Vertex_PCT(Vector3(bottomSouthCorner.x, yAndWidth, bottomSouthCorner.z), Rgba(leftSide)));
		quads.push_back(Vertex_PCT(Vector3(bottomSouthCorner.x, yAndWidth, zAndHeight), Rgba(leftSide)));
		quads.push_back(Vertex_PCT(Vector3(xAndLength, yAndWidth, zAndHeight), Rgba(leftSide)));
		quads.push_back(Vertex_PCT(Vector3(xAndLength, yAndWidth, bottomSouthCorner.z), Rgba(leftSide)));
	}
	DrawVertexArray(quads, PRIMITIVE_QUADS, nullptr);
}


void Renderer::DrawGridAlongXYPlane(const Vector2& size, const Rgba& color)
{
	Vector3 planeBottomLeftCorner = Vector3(-(size.x/2.f), -(size.y/2.f), 0.f);
	std::vector<Vertex_PCT> lines;

	{
		//Y lines.
		for (int i = 0; i <= size.y; i++)
		{
			lines.push_back(Vertex_PCT(Vector3(planeBottomLeftCorner.x + i, planeBottomLeftCorner.y, planeBottomLeftCorner.z), color));
			lines.push_back(Vertex_PCT(Vector3(planeBottomLeftCorner.x + i, planeBottomLeftCorner.y + size.y, planeBottomLeftCorner.z), color));
		}


		//X lines
		for (int i = 0; i <= size.x; i++)
		{
			lines.push_back(Vertex_PCT(Vector3(planeBottomLeftCorner.x, planeBottomLeftCorner.y + i, planeBottomLeftCorner.z), color));
			lines.push_back(Vertex_PCT(Vector3(planeBottomLeftCorner.x + size.x, planeBottomLeftCorner.y + i, planeBottomLeftCorner.z), color));
		}
	}
	setLineSize(1.0f);
	DrawVertexArray(lines, PRIMITIVE_LINES, nullptr);
}

void Renderer::DrawSingleTexturedCube(const Vector3& bottomSouthCorner, const Vector3& size, const Texture* texture, const AABB2 texCoords, const Rgba& tint)
{
	//bottom, top, front, back, right, left
	float xAndLength = bottomSouthCorner.x + size.x;
	float yAndWidth = bottomSouthCorner.y + size.y;
	float zAndHeight = bottomSouthCorner.z + size.z;
	std::vector<Vertex_PCT> quads;

	{
		//bottom
		quads.push_back(Vertex_PCT(Vector3(bottomSouthCorner.x, bottomSouthCorner.y, bottomSouthCorner.z), tint, Vector2(texCoords.mins.x, texCoords.mins.y)));
		quads.push_back(Vertex_PCT(Vector3(bottomSouthCorner.x, yAndWidth, bottomSouthCorner.z), tint, Vector2(texCoords.mins.x, texCoords.maxs.y)));
		quads.push_back(Vertex_PCT(Vector3(xAndLength, yAndWidth, bottomSouthCorner.z), tint, Vector2(texCoords.maxs.x, texCoords.maxs.y)));
		quads.push_back(Vertex_PCT(Vector3(xAndLength, bottomSouthCorner.z, bottomSouthCorner.z), tint, Vector2(texCoords.maxs.x, texCoords.mins.y)));

		//top
		quads.push_back(Vertex_PCT(Vector3(bottomSouthCorner.x, bottomSouthCorner.y, zAndHeight), tint, Vector2(texCoords.mins.x, texCoords.maxs.y)));
		quads.push_back(Vertex_PCT(Vector3(xAndLength, bottomSouthCorner.y, zAndHeight), tint, Vector2(texCoords.maxs.x, texCoords.maxs.y)));
		quads.push_back(Vertex_PCT(Vector3(xAndLength, yAndWidth, zAndHeight), tint, Vector2(texCoords.maxs.x, texCoords.mins.y)));
		quads.push_back(Vertex_PCT(Vector3(bottomSouthCorner.x, yAndWidth, zAndHeight), tint, Vector2(texCoords.mins.x, texCoords.mins.y)));

		//West side
		quads.push_back(Vertex_PCT(Vector3(bottomSouthCorner.x, bottomSouthCorner.y, bottomSouthCorner.z), tint, Vector2(texCoords.maxs.x, texCoords.maxs.y)));
		quads.push_back(Vertex_PCT(Vector3(bottomSouthCorner.x, bottomSouthCorner.y, zAndHeight), tint, Vector2(texCoords.maxs.x, texCoords.mins.y)));
		quads.push_back(Vertex_PCT(Vector3(bottomSouthCorner.x, yAndWidth, zAndHeight), tint, Vector2(texCoords.mins.x, texCoords.mins.y)));
		quads.push_back(Vertex_PCT(Vector3(bottomSouthCorner.x, yAndWidth, bottomSouthCorner.z), tint, Vector2(texCoords.mins.x, texCoords.maxs.y)));

		//East side
		quads.push_back(Vertex_PCT(Vector3(xAndLength, yAndWidth, bottomSouthCorner.z), tint, Vector2(texCoords.maxs.x, texCoords.maxs.y)));
		quads.push_back(Vertex_PCT(Vector3(xAndLength, yAndWidth, zAndHeight), tint, Vector2(texCoords.maxs.x, texCoords.mins.y)));
		quads.push_back(Vertex_PCT(Vector3(xAndLength, bottomSouthCorner.y, zAndHeight), tint, Vector2(texCoords.mins.x, texCoords.mins.y)));
		quads.push_back(Vertex_PCT(Vector3(xAndLength, bottomSouthCorner.y, bottomSouthCorner.z), tint, Vector2(texCoords.mins.x, texCoords.maxs.y)));

		//South side
		quads.push_back(Vertex_PCT(Vector3(xAndLength, bottomSouthCorner.y, bottomSouthCorner.z), tint, Vector2(texCoords.maxs.x, texCoords.maxs.y)));
		quads.push_back(Vertex_PCT(Vector3(xAndLength, bottomSouthCorner.y, zAndHeight), tint, Vector2(texCoords.maxs.x, texCoords.mins.y)));
		quads.push_back(Vertex_PCT(Vector3(bottomSouthCorner.x, bottomSouthCorner.y, zAndHeight), tint, Vector2(texCoords.mins.x, texCoords.mins.y)));
		quads.push_back(Vertex_PCT(Vector3(bottomSouthCorner.x, bottomSouthCorner.y, bottomSouthCorner.z), tint, Vector2(texCoords.mins.x, texCoords.maxs.y)));

		//North side
		quads.push_back(Vertex_PCT(Vector3(bottomSouthCorner.x, yAndWidth, bottomSouthCorner.z), tint, Vector2(texCoords.maxs.x, texCoords.maxs.y)));
		quads.push_back(Vertex_PCT(Vector3(bottomSouthCorner.x, yAndWidth, zAndHeight), tint, Vector2(texCoords.maxs.x, texCoords.mins.y)));
		quads.push_back(Vertex_PCT(Vector3(xAndLength, yAndWidth, zAndHeight), tint, Vector2(texCoords.mins.x, texCoords.mins.y)));
		quads.push_back(Vertex_PCT(Vector3(xAndLength, yAndWidth, bottomSouthCorner.z), tint, Vector2(texCoords.mins.x, texCoords.maxs.y)));
	}
	DrawVertexArray(quads, PRIMITIVE_QUADS, texture);
}
void Renderer::DrawGridLinesNEU(const float& length, const float& width, const float& height, const float& lineThickness)
{
	glDisable(GL_TEXTURE_2D);
	std::vector<Vertex_PCT> lines;
	{
		lines.push_back(Vertex_PCT(Vector3(0.f, 0.f, 0.f), Rgba(1.f, 0.f, 0.f, 1.f)));
		lines.push_back(Vertex_PCT(Vector3(length, 0.f, 0.f), Rgba(1.f, 0.f, 0.f, 1.f)));

		lines.push_back(Vertex_PCT(Vector3(0.f,0.f,0.f), Rgba(0.f, 1.f, 0.f, 1.f)));
		lines.push_back(Vertex_PCT(Vector3(0.f, width, 0.f), Rgba(0.f, 1.f, 0.f, 1.f)));

		lines.push_back(Vertex_PCT(Vector3(0.f, 0.f, 0.f), Rgba(0.f, 0.f, 1.f, 1.f)));
		lines.push_back(Vertex_PCT(Vector3(0.f, 0.f, height), Rgba(0.f, 0.f, 1.f, 1.f)));
	}
	setLineSize(lineThickness);
	DrawVertexArray(lines, PRIMITIVE_LINES, nullptr);
}

void Renderer::CreateOpenGLWindow(HINSTANCE& applicationInstanceHandle, const char*& APP_NAME, const int& OFFSET_FROM_WINDOWS_DESKTOP, const int& WINDOW_PHYSICAL_WIDTH,
	const int& WINDOW_PHYSICAL_HEIGHT, HWND& g_hWnd, HDC& g_displayDeviceContext, HGLRC g_openGLRenderingContext, const double& VIEW_LEFT, const double& VIEW_RIGHT,
	const double& VIEW_TOP, const double& VIEW_BOTTOM, WNDCLASSEX& windowClassDescription)
{
	/*
	// Define a window class
	WNDCLASSEX windowClassDescription;
	memset(&windowClassDescription, 0, sizeof(windowClassDescription));
	windowClassDescription.cbSize = sizeof(windowClassDescription);
	windowClassDescription.style = CS_OWNDC; // Redraw on move, request own Display Context
	windowClassDescription.lpfnWndProc = static_cast<WNDPROC>(WindowsMessageHandlingProcedure); // Assign a win32 message-handling function
	windowClassDescription.hInstance = GetModuleHandle(NULL);
	windowClassDescription.hIcon = NULL;
	windowClassDescription.hCursor = NULL;
	windowClassDescription.lpszClassName = TEXT("Simple Window Class");
	RegisterClassEx(&windowClassDescription);
	*/
	//method handles all the disgustingly bad parts of making the app window, just pass it the variables and it you won't have to see how ugly this code really is.

	const DWORD windowStyleFlags = WS_CAPTION | WS_BORDER | WS_THICKFRAME | WS_SYSMENU | WS_OVERLAPPED;
	const DWORD windowStyleExFlags = WS_EX_APPWINDOW;

	RECT desktopRect;
	HWND desktopWindowHandle = GetDesktopWindow();
	GetClientRect(desktopWindowHandle, &desktopRect);

	RECT windowRect = { OFFSET_FROM_WINDOWS_DESKTOP, OFFSET_FROM_WINDOWS_DESKTOP, OFFSET_FROM_WINDOWS_DESKTOP + WINDOW_PHYSICAL_WIDTH,
		OFFSET_FROM_WINDOWS_DESKTOP + WINDOW_PHYSICAL_HEIGHT };
	AdjustWindowRectEx(&windowRect, windowStyleFlags, FALSE, windowStyleExFlags);

	WCHAR windowTitle[1024];
	MultiByteToWideChar(GetACP(), 0, APP_NAME, -1, windowTitle, sizeof(windowTitle) / sizeof(windowTitle[0]));
	g_hWnd = CreateWindowEx(
		windowStyleExFlags,
		windowClassDescription.lpszClassName,
		windowTitle,
		windowStyleFlags,
		windowRect.left,
		windowRect.top,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		NULL,
		NULL,
		applicationInstanceHandle,
		NULL);

	ShowWindow(g_hWnd, SW_SHOW);
	SetForegroundWindow(g_hWnd);
	SetFocus(g_hWnd);

	g_displayDeviceContext = GetDC(g_hWnd);

	HCURSOR cursor = LoadCursor(NULL, IDC_ARROW);
	SetCursor(cursor);

	PIXELFORMATDESCRIPTOR pixelFormatDescriptor;
	memset(&pixelFormatDescriptor, 0, sizeof(pixelFormatDescriptor));
	pixelFormatDescriptor.nSize = sizeof(pixelFormatDescriptor);
	pixelFormatDescriptor.nVersion = 1;
	pixelFormatDescriptor.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pixelFormatDescriptor.iPixelType = PFD_TYPE_RGBA;
	pixelFormatDescriptor.cColorBits = 24;
	pixelFormatDescriptor.cDepthBits = 24;
	pixelFormatDescriptor.cAccumBits = 0;
	pixelFormatDescriptor.cStencilBits = 8;

	int pixelFormatCode = ChoosePixelFormat(g_displayDeviceContext, &pixelFormatDescriptor);
	SetPixelFormat(g_displayDeviceContext, pixelFormatCode, &pixelFormatDescriptor);
	g_openGLRenderingContext = wglCreateContext(g_displayDeviceContext);
	wglMakeCurrent(g_displayDeviceContext, g_openGLRenderingContext);

	//Ortho = orthographic; aka, not 3D. used for 2D games to set up coordinate system.
	//glOrtho(0.f, 1000.f, 10.f, 0.f,0.f,1.f);
	glOrtho(VIEW_LEFT, VIEW_RIGHT, VIEW_BOTTOM, VIEW_TOP, 0.f, 1.f);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glLineWidth(2.f);
	glEnable(GL_LINE_SMOOTH);
}
void Renderer::SetOrtho(const Vector2& bottomLeft, const Vector2& topRight)
{
	//sets the general screen location.
	glLoadIdentity();
	glOrtho(bottomLeft.x, topRight.x, bottomLeft.y, topRight.y, 0.f, 1.f);
}

void Renderer::SetPerspective(const float& fovDegreesY, const float& aspect, const float& nearDist, const float& farDist)
{
	glLoadIdentity();
	gluPerspective(fovDegreesY, aspect, nearDist, farDist);
}

void Renderer::Translate(const Vector2& translateBy)
{
	//translates the screen position
	glTranslatef(translateBy.x, translateBy.y, 0.f);
}
void Renderer::Translate(const Vector3& translateBy)
{
	//translates the screen position
	glTranslatef(translateBy.x, translateBy.y, translateBy.z);
}

void Renderer::Rotate(const float& degrees)
{
	//rotates the screen by degrees, along Z axis only.
	glRotatef(degrees, 0.f, 0.f, 1.f);
}

void Renderer::Rotate(const float& degrees, const EulerAngles& axis)
{
	glRotatef(degrees, axis.m_xRoll, axis.m_yPitch, axis.m_zYaw);
}

void Renderer::Push()
{
	//Pushes in the present screen orientation
	glPushMatrix();
}
void Renderer::Pop()
{
	//pops out the present screen orientation
	glPopMatrix();
}

void Renderer::screenShake(const float& shakeMagnitude)
{
	//suppose to be used to shake the screen.
	float trueDisplacementX = 10.f + (rand() * shakeMagnitude);
	float trueDisplacementY = 10.f + (rand() * shakeMagnitude);
	glTranslatef(trueDisplacementX, trueDisplacementY,0.f);
}
void Renderer::updateCameraPosition(const double& VIEW_LEFT, const double& VIEW_RIGHT, const double& VIEW_BOTTOM, const double& VIEW_TOP)
{
	//updates the camera's position
	glOrtho(VIEW_LEFT, VIEW_RIGHT, VIEW_BOTTOM, VIEW_TOP, 0.f, 1.f);
}

void Renderer::clearScreen(const Rgba& color, const bool& clearDepth)
{
	//generates a background screen color so as to prevent a hall of mirrors effect.
	glClearColor(color.Red, color.Green, color.Blue, 1.f);
	if (clearDepth == true)
	{
		glClearDepth(1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
	else
	{
		glClear(GL_COLOR_BUFFER_BIT);
	}
}
void Renderer::enableAlphaBlend()
{
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
}
void Renderer::disableAlphaBlend()
{
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}