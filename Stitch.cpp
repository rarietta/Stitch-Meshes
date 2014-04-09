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
	float totalWeight = 0.0;

	// go through each corner of base stitch model
	// to calculate corner-specific MVC weights
	int numCageVertices = cageVertices.length();
	for (int i = 0; i < numCageVertices; i++)
	{
		// cage vertices relevant for i-th weight
		MPoint v0, v1, v2;
		v1 = cageVertices[i];
		if (i == 0) v0 = cageVertices[numCageVertices-1];
		else		v0 = cageVertices[i-1];
		if (i == numCageVertices-1) v2 = cageVertices[0];
		else						v2 = cageVertices[i+1];

		// vectors from CV to cage vertices
		MVector r0 = v0 - cv;
		MVector r1 = v1 - cv;
		MVector r2 = v2 - cv;

		// angles
		float a0 = r0.angle(r1);
		float a1 = r1.angle(r2);
		
		// calculate MVC weight for each
		float weight_i = 2.0 * (tan(a0/2.0) + tan(a1/2.0)) / r1.length();

		// append weight_i to list of CV's MVC weights
		weights.append(weight_i);
		totalWeight += weight_i;
	}

	// normalize weights
	for (int i = 0; i < weights.length(); i++)
		weights[i] = weights[i] / totalWeight;

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