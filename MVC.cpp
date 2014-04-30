
#include "MVC.h"

//------------------------------------------------------//
// initialize quick union data structure				//
//------------------------------------------------------//

MVC::MVC(MPointArray &cageVertices)
{
	this->cageVertices.copy(cageVertices);
}

MVC::~MVC(void)
{
}

//------------------------------------------------------//
// find root of any item								//
//------------------------------------------------------//

MStatus	MVC::computeMVCWeights(MVCWeights &weights, MPoint &p)
{
	int numCageVertices = cageVertices.length();

	// check if point is coincident with one of the vertices
	for (int i = 0; i < numCageVertices; i++)
	{
		float r1 = (p - cageVertices[i]).length();
		if (r1 < 1.0e-3) {
			for (int j = 0; j < numCageVertices; j++)
				weights.append(0.0);
			weights[i] = 1.0;
			return MStatus::kSuccess;
		}
	}

	float totalWeight = 0.0;

	// go through each corner of base stitch model
	// to calculate corner-specific MVC weights
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
		MVector r0 = v0 - p;
		MVector r1 = v1 - p;
		MVector r2 = v2 - p;


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

	return MStatus::kSuccess;
}

MStatus MVC::computeMVCPosition(MVCWeights &weights, MPoint &p)
{
	if (cageVertices.length() != weights.length())
		return MStatus::kInvalidParameter;

	MPoint weightedPoint(0.0, 0.0, 0.0);
	for (int i = 0; i < weights.length(); i++)
		weightedPoint += weights[i] * MVector(cageVertices[i]);
	p = weightedPoint;

	return MStatus::kSuccess;
}