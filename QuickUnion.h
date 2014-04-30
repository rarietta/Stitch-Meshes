// ==========================================================================
// SitchMeshNode.h
// ==========================================================================

#include <maya/MGlobal.h>
#include <maya/MObject.h>
#include <maya/MIOStream.h>
#include <maya/MPointArray.h>
#include <vector>

//======================================================================================//
// Class that extends MObject to account for necessary features							//
// of polygonal input mesh faces after stitch direction decisions						//
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