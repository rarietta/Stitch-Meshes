// ==========================================================================
// SitchMeshNode.h
// ==========================================================================

#include <maya/MGlobal.h>
#include <maya/MTime.h>
#include <maya/MFnMesh.h>
#include <maya/MPoint.h>
#include <maya/MFloatPoint.h>
#include <maya/MFloatPointArray.h>
#include <maya/MIntArray.h>
#include <maya/MDoubleArray.h>
#include <maya/MFnUnitAttribute.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnStringData.h>

#include <maya/MPxNode.h>
#include <maya/MObject.h>
#include <maya/MPlug.h>
#include <maya/MDataBlock.h>
#include <maya/MFnMeshData.h>

#include <maya/MIOStream.h>

//======================================================================================//
// Class that extends MObject to account for necessary features							//
// of polygonal input mesh faces after stitch direction decisions						//
//======================================================================================//

class PolyMeshFace : public MObject
{
public:
				PolyMeshFace();
				PolyMeshFace(MIntArray& cBkwd, MIntArray& cFwrd, int faceIndex);
	virtual	   ~PolyMeshFace();

	MIntArray	courseEdgeBkwd;
	MIntArray	courseEdgeFwrd;
	int			faceIndex;

	MStatus		getWaleEdge1(int2& waleEdge1);
	MStatus		getWaleEdge2(int2& waleEdge2);
	MStatus		getCourseEdgeBkwd(MIntArray& bkwdEdge);
	MStatus		getCourseEdgeFwrd(MIntArray& FwrdEdge);
	MStatus		getCourseEdgeBkwd(int2& bkwdEdge);
	MStatus		getCourseEdgeFwrd(int2& FwrdEdge);
};