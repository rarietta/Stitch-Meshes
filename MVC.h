// ==========================================================================
// SitchMeshNode.h
// ==========================================================================

#include <maya/MGlobal.h>
#include <maya/MObject.h>
#include <maya/MIOStream.h>
#include <maya/MPointArray.h>
#include <maya/MFloatArray.h>
#include <vector>

typedef MFloatArray MVCWeights;

class MVC
{
	private:
		MPointArray cageVertices;

	public:
				 MVC(MPointArray &cageVertices);
		virtual	~MVC();
		MStatus	 computeMVCWeights(MVCWeights &weights, MPoint &p);
		MStatus	 computeMVCPosition(MVCWeights &weights, MPoint &p);
};