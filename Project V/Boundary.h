#pragma once
class Boundary
{
private:
	int **boundary_;
	int x_;
	int y_;
	int w_;
	int h_;
public:
	Boundary(void);
	virtual ~Boundary(void);
};
