#include "Stitch.h"

YarnCurve::~YarnCurve(void)
{
}

YarnCurve::YarnCurve(void)
{
}

YarnCurve::YarnCurve(MPointArray corners)
{
	cageVertices = corners;
}

MStatus
YarnCurve::addCV(MPoint cv)
{
	return MStatus::kSuccess;
}

MStatus
YarnCurve::addCV(float cvX, float cvY, float cvZ)
{
	MPoint cv(cvX, 0.0, cvZ);

	// create new set of MVC weights for CV
	MVCWeights weights;

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
	}

	// add CV to curve
	CVweights.push_back(weights);
	CVoffsets.push_back(cvY);

	// return success
	return MStatus::kSuccess;
}

Stitch::~Stitch(void)
{
}

Stitch::Stitch(void)
{
}