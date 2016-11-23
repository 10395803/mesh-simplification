/*!	\file 	structuredData.hpp
	\brief	Class building and handling the hexaedral Cartesian grid
			surrounding a three-dimensional mesh and employed in the
			structured data search. */
			
#ifndef HH_STRUCTUREDDATA_HH
#define HH_STRUCTUREDDATA_HH

#include <memory>
#include <unordered_set>

#include "boundingBox.hpp"
#include "bmesh.hpp"
#include "bmeshInfo.hpp"

namespace geometry
{
	/*!	The bounding boxes for each element of a three-dimensional
		mesh are stored in an unordered multi set. This should enable 
		a quick index-based access to the boxes. */
	template<typename SHAPE>
	class structuredData
	{
		private:
			/*!	Shared pointer to the mesh. */
			shared_ptr<bmesh<SHAPE>>	grid;
			
			/*!	Set of bounding boxes surrouing the elements. */
			unordered_multiset<bbox3d> 	boxes;
			
		public:
			//
			// Constructors
			//
			
			/*! (Default) constructor.
				\param g	the mesh */
			structuredData(const shared_ptr<bmesh<SHAPE>> & g = nullptr);
			
			/*!	Constructor.
				\param news	a bmeshInfo object
				
				\sa bmeshInfo.hpp */
			template<MeshType MT = MeshType::GEO>
			structuredData(bmeshInfo<SHAPE,MT> & news);
						
			/*!	Synthetic copy constructor.
				\param sd	another structured data */
			structuredData(const structuredData<SHAPE> & sd) = default;
			
			//
			// Operators
			//
			
			template<typename S>
			friend ostream & operator<<(ostream & out, const structuredData<S> & sd);
			
			//
			// Get methods
			//
			
			/*!	Get the mesh.
				\return		shared pointer to the mesh */
			shared_ptr<bmesh<SHAPE>> getMesh();
			
			/*!	Get bounding box surrouning an element.
				\param		element Id
				\return		the bounding box */
			bbox3d getBoundingBox(const UInt & Id) const;
			
			/*!	Get bounding boxes surrounding each element.
				\return		set of bounding boxes */
			unordered_multiset<bbox3d> getBoundingBox() const;
			
			/*!	Get Id's of elements whose bounding box intersect
				the bounding box of a given element.
				This method will be useful when checking for
				mesh self-intersections.
				
				\param Id	element Id
				\return		vector of Id's */ 
			vector<UInt> getIntersectingBoundingBoxes(const UInt & Id) const;
			
			//
			// Set methods
			//
			// These methods are designed to keep set of bounding boxes updated.
			
			/*!	Set the mesh.
				\param g	shared pointer to the mesh */
			void setMesh(const shared_ptr<bmesh<SHAPE>> & g);

			/*!	Set the mesh.
				\param news	a bmeshInfo object
				
				\sa bmeshInfo.hpp */			
			template<MeshType MT>
			void setMesh(bmeshInfo<SHAPE,MT> & news);
			
			//
			// Modify set of bounding boxes
			//
				
			/*!	Remove some elements from the set of bounding boxes.
				\param ids	vector of elements Id's */	
			void erase(const vector<UInt> & ids);
			
			/*!	Re-compute bounding boxes surrounding some elements.
				\param ids	vector of elements Id's */
			void update(const vector<UInt> & ids);
			
			/*!	Remove some elements from the set of bounding boxes
				and update some other ones.
				
				\param toRemove	elements to remove
				\param toKeep	elements to update */
			void update(const vector<UInt> & toRemove, const vector<UInt> & toKeep);
			
		private:
			//
			// Refresh methods
			//
			
			/*!	Re-build set of bounding boxes.
				\param news	a bmeshInfo object 
				
				\sa bmeshInfo.hpp */
			template<MeshType MT>
			void refresh(const bmeshInfo<SHAPE,MT> & news);
	};
}

/*!	Include implementations of class members. */
#include "implementation/imp_structuredData.hpp"

#endif