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

class StitchMeshNode : public MPxNode
{
public:

	//----------------------------------------------------------------------//
	// Functions															//
	//----------------------------------------------------------------------//

					StitchMeshNode() {};
	virtual 		~StitchMeshNode() {};
	virtual MStatus compute(const MPlug& plug, MDataBlock& data);
	static  void*	creator();
	static	MStatus initialize();
	
	//----------------------------------------------------------------------//
	// Class variables for stitch direction and tesselation					//
	//----------------------------------------------------------------------//

	MIntArray*		MWaleEdges;
	MIntArray*		MCourseEdgesBkwd;
	MIntArray*		MCourseEdgesFwd;

	//----------------------------------------------------------------------//
	// Node Attributes														//
	//----------------------------------------------------------------------//

	static MObject	inputMesh;
	static MObject	stitchSize;
	static MObject	outputMesh;
	static MTypeId	id;

protected:
	static MStatus GetSelectedMesh(MObjectArray &objects);
	//MObject createMesh(MObject& outData, MStatus& stat);
};