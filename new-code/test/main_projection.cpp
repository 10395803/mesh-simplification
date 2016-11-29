/*!	\file	main_projection.cpp
	\brief	A small executable testing the class projection<Triangle>. */
	
#include <iostream>
#include <chrono>

#include "projection.hpp"

int main()
{
	using namespace geometry;
	using namespace std::chrono;
	
	//
	// Read mesh from file
	//
	
	string inputfile("../../mesh/bunny.inp");
	projection<Triangle> prj(inputfile);
	
	//
	// Edge (6721,16057)
	//
	// Elements connected to 6721: 886,888,1119,8034,8194,8356
	// Elements connected to 16057: 1552,1833,1983,5775,8034,8194
	
	{
		#ifdef NDEBUG
		high_resolution_clock::time_point start, stop;
		start = high_resolution_clock::now();
		for (UInt t = 0; t < 1e5; ++t)
		{
		#endif
			UInt id1(6721), id2(16057);
	
			// Extract elements and data involved in collapsing
			auto invElems = prj.getElemsInvolvedInEdgeCollapsing(id1,id2);
			auto toRemove = prj.getElemsOnEdge(id1,id2);
			auto toKeep = prj.getElemsModifiedInEdgeCollapsing(id1,id2);
			auto toMove = prj.getDataModifiedInEdgeCollapsing(invElems);
	
			// Project
			auto temp = prj.project(toMove, invElems);
			prj.undo(toMove, temp);	
		#ifdef NDEBUG
		}
		stop = high_resolution_clock::now();
		auto duration = duration_cast<milliseconds>(stop-start).count();
		cout << "Elapsed time: " << duration << " ms" << endl;
		#endif
	}
	
	//
	// Collapse the edge (6721,16057)
	//
	
	{
		UInt id1(6721), id2(16057);
		
		// New point location
		// Uncomment only one line at time
		//point newPoint(prj.getPointerToMesh()->getNode(id1));
		//point newPoint(prj.getPointerToMesh()->getNode(id2));
		point newPoint(0.7*(prj.getPointerToMesh()->getNode(id1) + prj.getPointerToMesh()->getNode(id2)));
		
		// Modify id1, set id2 inactive
		prj.getPointerToMesh()->setNode(id1, newPoint);
		prj.getPointerToMesh()->setNodeInactive(id2);
		
		// Extract elements and data involved in collapsing
		auto invElems = prj.getElemsInvolvedInEdgeCollapsing(id1,id2);
		auto toRemove = prj.getElemsOnEdge(id1,id2);
		auto toKeep = prj.getElemsModifiedInEdgeCollapsing(id1,id2);
		auto toMove = prj.getDataModifiedInEdgeCollapsing(invElems); 
				
		// Update node-node, node-element, element-element connections
		prj.getPointerToConnectivity()->applyEdgeCollapsing(id2, id1, toRemove, toKeep);
		
		// Project data points and update data-element and element-data connections
		prj.project(toMove, toKeep);
	}
}
