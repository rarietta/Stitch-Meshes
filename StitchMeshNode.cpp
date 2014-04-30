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

#include "StitchMeshNode.h"

//======================================================================================================================//
// Some useful functions																								//
//======================================================================================================================//

#define McheckErr(stat,msg)								\
	if ( MS::kSuccess != stat ) {						\
		cerr << stat << ": " << msg;					\
		return MStatus::kFailure;						\
	}

#define LerpInt(v1,v2,pct)								\
	v1 + floor(pct*(v2-v1))

#define LerpVec(v1,v2,pct)								\
	v1 + pct*(v2-v1)

#define incrementWithWrap(index, size)					\
	(index + 1 < size) ? (index + 1) : 0

#define selectNodeAndBreak								\
	MGlobal::executeCommand("select " + this->name());	\
	break;


//======================================================================================================================//
// Node variables																										//
//======================================================================================================================//

MStatus returnStatus;
MObject StitchMeshNode::attr_inMesh;
MObject StitchMeshNode::attr_outMesh;
MObject StitchMeshNode::attr_nodeStage;
MObject StitchMeshNode::attr_stitchSize;
MObject StitchMeshNode::inputMeshName;
MObject StitchMeshNode::outputMeshName;
MTypeId StitchMeshNode::id( 0x00004 );
MString inputShapeName;
MString outputShapeName;
float	stitchSize;

//======================================================================================================================//
// Initialize the Node Plugin																							//
//======================================================================================================================//

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

	StitchMeshNode::attr_stitchSize = nAttr.create("stitchSize", "ss", MFnNumericData::kFloat, 1.0, &returnStatus);
	McheckErr(returnStatus, "ERROR creating StitchMeshNode stitchSize attribute\n");
	nAttr.setMin(0.25);
	nAttr.setMax(8.0);
	nAttr.setSoftMin(0.25);
	nAttr.setSoftMax(8.0);
	
	//----------------------------------------------------------------------------------------------------------//
	// Create "Generate Stitch Mesh" Boolean Attribute											                //
	//----------------------------------------------------------------------------------------------------------//

	StitchMeshNode::attr_nodeStage = nAttr.create("nodeStage", "sg", MFnNumericData::kInt, LOOP_SELECTION, &returnStatus);
	McheckErr(returnStatus, "ERROR creating StitchMeshNode nodeStage attribute\n");
	
	//----------------------------------------------------------------------------------------------------------//
	// Create Input Mesh Attribute																                //
	//----------------------------------------------------------------------------------------------------------//
	
	StitchMeshNode::attr_inMesh = tAttr.create("inputMesh", "in", MFnData::kMesh, &returnStatus);
	StitchMeshNode::inputMeshName = tAttr.create("inputMeshName", "inname", MFnData::kString, &returnStatus);
	McheckErr(returnStatus, "ERROR creating StitchMeshNode output attribute\n");
	
	//----------------------------------------------------------------------------------------------------------//
	// Create Output Mesh Attribute																                //
	//----------------------------------------------------------------------------------------------------------//

	StitchMeshNode::attr_outMesh = tAttr.create("outputMesh", "out", MFnData::kMesh, &returnStatus); 
	StitchMeshNode::outputMeshName = tAttr.create("outputMeshName", "outname", MFnData::kString, &returnStatus);
	tAttr.setStorable(false);
	McheckErr(returnStatus, "ERROR creating StitchMeshNode output attribute\n");

	//----------------------------------------------------------------------------------------------------------//
	//  Add Attributes to Node                                                                                  //
	//----------------------------------------------------------------------------------------------------------//

	// Add input mesh attribute
	returnStatus = addAttribute(StitchMeshNode::attr_inMesh);
	returnStatus = addAttribute(StitchMeshNode::inputMeshName);
	McheckErr(returnStatus, "ERROR adding input attributes\n");

	// Add stitch size attribute
	returnStatus = addAttribute(StitchMeshNode::attr_stitchSize);
	McheckErr(returnStatus, "ERROR adding stitchSize attribute\n");

	// Add stitch size attribute
	returnStatus = addAttribute(StitchMeshNode::attr_nodeStage);
	McheckErr(returnStatus, "ERROR adding nodeStage attribute\n");

	// Add mesh attribute
	returnStatus = addAttribute(StitchMeshNode::attr_outMesh);
	returnStatus = addAttribute(StitchMeshNode::outputMeshName);
	McheckErr(returnStatus, "ERROR adding outputMesh attributes\n");

	//----------------------------------------------------------------------------------------------------------//
	// Attach attributes to affect output mesh												 	                //
	//----------------------------------------------------------------------------------------------------------//
	
	returnStatus = attributeAffects(StitchMeshNode::attr_inMesh,	 StitchMeshNode::attr_outMesh);
	returnStatus = attributeAffects(StitchMeshNode::attr_nodeStage,  StitchMeshNode::attr_outMesh);
	returnStatus = attributeAffects(StitchMeshNode::attr_stitchSize, StitchMeshNode::attr_outMesh);
	McheckErr(returnStatus, "ERROR in attributeAffects\n");
	
	//----------------------------------------------------------------------------------------------------------//
	// Return successful initialization															                //
	//----------------------------------------------------------------------------------------------------------//
	
	return MStatus::kSuccess;
}

//======================================================================================================================//
// Create the Node																										//
//======================================================================================================================//

void* StitchMeshNode::creator()
{
	StitchMeshNode *node = new StitchMeshNode();
	node->numLoopFaces = 0;
	node->callbackId = -1;
	node->inputMeshFn = NULL;
	node->oMeshFnShape = NULL;
	node->outputMeshObj = MFnMeshData().create();
	node->inputMeshObj = MFnMeshData().create();
	node->CreateStitchLibrary();
	
	// end
	return node;
}

//======================================================================================================================//
// Function for finding the currently selected edge. If a valid edge is selected, this will return the index of			//
// that edge. If no valid edge is selected, this will return -1.														//
//======================================================================================================================//

bool GetSelectedEdge(int &index)
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

void LabelEdgeRows(void * data)
{
	//------------------------------------------------------------------------------------------------------------------//
	// Get index of currently selected edge (returns -1 if selection is not an edge)									//
	//------------------------------------------------------------------------------------------------------------------//
	
	int selectedEdgeIndex;
	if (!GetSelectedEdge(selectedEdgeIndex))
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
	MItMeshEdge inputMeshEdgeIt(stitchnode->inputMeshObj);

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
	inputMeshEdgeIt.setIndex(selectedEdgeIndex, p);
	inputMeshEdgeIt.getConnectedFaces(connectedFaces);
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
	
	if ((numBoundingRows == 2) || (numBoundingRows == 1 && inputMeshEdgeIt.onBoundary())
		|| (numBoundingRows == 0 && !inputMeshEdgeIt.onBoundary()))
	{
		stitchnode->callbackId = MEventMessage::addEventCallback("SelectionChanged", LabelEdgeRows, stitchnode);
		MString cmd = "select " + stitchnode->name();
		MGlobal::executeCommand(cmd);
		return;
	}

	//------------------------------------------------------------------------------------------------------------------//
	// CASE 2: This edge is part of a boundary edge loop that is not yet associated with a face loop					//
	//------------------------------------------------------------------------------------------------------------------//
	
	else if (numBoundingRows == 0 && inputMeshEdgeIt.onBoundary())
	{
		do {
			// get vertex endpoints of selected edge
			inputMeshFn->getEdgeVertices(inputMeshEdgeIt.index(), courseBkwdVtxs);
			if (inverted) { 
				int temp = courseBkwdVtxs[0];
				courseBkwdVtxs[0] = courseBkwdVtxs[1];
				courseBkwdVtxs[1] = temp;
			}

			// get connected edges and faces for current edge
			connectedEdges.clear();
			connectedFaces.clear();
			int numConnectedEdges = inputMeshEdgeIt.getConnectedEdges(connectedEdges);
			int numConnectedFaces = inputMeshEdgeIt.getConnectedFaces(connectedFaces);
			int faceIndex = connectedFaces[0];
			
			// go through connected edges
			for (int i = 0; i < connectedEdges.length(); i++)
			{
				inputMeshEdgeIt.setIndex(connectedEdges[i], p);

				// if the connected edge is on the boundary, then
				// it could be the next course edge in the loop
				if (inputMeshEdgeIt.onBoundary())
				{
					int2 courseVertices;
					inputMeshFn->getEdgeVertices(inputMeshEdgeIt.index(), courseVertices);
					
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
					inputMeshFn->getEdgeVertices(inputMeshEdgeIt.index(), waleVertices);

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
			inputMeshEdgeIt.setIndex(nextCourseEdgeIndex, p);
		
		} while (inputMeshEdgeIt.index() != selectedEdgeIndex);
	}
		
	//------------------------------------------------------------------------------------------------------------------//
	// CASE 3: This edge is part of an interior edge loop and is bounded by existing face loops on only one side		//
	//------------------------------------------------------------------------------------------------------------------//
	
	else if (numBoundingRows == 1 && !inputMeshEdgeIt.onBoundary())
	{
		do {
			// get course vertex endpoint indices in same direction as
			// adjacent face's forward edge
			stitchnode->MPolyMeshFaceLoops[adjFaceLoop][adjFaceIndex].getCourseEdgeFwrd(courseBkwdVtxs);
			int nextAdjFaceIndex = incrementWithWrap(adjFaceIndex, stitchnode->MPolyMeshFaceLoops[adjFaceLoop].size());

			// get connected edges and faces for current edge
			connectedEdges.clear();
			connectedFaces.clear();
			int numConnectedEdges = inputMeshEdgeIt.getConnectedEdges(connectedEdges);
			int numConnectedFaces = inputMeshEdgeIt.getConnectedFaces(connectedFaces);
			int currentLoop = stitchnode->MPolyMeshFaceLoops.size();
			int faceIndex;
			for (int i = 0; i < numConnectedFaces; i++) {
				if (stitchnode->faceLoopNumber[connectedFaces[i]] != adjFaceLoop) { faceIndex = connectedFaces[i]; }
			}

			// go through connected edges
			for (int i = 0; i < connectedEdges.length(); i++)
			{
				inputMeshEdgeIt.setIndex(connectedEdges[i], p);
				inputMeshEdgeIt.getConnectedFaces(connectedFaces);
				int faceId0 = connectedFaces[0];
				int faceId1 = connectedFaces[1];

				// if the connected edge has one adjacent face equal to the next adjacent face in the bkwd loop
				// and one face not currently in a loop, then it is the next bkwd course edge
				if ((faceId0 == stitchnode->MPolyMeshFaceLoops[adjFaceLoop][nextAdjFaceIndex].faceIndex && stitchnode->faceLoopNumber[faceId1] == -1) ||
					(faceId0 == stitchnode->MPolyMeshFaceLoops[adjFaceLoop][nextAdjFaceIndex].faceIndex && stitchnode->faceLoopNumber[faceId1] == currentLoop) ||
					(faceId1 == stitchnode->MPolyMeshFaceLoops[adjFaceLoop][nextAdjFaceIndex].faceIndex && stitchnode->faceLoopNumber[faceId0] == -1) ||
					(faceId1 == stitchnode->MPolyMeshFaceLoops[adjFaceLoop][nextAdjFaceIndex].faceIndex && stitchnode->faceLoopNumber[faceId0] == currentLoop))
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
					inputMeshFn->getEdgeVertices(inputMeshEdgeIt.index(), waleVertices);

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
			inputMeshEdgeIt.setIndex(nextCourseEdgeIndex, p);
			adjFaceIndex = nextAdjFaceIndex;
		
		} while (inputMeshEdgeIt.index() != selectedEdgeIndex);
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
		stitchnode->callbackId = MEventMessage::addEventCallback("SelectionChanged", LabelEdgeRows, stitchnode);
	
	//------------------------------------------------------------------------------------------------------------------//
	// Return from callback function																					//
	//------------------------------------------------------------------------------------------------------------------//
	
	MString cmd = "select " + stitchnode->name();
	MGlobal::executeCommand(cmd);
	return;
}

//======================================================================================================================//
// Tesselation Functions																								//
//======================================================================================================================//

int StitchMeshNode::ComputeNumWaleDivisions(PolyMeshFaceLoop &currentLoop)
{
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
	int numWaleDivisions = ceil(avgLength / (stitchSize));
	return numWaleDivisions;
}

MStatus StitchMeshNode::InterpolatePoints(vector<MPointArray> &stitchRowPts, PolyMeshFace &currentFace)
{
	// Determine the uniform number of wale divisions along the face loop
	PolyMeshFaceLoop currentLoop = MPolyMeshFaceLoops[faceLoopNumber[currentFace.faceIndex]];
	int numWaleDivisions = ComputeNumWaleDivisions(currentLoop);

	// determine number of divisions along fwrd & bkwd course edges
	MPointArray pts;
	oMeshFnShape->getPoints(pts);
	int numCourseDivisionsBkwd = 0;
	int numCourseDivisionsFwrd = 0;
	float courseLengthBkwd = 0.0;
	float courseLengthFwrd = 0.0;

	MIntArray courseVerticesBkwd;
	currentFace.getCourseEdgeBkwd(courseVerticesBkwd);
	for (int i = 0; i < courseVerticesBkwd.length()-1; i++) {
		MVector courseEdgeBkwd = pts[courseVerticesBkwd[i+1]] - pts[courseVerticesBkwd[i]];
		numCourseDivisionsBkwd += ceil(courseEdgeBkwd.length() / (stitchSize));
		courseLengthBkwd += courseEdgeBkwd.length();
	}
			
	MIntArray courseVerticesFwrd;
	currentFace.getCourseEdgeFwrd(courseVerticesFwrd);
	for (int i = 0; i < courseVerticesFwrd.length()-1; i++) {
		MVector courseEdgeFwrd = pts[courseVerticesFwrd[i+1]] - pts[courseVerticesFwrd[i]];
		numCourseDivisionsFwrd += ceil(courseEdgeFwrd.length() / (stitchSize));
		courseLengthFwrd += courseEdgeFwrd.length();
	}

	// create mean-value coordinate instance for tessellation
	MPointArray cagePoints;
	currentFace.getCage(cagePoints, *oMeshFnShape);
	MVC mvc(cagePoints);

	// find and store all interpolated points
	for (int u = 0; u <= numWaleDivisions; u++) 
	{
		double uPct = (double) u / numWaleDivisions;
		int numRowPts = LerpInt(numCourseDivisionsBkwd, numCourseDivisionsFwrd, uPct);

		for (int v = 0; v <= numRowPts; v++) 
		{
			double vPct = (double) v / numRowPts;
			MVCWeights weights;
			int i;

			// determine backward weights
			i = 0;
			MVCWeights bkwdWeights;
			MPoint bkwdOrigin = pts[courseVerticesBkwd[0]];
			float bkwdDistance = vPct*courseLengthBkwd;
			while (bkwdDistance > 1.0e-3)
			{
				MVector courseEdgeBkwd = pts[courseVerticesBkwd[i+1]] - pts[courseVerticesBkwd[i++]];
				float factor = min(courseEdgeBkwd.length(), bkwdDistance) / courseEdgeBkwd.length();
				bkwdOrigin += factor * courseEdgeBkwd;
				bkwdDistance -= factor * courseEdgeBkwd.length();
			}
			mvc.computeMVCWeights(bkwdWeights, bkwdOrigin);

			// determine forward weights
			i = 0;
			MVCWeights fwrdWeights;
			MPoint fwrdOrigin = pts[courseVerticesFwrd[0]];
			float fwrdDistance = vPct*courseLengthFwrd;
			while (fwrdDistance > 1.0e-3)
			{
				MVector courseEdgefwrd = pts[courseVerticesFwrd[i+1]] - pts[courseVerticesFwrd[i++]];
				float factor = min(courseEdgefwrd.length(), fwrdDistance) / courseEdgefwrd.length();
				fwrdOrigin += factor * courseEdgefwrd;
				fwrdDistance -= factor * courseEdgefwrd.length();
			}
			mvc.computeMVCWeights(fwrdWeights, fwrdOrigin);

			weights.clear();
			for (int i = 0; i < courseVerticesBkwd.length(); i++)
				weights.append(bkwdWeights[i] * (1.0 - uPct));
			for (int i = courseVerticesBkwd.length(); i < cagePoints.length(); i++)
				weights.append(fwrdWeights[i] * uPct);
		
			MPoint pt;
			mvc.computeMVCPosition(weights, pt);
			stitchRowPts[u].append(pt);
		}
	}
	return MStatus::kSuccess;
}

MStatus StitchMeshNode::BuildSubfaces(vector<MPointArray> &stitchRowPts)
{
	int subfaceId = 0;
	MPointArray vertexLoop;

	for (int u = 0; u < stitchRowPts.size() - 1; u++) 
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
			SubFace subface(2, 2);
			subface.bkwdPoints.clear();
			subface.fwrdPoints.clear();
			subface.bkwdPoints.append(stitchRowPts[ u ][ v ]);
			subface.bkwdPoints.append(stitchRowPts[ u ][v+1]);
			subface.fwrdPoints.append(stitchRowPts[u+1][ v ]);
			subface.fwrdPoints.append(stitchRowPts[u+1][v+1]);
			subface.stitchType = P;
			subface.nBkwd = 2; subface.nFwrd = 2;
			MSubFaces.push_back(subface);

			// add polygon to mesh
			oMeshFnShape->addPolygon(vertexLoop, subfaceId++, true, 1.0e-3);
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
			SubFace subface(2, 1);
			subface.bkwdPoints.clear();
			subface.fwrdPoints.clear();
			for (int v = numPts2-1; v < numPts1; v++)
				subface.bkwdPoints.append(stitchRowPts[u][v]);
			subface.fwrdPoints.append(stitchRowPts[u+1][numPts2-1]);
			subface.stitchType = X;
			subface.nBkwd = 2; subface.nFwrd = 1;
			MSubFaces.push_back(subface);
					
			// add polygon to mesh
			oMeshFnShape->addPolygon(vertexLoop, subfaceId++);
		}

		// forward edge has more points
		else if (numPts2 > numPts1) {
			vertexLoop.clear();
			vertexLoop.append(stitchRowPts[u][numPts1-1]);
			for (int v = numPts2-1; v >= numPts1-1; v--)
				vertexLoop.append(stitchRowPts[u+1][v]);
					
			// create output subface
			SubFace subface(1, 2);
			subface.bkwdPoints.clear();
			subface.fwrdPoints.clear();
			subface.bkwdPoints.append(stitchRowPts[u][numPts1-1]);
			for (int v = numPts1-1; v < numPts2; v++)
				subface.fwrdPoints.append(stitchRowPts[u+1][v]);
			subface.stitchType = Y1;
			subface.nBkwd = 1; subface.nFwrd = 2;
			MSubFaces.push_back(subface);
					
			// add polygon to mesh
			oMeshFnShape->addPolygon(vertexLoop, subfaceId++);
		}
	} 
	return MStatus::kSuccess;
}

MStatus StitchMeshNode::TessellateInputMesh(void)
{
	MSubFaces.clear();
	int numPolyMeshFaceLoops = (int) MPolyMeshFaceLoops.size();
	
	for (int n = 0; n < numPolyMeshFaceLoops; n++)
	{
		PolyMeshFaceLoop currentLoop = MPolyMeshFaceLoops[n];
		int numWaleDivisions = ComputeNumWaleDivisions(currentLoop);

		for (int f = 0; f < currentLoop.size(); f++)
		{
			PolyMeshFace currentFace = currentLoop[f];

			// Create vector of MPointArrays to store interpolated points
			vector<MPointArray> stitchRowPts;
			stitchRowPts.resize(numWaleDivisions + 1);
			for (int i = 0; i < stitchRowPts.size(); i++) { stitchRowPts[i].clear(); }

			// Compute the interpolated points used for building tessellated faces
			InterpolatePoints(stitchRowPts, currentFace);

			// Add interior faces to polygon based on stored stitch row points
			BuildSubfaces(stitchRowPts);
		}
	}
	
	// delete original un-tesselated face
	for (int i = 0; i < inputMeshFn->numPolygons(); i++)
		oMeshFnShape->deleteFace(0);
	oMeshFnShape->updateSurface();

	return MStatus::kSuccess;
}

//======================================================================================================================//
// Color all subfaces by the specified stitch type from the library														//
//======================================================================================================================//

MStatus StitchMeshNode::ColorByStitchType(void)
{
	int p;
	for (int i = 0; i < MSubFaces.size(); i++)
	{
		switch (MSubFaces[i].stitchType) {
			case (X):	  oMeshFnShape->setFaceColor(MColor(0.2, 0.2, 0.9), i); break;
			case (Y1):	  oMeshFnShape->setFaceColor(MColor(0.5, 0.9, 0.9), i); break;
			case (S):	  oMeshFnShape->setFaceColor(MColor(0.0, 1.0, 0.2), i); break;
			case (K):	  oMeshFnShape->setFaceColor(MColor(0.0, 0.7, 0.8), i); break;
			case (P):	  oMeshFnShape->setFaceColor(MColor(1.0, 1.0, 0.0), i); break;
			case (SK):	  oMeshFnShape->setFaceColor(MColor(0.4, 0.6, 0.1), i); break;
			case (KP):	  oMeshFnShape->setFaceColor(MColor(0.8, 0.3, 0.8), i); break;
			case (D12K):  oMeshFnShape->setFaceColor(MColor(1.0, 0.7, 0.4), i); break;
			case (K1Y):	  oMeshFnShape->setFaceColor(MColor(0.5, 1.0, 0.5), i); break;
			case (PY):	  oMeshFnShape->setFaceColor(MColor(1.0, 0.0, 1.0), i); break;
			case (YKY):   oMeshFnShape->setFaceColor(MColor(0.0, 1.0, 1.0), i); break;
			case (KYK):	  oMeshFnShape->setFaceColor(MColor(1.0, 0.0, 0.4), i); break;
			case (KPK):	  oMeshFnShape->setFaceColor(MColor(1.0, 1.0, 1.0), i); break;
			case (D312P): oMeshFnShape->setFaceColor(MColor(1.0, 0.5, 0.5), i); break;
			case (D123K): oMeshFnShape->setFaceColor(MColor(0.3, 0.3, 0.6), i); break;
		}
		MGlobal::executeCommand("setAttr " + outputShapeName + ".displayColors true");
	} 
	return MStatus::kSuccess;
}

MStatus StitchMeshNode::SetDefaultStitchType(int faceId)
{
	SubFace sf = MSubFaces[faceId];
	if		(sf.nBkwd == 2 && sf.nFwrd == 1) sf.stitchType = X;    
	else if (sf.nBkwd == 1 && sf.nFwrd == 2) sf.stitchType = Y1;   
	else if (sf.nBkwd == 2 && sf.nFwrd == 2) sf.stitchType = P;    
	else if (sf.nBkwd == 2 && sf.nFwrd == 3) sf.stitchType = KP;   
	else if (sf.nBkwd == 3 && sf.nFwrd == 2) sf.stitchType = D12K; 
	else if (sf.nBkwd == 2 && sf.nFwrd == 4) sf.stitchType = YKY;  
	else if (sf.nBkwd == 4 && sf.nFwrd == 2) sf.stitchType = D312P;
	MSubFaces[faceId] = sf;
	return MStatus::kSuccess;
}

//======================================================================================================================//
// Stitch level editing functions																						//
//======================================================================================================================//

MStatus StitchMeshNode::ChangeStitchType(int faceId, int stitchType)
{
	SubFace sf = MSubFaces[faceId];
	switch (stitchType) {
		case (X):	  if (sf.nBkwd == 2 && sf.nFwrd == 1) { sf.stitchType = stitchType; } break;
		case (Y1):	  if (sf.nBkwd == 1 && sf.nFwrd == 2) { sf.stitchType = stitchType; } break;
		case (S):	  if (sf.nBkwd == 2 && sf.nFwrd == 2) { sf.stitchType = stitchType; } break;
		case (K):	  if (sf.nBkwd == 2 && sf.nFwrd == 2) { sf.stitchType = stitchType; } break;
		case (P):	  if (sf.nBkwd == 2 && sf.nFwrd == 2) { sf.stitchType = stitchType; } break;
		case (SK):	  if (sf.nBkwd == 2 && sf.nFwrd == 3) { sf.stitchType = stitchType; } break;
		case (KP):	  if (sf.nBkwd == 2 && sf.nFwrd == 3) { sf.stitchType = stitchType; } break;
		case (D12K):  if (sf.nBkwd == 3 && sf.nFwrd == 2) { sf.stitchType = stitchType; } break;
		case (K1Y):	  if (sf.nBkwd == 2 && sf.nFwrd == 3) { sf.stitchType = stitchType; } break;
		case (PY):	  if (sf.nBkwd == 2 && sf.nFwrd == 3) { sf.stitchType = stitchType; } break;
		case (YKY):   if (sf.nBkwd == 2 && sf.nFwrd == 4) { sf.stitchType = stitchType; } break;
		case (KYK):	  if (sf.nBkwd == 2 && sf.nFwrd == 4) { sf.stitchType = stitchType; } break;
		case (KPK):	  if (sf.nBkwd == 2 && sf.nFwrd == 4) { sf.stitchType = stitchType; } break;
		case (D312P): if (sf.nBkwd == 4 && sf.nFwrd == 2) { sf.stitchType = stitchType; } break;
		case (D123K): if (sf.nBkwd == 4 && sf.nFwrd == 2) { sf.stitchType = stitchType; } break;
		default: SetDefaultStitchType(faceId); break;
	}
	MSubFaces[faceId] = sf;
	return MStatus::kSuccess;
}

MStatus StitchMeshNode::RemoveWaleEdge(int id)
{
	//oMeshFnShape->setObject(outputMeshObj);
	
	int p;
	MItMeshEdge edgeIter(outputMeshObj);
	edgeIter.setIndex(id, p);
		
	// assert edge is wale edge
	// TODO

	// find two connected faces
	MIntArray connectedFaceIds;
	edgeIter.getConnectedFaces(connectedFaceIds);
	int minId = min(connectedFaceIds[0],connectedFaceIds[1]);
	int maxId = max(connectedFaceIds[0],connectedFaceIds[1]);
	SubFace subface1 = MSubFaces[minId];
	SubFace subface2 = MSubFaces[maxId];
		
	// find sum of numBkwd values and sum of numFwrd values
	int bSum = subface1.nBkwd + subface2.nBkwd - 1;
	int fSum = subface1.nFwrd + subface2.nFwrd - 1;
	if ((bSum > 4) || (fSum > 4)) return MStatus::kInvalidParameter;
		
	// set numBkwd and numFwrd for combined face
	subface1.nBkwd = bSum;
	subface1.nFwrd = fSum;
	MSubFaces[minId] = subface1;
	SetDefaultStitchType(minId);

	// delete edge
	oMeshFnShape->deleteEdge(id);
	oMeshFnShape->updateSurface();
	MSubFaces.erase(MSubFaces.begin() + maxId);

	return MStatus::kSuccess;
}

MStatus StitchMeshNode::InsertWaleEdge(int vertexId1, int vertexId2)
{
	return MStatus::kSuccess;
}

//======================================================================================================================//
// Functions for relaxing the stitch mesh and generating the stitches													//
//======================================================================================================================//

MStatus StitchMeshNode::RelaxMesh(void)
{
	// TODO: Compute r_wale and r_course
	// TODO: Calculate Forces
		// for each face f in the mesh
			// for each subface s in f
				// for each vertex i in s
					// for each other vertex j in s
						// force on i += computeStretchForce(j)
				// for each vertex j in s
					// for each pair of neighbors i and k in s
						// shearForce = computeShearForce(i,j,k)
						// force on j += shearForce
						// force on i -= shearForce / 2.0
						// force on k -= shearForce / 2.0
		// for each wale edge w1 connecting vertex i & j
			// find next wale edge w2 connecting vertex j & k
			// wale strut force = computeWaleStrutForce(i,j,k)
			// force on i += wale strut force
			// force on k -= wale strut force
	// TODO: Solve System
	return MStatus::kSuccess;
}

MStatus StitchMeshNode::GenerateStitches(void)
{
	float maxEdgeLength = -1;
	MItMeshEdge subfaceEdges(outputMeshObj);
	while (!subfaceEdges.isDone()) {
		double length;
		subfaceEdges.getLength(length);
		if (maxEdgeLength < length || maxEdgeLength < 0)
			maxEdgeLength = length;
		subfaceEdges.next();
	}
	float offset = 1.0 / maxEdgeLength;

	MPointArray vertices;
	oMeshFnShape->getPoints(vertices);
	MItMeshPolygon subfaceIter(outputMeshObj);
	int p;

	MString circleCmd = MString("circle -c 0 0 0 -nr 0 1 0 -sw 360 -r ") + ((float) maxEdgeLength / 13.0f) 
					  + MString(" -d 3 -ut 0 -tol 0.01 -s 8 -ch 1 -name \"extrudingCircle\";");
	MGlobal::executeCommand(circleCmd);

	int numTotalCurves = 0;
	std::vector<MString> curve_names;
	std::vector<int> curve_subfaceIds;
	std::vector<std::vector<int>> subface_curveIds;
	std::vector<std::vector<MPoint>> curve_endpoints;

	for (int i = 0; i < MSubFaces.size(); i++)
	{
		// determine correct stitch type to generate
		SubFace subface = MSubFaces[i];
		Stitch stitch = stitches[subface.stitchType];		
		MVector subfaceNormal;
		if (subface.nBkwd == 1) 
			subfaceNormal = (subface.bkwdPoints[0]-subface.fwrdPoints[0]) ^ (subface.fwrdPoints[1]-subface.fwrdPoints[0]);
		else					
			subfaceNormal = (subface.bkwdPoints[1]-subface.bkwdPoints[0]) ^ (subface.fwrdPoints[0]-subface.bkwdPoints[0]);

		// create face specific cage from subface vertices
		MIntArray vtxIndices;
		MPointArray subfaceCage;
		subfaceIter.setIndex(i, p);
		subfaceIter.getVertices(vtxIndices);
		for (int j = 0; j < vtxIndices.length(); j++)
			subfaceCage.append(vertices[vtxIndices[j]]);

		std::vector<int> ids;
		subface_curveIds.push_back(ids);
		
		// build all yarns
		for (int j = 0; j < stitch.YarnCurves.size(); j++)
		{
			subface_curveIds[i].push_back(numTotalCurves);
			MString curveName = MString("stitchCurve_") + numTotalCurves;
			curve_names.push_back(curveName);
			curve_subfaceIds.push_back(i);

			std::vector<MPoint> endpoints;
			curve_endpoints.push_back(endpoints);

			YarnCurve yc = stitch.YarnCurves[j];
			MString curveCmd = "curve ";
			for (int k = 0; k < yc.CVweights.size(); k++)
			{
				// position in plane
				MPoint cv(0,0,0);
				MVC mvc(subfaceCage);
				MVCWeights weights = yc.CVweights[k];
				mvc.computeMVCPosition(weights, cv);
				
				// offset from plane
				cv += yc.CVoffsets[k] * subfaceNormal * offset * 3.0f;
				if (k == 0 || k == yc.CVweights.size()-1) curve_endpoints[numTotalCurves].push_back(cv);
						
				// add cv
				curveCmd += MString("-p ") + cv.x + " " + cv.y + " " + cv.z + " ";
			}

			// add knots
			for (int k = 0; k < yc.knotVector.size(); k++)
				curveCmd += MString("-k ") + yc.knotVector[k] + " ";
					
			// create yarn curve
			curveCmd += MString("-name \"") + curveName + "\";";
			MGlobal::executeCommand(curveCmd);
			
			// rebuild yarn curves for extrusion
			MString rebuildCmd = MString("rebuildCurve -ch 1 -rpo 1 -rt 0 -end 1 -kr 0 -kcp 0 -kep 0 -kt 0 -s 0 -d 3 -tol 0.01 \"") + curveName + "\"";
			MGlobal::executeCommand(rebuildCmd);

			numTotalCurves++;
		}
	}
	
	// Weighted Quick Union Algorithm with Path Compression for combining continuous stitch curves
	QuickUnion curveQU(numTotalCurves);
	for (int this_curveId = 0; this_curveId < numTotalCurves; this_curveId++)
	{
		MString this_curve = curve_names[this_curveId];
		int subfaceId = curve_subfaceIds[this_curveId];
		subfaceIter.setIndex(subfaceId, p);

		MIntArray connectedFaces;
		subfaceIter.getConnectedFaces(connectedFaces);

		for (int j = 0; j < connectedFaces.length(); j++)
		{
			for (int k = 0; k < subface_curveIds[connectedFaces[j]].size(); k++)
			{
				int that_curveId = subface_curveIds[connectedFaces[j]][k];
				MString that_curve = "stitchCurve_" + that_curveId;

				if (((curve_endpoints[this_curveId][0]-curve_endpoints[that_curveId][0]).length() < 1.0e-2 * maxEdgeLength) ||
					((curve_endpoints[this_curveId][0]-curve_endpoints[that_curveId][1]).length() < 1.0e-2 * maxEdgeLength) ||
					((curve_endpoints[this_curveId][1]-curve_endpoints[that_curveId][0]).length() < 1.0e-2 * maxEdgeLength) ||
					((curve_endpoints[this_curveId][1]-curve_endpoints[that_curveId][1]).length() < 1.0e-2 * maxEdgeLength))
				{
					curveQU.unite(this_curveId, that_curveId);
				}
			}
		}
	}
	
	int threadCount = 0;
	std::vector<bool> isAttached;
	for (int i = 0; i < numTotalCurves; i++) { isAttached.push_back(false); }

	for (int i = 0; i < numTotalCurves; i++)
	{
		if (!isAttached[i])
		{
			MString this_curveName = curve_names[i];
			MString attachCmd = "attachCurve -ch 0 -rpo 1 -kmk 0 -m 1 -bb 0.5 -bki 1 -p 1 " + this_curveName;

			int this_root = curveQU.root(i);

			int numOthers = 0;
			for (int j = i+1; j < numTotalCurves; j++)
			{
				if (curveQU.root(j) == this_root)
				{
					isAttached[j] = true;
					MString that_curveName = curve_names[j];
					attachCmd += " " + that_curveName;
					numOthers++;
				}
			}

			if (numOthers > 0)
				MGlobal::executeCommand(attachCmd);
			
			isAttached[i] = true;
			MString renameThreadCmd = "rename " + curve_names[i] + " thread_" + threadCount++;
			MGlobal::executeCommand(renameThreadCmd);
		}
	}

	cout << "number of threads = " << threadCount << endl;

	// delete old curves, leaving only combined threads
	for (int i = 0; i < numTotalCurves; i++) {
		MString deleteCmd = "if (`objExists " + curve_names[i] + "`) { delete " + curve_names[i] + "; };";
		MGlobal::executeCommand(deleteCmd);
	}

	// extrude along all remaing thread curves
	for (int i = 0; i < threadCount; i++) {
		cout << "extruding thread #" << i << endl;
		MString threadName = MString("thread_") + i;
		MString rebuildCmd = MString("rebuildCurve -ch 1 -rpo 1 -rt 0 -end 1 -kr 0 -kcp 0 -kep 0 -kt 0 -s 0 -d 3 -tol 0.01 \"") + threadName + "\"";
		MGlobal::executeCommand(rebuildCmd);
		MString extrudeCmd = MString("extrude -ch true -rn false -po 1 -et 2 -ucp 1 -fpt 1 -upn 1 -rotation 0 -scale 1 -rsp 1 \"extrudingCircle\" \"") + threadName + "\";";
		MGlobal::executeCommand(extrudeCmd);
	} MGlobal::executeCommand("delete extrudingCircle");

	return MStatus::kSuccess;
}

//======================================================================================================================//
// Compute the new Node when the parameters are adjusted																//
//======================================================================================================================//

MStatus StitchMeshNode::compute(const MPlug& plug, MDataBlock& data)
{
	if (plug == attr_outMesh) {
	
		// get object names for input/output
		MDataHandle inputMeshNameHandle  = data.inputValue(inputMeshName);
		MDataHandle outputMeshNameHandle = data.inputValue(outputMeshName);
		inputShapeName  = inputMeshNameHandle.asString();
		outputShapeName = outputMeshNameHandle.asString();

		// Get default stitch size data
		MDataHandle stitchSizeHandle = data.inputValue(attr_stitchSize);
		stitchSize = stitchSizeHandle.asFloat();

		// Get stitch generation stage data
		MDataHandle nodeStageHandle = data.inputValue(attr_nodeStage);
		int nodeStage = nodeStageHandle.asInt();

		// Get input attribute data
		MDataHandle inMeshHandle = data.inputValue(attr_inMesh);
		MObject inputMesh = inMeshHandle.asMesh();
		if (inputMeshFn	== NULL) inputMeshFn = new MFnMesh(inputMesh);
		inputMeshFn->copy(inputMesh, inputMeshObj);

		// Get output attribute data
		MDataHandle outMeshHandle = data.outputValue(attr_outMesh);
		if (oMeshFnShape == NULL) oMeshFnShape = new MFnMesh;
		
		// Perform appropriate compute function
		switch (nodeStage) {
			
			//TODO: case (CLEAR_SELECTION):

			// User interaction stage for selecting edge loops in direction of stitch
			case (LOOP_SELECTION):
				if (callbackId == -1) {
					for (int i = 0; i < inputMeshFn->numPolygons(); i++) {
						faceLoopIndex.append(-1);
						faceLoopNumber.append(-1);
					}
					callbackId = MEventMessage::addEventCallback("SelectionChanged", LabelEdgeRows, this);
				}
				selectNodeAndBreak;

			// Tessellate mesh according to stitch size
			case (TESSELLATION):
				if (numLoopFaces == inputMeshFn->numPolygons()) {
					outputMeshObj = MFnMeshData().create();
					oMeshFnShape->copy(inputMesh, outputMeshObj);
					oMeshFnShape->setObject(outputMeshObj);
					TessellateInputMesh();
					outMeshHandle.set(outputMeshObj);
				} selectNodeAndBreak;

			// Update output mesh colors to for stitch level editing			
			case (STITCH_EDITING):
				StitchMeshNode::ColorByStitchType();
				outMeshHandle.setMObject(outputMeshObj);
				selectNodeAndBreak;

			// Replace tessellated faces with stitch models			
			case (YARN_GENERATION):
				RelaxMesh();
				GenerateStitches();
				MGlobal::executeCommand(MString("hide ") + outputShapeName);
				selectNodeAndBreak;

			// Default case; should never reach this
			default:
				break;
		}

		//--------------------------------------------------------------------------------------------------------------//
		// End compute switch statement																					//
		//--------------------------------------------------------------------------------------------------------------//

		data.setClean( plug );
		return MStatus::kSuccess;
	} 
	else return MS::kUnknownParameter;
}
