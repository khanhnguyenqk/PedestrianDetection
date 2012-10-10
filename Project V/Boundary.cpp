#include "stdafx.h"
#include <stdio.h>
#include <string.h>
#include <new>
#include "Boundary.h"

using namespace std;

Boundary::Boundary(void)
{
	int *c;
	c = new int[100];
	memset(c, 0x00000002, 100*sizeof(int));
	for (int i = 0; i<100; i++) {
		printf("%d\t", c[i]);
	}
	w_ = 100;
	h_ = 100;
	boundary_ = new int*[100];
	for (int i=0; i<w_; i++) {
		boundary_[i] = new int[100];
	}
	for (int i=0; i<w_; i++) {
		memset(boundary_[i], 0, h_*sizeof(int));
	}
	for(int j = 0; j < 100; j++)
	{
		for(int i = 0; i < 100; i++)
		{  
			printf("%d\t", boundary_[i][j]);
		}
	}
}

Boundary::~Boundary(void)
{
	for (int i=0; i < w_; i++) {
		delete[] boundary_[i];
	}
	delete[] boundary_;
}
