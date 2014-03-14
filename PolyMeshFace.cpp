#include "PolyMeshFace.h"


PolyMeshFace::PolyMeshFace(void)
{
}

PolyMeshFace::PolyMeshFace(MIntArray& cBkwd, MIntArray& cFwrd)
{
	courseEdgeBkwd.copy(cBkwd);
	courseEdgeFwrd.copy(cFwrd);
}

PolyMeshFace::~PolyMeshFace(void)
{
	courseEdgeBkwd.clear();
	courseEdgeFwrd.clear();
}

MStatus	PolyMeshFace::getWaleEdge1(int2& waleEdge) {
	waleEdge[0] = courseEdgeBkwd[0];
	waleEdge[1] = courseEdgeFwrd[0];
	return MStatus::kSuccess;
}

MStatus PolyMeshFace::getWaleEdge2(int2& waleEdge) {
	waleEdge[0] = courseEdgeBkwd[courseEdgeBkwd.length()-1];
	waleEdge[1] = courseEdgeFwrd[courseEdgeFwrd.length()-1];
	return MStatus::kSuccess;
}

MStatus PolyMeshFace::getCourseEdgeBkwd(MIntArray& bkwdEdge) {
	bkwdEdge.copy(courseEdgeBkwd);
	return MStatus::kSuccess;
}

MStatus PolyMeshFace::getCourseEdgeFwrd(MIntArray& fwrdEdge) {
	fwrdEdge.copy(courseEdgeFwrd);
	return MStatus::kSuccess;
}