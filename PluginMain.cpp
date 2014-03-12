#include <maya/MPxCommand.h>
#include <maya/MFnPlugin.h>
#include <maya/MIOStream.h>
#include <maya/MString.h>
#include <maya/MArgList.h>
#include <maya/MGlobal.h>
#include <maya/MSimple.h>
#include <maya/MDoubleArray.h>
#include <maya/MPoint.h>
#include <maya/MPointArray.h>
#include <maya/MFnNurbsCurve.h>
#include <maya/MDGModifier.h>
#include <maya/MPlugArray.h>
#include <maya/MVector.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MStringArray.h>
#include <list>

#include "StitchMeshNode.h"

MStatus initializePlugin( MObject obj )
{
    MStatus   status = MStatus::kSuccess;
    MFnPlugin plugin( obj, "StitchMesh", "1.0", "Any");

	// Register Node
	status = plugin.registerNode("StitchMeshNode", StitchMeshNode::id,
						 StitchMeshNode::creator, StitchMeshNode::initialize);
	if (!status) {
		status.perror("registerNode");
		return status;
	}
	
	//----------------------------------------------------------------------------------------------//
	// Create Menu For LSystemCmd																	//
	//----------------------------------------------------------------------------------------------//
	
	// Remove existing menu if it still exists
	MGlobal::executeCommand("if(`menu -ex StitchMeshesMenu`) deleteUI -m StitchMeshesMenu");

	// Add menu to main Maya window
	MGlobal::executeCommand("menu -l \"Stitch Meshes\" -p MayaWindow StitchMeshesMenu");
	
	// Add menu options for calling commands
	MString menuCmd2 = "menuItem -l \"Create Stitch Mesh Node\" -command \"source \\\"" + plugin.loadPath() + "/StitchMeshNode.mel\\\"\"";
	MGlobal::executeCommand(menuCmd2);

	//----------------------------------------------------------------------------------------------//
	// End Menu Functions                                                                           //
	//----------------------------------------------------------------------------------------------//

    return status;
}

MStatus uninitializePlugin( MObject obj)
{
    MStatus   status = MStatus::kSuccess;
    MFnPlugin plugin( obj );

	status = plugin.deregisterNode(StitchMeshNode::id);
	if (!status) {
		status.perror("deregisterNode");
		return status;
	}

	// Remove existing menu if it still exists
	MGlobal::executeCommand("if(`menu -ex StitchMeshesMenu`) deleteUI -m StitchMeshesMenu");

    return status;
}


