#include "Stitch.h"

//----------------------------------------------------------------------//
// YarnCurve class function definitions									//
//----------------------------------------------------------------------//

YarnCurve::~YarnCurve(void)
{
}

YarnCurve::YarnCurve(void)
{
	cageVertices.clear();
	knotVector.clear();
	CVoffsets.clear();
	CVweights.clear();
}

YarnCurve::YarnCurve(MPointArray corners)
{
	cageVertices.copy(corners);
	knotVector.clear();
	CVoffsets.clear();
	CVweights.clear();
}

MStatus
YarnCurve::addCV(MPoint cv)
{
	return addCV(cv.x, cv.y, cv.z);
}

MStatus
YarnCurve::addCV(float cvX, float cvY, float cvZ)
{
	MPoint cv(cvX, 0.0, cvZ);

	// create new set of MVC weights for CV
	MVCWeights weights;
	MVC mvc(cageVertices);
	mvc.computeMVCWeights(weights, cv);

	// add CV to curve
	CVweights.push_back(weights);
	CVoffsets.push_back(cvY);

	// return success
	return MStatus::kSuccess;
}

MStatus
YarnCurve::addKnot(int k)
{
	knotVector.push_back(k);
	return MStatus::kSuccess;
}

//----------------------------------------------------------------------//
// Stitch class function definitions									//
//----------------------------------------------------------------------//

Stitch::Stitch(void)
{
	cageVertices.clear();
	YarnCurves.clear();
}

Stitch::Stitch(MPointArray corners)
{
	cageVertices.copy(corners);
	YarnCurves.clear();
}

Stitch::~Stitch(void)
{
}

MStatus
Stitch::addYarnCurve(YarnCurve y)
{
	YarnCurves.push_back(y);
	return MStatus::kSuccess;
}