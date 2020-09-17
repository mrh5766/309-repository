#include "AppClass.h"
void Application::InitVariables(void)
{
	//init the mesh
	m_pMesh = new MyMesh();
	//m_pMesh->GenerateCube(1.0f, C_WHITE);
	int invader[8][11] =
	{
		{0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0},
		{0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0},
		{0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0},
		{0, 1, 1, 0, 1, 1, 1, 0, 1, 1, 0},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1},
		{1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1},
		{0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0}
	};
	int counter = 0;
	for (int y = 0; y < 8; y++)
	{
		for (int x = 0; x < 11; x++)
		{
			if (invader[y][x] == 1)
			{
				MyMesh* cube = new MyMesh();
				cube->GenerateCube(1, vector3(0, 0, 0));
				matrix4 translation = glm::translate(IDENTITY_M4, vector3(-x + 5, -y + 5, 3));
				translations[counter] = translation;
				cubes[counter] = cube;
				counter++;
			}
		}
	}
	MyMesh* cube = new MyMesh();
	//cube->GenerateCube(1, vector3(255, 255, 255));
	//translations[0] = glm::translate(IDENTITY_M4, vector3(0, 2.0f, 3.0f));
	//m_pMesh->GenerateSphere(1.0f, 5, C_WHITE);
	matrix4 test = translations[3];
}
void Application::Update(void)
{
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	//Is the arcball active?
	ArcBall();

	//Is the first person camera active?
	CameraRotation();
}
void Application::Display(void)
{
	// Clear the screen
	ClearScreen();

	matrix4 m4View = m_pCameraMngr->GetViewMatrix();
	matrix4 m4Projection = m_pCameraMngr->GetProjectionMatrix();
	
	matrix4 m4Scale = glm::scale(IDENTITY_M4, vector3(2.0f,2.0f,2.0f));
	static float value = 0.0f;
	matrix4 m4Translate = glm::translate(IDENTITY_M4, vector3(value, 2.0f, 3.0f));
	value += 0.01f;

	//matrix4 m4Model = m4Translate * m4Scale;
	matrix4 m4Model = m4Scale * m4Translate;
	for (int i = 0; i < numCubes; i++)
	{
		translations[i] *= glm::translate(IDENTITY_M4, vector3(0.05, 0, 0));
		cubes[i]->Render(m4Projection, m4View, translations[i]);
	}

	m_pMesh->Render(m4Projection, m4View, m4Model);
	
	// draw a skybox
	m_pMeshMngr->AddSkyboxToRenderList();
	
	//render list call
	m_uRenderCallCount = m_pMeshMngr->Render();

	//clear the render list
	m_pMeshMngr->ClearRenderList();
	
	//draw gui
	DrawGUI();
	
	//end the current frame (internally swaps the front and back buffers)
	m_pWindow->display();
}
void Application::Release(void)
{
	SafeDelete(m_pMesh);
	for (int i = 0; i < numCubes; i++)
	{
		SafeDelete(cubes[i]);
	}
	//release GUI
	ShutdownGUI();
}