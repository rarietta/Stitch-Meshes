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
#include <maya/MItMeshPolygon.h>
#include <maya/MItMeshEdge.h>
#include <maya/MFnMesh.h>
#include <maya/MPointArray.h>

#include <maya/MIOStream.h>
#include "PolyMeshFace.h"
#include <vector>

using namespace std;

typedef vector<PolyMeshFace> PolyMeshFaceLoop;

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

	vector<PolyMeshFaceLoop> MPolyMeshFaceLoops;

	//----------------------------------------------------------------------//
	// Node Attributes														//
	//----------------------------------------------------------------------//

	static MObject	inputMesh;
	static MObject	stitchSize;
	static MObject	outputMesh;
	static MTypeId	id;
	int numLoopFaces;

protected:

	// callback function for edge loop selection
	void edgeSelectCB(MFnMesh &inputMeshFn, MItMeshEdge &inputMeshEdgeIt);

	// function for selecting/defining stitch face loops
	MStatus defineStitchLoops(MFnMesh &inputMeshFn, MItMeshEdge &inputMeshEdgeIt);

	// function for performing tessellation
	MStatus tessellateInputMesh(int numPolyMeshFaceLoops, float stitchSizeData,
								MFnMesh &inputMeshFn, MFnMesh &outputMeshFn);
};
