// ==========================================================================
// SitchMeshNode.h
// ==========================================================================

#include <vector>
#include <maya/MGlobal.h>
#include <maya/MObject.h>
#include <maya/MIOStream.h>
#include <maya/MPointArray.h>
#include <maya/MFloatArray.h>

typedef MFloatArray MVCWeights;

//======================================================================================//
// Class that extends MObject to account for necessary features							//
// of polygonal input mesh faces after stitch direction decisions						//
//======================================================================================//

class YarnCurve 
{
public:
				YarnCurve();
				YarnCurve(MPointArray corners);
	virtual	   ~YarnCurve();

	MStatus		addCV(MPoint cv);
	MStatus		addCV(float cvX, float cvY, float cvZ);

	MPointArray cageVertices;			// corners of cage
	std::vector<int> knotVector;		// knot vector
	std::vector<float> CVoffsets;		// y-value of CV along normal
	std::vector<MVCWeights> CVweights;	// series of MVC weights for CV
};

class Stitch
{
public:
				Stitch(void);
	virtual	   ~Stitch();

	MStatus		addYarnCurve(YarnCurve curve);

	std::vector<YarnCurve> YarnCurves;
};