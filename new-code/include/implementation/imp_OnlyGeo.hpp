/*!	\file	imp_OnlyGeo.hpp
	\brief	Implementations of members of class OnlyGeo. */
	
#ifndef HH_IMPONLYGEO_HH
#define HH_IMPONLYGEO_HH

#include "array_operators.hpp"
#include "Eigen/Dense"

namespace geometry
{
	// Simplify notation for Eigen matrices and vectors
	using namespace Eigen;
	using Vector3r = Matrix<Real,3,1>;
	using Matrix3r = Matrix<Real,3,3>;
	
	
	//
	// Constructors
	//
	
	template<MeshType MT>
	OnlyGeo<MT>::OnlyGeo(bmeshOperation<Triangle,MT> * bmo) :
		bcost<Triangle, MT, OnlyGeo<MT>>(bmo)
	{
		// Set up list of Q matrices
		if (this->oprtr != nullptr)
			buildQs();
	}
	
	
	//
	// Access members
	//
	
	template<MeshType MT>
	INLINE array<Real,10> OnlyGeo<MT>::getQMatrix(const UInt & id) const
	{
		return Qs[id];
	}
	
	
	template<MeshType MT>
	INLINE vector<array<Real,10>> OnlyGeo<MT>::getQs() const
	{
		return Qs;
	}
	
	
	//
	// Handle list of Q matrices
	//
	
	template<MeshType MT>
	array<Real,10> OnlyGeo<MT>::getKMatrix(const UInt & id) const
	{
		assert(id < this->oprtr->getPointerToMesh()->getNumElems());
		
		// Extract the first vertex of the triangle
		auto elem = this->oprtr->getPointerToMesh()->getElem(id);
		auto p = this->oprtr->getPointerToMesh()->getNode(elem[0]);
		
		// Compute unit normal and (signed) distance from the origin
		// for the plane identified by the triangle
		auto N = this->oprtr->getNormal(id);
		auto d = -(N*p);
		
		// Construct matrix K
		return array<Real,10>({N[0]*N[0], N[0]*N[1], N[0]*N[2], N[0]*d,
			N[1]*N[1], N[1]*N[2], N[1]*d, N[2]*N[2], N[2]*d, d*d});
	}
	
	
	template<MeshType MT>
	void OnlyGeo<MT>::buildQs()
	{
		assert(this->oprtr != nullptr);
		
		// Extract number of nodes and elements
		auto numNodes = this->oprtr->getPointerToMesh()->getNumNodes();
		auto numElems = this->oprtr->getPointerToMesh()->getNumElems();
		
		// Reserve memory and initialize Q matrices to zero
		Qs.clear();
		Qs.reserve(numNodes);
		for (UInt i = 0; i < numNodes; ++i)
		{
			Qs.emplace_back();
			Qs[i] = {0.,0.,0.,0.,0.,0.,0.,0.,0.,0.};
		}
			
		// Loop over all elements, for each triangle compute the 
		// related matrix K and add it to Q matrices of the vertices 
		// of the triangle
		for (UInt j = 0; j < numElems; ++j)
		{
			auto elem = this->oprtr->getPointerToMesh()->getElem(j);
			auto K = getKMatrix(j);
			Qs[elem[0]] += K;
			Qs[elem[1]] += K;
			Qs[elem[2]] += K;
		}
	}
	
	
	template<MeshType MT>
	void OnlyGeo<MT>::imp_update(const UInt & id)
	{
		assert(this->oprtr != nullptr);
		
		// Extract the nodes connected to id
		auto nodes = this->oprtr->getPointerToConnectivity()->getNode2Node(id).getConnected();
		
		// 
		// Re-build Q matrix for the collapsing point
		//
		
		// Set Q to zero
		Qs[id] = {0.,0.,0.,0.,0.,0.,0.,0.,0.,0.};
		
		// Loop over all elements sharing the collapsing point,
		// compute K and add it to Q
		auto id_elems = this->oprtr->getPointerToConnectivity()->getNode2Elem(id).getConnected();
		for (auto elem : id_elems)
		{
			Qs[id] += getKMatrix(elem);
		}
		
		// 
		// Re-build Q matrix for all nodes connected to
		// the collapsing point
		//
		
		for (auto node : nodes)
		{
			// Set Q to zero
			Qs[node] = {0.,0.,0.,0.,0.,0.,0.,0.,0.,0.};
	
			// Loop over all elements sharing the node,
			// compute K and add it to Q
			auto node_elems = this->oprtr->getPointerToConnectivity()->getNode2Elem(node).getConnected();
			for (auto elem : node_elems)
				Qs[node] += getKMatrix(elem);
		}
	}
	
	
	//
	// Set methods
	//
	
	template<MeshType MT>
	void OnlyGeo<MT>::imp_setMeshOperation(bmeshOperation<Triangle,MT> * bmo)
	{
		this->oprtr = bmo;
		
		// (Re-)build list of Q matrices
		buildQs();
	}
	
	
	//
	// Get methods
	//
	
	template<MeshType MT>
	pair<bool,point> OnlyGeo<MT>::getOptimumPoint(const UInt & id1, const UInt & id2) const
	{
		// The point (x,y,z) minimizing the geometric cost function
		// is given by the linear system 
		//
		// | Q(0,0) Q(0,1) Q(0,2) | | x |   | -Q(0,3) |
		// | Q(1,0) Q(1,1) Q(1,2) | | y | = | -Q(1,3) |
		// | Q(2,0) Q(2,1) Q(2,2) | | z |   | -Q(2,3) |
		//
		// where Q is the matrix associated with the edge.
		// To solve this system, we employ the Eigen library.
		
		//
		// Compute the matrix Q associated with the edge
		//
		// It is the average of the matrices associated 
		// with the end-points
		
		auto Q = 0.5 * (Qs[id1] + Qs[id2]);
		
		//
		// Construct the linear system
		//
		
		// System matrix
		Matrix3r A;
		A << Q[0], Q[1], Q[2], 
			Q[1], Q[4], Q[5],
			Q[2], Q[5], Q[7];
			
		// Right-hand side
		Vector3r b(-Q[3], -Q[6], -Q[8]);
		
		//
		// Solve the linear system
		//
		// Exploit QR decomposition with column pivoting
		// This choice should be a good compromise bewteen
		// performance and accuracy
		
		Vector3r x = A.colPivHouseholderQr().solve(b);
		
		//
		// Check if the solution exists and return
		//
		// As suggested on the Eigen wiki, to know if the
		// solution exists one may compute the relative
		// a posteriori error and check it is below an
		// user-defined tolerance
		
		auto err = (A*x - b).norm() / b.norm();
		if (err < TOLL)
			return make_pair<bool,point>(true, {x(0), x(1), x(2)});
		return make_pair<bool,point>(false, {0.,0.,0.});
	}
	
	
	template<MeshType MT>
	vector<point> OnlyGeo<MT>::imp_getPointsList(const UInt & id1, const UInt & id2) const
	{
		// The collapsing point for the edge (P,Q) is searched
		// among the following alternatives:
		//	- P
		//	- Q
		//	- (P+Q)/2
		//	- optimum point
		// However, the list may be smaller due to the non-existence 
		// of the optimum point and/or boundary flags of P and Q.
		// Indeed, points on the boundaries can be moved only along
		// the boundaries, while triple points cannot be move at all.
		// This to preserve the initial shape of the domain.
		
		// Extract the end-points and their boundary flags
		point P(this->oprtr->getPointerToMesh()->getNode(id1));
		auto bP = P.getBoundary();
		point Q(this->oprtr->getPointerToMesh()->getNode(id2));
		auto bQ = Q.getBoundary();
		
		//
		// Both points are internal or on boundary
		//
		// In this case, all alternatives are potentially valid
		
		if (((bP == 0) && (bQ == 0)) || ((bP == 1) && (bQ == 1)))
		{
			auto ans = getOptimumPoint(id1,id2);
			if (ans.first)
				return {P, Q, 0.5*(P+Q), ans.second};
			return {P, Q, 0.5*(P+Q)};
		}
		
		// 
		// Unique valid alternative is the first end-point
		//
		
		if (((bP == 1) && (bQ == 0)) || ((bP == 2) && (bQ != 2)))
			return {P};
			
		// 
		// Unique valid alternative is the second end-point
		//
		
		if (((bP == 0) && (bQ == 1)) || ((bP != 2) && (bQ == 2)))
			return {P};
			
		//
		// Remaining scenario: both end-points are triple points
		//
		// In this case, no alternative is valid
		
		return {};
	}
	
	
	template<MeshType MT>
	Real OnlyGeo<MT>::imp_getCost(const UInt & id1, const UInt & id2, const point3d & p) const
	{
		// Extract the matrix Q associated to the edge
		auto Q = 0.5 * (Qs[id1] + Qs[id2]);
		
		// Compute the quadratic form
		return Q[0]*p[0]*p[0] + Q[4]*p[1]*p[1] + Q[7]*p[2]*p[2]
			+ 2*Q[1]*p[0]*p[1] + 2*Q[2]*p[0]*p[2] + 2*Q[5]*p[1]*p[2]
			+ 2*Q[3]*p[0] + 2*Q[6]*p[1] + 2*Q[8]*p[2] + Q[9];
	}
}

#endif
