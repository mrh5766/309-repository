#include "MyMesh.h"
void MyMesh::Init(void)
{
	m_bBinded = false;
	m_uVertexCount = 0;

	m_VAO = 0;
	m_VBO = 0;

	m_pShaderMngr = ShaderManager::GetInstance();
}
void MyMesh::Release(void)
{
	m_pShaderMngr = nullptr;

	if (m_VBO > 0)
		glDeleteBuffers(1, &m_VBO);

	if (m_VAO > 0)
		glDeleteVertexArrays(1, &m_VAO);

	m_lVertex.clear();
	m_lVertexPos.clear();
	m_lVertexCol.clear();
}
MyMesh::MyMesh()
{
	Init();
}
MyMesh::~MyMesh() { Release(); }
MyMesh::MyMesh(MyMesh& other)
{
	m_bBinded = other.m_bBinded;

	m_pShaderMngr = other.m_pShaderMngr;

	m_uVertexCount = other.m_uVertexCount;

	m_VAO = other.m_VAO;
	m_VBO = other.m_VBO;
}
MyMesh& MyMesh::operator=(MyMesh& other)
{
	if (this != &other)
	{
		Release();
		Init();
		MyMesh temp(other);
		Swap(temp);
	}
	return *this;
}
void MyMesh::Swap(MyMesh& other)
{
	std::swap(m_bBinded, other.m_bBinded);
	std::swap(m_uVertexCount, other.m_uVertexCount);

	std::swap(m_VAO, other.m_VAO);
	std::swap(m_VBO, other.m_VBO);

	std::swap(m_lVertex, other.m_lVertex);
	std::swap(m_lVertexPos, other.m_lVertexPos);
	std::swap(m_lVertexCol, other.m_lVertexCol);

	std::swap(m_pShaderMngr, other.m_pShaderMngr);
}
void MyMesh::CompleteMesh(vector3 a_v3Color)
{
	uint uColorCount = m_lVertexCol.size();
	for (uint i = uColorCount; i < m_uVertexCount; ++i)
	{
		m_lVertexCol.push_back(a_v3Color);
	}
}
void MyMesh::AddVertexPosition(vector3 a_v3Input)
{
	m_lVertexPos.push_back(a_v3Input);
	m_uVertexCount = m_lVertexPos.size();
}
void MyMesh::AddVertexColor(vector3 a_v3Input)
{
	m_lVertexCol.push_back(a_v3Input);
}
void MyMesh::CompileOpenGL3X(void)
{
	if (m_bBinded)
		return;

	if (m_uVertexCount == 0)
		return;

	CompleteMesh();

	for (uint i = 0; i < m_uVertexCount; i++)
	{
		//Position
		m_lVertex.push_back(m_lVertexPos[i]);
		//Color
		m_lVertex.push_back(m_lVertexCol[i]);
	}
	glGenVertexArrays(1, &m_VAO);//Generate vertex array object
	glGenBuffers(1, &m_VBO);//Generate Vertex Buffered Object

	glBindVertexArray(m_VAO);//Bind the VAO
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);//Bind the VBO
	glBufferData(GL_ARRAY_BUFFER, m_uVertexCount * 2 * sizeof(vector3), &m_lVertex[0], GL_STATIC_DRAW);//Generate space for the VBO

	// Position attribute
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vector3), (GLvoid*)0);

	// Color attribute
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vector3), (GLvoid*)(1 * sizeof(vector3)));

	m_bBinded = true;

	glBindVertexArray(0); // Unbind VAO
}
void MyMesh::Render(matrix4 a_mProjection, matrix4 a_mView, matrix4 a_mModel)
{
	// Use the buffer and shader
	GLuint nShader = m_pShaderMngr->GetShaderID("Basic");
	glUseProgram(nShader); 

	//Bind the VAO of this object
	glBindVertexArray(m_VAO);

	// Get the GPU variables by their name and hook them to CPU variables
	GLuint MVP = glGetUniformLocation(nShader, "MVP");
	GLuint wire = glGetUniformLocation(nShader, "wire");

	//Final Projection of the Camera
	matrix4 m4MVP = a_mProjection * a_mView * a_mModel;
	glUniformMatrix4fv(MVP, 1, GL_FALSE, glm::value_ptr(m4MVP));
	
	//Solid
	glUniform3f(wire, -1.0f, -1.0f, -1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawArrays(GL_TRIANGLES, 0, m_uVertexCount);  

	//Wire
	glUniform3f(wire, 1.0f, 0.0f, 1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnable(GL_POLYGON_OFFSET_LINE);
	glPolygonOffset(-1.f, -1.f);
	glDrawArrays(GL_TRIANGLES, 0, m_uVertexCount);
	glDisable(GL_POLYGON_OFFSET_LINE);

	glBindVertexArray(0);// Unbind VAO so it does not get in the way of other objects
}
void MyMesh::AddTri(vector3 a_vBottomLeft, vector3 a_vBottomRight, vector3 a_vTopLeft)
{
	//C
	//| \
	//A--B
	//This will make the triangle A->B->C 
	AddVertexPosition(a_vBottomLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopLeft);
}
void MyMesh::AddQuad(vector3 a_vBottomLeft, vector3 a_vBottomRight, vector3 a_vTopLeft, vector3 a_vTopRight)
{
	//C--D
	//|  |
	//A--B
	//This will make the triangle A->B->C and then the triangle C->B->D
	AddVertexPosition(a_vBottomLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopLeft);

	AddVertexPosition(a_vTopLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopRight);
}
void MyMesh::GenerateCube(float a_fSize, vector3 a_v3Color)
{
	if (a_fSize < 0.01f)
		a_fSize = 0.01f;

	Release();
	Init();

	float fValue = a_fSize * 0.5f;
	//3--2
	//|  |
	//0--1

	vector3 point0(-fValue,-fValue, fValue); //0
	vector3 point1( fValue,-fValue, fValue); //1
	vector3 point2( fValue, fValue, fValue); //2
	vector3 point3(-fValue, fValue, fValue); //3

	vector3 point4(-fValue,-fValue,-fValue); //4
	vector3 point5( fValue,-fValue,-fValue); //5
	vector3 point6( fValue, fValue,-fValue); //6
	vector3 point7(-fValue, fValue,-fValue); //7

	//F
	AddQuad(point0, point1, point3, point2);

	//B
	AddQuad(point5, point4, point6, point7);

	//L
	AddQuad(point4, point0, point7, point3);

	//R
	AddQuad(point1, point5, point2, point6);

	//U
	AddQuad(point3, point2, point7, point6);

	//D
	AddQuad(point4, point5, point0, point1);

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCuboid(vector3 a_v3Dimensions, vector3 a_v3Color)
{
	Release();
	Init();

	vector3 v3Value = a_v3Dimensions * 0.5f;
	//3--2
	//|  |
	//0--1
	vector3 point0(-v3Value.x, -v3Value.y, v3Value.z); //0
	vector3 point1(v3Value.x, -v3Value.y, v3Value.z); //1
	vector3 point2(v3Value.x, v3Value.y, v3Value.z); //2
	vector3 point3(-v3Value.x, v3Value.y, v3Value.z); //3

	vector3 point4(-v3Value.x, -v3Value.y, -v3Value.z); //4
	vector3 point5(v3Value.x, -v3Value.y, -v3Value.z); //5
	vector3 point6(v3Value.x, v3Value.y, -v3Value.z); //6
	vector3 point7(-v3Value.x, v3Value.y, -v3Value.z); //7

	//F
	AddQuad(point0, point1, point3, point2);

	//B
	AddQuad(point5, point4, point6, point7);

	//L
	AddQuad(point4, point0, point7, point3);

	//R
	AddQuad(point1, point5, point2, point6);

	//U
	AddQuad(point3, point2, point7, point6);

	//D
	AddQuad(point4, point5, point0, point1);

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCone(float a_fRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	// Replace this with your code
	vector3 baseCenter(0, 0, -a_fHeight / 2); //Z coord is to make sure it's centered at 0,0,0
	vector3 topCenter(0, 0, a_fHeight / 2);
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		vector3 base1(a_fRadius * cos(2 * PI * i / a_nSubdivisions), a_fRadius * sin(2 * PI * i / a_nSubdivisions), -a_fHeight / 2); //First point at the edge of the base
		vector3 base2(a_fRadius * cos(2 * PI * (i + (uint)1) / a_nSubdivisions), a_fRadius * sin(2 * PI * (i + (uint)1) / a_nSubdivisions), -a_fHeight / 2); //Second point at the edge of the base
		AddTri(base2, base1, baseCenter);
		AddTri(base1, base2, topCenter);
	}
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCylinder(float a_fRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	// Replace this with your code
	vector3 baseCenter(0, 0, -a_fHeight / 2); //Z coord is to make sure it's centered at 0,0,0
	vector3 topCenter(0, 0, a_fHeight / 2);
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		vector3 base1(a_fRadius * cos(2 * PI * i / a_nSubdivisions), a_fRadius * sin(2 * PI * i / a_nSubdivisions), -a_fHeight / 2); //First point at the edge of the base
		vector3 base2(a_fRadius * cos(2 * PI * (i + (uint)1) / a_nSubdivisions), a_fRadius * sin(2 * PI * (i + (uint)1) / a_nSubdivisions), -a_fHeight / 2); //Second point at the edge of the base
		vector3 top1(a_fRadius * cos(2 * PI * i / a_nSubdivisions), a_fRadius * sin(2 * PI * i / a_nSubdivisions), a_fHeight / 2); //First point at the edge of the top
		vector3 top2(a_fRadius * cos(2 * PI * (i + (uint)1) / a_nSubdivisions), a_fRadius * sin(2 * PI * (i + (uint)1) / a_nSubdivisions), a_fHeight / 2); //Second point at the edge of the top
		AddTri(base2, base1, baseCenter);
		AddTri(top1, top2, topCenter);
		AddQuad(base1, base2, top1, top2);
	}
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateTube(float a_fOuterRadius, float a_fInnerRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fOuterRadius < 0.01f)
		a_fOuterRadius = 0.01f;

	if (a_fInnerRadius < 0.005f)
		a_fInnerRadius = 0.005f;

	if (a_fInnerRadius > a_fOuterRadius)
		std::swap(a_fInnerRadius, a_fOuterRadius);

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	// Replace this with your code
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		float xRadians1 = cos(2 * PI * i / a_nSubdivisions);
		float yRadians1 = sin(2 * PI * i / a_nSubdivisions);

		float xRadians2 = cos(2 * PI * (i + (uint)1) / a_nSubdivisions);
		float yRadians2 = sin(2 * PI * (i + (uint)1) / a_nSubdivisions);
		vector3 outerTop1(a_fOuterRadius * xRadians1, a_fOuterRadius * yRadians1, a_fHeight / 2); //First point at the outer edge of the top
		vector3 outerTop2(a_fOuterRadius * xRadians2, a_fOuterRadius * yRadians2, a_fHeight / 2); //Second point at the outer edge of the top
		
		vector3 innerTop1(a_fInnerRadius * xRadians1, a_fInnerRadius * yRadians1, a_fHeight / 2); //First point at the inner edge of the top
		vector3 innerTop2(a_fInnerRadius * xRadians2, a_fInnerRadius * yRadians2, a_fHeight / 2); //Second point at the inner edge of the top

		vector3 outerBot1(a_fOuterRadius * xRadians1, a_fOuterRadius * yRadians1, -a_fHeight / 2); //First point at the outer edge of the bottom
		vector3 outerBot2(a_fOuterRadius * xRadians2, a_fOuterRadius * yRadians2, -a_fHeight / 2); //Second point at the outer edge of the bottom

		vector3 innerBot1(a_fInnerRadius * xRadians1, a_fInnerRadius * yRadians1, -a_fHeight / 2); //First point at the inner edge of the bottom
		vector3 innerBot2(a_fInnerRadius * xRadians2, a_fInnerRadius * yRadians2, -a_fHeight / 2); //Second point at the inner edge of the bottom

		AddQuad(outerBot1, outerBot2, outerTop1, outerTop2); //The quad on the outside of the tube
		AddQuad(innerBot2, innerBot1, innerTop2, innerTop1); //The quad on the inside of the tube
		AddQuad(innerTop2, innerTop1, outerTop2, outerTop1); //The quad on the top of the tube
		AddQuad(innerBot1, innerBot2, outerBot1, outerBot2); //The quad on the bottom of the tube
	}
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateTorus(float a_fOuterRadius, float a_fInnerRadius, int a_nSubdivisionsA, int a_nSubdivisionsB, vector3 a_v3Color)
{
	if (a_fOuterRadius < 0.01f)
		a_fOuterRadius = 0.01f;

	if (a_fInnerRadius < 0.005f)
		a_fInnerRadius = 0.005f;

	if (a_fInnerRadius > a_fOuterRadius)
		std::swap(a_fInnerRadius, a_fOuterRadius);

	if (a_nSubdivisionsA < 3)
		a_nSubdivisionsA = 3;
	if (a_nSubdivisionsA > 360)
		a_nSubdivisionsA = 360;

	if (a_nSubdivisionsB < 3)
		a_nSubdivisionsB = 3;
	if (a_nSubdivisionsB > 360)
		a_nSubdivisionsB = 360;

	Release();
	Init();

	// Replace this with your code
	for (int i = 0; i < a_nSubdivisionsA; i++)
	{
		float xRadians1 = cos(2 * PI * i / a_nSubdivisionsA); //The radians going around in a circle from the center of the torus
		float yRadians1 = sin(2 * PI * i / a_nSubdivisionsA);

		float xRadians2 = cos(2 * PI * (i + 1) / a_nSubdivisionsA);
		float yRadians2 = sin(2 * PI * (i + 1) / a_nSubdivisionsA);

		float cylCenterRadius = (a_fOuterRadius + a_fInnerRadius) / 2;
		float cylRadius = (a_fOuterRadius - a_fInnerRadius) / 2; //The radius of the cylindrical outer edge
		for (int n = 0; n < a_nSubdivisionsB; n++)
		{
			float xRadians3 = cos(2 * PI * n / a_nSubdivisionsB); //The radians going around in a circle from the center of the cylindrical edge of the torus
			float zRadians3 = sin(2 * PI * n / a_nSubdivisionsB);

			float xRadians4 = cos(2 * PI * (n + 1) / a_nSubdivisionsB);
			float zRadians4 = sin(2 * PI * (n + 1) / a_nSubdivisionsB);
			vector3 circle1(cylCenterRadius + cylRadius * xRadians3, 0, cylRadius * zRadians3); // The first point around the circle
			vector3 circle2(cylCenterRadius + cylRadius * xRadians4, 0, cylRadius * zRadians4); //The second point around the circle

			vector3 cylBot1(circle1.x * xRadians1, circle1.x * yRadians1, circle1.z); //Rotates the points in the circle such that they form a quad making up the cylinder in the torus
			vector3 cylBot2(circle2.x * xRadians1, circle2.x * yRadians1, circle2.z);

			vector3 cylTop1(circle1.x * xRadians2, circle1.x * yRadians2, circle1.z);
			vector3 cylTop2(circle2.x * xRadians2, circle2.x * yRadians2, circle2.z);


			

			AddQuad(cylBot1, cylBot2, cylTop1, cylTop2);
		}
	}
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateSphere(float a_fRadius, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	//Sets minimum and maximum of subdivisions
	if (a_nSubdivisions < 1)
	{
		GenerateCube(a_fRadius * 2.0f, a_v3Color);
		return;
	}
	if (a_nSubdivisions > 6)
		a_nSubdivisions = 6;

	Release();
	Init();
	//a_nSubdivisions = 20; //Looks much more spherical if it's a larger number, rather than a maximum of 6.
	// Replace this with your code
	vector3 top(0, 0, a_fRadius);
	vector3 bottom(0, 0, -a_fRadius);
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		float xRadians1 = cos(2 * PI * i / a_nSubdivisions);
		float yRadians1 = sin(2 * PI * i / a_nSubdivisions);

		float xRadians2 = cos(2 * PI * (i + (uint)1) / a_nSubdivisions);
		float yRadians2 = sin(2 * PI * (i + (uint)1) / a_nSubdivisions);
		int numVerticalDivisions = 11; //This makes one of the ends of the sphere (the 'triangles') disappear at exactly 10 and 15. I'm not really sure why, but it's worked properly for other numbers I've tested (5, 9, 11, 20, 40).
		float verticalDivisionHeight = a_fRadius * 2 / numVerticalDivisions; //the height of divisions vertically - right now, each encompasses 10 of the height.


		//Uses quads from the top to the bottom of the sphere to represent it, 1 for each vertical division.
		for (int n = 0; n <= numVerticalDivisions; n++)
		{
			float topHeight = a_fRadius - n * verticalDivisionHeight;
			float topRadius = sqrt(a_fRadius * a_fRadius - topHeight * topHeight);

			float botHeight = topHeight - verticalDivisionHeight;
			float botRadius = sqrt(a_fRadius * a_fRadius - botHeight * botHeight);

			vector3 top1(topRadius * xRadians1, topRadius * yRadians1, topHeight); //Top left point in the quad
			vector3 top2(topRadius * xRadians2, topRadius * yRadians2, topHeight); //Top right point in the quad

			vector3 bot1(botRadius * xRadians1, botRadius * yRadians1, botHeight); //Bottom left point in the quad
			vector3 bot2(botRadius * xRadians2, botRadius * yRadians2, botHeight); //Bottom right point in the quad

			AddQuad(bot1, bot2, top1, top2);
		}
	}
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}