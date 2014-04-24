#ifndef StitchLevelEdits_H_
#define StitchLevelEdits_H_

#include <maya/MPxCommand.h>
#include <maya/MSyntax.h>
#include <maya/MArgDatabase.h>
#include <string>

// Flags
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

class InsertWaleEdgeCmd : public MPxCommand
{
	public:
		InsertWaleEdgeCmd();
		virtual ~InsertWaleEdgeCmd();
		static void* creator() { return new InsertWaleEdgeCmd(); }
		MStatus doIt( const MArgList& args );
		static MSyntax syntaxCreator();
};

class RemoveWaleEdgeCmd : public MPxCommand
{
	public:
		RemoveWaleEdgeCmd();
		virtual ~RemoveWaleEdgeCmd();
		static void* creator() { return new RemoveWaleEdgeCmd(); }
		MStatus doIt( const MArgList& args );
		static MSyntax syntaxCreator();
};

class ShiftWaleEdgeCmd : public MPxCommand
{
	public:
		ShiftWaleEdgeCmd();
		virtual ~ShiftWaleEdgeCmd();
		static void* creator() { return new ShiftWaleEdgeCmd(); }
		MStatus doIt( const MArgList& args );
		static MSyntax syntaxCreator();
};

class CollapseCourseEdgeCmd : public MPxCommand
{
	public:
		CollapseCourseEdgeCmd();
		virtual ~CollapseCourseEdgeCmd();
		static void* creator() { return new CollapseCourseEdgeCmd(); }
		MStatus doIt( const MArgList& args );
		static MSyntax syntaxCreator();
};

class SplitCourseEdgeCmd : public MPxCommand
{
	public:
		SplitCourseEdgeCmd();
		virtual ~SplitCourseEdgeCmd();
		static void* creator() { return new SplitCourseEdgeCmd(); }
		MStatus doIt( const MArgList& args );
		static MSyntax syntaxCreator();
};

class SetCableEdges : public MPxCommand
{
	public:
		SetCableEdges();
		virtual ~SetCableEdges();
		static void* creator() { return new SetCableEdges(); }
		MStatus doIt( const MArgList& args );
		static MSyntax syntaxCreator();
};


#endif