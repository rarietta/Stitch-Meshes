
//======================================================================================//
// Implementation of QuickUnion taken from Robert Sedgwick at Princeton University		//
// https://www.cs.princeton.edu/~rs/AlgsDS07/01UnionFind.pdf
//======================================================================================//

#include "QuickUnion.h"

//------------------------------------------------------//
// initialize quick union data structure				//
//------------------------------------------------------//

QuickUnion::QuickUnion(int N)
{
	for (int i = 0; i < N; i++) {
		id.push_back(i);
		sz.push_back(1);
	}
}

QuickUnion::~QuickUnion(void)
{
}

//------------------------------------------------------//
// find root of any item								//
//------------------------------------------------------//

int QuickUnion::root(int i)
{
	while (i != id[i])
	{
		id[i] = id[id[i]];
		i = id[i];
	}
	return i;
}

//------------------------------------------------------//
// determine if two items are in union					//
//------------------------------------------------------//

bool QuickUnion::find(int p, int q)
{
	return root(p) == root(q);
}

//------------------------------------------------------//
// join to items into same union						//
//------------------------------------------------------//

void QuickUnion::unite(int p, int q)
{
	int i = root(p);
	int j = root(q);
	if (sz[i] < sz[j]) { id[i] = j; sz[j] += sz[i]; }
	else			   { id[j] = i; sz[i] += sz[j]; }
}