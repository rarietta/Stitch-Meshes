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
#include <vector>

using namespace std;

#define McheckErr(stat,msg)				\
	if ( MS::kSuccess != stat ) {		\
		cerr << stat << ": " << msg;	\
		return MStatus::kFailure;		\
	}

#define LerpInt(v1,v2,pct)				\
	v1 + floor(pct*(v2-v1))

#define LerpVec(v1,v2,pct)				\
	v1 + pct*(v2-v1)

MStatus returnStatus;
MObject StitchMeshNode::inputMesh;
MObject StitchMeshNode::stitchSize;
MObject StitchMeshNode::outputMesh;
MTypeId StitchMeshNode::id( 0x00004 );


//==============================================================================================================//
// Tessellate the Input Mesh																					//
//==============================================================================================================//
MStatus StitchMeshNode::tessellateInputMesh(int numPolyMeshFaceLoops, float stitchSizeData,
											MFnMesh &inputMeshFn, MFnMesh &outputMeshFn)
{
	//--------------------------------------------------------------------------------------------------------------//
	// Iterate through all poly face edge loops to tesselate														//
	//--------------------------------------------------------------------------------------------------------------//

	for (int n = 0; n < numPolyMeshFaceLoops; n++)
	{
		//----------------------------------------------------------------------------------------------------------//
		// Find the average length of the wale edges in order to determine the uniform number of subdivisions		//
		// along the entirety of the input face loop																//
		//----------------------------------------------------------------------------------------------------------//
			
		// get current poly mesh face loop
		PolyMeshFaceLoop currentLoop = MPolyMeshFaceLoops[n];

		// local variables
		int2 vertices; 
		MPoint w0, w1;
		double totalLength = 0.0;
			
		// loop through all faces to examine wale edge lengths
		for (int i = 0; i < currentLoop.size(); i++) {

			// get first wale edge
			PolyMeshFace currentFace = currentLoop[i];
			currentFace.getWaleEdge1(vertices);
			inputMeshFn.getPoint(vertices[0], w0);
			inputMeshFn.getPoint(vertices[1], w1);
			totalLength += (w1 - w0).length();

			// get second wale edge only for final face
			// (assuming potential for unclosed loops)
			if (i == currentLoop.size()-1) {
				currentFace.getWaleEdge2(vertices);
				inputMeshFn.getPoint(vertices[0], w0);
				inputMeshFn.getPoint(vertices[1], w1);
				totalLength += (w1 - w0).length();
			}
		} 

		// divide total sum by number of edges
		double avgLength = totalLength / (currentLoop.size() + 1);
		int numWaleDivisions = floor(avgLength / (stitchSizeData));

		//----------------------------------------------------------------------------------------------------------//
		// Loop through each face of the current loop to create tessellated polygon subfaces						//
		//----------------------------------------------------------------------------------------------------------//

		for (int f = 0; f < currentLoop.size(); f++)
		{
			//------------------------------------------------------------------------------------------------------//
			// Get current PolyMeshFace																				//
			//------------------------------------------------------------------------------------------------------//

			PolyMeshFace currentFace = currentLoop[f];

			//------------------------------------------------------------------------------------------------------//
			// Determine edge direction data for current face (HACK FOR QUAD FACES)									//
			//------------------------------------------------------------------------------------------------------//

			MPoint v0, v1;
			int2 waleVtxs;
			MIntArray courseVtxs;
				
			// get backwards-first corner as origin
			MPoint origin;
			inputMeshFn.getPoint(currentFace.courseEdgeBkwd[0], origin);

			// vector corresponding to first wale edge
			MFloatVector wale1Dir;
			currentFace.getWaleEdge1(waleVtxs); 
			inputMeshFn.getPoint(waleVtxs[0], v0);
			inputMeshFn.getPoint(waleVtxs[1], v1);
			wale1Dir = v1-v0;

			// vector corresponding to second wale edge
			MFloatVector wale2Dir;
			currentFace.getWaleEdge2(waleVtxs); 
			inputMeshFn.getPoint(waleVtxs[0], v0);
			inputMeshFn.getPoint(waleVtxs[1], v1);
			wale2Dir = v1-v0;

			// vector corresponding to backwards course edge
			MFloatVector course1Dir;
			currentFace.getCourseEdgeBkwd(courseVtxs);
			inputMeshFn.getPoint(courseVtxs[0], v0);
			inputMeshFn.getPoint(courseVtxs[1], v1);
			course1Dir = v1-v0;
				
			// vector corresponding to forwards course edge
			MFloatVector course2Dir;
			currentFace.getCourseEdgeFwrd(courseVtxs);
			inputMeshFn.getPoint(courseVtxs[0], v0);
			inputMeshFn.getPoint(courseVtxs[1], v1);
			course2Dir = v1-v0;

			//------------------------------------------------------------------------------------------------------//
			// Determine number of course edge subdivisions for the current face									//
			//------------------------------------------------------------------------------------------------------//

			double courseEdgeBkwdLength = course1Dir.length();
			double courseEdgeFwrdLength = course2Dir.length();
			int numCourseDivisionsBkwd = floor(courseEdgeBkwdLength / (stitchSizeData));
			int numCourseDivisionsFwrd = floor(courseEdgeFwrdLength / (stitchSizeData));
			cout << "num bkwd div = " << numCourseDivisionsBkwd << endl;
			cout << "num fwrd div = " << numCourseDivisionsFwrd << endl;

			//------------------------------------------------------------------------------------------------------//
			// Create vector of MPointArrays to store interpolated stitch row points								//
			// (including along course edges, so there are numWaleDivisions+1 stitch rows)							//
			//------------------------------------------------------------------------------------------------------//

			// allocate stitch row point arrays
			vector<MPointArray> stitchRowPts;
			for (int i = 0; i < stitchRowPts.size(); i++) { stitchRowPts[i].clear(); }
			stitchRowPts.resize(numWaleDivisions + 1);
				
			// populate stitch row point arrays
			for (int u = 0; u <= numWaleDivisions; u++) 
			{
				// u-direction percentage
				double uPct = (double) u / numWaleDivisions;

				// determine number of points to add for current stitch row r
				int numRowPts = LerpInt(numCourseDivisionsBkwd, numCourseDivisionsFwrd, uPct);

				// for each point in row
				for (int v = 0; v <= numRowPts; v++) 
				{
					// v direction percentage
					double vPct = (double) v / numRowPts;

					// determine point location
					MPoint pt = origin + wale1Dir*uPct + LerpVec(course1Dir*vPct, course2Dir*vPct, uPct);
						
					// add point to row-specific MPointArray
					stitchRowPts[u].append(pt);
				}
			}

			//------------------------------------------------------------------------------------------------------//
			// Add interior faces to polygon based on stored stitch row points										//
			//------------------------------------------------------------------------------------------------------//

			MPointArray vertexLoop;

			for (int u = 0; u < numWaleDivisions; u++) 
			{
				//--------------------------------------------------------------------------------------------------//
				// Add regular quad face stitches																	//
				//--------------------------------------------------------------------------------------------------//

				// find smaller number of points between two consecutive stitch rows
				int numPts1 = stitchRowPts[ u ].length();
				int numPts2 = stitchRowPts[u+1].length();
				int minRowPts = min(numPts1, numPts2);

				// use this number to add quad stitch faces
				for (int v = 0; v < minRowPts-1; v++)
				{
					// clear current subface vertex loop
					vertexLoop.clear();

					// get points in counterclockwise order
					vertexLoop.append(stitchRowPts[ u ][ v ]);
					vertexLoop.append(stitchRowPts[ u ][v+1]);
					vertexLoop.append(stitchRowPts[u+1][v+1]);
					vertexLoop.append(stitchRowPts[u+1][ v ]);
						
					// add polygon to mesh
					outputMeshFn.addPolygon(vertexLoop);
				}

				//--------------------------------------------------------------------------------------------------//
				// If the number of points in each row was not equal, add any										//
				// remaining points as an increase / decrease face at the end of the stitch row						//
				//--------------------------------------------------------------------------------------------------//

				// first row has more points
				if (numPts2 <  numPts1) {
					vertexLoop.clear();
					for (int v = numPts2-1; v < numPts1; v++)
						vertexLoop.append(stitchRowPts[u][v]);
					vertexLoop.append(stitchRowPts[u+1][numPts2-1]);
					outputMeshFn.addPolygon(vertexLoop);
				}

				// lower row has more points
				else if (numPts1 < numPts2) {
					vertexLoop.clear();
					for (int v = numPts1-1; v < numPts2; v++)
						vertexLoop.append(stitchRowPts[u+1][v]);
					vertexLoop.append(stitchRowPts[u][numPts1-1]);
					outputMeshFn.addPolygon(vertexLoop);
				}
			} 
		} // per PolyMeshFace loop
	} // per PolyMeshFaceLoop loop

	return MStatus::kSuccess;
}


//==============================================================================================================//
// Create the Node		                                                                                        //
//==============================================================================================================//
void* StitchMeshNode::creator()
{
	return new StitchMeshNode;
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
	nAttr.setMin(0.25);
	nAttr.setMax(8.0);
	nAttr.setSoftMin(0.25);
	nAttr.setSoftMax(8.0);
	
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

/*
MStatus StitchMeshNode::GetSelectedMesh(MObjectArray& objects) {
 +	 
 +	 // get the current selection list from maya
 +	 MSelectionList selected;
 +	 MGlobal::getActiveSelectionList(selected);
 +	 MItSelectionList itSelected(selected);
 +	 MObject obj;	
 +
 +	 cout << "number of selected items = " << selected.length() << endl;
 +
 +	 // iterate through all selected items
 +	 while (!itSelected.isDone()) {
 +
 +		MDagPath dagPath;
 +		MObject	 component;
 +
 +		itSelected.getDependNode(obj);
 +
 +		// if the selected object is of the type we are looking for
 +		if (obj.hasFn(MFn::kMesh)) { objects.append(obj); }
 +		
 +		// if the selected object is a transform, check it's kids
 +		else if (obj.hasFn(MFn::kTransform)) {
 +		
 +			MFnTransform fn(obj);
 +			
 +			// loop through each child of the transform
 +			for (int j = 0; j < fn.childCount(); j++)
 +			{
 +				// retrieve the j'th child of the transform node
 +				MObject child = fn.child(j);
 +			
 +				// if the child is MFn::kMesh append it to the list
 +				if (child.hasFn(MFn::kMesh)) { objects.append(child); }
 +			}
 +		}
 +
 +		itSelected.next();
 +	}
 +	cout << objects.length() << endl;
 +	return MStatus::kSuccess;
 + }*/

//======================================================================================================================//
// Callback function for selecting edges																				//
//======================================================================================================================//

void StitchMeshNode::edgeSelectCB(MFnMesh &inputMeshFn, MItMeshEdge &inputMeshEdgeIt)
{
	MIntArray connectedEdges;
	MIntArray connectedFaces;
	connectedEdges.clear();
	connectedFaces.clear();

	// MStatus edgeSelected = getSelectedEdge();
	// if (edgeSelected && numLoopFaces == 0)
		// this is first loop
		// check if boundary edge
		// if not, return
		// process boundary loops
		// add new face loop
		// store new forward edge
	// if (edgeSelected && numLoopFaces != 0)
		// this is not first loop
		// check if edge in standing forward edge
		// if not, return
		// process interior loops
		// add new face loop
		// store new forward edge
}

//--------------------------------------------------------------------------------------------------------------//
// User interaction stage for selecting edge loops in direction of stitch
//--------------------------------------------------------------------------------------------------------------//

MStatus StitchMeshNode::defineStitchLoops(MFnMesh &inputMeshFn, MItMeshEdge &inputMeshEdgeIt) {

	// add callback function
	
	int numInputFaces = inputMeshFn.numPolygons();
	while (numLoopFaces != numInputFaces) {;}

	// remove callback function

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
		MDagPath inputDagPath;
		MDagPath::getAPathTo(inputMeshData, inputDagPath);		// path to input mesh object in DAG
		MFnMesh inputMeshFn(inputMeshData);						// mesh function set for mesh object
		MItMeshPolygon inputItFaces(inputDagPath);				// face iterator for mesh object
		MItMeshEdge inputItEdges(inputDagPath);					// edge iterator for mesh object
		
		// Get default stitch size
		MDataHandle stitchSizeDataHandle = data.inputValue(stitchSize, &returnStatus); 
		McheckErr(returnStatus, "Error getting stitch size data handle\n");
		float stitchSizeData = stitchSizeDataHandle.asFloat();

		//--------------------------------------------------------------------------------------------------------------//
		// Get input data from edge loop/stitch direction specification by user											//
		// HARD-CODED UNTIL THIS PART IS DONE																			//
		//--------------------------------------------------------------------------------------------------------------//

		int numPolyMeshFaceLoops = 2;
		for (int i = 0; i < MPolyMeshFaceLoops.size(); i++)
			MPolyMeshFaceLoops[i].clear();
		MPolyMeshFaceLoops.resize(numPolyMeshFaceLoops);
		MIntArray cB, cF; 
		
		// Loop 1, Face 1
		cB.clear(); cF.clear();
		cB.append(0); cB.append(1);
		cF.append(2); cF.append(3);
		PolyMeshFace pmf11(cB, cF);
		MPolyMeshFaceLoops[0].push_back(pmf11);
		
		// Loop 1, Face 2
		cB.clear(); cF.clear();
		cB.append(1); cB.append(11);
		cF.append(3); cF.append(9);
		PolyMeshFace pmf12(cB, cF);
		MPolyMeshFaceLoops[0].push_back(pmf12);
		
		// Loop 2, Face 1
		cB.clear(); cF.clear();
		cB.append(2); cB.append(3);
		cF.append(4); cF.append(5);
		PolyMeshFace pmf21(cB, cF);
		MPolyMeshFaceLoops[1].push_back(pmf21);
		
		// Loop 2, Face 2
		cB.clear(); cF.clear();
		cB.append(3); cB.append(9);
		cF.append(5); cF.append(7);
		PolyMeshFace pmf22(cB, cF);
		MPolyMeshFaceLoops[1].push_back(pmf22);
		
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
		// User interaction stage for selecting edge loops in direction of stitch
		//--------------------------------------------------------------------------------------------------------------//

		defineStitchLoops(inputMeshFn, inputItEdges);

		//--------------------------------------------------------------------------------------------------------------//
		// Call tessellation function																					//
		//--------------------------------------------------------------------------------------------------------------//

		tessellateInputMesh(numPolyMeshFaceLoops, stitchSizeData, inputMeshFn, outputMeshFn);
		
		//--------------------------------------------------------------------------------------------------------------//
		// Update output mesh to return																					//
		//--------------------------------------------------------------------------------------------------------------//

		outputMeshFn.updateSurface();
		outputHandle.set(newOutputData);
		data.setClean( plug );
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
