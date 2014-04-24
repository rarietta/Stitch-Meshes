#include "SubFace.h"


SubFace::~SubFace(void)
{
}

SubFace::SubFace(int numBackward, int numForward/*, int faceIndex*/)
{
	nBkwd  = numBackward;
	nFwrd  = numForward;
	//faceId = faceIndex;
}