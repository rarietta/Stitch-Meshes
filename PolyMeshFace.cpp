#include "PolyMeshFace.h"


PolyMeshFace::PolyMeshFace(void)
{
}

PolyMeshFace::PolyMeshFace(MIntArray& cBkwd, MIntArray& cFwrd, int faceId)
{
	courseEdgeBkwd.copy(cBkwd);
	courseEdgeFwrd.copy(cFwrd);
	faceIndex = faceId;
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

MStatus PolyMeshFace::getCourseEdgeBkwd(int2& bkwdEdge) {
	bkwdEdge[0] = courseEdgeBkwd[0];
	bkwdEdge[1] = courseEdgeBkwd[1];
	return MStatus::kSuccess;
}

MStatus PolyMeshFace::getCourseEdgeFwrd(MIntArray& fwrdEdge) {
	fwrdEdge.copy(courseEdgeFwrd);
	return MStatus::kSuccess;
}

MStatus PolyMeshFace::getCourseEdgeFwrd(int2& fwrdEdge) {
	fwrdEdge[0] = courseEdgeFwrd[0];
	fwrdEdge[1] = courseEdgeFwrd[1];
	return MStatus::kSuccess;
}