#pragma once
class Boundary
{
protected:
	int **boundary_;
	int x_;
	int y_;
	int w_;
	int h_;
public:
	Boundary(void);
	virtual ~Boundary(void);
};
