// ==========================================================================
// SitchMeshNode.h
// ==========================================================================

#include <maya/MGlobal.h>
#include <maya/MObject.h>
#include <maya/MIOStream.h>
#include <maya/MPointArray.h>

//======================================================================================//
// Class that extends MObject to account for necessary features							//
// of polygonal input mesh faces after stitch direction decisions						//
//======================================================================================//

class SubFace : public MObject
{
public:
				SubFace(int numBackward, int numForward);
	virtual	   ~SubFace();

	int			nBkwd;
	int			nFwrd;
	MPointArray bkwdPoints; // TODO: remove
	MPointArray fwrdPoints;	// TODO: remove
	int			stitchType;
	MVector		scaledNormal;
};