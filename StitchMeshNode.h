//======================================================================================================================//
//======================================================================================================================//
// StitchMeshNode.cpp																									//
//																														//
// Ricky Arietta																										//
// University of Pennsylvania																							//
// CIS660 Spring 2014																									//
//																														//
// Create an StitchMesh from an input poly mesh.																		//
//======================================================================================================================//
//======================================================================================================================//

#include <string>
#include <vector>
#include <stdio.h>
#include <string.h>

#include "Stitch.h"
#include "SubFace.h"
#include "QuickUnion.h"
#include "PolyMeshFace.h"

#include <maya/MTime.h>
#include <maya/MPlug.h>
#include <maya/MPoint.h>
#include <maya/MGlobal.h>
#include <maya/MFnMesh.h>
#include <maya/MMatrix.h>
#include <maya/MPxNode.h>
#include <maya/MObject.h>
#include <maya/MFnMesh.h>
#include <maya/MDagPath.h>
#include <maya/MIOStream.h>
#include <maya/MIntArray.h>
#include <maya/MDataBlock.h>
#include <maya/MFloatPoint.h>
#include <maya/MFnMeshData.h>
#include <maya/MItMeshEdge.h>
#include <maya/MPointArray.h>
#include <maya/MFnTransform.h>
#include <maya/MDoubleArray.h>
#include <maya/MFnStringData.h>
#include <maya/MEventMessage.h>
#include <maya/MSceneMessage.h>
#include <maya/MEulerRotation.h>
#include <maya/MItMeshPolygon.h>
#include <maya/MSelectionList.h>
#include <maya/MItSelectionList.h>
#include <maya/MFloatPointArray.h>
#include <maya/MFnUnitAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnNumericAttribute.h>

using namespace std;

enum {LOOP_SELECTION, TESSELLATION, STITCH_EDITING, YARN_GENERATION};
enum {X, Y1, S, K, P, SK, KP, D12K, K1Y, PY, YKY, KYK, KPK, D312P, D123K};
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
	static MStatus	initialize();
	
	//----------------------------------------------------------------------//
	// Class variables for stitch direction and tesselation					//
	//----------------------------------------------------------------------//

	MFnMesh					*inputMeshFn;
	MFnMesh					*oMeshFnShape;
	MObject					 outputMeshObj;
	MObject					 inputMeshObj;
	vector<PolyMeshFaceLoop> MPolyMeshFaceLoops;
	vector<SubFace>			 MSubFaces;

	//----------------------------------------------------------------------//
	// Node Attributes														//
	//----------------------------------------------------------------------//

	static MTypeId	id;
	static MObject	attr_nodeStage;
	static MObject	attr_inMesh;
	static MObject	attr_outMesh;
	static MObject	attr_stitchSize;
	static MObject	inputMeshName;
	static MObject	outputMeshName;

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

public:
	
	// initialization functions
	MStatus CreateStitchLibrary(void);

	// function for performing tessellation
	int		ComputeNumWaleDivisions(PolyMeshFaceLoop &currentLoop);
	MStatus InterpolatePoints(vector<MPointArray> &stitchRowPts, PolyMeshFace &currentFace);
	MStatus BuildSubfaces(vector<MPointArray> &stitchRowPts);
	MStatus TessellateInputMesh();

	// stitch level editing functions
	MStatus ColorByStitchType(void);
	MStatus SetDefaultStitchType(int faceId);
	MStatus ChangeStitchType(int faceId, int stitchType);
	MStatus InsertWaleEdge(int vertexId1, int vertexId2);
	MStatus RemoveWaleEdge(int id);

	// stitch generation functions
	MStatus RelaxMesh();
	MStatus GenerateStitches();
};
