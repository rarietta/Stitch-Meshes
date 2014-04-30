#include "QuickUnion.h"


QuickUnion::~QuickUnion(void)
{
}

QuickUnion::QuickUnion(int N)
{
	for (int i = 0; i < N; i++) {
		id.push_back(i);
		sz.push_back(1);
	}
}

int
QuickUnion::root(int i)
{
	while (i != id[i])
	{
		id[i] = id[id[i]];
		i = id[i];
	}
	return i;
}

bool
QuickUnion::find(int p, int q)
{
	return root(p) == root(q);
}

void
QuickUnion::unite(int p, int q)
{
	int i = root(p);
	int j = root(q);
	if (sz[i] < sz[j]) { id[i] = j; sz[j] += sz[i]; }
	else			   { id[j] = i; sz[i] += sz[j]; }
}