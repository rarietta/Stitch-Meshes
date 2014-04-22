#include "StitchLevelEdits.h"
#include "StitchMeshNode.h"

ChangeStitchTypeCmd::ChangeStitchTypeCmd() : MPxCommand()
{
}

ChangeStitchTypeCmd::~ChangeStitchTypeCmd() 
{
}

MStatus ChangeStitchTypeCmd::doIt( const MArgList& args )
{
	MStatus returnStatus;

	// Setup Argument Defaults
	MString nodeName = "none";
	int faceIndex = 0, stitchType = 0;

	// Get input arguments if provided
	MArgDatabase argData = MArgDatabase(syntax(), args);
	if (argData.isFlagSet(kFaceIndexFlag))
		faceIndex = argData.flagArgumentString(kFaceIndexFlag, 0).asInt();
	if (argData.isFlagSet(kStitchTypeFlag))
		stitchType = argData.flagArgumentString(kStitchTypeFlag, 0).asInt();
	if (argData.isFlagSet(kNodeNameFlag))
		nodeName = MString(argData.flagArgumentString(kNodeNameFlag, 0).asChar());
	
	// Get the current selection list from maya
	MSelectionList userSelected;
	MGlobal::getActiveSelectionList(userSelected);
	MStringArray selections;
	userSelected.getSelectionStrings(selections);
	std::vector<int> indices;
	for (int i = 0; i < selections.length(); i++)
	{
		cout << i << endl;
		cout << selections[i] << endl;

		// split the name of the selection to
		// get the edge index
		char* pch;
		pch = strtok((char *) selections[i].asChar(), ".[]");
		vector<char*> selectedNameTokens;
		while (pch != NULL) {
			selectedNameTokens.push_back(pch);
			pch = strtok(NULL, ".[]");
		}
		
		// return the edge index
		if (selectedNameTokens.size() > 2 && strcmp(selectedNameTokens[1], "f") == 0) {
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
				indices.push_back(j);
		}
	}

	for (int i = 0; i < indices.size(); i++) 
		cout << "selected index " << i << " = " << indices[i] << endl;

	// Obtain user node for stitch level operations
	MSelectionList nodeSelection;
	MGlobal::executeCommand("select " + nodeName);
	MGlobal::getActiveSelectionList(nodeSelection);
	MObject selectedMObject;
	nodeSelection.getDependNode(0, selectedMObject);
	MFnDependencyNode dep(selectedMObject);
	StitchMeshNode *node = (StitchMeshNode *)dep.userNode();

	for (int i = 0; i < indices.size(); i++)
		node->ChangeStitchType(indices[i], stitchType);
	MGlobal::executeCommand("setAttr " + nodeName + ".nodeStage 2");

	// Reselect initial data and return
	MGlobal::setActiveSelectionList(userSelected);
	return MStatus::kSuccess;
}

// Syntax Creator
MSyntax ChangeStitchTypeCmd::syntaxCreator()
{
    MSyntax syntax;
	syntax.addFlag(kFaceIndexFlag,	kFaceIndexLongFlag,  MSyntax::kUnsigned);
	syntax.addFlag(kStitchTypeFlag,	kStitchTypeFlagLong, MSyntax::kUnsigned);
	syntax.addFlag(kNodeNameFlag,   kNodeNameFlagLong,   MSyntax::kString  );
    return syntax;
}   

