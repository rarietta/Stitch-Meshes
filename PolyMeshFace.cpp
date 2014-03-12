#include "PolyMeshFace.h"


PolyMeshFace::PolyMeshFace(void)
{
}

PolyMeshFace::PolyMeshFace(MIntArray& cBkwd, MIntArray& cFwd)
{
	courseEdgeBkwd.copy(cBkwd);
	courseEdgeFwd.copy(cFwd);
	
	for (int i = 0; i < courseEdgeBkwd.length(); i++)
		cout << courseEdgeBkwd[i] << "\t";
	cout << "\n";
	
	for (int i = 0; i < courseEdgeFwd.length(); i++)
		cout << courseEdgeFwd[i] << "\t";
	cout << "\n";
}

PolyMeshFace::~PolyMeshFace(void)
{
	courseEdgeBkwd.clear();
	courseEdgeFwd.clear();
}

MStatus	PolyMeshFace::getWaleEdge1(int2& waleEdge) {
	waleEdge[0] = courseEdgeBkwd[0];
	waleEdge[1] = courseEdgeFwd[0];
	return MStatus::kSuccess;
}

MStatus PolyMeshFace::getWaleEdge2(int2& waleEdge) {
	waleEdge[0] = courseEdgeBkwd[courseEdgeBkwd.length()-1];
	waleEdge[1] = courseEdgeFwd[courseEdgeFwd.length()-1];
	return MStatus::kSuccess;
}

MStatus PolyMeshFace::getCourseEdgeBkwd(MIntArray& bkwdEdge) {
	bkwdEdge.copy(courseEdgeBkwd);
	return MStatus::kSuccess;
}

MStatus PolyMeshFace::getCourseEdgeFwd(MIntArray& fwdEdge) {
	fwdEdge.copy(courseEdgeFwd);
	return MStatus::kSuccess;
}