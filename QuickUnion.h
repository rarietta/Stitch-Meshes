// ==========================================================================
// SitchMeshNode.h
// ==========================================================================

#include <maya/MGlobal.h>
#include <maya/MObject.h>
#include <maya/MIOStream.h>
#include <maya/MPointArray.h>
#include <vector>

//======================================================================================//
// Implementation of QuickUnion taken from Robert Sedgwick at Princeton University		//
// https://www.cs.princeton.edu/~rs/AlgsDS07/01UnionFind.pdf
//======================================================================================//

class QuickUnion
{
	private:
		std::vector<int> id;
		std::vector<int> sz;

	public:
				 QuickUnion(int N);
		virtual	~QuickUnion();
		int		 root(int i);
		bool	 find(int p, int q);
		void	 unite(int p, int q);
};