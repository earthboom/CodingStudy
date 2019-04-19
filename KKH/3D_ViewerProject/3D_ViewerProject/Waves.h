#pragma once

class Waves
{
public:
	explicit Waves(int m, int n, float dx, float dt, float speed, float damping);
	Waves(const Waves&) = delete;
	Waves& operator=(const Waves&) = delete;
	~Waves(void);
};
