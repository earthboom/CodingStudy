#pragma once

class Cube : public Object
{
public:
	explicit Cube(void);
	Cube(const Cube&) = delete;
	Cube& operator=(const Cube&) = delete;
	virtual ~Cube(void);

public:
	virtual bool Initialize(void);
	virtual bool Update(DX::StepTimer const& timer);
	virtual bool Render(void);
};