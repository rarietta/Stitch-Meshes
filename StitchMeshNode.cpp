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

#define incrementWithWrap(index, size)	\
	(index + 1 < size) ? (index + 1) : 0

MStatus returnStatus;
MObject StitchMeshNode::inputMesh;
MObject StitchMeshNode::outputMesh;
MObject StitchMeshNode::inputMeshName;
MObject StitchMeshNode::outputMeshName;
MObject StitchMeshNode::nodeStage;
MObject StitchMeshNode::stitchSize;
MTypeId StitchMeshNode::id( 0x00004 );
MString inputShapeName;
MString outputShapeName;

//==============================================================================================================//
// Tessellate the Input Mesh																					//
//==============================================================================================================//

MStatus StitchMeshNode::tessellateInputMesh(float stitchSizeData, MFnMesh &outputMeshFn)
{
	//--------------------------------------------------------------------------------------------------------------//
	// Iterate through all poly face edge loops to tesselate														//
	//--------------------------------------------------------------------------------------------------------------//

	MSubFaces.clear();
	int subfaceId = inputMeshFn->numPolygons();
	int numPolyMeshFaceLoops = (int) MPolyMeshFaceLoops.size();
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
			inputMeshFn->getPoint(vertices[0], w0);
			inputMeshFn->getPoint(vertices[1], w1);
			totalLength += (w1 - w0).length();

			// get second wale edge only for final face
			// (assuming potential for unclosed loops)
			if (i == currentLoop.size()-1) {
				currentFace.getWaleEdge2(vertices);
				inputMeshFn->getPoint(vertices[0], w0);
				inputMeshFn->getPoint(vertices[1], w1);
				totalLength += (w1 - w0).length();
			}
		} 

		// divide total sum by number of edges
		double avgLength = totalLength / (currentLoop.size() + 1);
		int numWaleDivisions = ceil(avgLength / (stitchSizeData));

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
			inputMeshFn->getPoint(currentFace.courseEdgeBkwd[0], origin);

			// vector corresponding to first wale edge
			MFloatVector wale1Dir;
			currentFace.getWaleEdge1(waleVtxs); 
			inputMeshFn->getPoint(waleVtxs[0], v0);
			inputMeshFn->getPoint(waleVtxs[1], v1);
			wale1Dir = v1-v0;

			// vector corresponding to second wale edge
			MFloatVector wale2Dir;
			currentFace.getWaleEdge2(waleVtxs); 
			inputMeshFn->getPoint(waleVtxs[0], v0);
			inputMeshFn->getPoint(waleVtxs[1], v1);
			wale2Dir = v1-v0;

			// vector corresponding to backwards course edge
			MFloatVector course1Dir;
			currentFace.getCourseEdgeBkwd(courseVtxs);
			inputMeshFn->getPoint(courseVtxs[0], v0);
			inputMeshFn->getPoint(courseVtxs[1], v1);
			course1Dir = v1-v0;
				
			// vector corresponding to forwards course edge
			MFloatVector course2Dir;
			currentFace.getCourseEdgeFwrd(courseVtxs);
			inputMeshFn->getPoint(courseVtxs[0], v0);
			inputMeshFn->getPoint(courseVtxs[1], v1);
			course2Dir = v1-v0;

			//------------------------------------------------------------------------------------------------------//
			// Determine number of course edge subdivisions for the current face									//
			//------------------------------------------------------------------------------------------------------//

			double courseEdgeBkwdLength = course1Dir.length();
			double courseEdgeFwrdLength = course2Dir.length();
			int numCourseDivisionsBkwd = ceil(courseEdgeBkwdLength / (stitchSizeData));
			int numCourseDivisionsFwrd = ceil(courseEdgeFwrdLength / (stitchSizeData));

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
					
					// create output subface
					SubFace subface(2, 2, subfaceId);
					subface.bkwdPoints.clear();
					subface.fwrdPoints.clear();
					subface.bkwdPoints.append(stitchRowPts[ u ][ v ]);
					subface.bkwdPoints.append(stitchRowPts[ u ][v+1]);
					subface.fwrdPoints.append(stitchRowPts[u+1][ v ]);
					subface.fwrdPoints.append(stitchRowPts[u+1][v+1]);
					MSubFaces.push_back(subface);

					// add polygon to mesh
					outputMeshFn.addPolygon(vertexLoop, subfaceId++, true, 1.0e-3);
				}

				//--------------------------------------------------------------------------------------------------//
				// If the number of points in each row was not equal, add any										//
				// remaining points as an increase / decrease face at the end of the stitch row						//
				//--------------------------------------------------------------------------------------------------//

				// backward edge has more points
				if (numPts1 > numPts2) {
					vertexLoop.clear();
					for (int v = numPts2-1; v < numPts1; v++)
						vertexLoop.append(stitchRowPts[u][v]);
					vertexLoop.append(stitchRowPts[u+1][numPts2-1]);
					
					// create output subface
					SubFace subface(2, 1, subfaceId);
					for (int v = numPts2-1; v < numPts1; v++)
						subface.bkwdPoints.append(stitchRowPts[u][v]);
					subface.fwrdPoints.append(stitchRowPts[u+1][numPts2-1]);
					MSubFaces.push_back(subface);
					
					// add polygon to mesh
					outputMeshFn.addPolygon(vertexLoop, subfaceId++);
				}

				// forward edge has more points
				else if (numPts2 > numPts1) {
					vertexLoop.clear();
					vertexLoop.append(stitchRowPts[u][numPts1-1]);
					for (int v = numPts2-1; v >= numPts1-1; v--)
						vertexLoop.append(stitchRowPts[u+1][v]);
					
					// create output subface
					SubFace subface(1, 2, subfaceId);
					subface.bkwdPoints.append(stitchRowPts[u][numPts1-1]);
					for (int v = numPts1; v < numPts2; v++)
						subface.fwrdPoints.append(stitchRowPts[u+1][v]);
					MSubFaces.push_back(subface);
					
					// add polygon to mesh
					outputMeshFn.addPolygon(vertexLoop, subfaceId++);
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
	StitchMeshNode *node = new StitchMeshNode();
	node->numLoopFaces = 0;
	node->callbackId = -1;
	node->inputMeshFn = NULL;
	node->inputMeshItEdges = NULL;
	node->inputMeshItFaces = NULL;
	return node;
}

//==============================================================================================================//
// Initialize the Node                                                                                          //
//==============================================================================================================//

MStatus StitchMeshNode::initialize()
{
	//----------------------------------------------------------------------------------------------------------//
	// Initialize Variables																		                //
	//----------------------------------------------------------------------------------------------------------//
	
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
	// Create "Generate Stitch Mesh" Boolean Attribute											                //
	//----------------------------------------------------------------------------------------------------------//

	StitchMeshNode::nodeStage = nAttr.create("nodeStage", "sg", MFnNumericData::kInt, LOOP_SELECTION, &returnStatus);
	McheckErr(returnStatus, "ERROR creating StitchMeshNode nodeStage attribute\n");
	
	//----------------------------------------------------------------------------------------------------------//
	// Create Input Mesh Attribute																                //
	//----------------------------------------------------------------------------------------------------------//
	
	StitchMeshNode::inputMesh = tAttr.create("inputMesh", "in", MFnData::kMesh, &returnStatus);
	StitchMeshNode::inputMeshName = tAttr.create("inputMeshName", "inname", MFnData::kString, &returnStatus);
	McheckErr(returnStatus, "ERROR creating StitchMeshNode output attribute\n");
	
	//----------------------------------------------------------------------------------------------------------//
	// Create Output Mesh Attribute																                //
	//----------------------------------------------------------------------------------------------------------//

	StitchMeshNode::outputMesh = tAttr.create("outputMesh", "out", MFnData::kMesh, &returnStatus); 
	StitchMeshNode::outputMeshName = tAttr.create("outputMeshName", "outname", MFnData::kString, &returnStatus);
	tAttr.setStorable(false);
	McheckErr(returnStatus, "ERROR creating StitchMeshNode output attribute\n");

	//----------------------------------------------------------------------------------------------------------//
	//  Add Attributes to Node                                                                                  //
	//----------------------------------------------------------------------------------------------------------//

	// Add input mesh attribute
	returnStatus = addAttribute(StitchMeshNode::inputMesh);
	returnStatus = addAttribute(StitchMeshNode::inputMeshName);
	McheckErr(returnStatus, "ERROR adding input attributes\n");

	// Add stitch size attribute
	returnStatus = addAttribute(StitchMeshNode::stitchSize);
	McheckErr(returnStatus, "ERROR adding stitchSize attribute\n");

	// Add stitch size attribute
	returnStatus = addAttribute(StitchMeshNode::nodeStage);
	McheckErr(returnStatus, "ERROR adding nodeStage attribute\n");

	// Add mesh attribute
	returnStatus = addAttribute(StitchMeshNode::outputMesh);
	returnStatus = addAttribute(StitchMeshNode::outputMeshName);
	McheckErr(returnStatus, "ERROR adding outputMesh attributes\n");

	//----------------------------------------------------------------------------------------------------------//
	// Attach attributes to affect output mesh												 	                //
	//----------------------------------------------------------------------------------------------------------//
	
	returnStatus = attributeAffects(StitchMeshNode::inputMesh,	StitchMeshNode::outputMesh);
	returnStatus = attributeAffects(StitchMeshNode::nodeStage,  StitchMeshNode::outputMesh);
	returnStatus = attributeAffects(StitchMeshNode::stitchSize, StitchMeshNode::outputMesh);
	McheckErr(returnStatus, "ERROR in attributeAffects\n");
	
	//----------------------------------------------------------------------------------------------------------//
	// Create hard-coded stitch library																			//
	// This will STAY hardcoded since this data is not variable													//
	//----------------------------------------------------------------------------------------------------------//
	/*
	Stitch stitch1(;
	YarnCurve curve1;
	curve1.
	YarnCurve curve2;
	YarnCurve curve3;
	stitch1.addYarnCurve(curve1);
	*/
	//----------------------------------------------------------------------------------------------------------//
	// Return successful initialization															                //
	//----------------------------------------------------------------------------------------------------------//
	
	return MStatus::kSuccess;
}

//======================================================================================================================//
// Function for finding the currently selected edge. If a valid edge is selected, this will return the index of			//
// that edge. If no valid edge is selected, this will return -1.														//
//======================================================================================================================//

bool getSelectedEdge(int &index)
{ 	 
	// get the current selection list from maya
	MSelectionList selected;
	MGlobal::getActiveSelectionList(selected);
	if (selected.length() == 0)
		return -1;
	
	// get the dagPath to the current selection
	MDagPath dagPath;
	selected.getDagPath(0,dagPath);

	// if the selection is a mesh geometry component
	if (dagPath.apiType() == 296) {

		// get the name of the selection
		MStringArray selectedEdges;
		selected.getSelectionStrings(selectedEdges);
		char *selectedName = (char *)selectedEdges[0].asChar();

		// split the name of the selection to
		// get the edge index
		char* pch;
		pch = strtok(selectedName, ".[]");
		vector<char*> selectedNameTokens;
		while (pch != NULL) {
			selectedNameTokens.push_back(pch);
			pch = strtok(NULL, ".[]");
		}

		// return the edge index
		if (selectedNameTokens.size() > 2 && strcmp(selectedNameTokens[1], "e") == 0) {
			index = atoi(selectedNameTokens[2]);
			MString meshShapeName = MString(selectedNameTokens[0]);
			// TODO: test if meshShapeName equals inputShapeName
			return true;
		}
	}
 	return false;
}

//======================================================================================================================//
// Callback function for selecting edges																				//
//======================================================================================================================//

//void StitchMeshNode::edgeSelectCB(MFnMesh &inputMeshFn, MItMeshEdge &inputMeshEdgeIt)
void edgeSelectCB(void * data)
{
	//------------------------------------------------------------------------------------------------------------------//
	// Get index of currently selected edge (returns -1 if selection is not an edge)									//
	//------------------------------------------------------------------------------------------------------------------//
	
	int selectedEdgeIndex;
	if (!getSelectedEdge(selectedEdgeIndex))
		return;
		
	//------------------------------------------------------------------------------------------------------------------//
	// Local variables necessary for computation																		//
	//------------------------------------------------------------------------------------------------------------------//
	
	// input stitch mesh node from client data pointer
	StitchMeshNode *stitchnode = (StitchMeshNode*) data;

	// remove callback since new edges will be selected
	// it will be added back to the program at the end
	MEventMessage::removeCallback(stitchnode->callbackId);

	MFnMesh *inputMeshFn = stitchnode->inputMeshFn;
	MItMeshEdge *inputMeshEdgeIt = stitchnode->inputMeshItEdges;

	int p;								// used in setting index of iterator
	int2 courseBkwdVtxs;				// array for storing indices of PolyMeshFace's bkwd course vertices
	int2 courseFwrdVtxs;				// array for storing indices of PolyMeshFace's fwrd course vertices
	bool inverted = false;				// does the current edge need to be flipped to match direction?
	MIntArray connectedEdges;			// array for storing indices of edge's adjacent edges
	MIntArray connectedFaces;			// array for storing indices of edge's adjacent faces
	PolyMeshFaceLoop faceLoop;			// declare new faceLoop
	int nextCourseEdgeIndex = -1;		// store next course edge index

	//------------------------------------------------------------------------------------------------------------------//
	// Get selected edge from iterator and assert it is part of a new face loop (i.e. not bounded by preexisting loops)	//
	//------------------------------------------------------------------------------------------------------------------//
	
	int numBoundingRows = 0;
	int adjFaceLoop = -1;
	int adjFaceIndex = -1;
	inputMeshEdgeIt->setIndex(selectedEdgeIndex, p);
	inputMeshEdgeIt->getConnectedFaces(connectedFaces);
	int numConnectedFaces = connectedFaces.length();
	for (int i = 0; i < numConnectedFaces; i++) {
		int loop = stitchnode->faceLoopNumber[connectedFaces[i]];
		if (loop >= 0) {
			adjFaceLoop = loop;
			adjFaceIndex = stitchnode->faceLoopIndex[connectedFaces[i]];
			numBoundingRows++;
		}
	}

	//------------------------------------------------------------------------------------------------------------------//
	// CASE 1: This edge is already bounded by face loops and there is no need to create a new one, or it is an			//
	//		   interior edge with no adjacent loops																		//
	//------------------------------------------------------------------------------------------------------------------//
	
	if ((numBoundingRows == 2) || (numBoundingRows == 1 && inputMeshEdgeIt->onBoundary())
		|| (numBoundingRows == 0 && !inputMeshEdgeIt->onBoundary()))
	{
		stitchnode->callbackId = MEventMessage::addEventCallback("SelectionChanged", edgeSelectCB, stitchnode);
		MString cmd = "select " + stitchnode->name();
		MGlobal::executeCommand(cmd);
		return;
	}

	//------------------------------------------------------------------------------------------------------------------//
	// CASE 2: This edge is part of a boundary edge loop that is not yet associated with a face loop					//
	//------------------------------------------------------------------------------------------------------------------//
	
	else if (numBoundingRows == 0 && inputMeshEdgeIt->onBoundary())
	{
		do {
			// get vertex endpoints of selected edge
			inputMeshFn->getEdgeVertices(inputMeshEdgeIt->index(), courseBkwdVtxs);
			if (inverted) { 
				int temp = courseBkwdVtxs[0];
				courseBkwdVtxs[0] = courseBkwdVtxs[1];
				courseBkwdVtxs[1] = temp;
			}

			// get connected edges and faces for current edge
			connectedEdges.clear();
			connectedFaces.clear();
			int numConnectedEdges = inputMeshEdgeIt->getConnectedEdges(connectedEdges);
			int numConnectedFaces = inputMeshEdgeIt->getConnectedFaces(connectedFaces);
			int faceIndex = connectedFaces[0];
			
			// go through connected edges
			for (int i = 0; i < connectedEdges.length(); i++)
			{
				inputMeshEdgeIt->setIndex(connectedEdges[i], p);

				// if the connected edge is on the boundary, then
				// it could be the next course edge in the loop
				if (inputMeshEdgeIt->onBoundary())
				{
					int2 courseVertices;
					inputMeshFn->getEdgeVertices(inputMeshEdgeIt->index(), courseVertices);
					
					// connected to edge in correct direction
					if (courseVertices[0] != courseBkwdVtxs[0] && courseVertices[1] != courseBkwdVtxs[0]) {
						nextCourseEdgeIndex = connectedEdges[i];
						if (courseVertices[0] == courseBkwdVtxs[1]) { inverted = false; }
						if (courseVertices[1] == courseBkwdVtxs[1]) { inverted = true;  }
					}
				}

				// if it is not on the boundary of the mesh, it
				// cannot be a course edge and must be a wale edge
				else
				{
					int2 waleVertices; 
					inputMeshFn->getEdgeVertices(inputMeshEdgeIt->index(), waleVertices);

					// connected to left, direction is forward
					if (waleVertices[0] == courseBkwdVtxs[0])	   { courseFwrdVtxs[0] = waleVertices[1]; } 

					// connected to left, direction is inverted
					else if (waleVertices[1] == courseBkwdVtxs[0]) { courseFwrdVtxs[0] = waleVertices[0]; }

					// connected to right, direction is forward
					else if (waleVertices[0] == courseBkwdVtxs[1]) { courseFwrdVtxs[1] = waleVertices[1]; }

					// connected to right, direction is inverted
					else if (waleVertices[1] == courseBkwdVtxs[1]) { courseFwrdVtxs[1] = waleVertices[0]; }
				}

			}

			// Make a face with connectedVertices and faceTopVertices
			MIntArray courseBwkd, courseFwrd;
			courseBwkd.append(courseBkwdVtxs[0]); courseBwkd.append(courseBkwdVtxs[1]);
			courseFwrd.append(courseFwrdVtxs[0]); courseFwrd.append(courseFwrdVtxs[1]);
			PolyMeshFace face = PolyMeshFace(courseBwkd, courseFwrd, faceIndex);
			stitchnode->faceLoopIndex[faceIndex] = (int) faceLoop.size();
			stitchnode->faceLoopNumber[faceIndex] = (int) stitchnode->MPolyMeshFaceLoops.size();
			faceLoop.push_back(face);
			
			// continue to the next course edge for the loop
			inputMeshEdgeIt->setIndex(nextCourseEdgeIndex, p);
		
		} while (inputMeshEdgeIt->index() != selectedEdgeIndex);
	}
		
	//------------------------------------------------------------------------------------------------------------------//
	// CASE 3: This edge is part of an interior edge loop and is bounded by existing face loops on only one side		//
	//------------------------------------------------------------------------------------------------------------------//
	
	else if (numBoundingRows == 1 && !inputMeshEdgeIt->onBoundary())
	{
		do {
			// get course vertex endpoint indices in same direction as
			// adjacent face's forward edge
			stitchnode->MPolyMeshFaceLoops[adjFaceLoop][adjFaceIndex].getCourseEdgeFwrd(courseBkwdVtxs);
			int nextAdjFaceIndex = incrementWithWrap(adjFaceIndex, stitchnode->MPolyMeshFaceLoops[adjFaceLoop].size());

			// get connected edges and faces for current edge
			connectedEdges.clear();
			connectedFaces.clear();
			int numConnectedEdges = inputMeshEdgeIt->getConnectedEdges(connectedEdges);
			int numConnectedFaces = inputMeshEdgeIt->getConnectedFaces(connectedFaces);
			int currentLoop = stitchnode->MPolyMeshFaceLoops.size();
			int faceIndex;
			for (int i = 0; i < numConnectedFaces; i++) {
				if (stitchnode->faceLoopNumber[connectedFaces[i]] != adjFaceLoop) { faceIndex = connectedFaces[i]; }
			}

			// go through connected edges
			for (int i = 0; i < connectedEdges.length(); i++)
			{
				inputMeshEdgeIt->setIndex(connectedEdges[i], p);
				inputMeshEdgeIt->getConnectedFaces(connectedFaces);
				int faceId0 = connectedFaces[0];
				int faceId1 = connectedFaces[1];

				// if the connected edge has one adjacent face equal to the next adjacent face in the bkwd loop
				// and one face not currently in a loop, then it is the next bkwd course edge
				if ((faceId0 == stitchnode->MPolyMeshFaceLoops[adjFaceLoop][nextAdjFaceIndex].faceIndex && faceId1 == -1) ||
					(faceId1 == stitchnode->MPolyMeshFaceLoops[adjFaceLoop][nextAdjFaceIndex].faceIndex && faceId0 == -1))
				{
						nextCourseEdgeIndex = connectedEdges[i];
				}

				// if it is not the next backward course edge, then it must have two adjacent faces not in a loop 
				// or one (or two, in the case of the last face) adjacent face(s) in the current loop to be one of the wale edges
				else if ((stitchnode->faceLoopNumber[faceId0] == -1			 && stitchnode->faceLoopNumber[faceId1] == -1		  ) ||
						 (stitchnode->faceLoopNumber[faceId0] == -1			 && stitchnode->faceLoopNumber[faceId1] == currentLoop) ||
						 (stitchnode->faceLoopNumber[faceId0] == currentLoop && stitchnode->faceLoopNumber[faceId1] == -1		  ) ||
						 (stitchnode->faceLoopNumber[faceId0] == currentLoop && stitchnode->faceLoopNumber[faceId1] == currentLoop) )
				{
					int2 waleVertices; 
					inputMeshFn->getEdgeVertices(inputMeshEdgeIt->index(), waleVertices);

					// connected to left, direction is forward
					if (waleVertices[0] == courseBkwdVtxs[0])	   { courseFwrdVtxs[0] = waleVertices[1]; } 

					// connected to left, direction is inverted
					else if (waleVertices[1] == courseBkwdVtxs[0]) { courseFwrdVtxs[0] = waleVertices[0]; }

					// connected to right, direction is forward
					else if (waleVertices[0] == courseBkwdVtxs[1]) { courseFwrdVtxs[1] = waleVertices[1]; }

					// connected to right, direction is inverted
					else if (waleVertices[1] == courseBkwdVtxs[1]) { courseFwrdVtxs[1] = waleVertices[0]; }
				}
			}

			// Make a face with connectedVertices and faceTopVertices
			MIntArray courseBwkd, courseFwrd;
			courseBwkd.append(courseBkwdVtxs[0]); courseBwkd.append(courseBkwdVtxs[1]);
			courseFwrd.append(courseFwrdVtxs[0]); courseFwrd.append(courseFwrdVtxs[1]);
			PolyMeshFace face = PolyMeshFace(courseBwkd, courseFwrd, faceIndex);
			stitchnode->faceLoopIndex[faceIndex] = (int) faceLoop.size();
			stitchnode->faceLoopNumber[faceIndex] = (int) stitchnode->MPolyMeshFaceLoops.size();
			faceLoop.push_back(face);
			
			// continue to the next course edge for the loop
			inputMeshEdgeIt->setIndex(nextCourseEdgeIndex, p);
			adjFaceIndex = nextAdjFaceIndex;
		
		} while (inputMeshEdgeIt->index() != selectedEdgeIndex);
	}
	
	//------------------------------------------------------------------------------------------------------------------//
	// Push new face loop onto node's vector and update the number of faces already accounted for by loops				//
	//------------------------------------------------------------------------------------------------------------------//

	stitchnode->MPolyMeshFaceLoops.push_back(faceLoop);
	stitchnode->numLoopFaces += (int) faceLoop.size();
	
	//------------------------------------------------------------------------------------------------------------------//
	// Display colors for selected face loop																			//
	//------------------------------------------------------------------------------------------------------------------//

	for (int i = 0; i < faceLoop.size(); i++) {

		// clear current vertex-face selection
		MGlobal::executeCommand("select -d");
		
		// get current polymeshface of loop for coloring
		PolyMeshFace currentFace = faceLoop[i];
		MString cmd = "select " + inputShapeName + ".f[" + currentFace.faceIndex + "];";
		MGlobal::executeCommand(cmd);

		// color per vertex
		MGlobal::executeCommand("polyColorPerVertex -r 0 -g 1 -b 0");
		MGlobal::executeCommand("polyOptions -colorShadedDisplay true");
		MGlobal::executeCommand("polyOptions -colorMaterialChannel \"DIFFUSE\"");
		
		// clear current vertex-face selection
		MGlobal::executeCommand("select -d");
	}
	
	//------------------------------------------------------------------------------------------------------------------//
	// Re-add callback to program if it is still needed																	//
	//------------------------------------------------------------------------------------------------------------------//

	if (stitchnode->numLoopFaces < inputMeshFn->numPolygons())
		stitchnode->callbackId = MEventMessage::addEventCallback("SelectionChanged", edgeSelectCB, stitchnode);
	
	//------------------------------------------------------------------------------------------------------------------//
	// Return from callback function																					//
	//------------------------------------------------------------------------------------------------------------------//
	
	MString cmd = "select " + stitchnode->name();
	MGlobal::executeCommand(cmd);
	return;
}

//======================================================================================================================//
// User interaction stage for selecting edge loops in direction of stitch												//
// This function simply adds the callback to the program for edge loop selection (if it is needed)						//
//======================================================================================================================//

MStatus StitchMeshNode::defineStitchLoops(void) {

	// add callback function for edge selection
	if (callbackId == -1) { callbackId = MEventMessage::addEventCallback("SelectionChanged", edgeSelectCB, this); }
	MGlobal::executeCommand("selectType -pe 1");
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
	
		MDataHandle inputMeshNameHandle = data.inputValue(inputMeshName, &returnStatus);
		MDataHandle outputMeshNameHandle = data.inputValue(outputMeshName, &returnStatus);
		inputShapeName = inputMeshNameHandle.asString();
		outputShapeName = outputMeshNameHandle.asString();

		//--------------------------------------------------------------------------------------------------------------//
		// Get input attributes																							//
		//--------------------------------------------------------------------------------------------------------------//
	
		// Get input mesh object
		MDataHandle inputMeshDataHandle = data.inputValue(inputMesh, &returnStatus); 
		McheckErr(returnStatus, "Error getting input mesh data handle\n");
		MObject inputMeshData = inputMeshDataHandle.asMesh();
		
		// get dag path
		MDagPath inputDagPath;
		MDagPath::getAPathTo(inputMeshData, inputDagPath);		// path to input mesh object in DAG

		// delete pointers to old input function sets
		if (inputMeshFn != NULL) delete inputMeshFn;
		if (inputMeshItEdges != NULL) delete inputMeshItEdges;
		if (inputMeshItFaces != NULL) delete inputMeshItFaces;

		// create new input function sets
		inputMeshFn = new MFnMesh(inputMeshData);				// mesh function set for mesh object
		inputMeshItEdges = new MItMeshEdge(inputMeshData);		// edge iterator for mesh object
		inputMeshItFaces = new MItMeshPolygon(inputMeshData);	// face iterator for mesh object
		for (int i = 0; i < inputMeshFn->numPolygons(); i++) {
			faceLoopIndex.append(-1);
			faceLoopNumber.append(-1);
		}
		
		// Get default stitch size
		MDataHandle stitchSizeDataHandle = data.inputValue(stitchSize, &returnStatus); 
		McheckErr(returnStatus, "Error getting stitch size data handle\n");
		float stitchSizeData = stitchSizeDataHandle.asFloat();

		// Get binary stitch generation value
		MDataHandle nodeStageDataHandle = data.inputValue(nodeStage, &returnStatus); 
		McheckErr(returnStatus, "Error getting stitch generation data handle\n");
		int nodeStageData = nodeStageDataHandle.asInt();

		//--------------------------------------------------------------------------------------------------------------//
		// Get output handle for mesh creation																			//
		//--------------------------------------------------------------------------------------------------------------//
		
		// get handle to output attribute
		MDataHandle outputHandle = data.outputValue(outputMesh, &returnStatus);
		McheckErr(returnStatus, "ERROR getting output mesh data handle\n");

		// create output object
		MFnMeshData dataCreator;
		MObject outputMeshData = dataCreator.create(&returnStatus);
		McheckErr(returnStatus, "ERROR creating outputData");

		// put copy of input mesh in output object
		MObject inputData = data.inputValue(inputMesh).data();
		MObject outputMeshShape = MFnMeshData().create();
		MFnMesh outputMeshFnShape = MFnMesh().copy(inputData, outputMeshShape);
		MFnMesh outputMeshFnData  = MFnMesh().copy(inputMeshData, outputMeshData);
		outputMeshFnData.setObject(outputMeshShape);

		switch (nodeStageData) {
			
		//--------------------------------------------------------------------------------------------------------------//
		// User interaction stage for selecting edge loops in direction of stitch
		//--------------------------------------------------------------------------------------------------------------//
		
		//case (CLEAR_LOOP_SELECTION):
		//	clearStitchLoops();

		case (LOOP_SELECTION):
			defineStitchLoops();
			break;

		//--------------------------------------------------------------------------------------------------------------//
		// Call tessellation function																					//
		//--------------------------------------------------------------------------------------------------------------//
		
		case (YARN_GENERATION):
		case (STITCH_EDITING):
		case (TESSELLATION):

			// if all faces are designated by the user during loop selection,
			// proceed with tessellation

			if (numLoopFaces == inputMeshFn->numPolygons() && nodeStageData)
			{
				// tessellate the mesh
				tessellateInputMesh(stitchSizeData, outputMeshFnData);

				// delete original input faces. only keep tesselated rows
				for (int q = inputMeshFn->numPolygons()-1; q >= 0; q--)
					outputMeshFnData.deleteFace(q);

				// update surface with new faces
				outputMeshFnData.updateSurface();
				outputHandle.set(outputMeshShape);
				MGlobal::executeCommand("select " + this->name());
			}

			// if not all faces are designated, reset the nodeStage attribute
			else {
				MString resetStageCmd = "setAttr " + this->name() + ".nodeStage 0";
				MGlobal::executeCommand(resetStageCmd);
			}
			
			if (nodeStageData == TESSELLATION) {
				MGlobal::executeCommand("select " + this->name());
				break;
			}

			//----------------------------------------------------------------------------------------------------------//
			// Update output mesh colors to for stitch level editing													//
			//----------------------------------------------------------------------------------------------------------//
			
			for (int i = 0; i < MSubFaces.size(); i++)
			{
				SubFace subface = MSubFaces[i];
				if (subface.nBkwd == 2 && subface.nFwrd == 2)
					outputMeshFnShape.setFaceColor(MColor(1,1,0), subface.faceId-inputMeshFn->numPolygons());
				if (subface.nBkwd == 2 && subface.nFwrd == 1)
					outputMeshFnShape.setFaceColor(MColor(0,0,1), subface.faceId-inputMeshFn->numPolygons());
				if (subface.nBkwd == 1 && subface.nFwrd == 2)
					outputMeshFnShape.setFaceColor(MColor(1,0,0), subface.faceId-inputMeshFn->numPolygons());
				MGlobal::executeCommand("setAttr " + outputShapeName + ".displayColors true");
				outputHandle.setMObject(outputMeshShape);
			} 
			
			if (nodeStageData == STITCH_EDITING) break;

			//----------------------------------------------------------------------------------------------------------//
			// Replace tessellated faces with stitch models																//
			//----------------------------------------------------------------------------------------------------------//
			for (int i = 0; i < MSubFaces.size(); i++)
			{
				int p;
				MVector normal;
				MItMeshPolygon outputFaceIter(outputMeshShape);
				MString stitchName = MString("stitch1_group") + (i+1);
				MVector stitchNormal(0,1,0);
				MString importCmd = MString("file -import -type \"mayaBinary\" -ra true -mergeNamespacesOnClash false -options \"v=0;\"")
								  + MString("-pr \"C:/Users/Richard/Documents/_Academics/Penn/02_S2014/CIS_660/AuthoringTool/StitchMeshes/MayaFiles/stitch1.mb\";");
				MGlobal::executeCommand(importCmd);
				SubFace subface = MSubFaces[i];
				outputFaceIter.setIndex(subface.faceId-inputMeshFn->numPolygons(), p);
				outputFaceIter.getNormal(normal);
				MVector axis = stitchNormal ^ normal;
				float angle = acos(stitchNormal * normal);
				MEulerRotation euler(0.0,0.0,0.0);
				euler.incrementalRotateBy(axis, angle);
				double rotX = euler.x * 180.0 / M_PI;
				double rotY = euler.y * 180.0 / M_PI;
				double rotZ = euler.z * 180.0 / M_PI;
				MString rotXcmd = MString("setAttr ") + stitchName + MString(".rotateX ") + rotX;
				MString rotYcmd = MString("setAttr ") + stitchName + MString(".rotateY ") + rotY;
				MString rotZcmd = MString("setAttr ") + stitchName + MString(".rotateZ ") + rotZ;
				MGlobal::executeCommand(rotXcmd);
				MGlobal::executeCommand(rotYcmd);
				MGlobal::executeCommand(rotZcmd);
				MPoint center = outputFaceIter.center();
				MGlobal::executeCommand("select " + stitchName);
				MString moveCmd = MString("move ") + center.x + " " + center.y + " " + center.z;
				MGlobal::executeCommand(moveCmd);
				double courseScale = (subface.fwrdPoints[0] - subface.bkwdPoints[0]).length();
				double waleScale = (subface.fwrdPoints[subface.fwrdPoints.length()-1] - subface.fwrdPoints[0]).length();
				MString scaleCmd = MString("scale ") + waleScale + " " + waleScale*2.0 + " " + courseScale;
				MGlobal::executeCommand(scaleCmd);
				MSelectionList selected;
				MGlobal::getActiveSelectionList(selected);
				MDagPath dagPath;
				selected.getDagPath(0,dagPath);
				MFnTransform stitchTransform(dagPath);
				while (true) {
					MMatrix matrix = stitchTransform.transformationMatrix();
					MVector localZ = MVector(0,0,-1) * matrix; localZ.normalize();
					angle = acos(MVector(0,1,0) * localZ);
					if (angle < 1.0e-4) break;
					euler.incrementalRotateBy(normal, angle);
					rotX = euler.x * 180.0 / M_PI;
					rotY = euler.y * 180.0 / M_PI;
					rotZ = euler.z * 180.0 / M_PI;
					rotXcmd = MString("setAttr ") + stitchName + MString(".rotateX ") + rotX;
					rotYcmd = MString("setAttr ") + stitchName + MString(".rotateY ") + rotY;
					rotZcmd = MString("setAttr ") + stitchName + MString(".rotateZ ") + rotZ;
					MGlobal::executeCommand(rotXcmd);
					MGlobal::executeCommand(rotYcmd);
					MGlobal::executeCommand(rotZcmd);
					MGlobal::executeCommand(MString("hide ") + outputShapeName);
				}
			}
			
			// clear selection
			MGlobal::executeCommand("select " + this->name());
			break;

		default:
			break;
		}

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
