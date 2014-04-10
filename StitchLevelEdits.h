#ifndef StitchLevelEdits_H_
#define StitchLevelEdits_H_

#include <maya/MPxCommand.h>
#include <maya/MSyntax.h>
#include <maya/MArgDatabase.h>
#include <string>

// Flags
static const char* kFaceIndexFlag      = "-f";
static const char* kFaceIndexLongFlag  = "-faceIndex";
static const char* kStitchTypeFlag     = "-s";
static const char* kStitchTypeFlagLong = "-stitchType";
static const char* kNodeNameFlag	   = "-n";
static const char* kNodeNameFlagLong   = "-nodeName";

class ChangeStitchTypeCmd : public MPxCommand
{
	public:
		ChangeStitchTypeCmd();
		virtual ~ChangeStitchTypeCmd();
		static void* creator() { return new ChangeStitchTypeCmd(); }
		MStatus doIt( const MArgList& args );
		static MSyntax syntaxCreator();
};

#endif