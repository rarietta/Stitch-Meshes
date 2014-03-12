//==============================================================================================================//
//==============================================================================================================//
// StitchMeshNode.cpp																							//
//																												//
// Ricky Arietta																								//
// University of Pennsylvania																					//
// CIS660 Spring 2014																							//
//																												//
// Create an StitchMesh from an input poly mesh.																//
//																												//
//==============================================================================================================//
//==============================================================================================================//

#include "StitchMeshNode.h"
#include <maya/MSelectionList.h>
#include <maya/MItSelectionList.h>
#include <maya/MFnTransform.h>
#include <maya/MDagPath.h>
#include <maya/MItMeshPolygon.h>
#include <maya/MItMeshEdge.h>
#include <maya/MFnMesh.h>

#define McheckErr(stat,msg)				\
	if ( MS::kSuccess != stat ) {		\
		cerr << stat << ": " << msg;	\
		return MStatus::kFailure;		\
	}

MStatus returnStatus;
MObject StitchMeshNode::inputMesh;
MObject StitchMeshNode::stitchSize;
MObject StitchMeshNode::outputMesh;
MTypeId StitchMeshNode::id( 0x00004 );

//==============================================================================================================//
// Adapted from: http://nccastaff.bournemouth.ac.uk/jmacey/RobTheBloke/www/maya/MSelectionList.html
//==============================================================================================================//

 MStatus StitchMeshNode::GetSelectedMesh(MObjectArray& objects) {
	 
	 // get the current selection list from maya
	 MSelectionList selected;
	 MGlobal::getActiveSelectionList(selected);
	 MItSelectionList itSelected(selected);
	 MObject obj;	

	 cout << "number of selected items = " << selected.length() << endl;

	 // iterate through all selected items
	 while (!itSelected.isDone()) {

		MDagPath dagPath;
		MObject	 component;

		itSelected.getDependNode(obj);

		// if the selected object is of the type we are looking for
		if (obj.hasFn(MFn::kMesh)) { objects.append(obj); }
		
		// if the selected object is a transform, check it's kids
		else if (obj.hasFn(MFn::kTransform)) {
		
			MFnTransform fn(obj);
			
			// loop through each child of the transform
			for (int j = 0; j < fn.childCount(); j++)
			{
				// retrieve the j'th child of the transform node
				MObject child = fn.child(j);
			
				// if the child is MFn::kMesh append it to the list
				if (child.hasFn(MFn::kMesh)) { objects.append(child); }
			}
		}

		itSelected.next();
	}
	cout << objects.length() << endl;
	return MStatus::kSuccess;
 }

//==============================================================================================================//
// Initialize the Node                                                                                          //
//==============================================================================================================//
void* StitchMeshNode::creator()
{
	StitchMeshNode *newNode = new StitchMeshNode;
	/*
	// Get selected object
	MObjectArray objs;
	GetSelectedMesh(objs);

	// Assert that only one mesh object is given as input
	if (objs.length() != 1) {
		cerr << "Incorrect number of meshes selected." << endl;
		cerr << "Please select one poly mesh for the StitchMeshes tool.\n" << endl;
		exit(-1);
	}

	cout << objs[0].apiTypeStr() << endl;

	newNode->inputMesh = objs[0];*/
	return newNode;
}

//==============================================================================================================//
// Initialize the Node                                                                                          //
//==============================================================================================================//
MStatus StitchMeshNode::initialize()
{
	//----------------------------------------------------------------------------------------------------------//
	// Initialize Variables																		                //
	//----------------------------------------------------------------------------------------------------------//
	
	MFnUnitAttribute uAttr;
	MFnTypedAttribute tAttr;
	MFnNumericAttribute nAttr;

	//----------------------------------------------------------------------------------------------------------//
	// Create Default Stitch Size Attribute														                //
	//----------------------------------------------------------------------------------------------------------//

	StitchMeshNode::stitchSize = nAttr.create("stitchSize", "ss", MFnNumericData::kFloat, 1.0, &returnStatus);
	McheckErr(returnStatus, "ERROR creating StitchMeshNode stitchSize attribute\n");
	nAttr.setSoftMin(0.0);
	nAttr.setSoftMax(90.0);
	
	//----------------------------------------------------------------------------------------------------------//
	// Create Input Mesh Attribute																                //
	//----------------------------------------------------------------------------------------------------------//
	
	StitchMeshNode::inputMesh = tAttr.create("inputMesh", "in", MFnData::kMesh, &returnStatus);
	McheckErr(returnStatus, "ERROR creating StitchMeshNode output attribute\n");
	
	//----------------------------------------------------------------------------------------------------------//
	// Create Output Mesh Attribute																                //
	//----------------------------------------------------------------------------------------------------------//

	StitchMeshNode::outputMesh = tAttr.create("outputMesh", "out", MFnData::kMesh, &returnStatus); 
	tAttr.setStorable(false);
	McheckErr(returnStatus, "ERROR creating StitchMeshNode output attribute\n");

	//----------------------------------------------------------------------------------------------------------//
	//  Add Attributes to Node                                                                                  //
	//----------------------------------------------------------------------------------------------------------//

	// Add input mesh attribute
	returnStatus = addAttribute(StitchMeshNode::inputMesh);
	McheckErr(returnStatus, "ERROR adding time attribute\n");

	// Add stitch size attribute
	returnStatus = addAttribute(StitchMeshNode::stitchSize);
	McheckErr(returnStatus, "ERROR adding stitchSize attribute\n");

	// Add mesh attribute
	returnStatus = addAttribute(StitchMeshNode::outputMesh);
	McheckErr(returnStatus, "ERROR adding outputMesh attribute\n");

	//----------------------------------------------------------------------------------------------------------//
	// Attach attributes to affect output mesh												 	                //
	//----------------------------------------------------------------------------------------------------------//
	
	returnStatus = attributeAffects(StitchMeshNode::inputMesh,	StitchMeshNode::outputMesh);
	returnStatus = attributeAffects(StitchMeshNode::stitchSize, StitchMeshNode::outputMesh);
	McheckErr(returnStatus, "ERROR in attributeAffects\n");
	
	//----------------------------------------------------------------------------------------------------------//
	// Return successful initialization															                //
	//----------------------------------------------------------------------------------------------------------//
	
	return MStatus::kSuccess;
}


//======================================================================================================================//
// Compute the new Node when the parameters are adjusted																//
//======================================================================================================================//
MStatus StitchMeshNode::compute(const MPlug& plug, MDataBlock& data)
{
	//------------------------------------------------------------------------------------------------------------------//
	// Assure valid plug, then compute																					//
	//------------------------------------------------------------------------------------------------------------------//
	
	if (plug == outputMesh) {
	
		//--------------------------------------------------------------------------------------------------------------//
		// Get input attributes																							//
		//--------------------------------------------------------------------------------------------------------------//
	
		// Get input mesh object
		MDataHandle inputMeshDataHandle = data.inputValue(inputMesh, &returnStatus); 
		McheckErr(returnStatus, "Error getting input mesh data handle\n");
		MObject inputMeshData = inputMeshDataHandle.asMesh();
		
		// Get default stitch size
		MDataHandle stitchSizeDataHandle = data.inputValue(stitchSize, &returnStatus); 
		McheckErr(returnStatus, "Error getting stitch size data handle\n");
		float stitchSizeData = stitchSizeDataHandle.asFloat();

		//--------------------------------------------------------------------------------------------------------------//
		// TEMPORARY HACK - Get selected input mesh																		//
		//--------------------------------------------------------------------------------------------------------------//
		/*
		MObjectArray objs;
		GetSelectedMesh(objs);

		// Assert that only one mesh object is given as input
		if (objs.length() != 1) {
			cerr << "Incorrect number of meshes selected." << endl;
			cerr << "Please select one poly mesh for the StitchMeshes tool.\n" << endl;
			return MStatus::kFailure;
		}
		*/
		// Important Maya API function sets
		MDagPath inputDagPath;
		MDagPath::getAPathTo(inputMeshData, inputDagPath);	// path to input mesh object in DAG
		MFnMesh inputMeshFn(inputDagPath);					// mesh function set for mesh object
		MItMeshPolygon inputItFaces(inputDagPath);			// face iterator for mesh object
		MItMeshEdge inputItEdges(inputDagPath);				// edge iterator for mesh object

		//--------------------------------------------------------------------------------------------------------------//
		// Get input data from edge loop/stitch direction specification by user											//
		//--------------------------------------------------------------------------------------------------------------//

		int numPolyFaceLoops = 1;
		MIntArray *MWaleEdges		= new MIntArray[numPolyFaceLoops];	// indices of wale edges for poly loop
		MIntArray *MCourseEdgesBkwd = new MIntArray[numPolyFaceLoops];	// indices of backwards course edges for poly loop 
		MIntArray *MCourseEdgesFwd	= new MIntArray[numPolyFaceLoops];	// indices of forwards course edges for poly loop
		
		// HARDCODED HACK FOR NOW
		int waleEdges[] = {4, 5};
		int numWaleEdges = sizeof(waleEdges) / sizeof(waleEdges[0]);
		MWaleEdges[0] = MIntArray(waleEdges, numWaleEdges);

		//--------------------------------------------------------------------------------------------------------------//
		// Get output handle for mesh creation																			//
		//--------------------------------------------------------------------------------------------------------------//
		
		// get handle to output attribute
		MDataHandle outputHandle = data.outputValue(outputMesh, &returnStatus);
		McheckErr(returnStatus, "ERROR getting output mesh data handle\n");

		// create output object
		MFnMeshData dataCreator;
		MObject newOutputData = dataCreator.create(&returnStatus);
		McheckErr(returnStatus, "ERROR creating outputData");

		// put copy of input mesh in output object
		MFnMesh outputMeshFn;
		outputMeshFn.copy(inputMeshData, newOutputData);

		//--------------------------------------------------------------------------------------------------------------//
		// Iterate through all poly face edge loops to tesselate														//
		//--------------------------------------------------------------------------------------------------------------//

		for (int n = 0; n < numPolyFaceLoops; n++)
		{
			//----------------------------------------------------------------------------------------------------------//
			// Find the average length of the wale edges in order to determine the uniform number of subdivisions		//
			// along the entirety of the input face loop																//
			//----------------------------------------------------------------------------------------------------------//
			
			int2 vertices;
			MPoint v0, v1;
			double totalLength;
			for (int i = 0; i < numWaleEdges; i++) {
				outputMeshFn.getEdgeVertices(MWaleEdges[n][i], vertices);
				outputMeshFn.getPoint(vertices[0], v0);
				outputMeshFn.getPoint(vertices[1], v1);
				totalLength += (v0 - v1).length();
			} 
			double avgLength = totalLength / (double) numWaleEdges;
			int numWaleDivisions = floor(avgLength / stitchSizeData);
		
			//----------------------------------------------------------------------------------------------------------//
			// Subdivide the wale edges of the input mesh and split the polygon faces across the wale edges in sequence	//
			//----------------------------------------------------------------------------------------------------------//
			
			/*
			double edgeLength;
			MIntArray edgeID(1,0);
			for (int i = 0; i < numWaleEdges;i++) {
				edgeID.set(MWaleEdges[n][i], 0);
				outputMeshFn.subdivideEdges(edgeID, numWaleDivisions);
			}
			*/
			
			// sequence of internal points
			MFloatPointArray MInternalPoints;
			MInternalPoints.clear();

			// placement types (internal point vs. edge point)
			MIntArray MPlacements;
			MPlacements.clear();
			for (int i = 0; i < numWaleEdges; i++)
				MPlacements.append(MFnMesh::kOnEdge);
		
			// sequence of edge indices
			MIntArray MEdgeList;
			MEdgeList.clear();
			for (int i = 0; i < numWaleEdges; i++)
				MEdgeList.append(waleEdges[i]);
		
			// sequence of edge factors
			// (length along edges for all edge subpoints)
			MFloatArray MEdgeFactors;
			for (int i = numWaleDivisions; i > 1; i--)
			{
				MEdgeFactors.clear();
				for (int j = 0; j < numWaleEdges; j++)
					MEdgeFactors.append(1.0 - (1.0/i));
				returnStatus = outputMeshFn.split(MPlacements, MEdgeList, MEdgeFactors, MInternalPoints);
				cerr << "splitStatus = " << returnStatus << endl;
			}
		
			outputMeshFn.updateSurface();
		}

		outputHandle.set(newOutputData);
		data.setClean( plug );
		
		//delete MWaleEdges;
		//delete MCourseEdgesBkwd;
		//delete MCourseEdgesFwd;
	} 
	
	//------------------------------------------------------------------------------------------------------------------//
	// Return if output plug is invalid																					//
	//------------------------------------------------------------------------------------------------------------------//
	
	else return MS::kUnknownParameter;
	
	//------------------------------------------------------------------------------------------------------------------//
	// Return success																									//
	//------------------------------------------------------------------------------------------------------------------//
	
	return MStatus::kSuccess;
}
