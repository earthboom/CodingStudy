#pragma once

#include "Camera.h"

#define CAM_MGR Camera_Manager::GetInstnace()
#define CURR_CAM Camera_Manager::GetInstnace().Get_CurrentCamera()

class Camera_Manager : public CSingleton<Camera_Manager>
{
public:
	typedef enum CameraType
	{
		CAM_TYPE_MAIN,
		CAM_TYPE_END
	}CAMERA_TYPE;

public:
	explicit Camera_Manager(void);
	Camera_Manager(const Camera_Manager&) = delete;
	Camera_Manager& operator = (const Camera_Manager&) = delete;
	~Camera_Manager(void);

public:
	void CreateCamera(void);
	void ChangeCurrentCamera(CAMERA_TYPE& _type, float fovY, float aspect, float zn, float zf);

	PCAM& Get_CurrentCamera(void);

private:
	std::vector<PCAM> m_vecCamera;

	CAMERA_TYPE mCurrCamType;
};