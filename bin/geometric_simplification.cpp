#include <iostream>
#include <chrono>

#include "meshSimplification.h"

int main()
{	
	using namespace geometry;
	using namespace std;
	using namespace std::chrono;
	
	//
	// Read the mesh
	//
	
    // Variables 
    mesh2d<Triangle> surf;
    downloadMesh down;
    
    // Path to the file
    string filename("../mesh/pawn.inp");
    
    // Reading
    down.fileFromParaview(filename, &surf);
    
    //
    // Simplificate
    //
    
    UInt numNodesMax(2000);
        
    //simplification2d<Triangle> s(&surf);
    //s.simplificateGreedy(numNodesMax);
        
    //garlandCostFunction cf;
    //vector<UInt> pointMaterialId(2522, 0);
    //simplification2dCostFunctionBased s(&cf, &surf, pointMaterialId);
    //s.simplificateGreedy(numNodesMax);
    
    meshDataSimplification<Triangle> s;
    s.setMeshPointer(&surf);
    s.simplificationProcess(numNodesMax);
        
    createFile up;
    up.fileForParaview("../mesh/pawn_2000_033_033_033.inp", &surf);
}