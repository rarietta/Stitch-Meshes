#include "StitchLevelEdits.h"
#include "StitchMeshNode.h"

//======================================================================================================================//
// Helper function for getting selected indices																			//
//======================================================================================================================//

MStatus GetSelectedIndices(MIntArray& indices, MString componentType)
{
	// Get the current selection list from maya
	MSelectionList userSelected;
	MGlobal::getActiveSelectionList(userSelected);
	MStringArray selections;
	userSelected.getSelectionStrings(selections);
	for (int i = 0; i < selections.length(); i++)
	{
		// split the name of the selection to get the edge index
		char* pch = strtok((char *) selections[i].asChar(), ".[]");
		vector<char*> selectedNameTokens;
		while (pch != NULL) {
			selectedNameTokens.push_back(pch);
			pch = strtok(NULL, ".[]");
		}
		
		// return the edge index
		if (selectedNameTokens.size() > 2 && strcmp(selectedNameTokens[1], componentType.asChar()) == 0) {
			pch = strtok(selectedNameTokens[2], ":");
			vector<char*> consecutiveIndices;
			while (pch != NULL) {
				consecutiveIndices.push_back(pch);
				pch = strtok(NULL, ":");
			}

			int lower = atoi(consecutiveIndices[0]);
			int upper = atoi(consecutiveIndices[0]);
			if (consecutiveIndices.size() > 1) upper = atoi(consecutiveIndices[1]);
			for (int j = lower; j <= upper; j++)
				indices.append(j);
		}

		else return MStatus::kInvalidParameter;
	}

	return MStatus::kSuccess;
}

//======================================================================================================================//
// Change stitch type of selected tessellation faces																	//
//======================================================================================================================//

ChangeStitchTypeCmd::ChangeStitchTypeCmd() : MPxCommand()
{
}

ChangeStitchTypeCmd::~ChangeStitchTypeCmd() 
{
}

MStatus ChangeStitchTypeCmd::doIt( const MArgList& args )
{
	MStatus returnStatus;

	// Get input arguments if provided
	int stitchType;
	MString nodeName;
	MArgDatabase argData = MArgDatabase(syntax(), args);
	if (argData.isFlagSet(kStitchTypeFlag))
		stitchType = argData.flagArgumentString(kStitchTypeFlag, 0).asInt();
	if (argData.isFlagSet(kNodeNameFlag))
		nodeName = MString(argData.flagArgumentString(kNodeNameFlag, 0).asChar());
	
	// Get the current selection list from maya
	MIntArray indices;
	if (!GetSelectedIndices(indices, "f")) return MStatus::kInvalidParameter;
	
	// Obtain user node for stitch level operations
	MSelectionList nodeSelection;
	MGlobal::executeCommand("select " + nodeName);
	MGlobal::getActiveSelectionList(nodeSelection);
	MObject selectedMObject;
	nodeSelection.getDependNode(0, selectedMObject);
	MFnDependencyNode dep(selectedMObject);
	StitchMeshNode *node = (StitchMeshNode *)dep.userNode();
	
	for (int i = 0; i < indices.length(); i++)
		node->ChangeStitchType(indices[i], stitchType);
	MGlobal::executeCommand("setAttr " + nodeName + ".nodeStage 2");

	// Return
	return MStatus::kSuccess;
}

// Syntax Creator
MSyntax ChangeStitchTypeCmd::syntaxCreator()
{
    MSyntax syntax;
	syntax.addFlag(kStitchTypeFlag,	kStitchTypeFlagLong, MSyntax::kUnsigned);
	syntax.addFlag(kNodeNameFlag,   kNodeNameFlagLong,   MSyntax::kString  );
    return syntax;
}   


//======================================================================================================================//
// TODO - Insert wale edge into stitch mesh and split intersected stitch face											//
//======================================================================================================================//

InsertWaleEdgeCmd::InsertWaleEdgeCmd() : MPxCommand()
{
}

InsertWaleEdgeCmd::~InsertWaleEdgeCmd() 
{
}

MStatus InsertWaleEdgeCmd::doIt( const MArgList& args )
{
	MStatus returnStatus;

	// Get input arguments if provided
	MString nodeName;
	MArgDatabase argData = MArgDatabase(syntax(), args);
	if (argData.isFlagSet(kNodeNameFlag))
		nodeName = MString(argData.flagArgumentString(kNodeNameFlag, 0).asChar());
	
	// Get the current selection list from maya
	MIntArray indices;
	if (!GetSelectedIndices(indices, "e")) return MStatus::kInvalidParameter;
	
	// Obtain user node for stitch level operations
	MSelectionList nodeSelection;
	MGlobal::executeCommand("select " + nodeName);
	MGlobal::getActiveSelectionList(nodeSelection);
	MObject selectedMObject;
	nodeSelection.getDependNode(0, selectedMObject);
	MFnDependencyNode dep(selectedMObject);
	StitchMeshNode *node = (StitchMeshNode *)dep.userNode();

	//TODO: Stitch Edit Function Implementation
	
	MGlobal::executeCommand("setAttr " + nodeName + ".nodeStage 2");

	// Return
	return MStatus::kSuccess;
}

MSyntax InsertWaleEdgeCmd::syntaxCreator()
{
    MSyntax syntax;
	syntax.addFlag(kNodeNameFlag, kNodeNameFlagLong, MSyntax::kString);
    return syntax;
}   

//======================================================================================================================//
// TODO - Remove wale edge from stitch mesh and combine adjacent stitch faces											//
//======================================================================================================================//

RemoveWaleEdgeCmd::RemoveWaleEdgeCmd() : MPxCommand()
{
}

RemoveWaleEdgeCmd::~RemoveWaleEdgeCmd() 
{
}

MStatus RemoveWaleEdgeCmd::doIt( const MArgList& args )
{
	MStatus returnStatus;

	// Get input arguments if provided
	MString nodeName;
	MArgDatabase argData = MArgDatabase(syntax(), args);
	if (argData.isFlagSet(kNodeNameFlag))
		nodeName = MString(argData.flagArgumentString(kNodeNameFlag, 0).asChar());
	
	// Get the current selection list from maya
	MIntArray indices;
	GetSelectedIndices(indices, "e");
	
	// Obtain user node for stitch level operations
	MSelectionList nodeSelection;
	MGlobal::executeCommand("select " + nodeName);
	MGlobal::getActiveSelectionList(nodeSelection);
	MObject selectedMObject;
	nodeSelection.getDependNode(0, selectedMObject);
	MFnDependencyNode dep(selectedMObject);
	StitchMeshNode *node = (StitchMeshNode *)dep.userNode();

	//TODO: Stitch Edit Function Implementation
	//for (int i = 0; i < indices.length(); i++) {
		//node->RemoveWaleEdge(indices[i]);
		//node->oMeshFnShape->updateSurface();
	//}

	MGlobal::executeCommand("setAttr " + nodeName + ".nodeStage 2");

	// Return
	return MStatus::kSuccess;
}

MSyntax RemoveWaleEdgeCmd::syntaxCreator()
{
    MSyntax syntax;
	syntax.addFlag(kNodeNameFlag, kNodeNameFlagLong, MSyntax::kString);
    return syntax;
}   

//======================================================================================================================//
// TODO - Shift wale edge of stitch mesh and adjust adjacent stitch faces												//
//======================================================================================================================//

ShiftWaleEdgeCmd::ShiftWaleEdgeCmd() : MPxCommand()
{
}

ShiftWaleEdgeCmd::~ShiftWaleEdgeCmd() 
{
}

MStatus ShiftWaleEdgeCmd::doIt( const MArgList& args )
{
	MStatus returnStatus;

	// Get input arguments if provided
	MString nodeName;
	MArgDatabase argData = MArgDatabase(syntax(), args);
	if (argData.isFlagSet(kNodeNameFlag))
		nodeName = MString(argData.flagArgumentString(kNodeNameFlag, 0).asChar());
	
	// Get the current selection list from maya
	MIntArray indices;
	GetSelectedIndices(indices, "e");
	
	// Obtain user node for stitch level operations
	MSelectionList nodeSelection;
	MGlobal::executeCommand("select " + nodeName);
	MGlobal::getActiveSelectionList(nodeSelection);
	MObject selectedMObject;
	nodeSelection.getDependNode(0, selectedMObject);
	MFnDependencyNode dep(selectedMObject);
	StitchMeshNode *node = (StitchMeshNode *)dep.userNode();

	//TODO: Stitch Edit Function Implementation

	MGlobal::executeCommand("setAttr " + nodeName + ".nodeStage 2");

	// Return
	return MStatus::kSuccess;
}

MSyntax ShiftWaleEdgeCmd::syntaxCreator()
{
    MSyntax syntax;
	syntax.addFlag(kNodeNameFlag, kNodeNameFlagLong, MSyntax::kString);
    return syntax;
}   

//======================================================================================================================//
// TODO - Collapse course edge of stitch mesh and adjust adjacent stitch faces											//
//======================================================================================================================//

CollapseCourseEdgeCmd::CollapseCourseEdgeCmd() : MPxCommand()
{
}

CollapseCourseEdgeCmd::~CollapseCourseEdgeCmd() 
{
}

MStatus CollapseCourseEdgeCmd::doIt( const MArgList& args )
{
	MStatus returnStatus;

	// Get input arguments if provided
	MString nodeName;
	MArgDatabase argData = MArgDatabase(syntax(), args);
	if (argData.isFlagSet(kNodeNameFlag))
		nodeName = MString(argData.flagArgumentString(kNodeNameFlag, 0).asChar());
	
	// Get the current selection list from maya
	MIntArray indices;
	GetSelectedIndices(indices, "e");
	
	// Obtain user node for stitch level operations
	MSelectionList nodeSelection;
	MGlobal::executeCommand("select " + nodeName);
	MGlobal::getActiveSelectionList(nodeSelection);
	MObject selectedMObject;
	nodeSelection.getDependNode(0, selectedMObject);
	MFnDependencyNode dep(selectedMObject);
	StitchMeshNode *node = (StitchMeshNode *)dep.userNode();

	//TODO: Stitch Edit Function Implementation
	//	*assert edge is course edge
	//	*find two connected faces
	//	*for each face, check if either endpoint is the first face vertex
	//		if it is, then subtract one from the numBkwd value for the face
	//		if it is not, then subtract one from the numFwrd value for the face
	//	*change stitch type for each
	//	*MEL polyCollapseEdge command

	MGlobal::executeCommand("setAttr " + nodeName + ".nodeStage 2");

	// Return
	return MStatus::kSuccess;
}

MSyntax CollapseCourseEdgeCmd::syntaxCreator()
{
    MSyntax syntax;
	syntax.addFlag(kNodeNameFlag, kNodeNameFlagLong, MSyntax::kString);
    return syntax;
}   

//======================================================================================================================//
// TODO - Split course edges in stitch mesh and adjust adjacent stitch faces											//
//======================================================================================================================//

SplitCourseEdgeCmd::SplitCourseEdgeCmd() : MPxCommand()
{
}

SplitCourseEdgeCmd::~SplitCourseEdgeCmd() 
{
}

MStatus SplitCourseEdgeCmd::doIt( const MArgList& args )
{
	MStatus returnStatus;

	// Get input arguments if provided
	MString nodeName;
	MArgDatabase argData = MArgDatabase(syntax(), args);
	if (argData.isFlagSet(kNodeNameFlag))
		nodeName = MString(argData.flagArgumentString(kNodeNameFlag, 0).asChar());
	
	// Get the current selection list from maya
	MIntArray indices;
	GetSelectedIndices(indices, "e");
	
	// Obtain user node for stitch level operations
	MSelectionList nodeSelection;
	MGlobal::executeCommand("select " + nodeName);
	MGlobal::getActiveSelectionList(nodeSelection);
	MObject selectedMObject;
	nodeSelection.getDependNode(0, selectedMObject);
	MFnDependencyNode dep(selectedMObject);
	StitchMeshNode *node = (StitchMeshNode *)dep.userNode();

	//TODO: Stitch Edit Function Implementation

	MGlobal::executeCommand("setAttr " + nodeName + ".nodeStage 2");

	// Return
	return MStatus::kSuccess;
}

MSyntax SplitCourseEdgeCmd::syntaxCreator()
{
    MSyntax syntax;
	syntax.addFlag(kNodeNameFlag, kNodeNameFlagLong, MSyntax::kString);
    return syntax;
}   

//======================================================================================================================//
// TODO - Set series of wale edges as cable edges																		//
//======================================================================================================================//

SetCableEdges::SetCableEdges() : MPxCommand()
{
}

SetCableEdges::~SetCableEdges() 
{
}

MStatus SetCableEdges::doIt( const MArgList& args )
{
	MStatus returnStatus;

	// Get input arguments if provided
	MString nodeName;
	MArgDatabase argData = MArgDatabase(syntax(), args);
	if (argData.isFlagSet(kNodeNameFlag))
		nodeName = MString(argData.flagArgumentString(kNodeNameFlag, 0).asChar());
	
	// Get the current selection list from maya
	MIntArray indices;
	GetSelectedIndices(indices, "e");
	
	// Obtain user node for stitch level operations
	MSelectionList nodeSelection;
	MGlobal::executeCommand("select " + nodeName);
	MGlobal::getActiveSelectionList(nodeSelection);
	MObject selectedMObject;
	nodeSelection.getDependNode(0, selectedMObject);
	MFnDependencyNode dep(selectedMObject);
	StitchMeshNode *node = (StitchMeshNode *)dep.userNode();

	//TODO: Stitch Edit Function Implementation

	MGlobal::executeCommand("setAttr " + nodeName + ".nodeStage 2");

	// Return
	return MStatus::kSuccess;
}

MSyntax SetCableEdges::syntaxCreator()
{
    MSyntax syntax;
	syntax.addFlag(kNodeNameFlag, kNodeNameFlagLong, MSyntax::kString);
    return syntax;
}   

