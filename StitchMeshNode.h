// ==========================================================================
// SitchMeshNode.h
// ==========================================================================

#include <string>
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

#include <maya/MMatrix.h>
#include <maya/MPxNode.h>
#include <maya/MObject.h>
#include <maya/MPlug.h>
#include <maya/MDataBlock.h>
#include <maya/MFnMeshData.h>
#include <maya/MItMeshPolygon.h>
#include <maya/MItMeshEdge.h>
#include <maya/MFnMesh.h>
#include <maya/MPointArray.h>
#include <maya/MEventMessage.h>
#include <maya/MSceneMessage.h>
#include <maya/MEulerRotation.h>

#include <maya/MIOStream.h>
#include "PolyMeshFace.h"
#include "SubFace.h"
#include "Stitch.h"
#include <vector>
#include <stdio.h>
#include <string.h>

using namespace std;

enum {LOOP_SELECTION, TESSELLATION, STITCH_EDITING, YARN_GENERATION};
enum {P, PY, YKY, KPK, D312P, K1Y, S, SK, X, Y1, K, KP, D12K, KYK, D123K};
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
	static MStatus initialize();
	
	//----------------------------------------------------------------------//
	// Class variables for stitch direction and tesselation					//
	//----------------------------------------------------------------------//

	MFnMesh* inputMeshFn;
	MItMeshEdge *inputMeshItEdges;
	MItMeshPolygon *inputMeshItFaces;
	vector<PolyMeshFaceLoop> MPolyMeshFaceLoops;
	vector<SubFace> MSubFaces;

	//----------------------------------------------------------------------//
	// Node Attributes														//
	//----------------------------------------------------------------------//

	static MObject	inputMesh;
	static MObject	outputMesh;
	static MObject	inputMeshName;
	static MObject	outputMeshName;
	static MObject	nodeStage;
	static MObject	stitchSize;
	static MTypeId	id;

	//----------------------------------------------------------------------//
	// Stitch mesh node variables											//
	//----------------------------------------------------------------------//
	
	int numLoopFaces;
	MCallbackId callbackId;
	MIntArray faceLoopIndex;		// index of each face in its face loop
	MIntArray faceLoopNumber;		// index of loop that each face is in

	//----------------------------------------------------------------------//
	// Library of stitch types												//
	//----------------------------------------------------------------------//

	std::vector<Stitch> stitches;

protected:

	// get the currently selected edge
	//int getSelectedEdge(MItMeshEdge &edgeIt);
	static bool getSelectedEdge(int &index);

	// function for selecting/defining stitch face loops
	MStatus defineStitchLoops(void);

	// function for performing tessellation
	MStatus tessellateInputMesh(float stitchSizeData, MFnMesh &outputMeshFn);
};
