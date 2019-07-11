#include "pch.h"
#include "Camera_Manager.h"

Camera_Manager::Camera_Manager(void)
	: mCurrCamType(CAM_TYPE_MAIN)
{
	m_vecCamera.resize((int)CAM_TYPE_END);
}

Camera_Manager::~Camera_Manager(void)
{
}

void Camera_Manager::CreateCamera(void)
{
	PCAM pCam;
	for (int i = 0; i < (int)CAM_TYPE_END; ++i)
	{
		pCam = std::make_unique<Camera>();
		m_vecCamera[i] = std::move(pCam);
	}
}

void Camera_Manager::ChangeCurrentCamera(CAMERA_TYPE& _type, float fovY, float aspect, float zn, float zf)
{
	mCurrCamType = _type;

	Get_CurrentCamera()->SetLens(fovY, aspect, zn, zf);
}

PCAM& Camera_Manager::Get_CurrentCamera(void)
{
	return m_vecCamera[mCurrCamType];
}

