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
#include <maya/MPointArray.h>
#include <vector>

using namespace std;

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
// Initialize the Node                                                                                          //
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

		int numPolyMeshFaceLoops = 1;
		for (int i = 0; i < MPolyMeshFaceLoops.size(); i++)
			MPolyMeshFaceLoops[i].clear();
		MPolyMeshFaceLoops.resize(numPolyMeshFaceLoops);
		MIntArray cB, cF; 
		
		// polyMeshFace1
		cB.clear(); cF.clear();
		cB.append(0); cB.append(1);
		cF.append(2); cF.append(3);
		PolyMeshFace pmf1(cB, cF);
		MPolyMeshFaceLoops[0].push_back(pmf1);
		
		// polyMeshFace2
		cB.clear(); cF.clear();
		cB.append(1); cB.append(7);
		cF.append(3); cF.append(5);
		PolyMeshFace pmf2(cB, cF);
		MPolyMeshFaceLoops[0].push_back(pmf2);
		
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
				cout << "w0 = (" << w0[0] << ", " << w0[1] << ", " << w0[2] << ")" << endl;
				cout << "w1 = (" << w1[0] << ", " << w1[1] << ", " << w1[2] << ")" << endl;
				cout << "length of (w0, w1) = " << (w1-w0).length() << "\n" << endl;
				totalLength += (w1 - w0).length();

				// get second wale edge only for final face
				// (assuming potential for unclosed loops)
				if (i == currentLoop.size()-1) {
					currentFace.getWaleEdge1(vertices);
					inputMeshFn.getPoint(vertices[0], w0);
					inputMeshFn.getPoint(vertices[1], w1);
					totalLength += (w1 - w0).length();
				}
			} 

			// divide total sum by number of edges
			double avgLength = totalLength / (currentLoop.size() + 1);
			int numWaleDivisions = floor(avgLength / (stitchSizeData));
			cout << "totalLength = " << totalLength << endl;
			cout << "averageLength = " << avgLength << endl;
			cout << "stitchSizeData = " << stitchSizeData << endl;
			cout << "numDivisions = " << numWaleDivisions << "/n" << endl;
			cout << "--------------------------------------" << endl;

			//----------------------------------------------------------------------------------------------------------//
			// Create tessellated polygon subfaces																		//
			//----------------------------------------------------------------------------------------------------------//

			// loop through each face in loop to tessellate
			for (int i = 0; i < currentLoop.size(); i++)
			{
				MPoint v0, v1;
				int2 waleVtxs;
				MIntArray courseVtxs;
				PolyMeshFace currentFace = currentLoop[i];
				
				// HACK FOR QUAD FACE
				// get backwards-first corner as origin
				MPoint origin;
				inputMeshFn.getPoint(currentFace.courseEdgeBkwd[0], origin);

				// HACK FOR QUAD FACE
				// vector corresponding to first wale edge
				MFloatVector wale1Dir;
				currentFace.getWaleEdge1(waleVtxs); 
				inputMeshFn.getPoint(waleVtxs[0], v0);
				inputMeshFn.getPoint(waleVtxs[1], v1);
				wale1Dir = v1-v0;

				// HACK FOR QUAD FACE
				// vector corresponding to second wale edge
				MFloatVector wale2Dir;
				currentFace.getWaleEdge2(waleVtxs); 
				inputMeshFn.getPoint(waleVtxs[0], v0);
				inputMeshFn.getPoint(waleVtxs[1], v1);
				wale2Dir = v1-v0;

				// HACK FOR QUAD FACE
				// vector corresponding to backwards course edge
				MFloatVector course1Dir;
				currentFace.getCourseEdgeBkwd(courseVtxs);
				inputMeshFn.getPoint(courseVtxs[0], v0);
				inputMeshFn.getPoint(courseVtxs[1], v1);
				course1Dir = v1-v0;
				
				// HACK FOR QUAD FACE
				// vector corresponding to backwards course edge
				MFloatVector course2Dir;
				currentFace.getCourseEdgeBkwd(courseVtxs);
				inputMeshFn.getPoint(courseVtxs[0], v0);
				inputMeshFn.getPoint(courseVtxs[1], v1);
				course2Dir = v1-v0;

				MPointArray vertexLoop;
				for (int j = 0; j < numWaleDivisions; j++) {
					for (int k = 0; k < numWaleDivisions; k++) {
						
						// clear current face loop
						vertexLoop.clear();

						// get points in counterclockwise order
						MPoint p0 = origin
							+ (wale1Dir * j / numWaleDivisions) * (1.0 - (double)k/numWaleDivisions)
							+ (wale2Dir * j / numWaleDivisions) * ((double)k/numWaleDivisions) 
							+ (course1Dir * k / numWaleDivisions) * (1.0 - (double)j/numWaleDivisions)
							+ (course2Dir * k / numWaleDivisions) * ((double)j/numWaleDivisions);
						MPoint p1 = origin 
							+ (wale1Dir * (j+1) / numWaleDivisions) * (1.0 - (double)k/numWaleDivisions)
							+ (wale2Dir * (j+1) / numWaleDivisions) * ((double)k/numWaleDivisions)
							+ (course1Dir *   k   / numWaleDivisions) * (1.0 - (double)(j+1)/numWaleDivisions)
							+ (course2Dir *   k   / numWaleDivisions) * ((double)(j+1)/numWaleDivisions);
						MPoint p2 = origin 
							+ (wale1Dir * (j+1) / numWaleDivisions) * (1.0 - (double)(k+1)/numWaleDivisions)
							+ (wale2Dir * (j+1) / numWaleDivisions) * ((double)(k+1)/numWaleDivisions)
							+ (course1Dir * (k+1) / numWaleDivisions) * (1.0 - (double)(j+1)/numWaleDivisions)
							+ (course2Dir * (k+1) / numWaleDivisions) * ((double)(j+1)/numWaleDivisions);
						MPoint p3 = origin 
							+ (wale1Dir *   j   / numWaleDivisions) * (1.0 - (double)(k+1)/numWaleDivisions)
							+ (wale2Dir *   j   / numWaleDivisions) * ((double)(k+1)/numWaleDivisions)
							+ (course1Dir * (k+1) / numWaleDivisions) * (1.0 - (double)j/numWaleDivisions)
							+ (course2Dir * (k+1) / numWaleDivisions) * ((double)j/numWaleDivisions);

						// append to list
						vertexLoop.append(p0);
						vertexLoop.append(p1);
						vertexLoop.append(p2);
						vertexLoop.append(p3);

						// add polygon to mesh
						outputMeshFn.addPolygon(vertexLoop);

					} // j face direction loop
				} // k face direction loop 
			} // per face loop
		} // per face-loop loop

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
