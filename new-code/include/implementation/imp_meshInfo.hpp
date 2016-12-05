/*!	\file	imp_meshInfo.hpp
	\brief	Implementations of members of class meshInfo. */
	
#ifndef HH_IMPMESHINFO_HH
#define HH_IMPMESHINFO_HH

namespace geometry
{
	//
	// Constructors (MeshType::GEO)
	//
	
	template<typename SHAPE>
	meshInfo<SHAPE, MeshType::GEO>::meshInfo(const bmesh<SHAPE> & bg) :
		bmeshInfo<SHAPE, MeshType::GEO>(bg)
	{
	}
	
		
	template<typename SHAPE>
	template<typename... Args>
	meshInfo<SHAPE, MeshType::GEO>::meshInfo(Args... args) :
		bmeshInfo<SHAPE, MeshType::GEO>(args...)
	{
	}
	
	
	//
	// Constructors (MeshType::DATA)
	//
	
	template<typename SHAPE>
	meshInfo<SHAPE, MeshType::DATA>::meshInfo
		(const bmesh<SHAPE> & bg) :
		bmeshInfo<SHAPE, MeshType::DATA>(bg)
	{
	}
	
	
	template<typename SHAPE>
	template<typename... Args>
	meshInfo<SHAPE, MeshType::DATA>::meshInfo(Args... args) :
		bmeshInfo<SHAPE, MeshType::DATA>(args...)
	{
	}
	
	
	//
	// Get topological info
	//
	
	template<typename SHAPE>
	vector<UInt> meshInfo<SHAPE, MeshType::DATA>::getDataInvolvedInEdgeCollapsing
		(const UInt & id1, const UInt & id2) const
	{
		// Get elements involved in the collapsing
		auto invElems = this->getElemsInvolvedInEdgeCollapsing(id1, id2);
		
		// A check (only debug mode)
		assert(invElems.size() > 0);
		
		// Get data associated to the involved elements
		if (invElems.size() == 1)
			return this->connectivity.elem2data[invElems[0]].getConnected();
		else
		{
			auto s = set_union(this->connectivity.elem2data[invElems[0]], 
				this->connectivity.elem2data[invElems[1]]);
			for (vector<UInt>::size_type el = 2; el < invElems.size(); el++)
				s = set_union(graphItem(s), this->connectivity.elem2data[invElems[el]]);
			return vector<UInt>(s.begin(), s.end());
		}
	}
	
	
	template<typename SHAPE>
	vector<UInt> meshInfo<SHAPE, MeshType::DATA>::getDataInvolvedInEdgeCollapsing
		(const vector<UInt> & invElems) const
	{
		assert(invElems.size() > 0);
		
		// Get data associated to the involved elements
		if (invElems.size() == 1)
			return this->connectivity.elem2data[invElems[0]].getConnected();
		else
		{
			auto s = set_union(this->connectivity.elem2data[invElems[0]], 
				this->connectivity.elem2data[invElems[1]]);
			for (vector<UInt>::size_type el = 2; el < invElems.size(); el++)
				s = set_union(graphItem(s), this->connectivity.elem2data[invElems[el]]);
			return vector<UInt>(s.begin(), s.end());
		}
	}
	
	
	template<typename SHAPE>
	vector<UInt> meshInfo<SHAPE, MeshType::DATA>::getDataModifiedInEdgeCollapsing
		(const UInt & id1, const UInt & id2) const
	{
		// Get elements involved in the collapsing
		auto invElems = this->getElemsInvolvedInEdgeCollapsing(id1, id2);
		
		// Get data involved in the collapsing
		auto invData = this->getDataInvolvedInEdgeCollapsing(invElems);
		
		// Keep only the data whose connected elements are all 
		// involved in the collapsing, i.e. disregard the data 
		// which lie on the border of the region involved in the collapsing
		return this->getDataModifiedInEdgeCollapsing(invElems, invData);
	}
	
	
	template<typename SHAPE>
	vector<UInt> meshInfo<SHAPE, MeshType::DATA>::getDataModifiedInEdgeCollapsing
		(const vector<UInt> & invElems) const
	{
		// Get data involved in the collapsing
		auto invData = getDataInvolvedInEdgeCollapsing(invElems);
		
		// Keep only the data whose connected elements are all 
		// involved in the collapsing, i.e. disregard the data 
		// which lie on the border of the region involved in the collapsing
		set<UInt> toKeep;
		for (auto datum : invData)
		{
			auto s = set_difference(this->connectivity.data2elem[datum], graphItem(invElems));
			if (s.size() == 0)
				toKeep.insert(datum);
		}
		
		return vector<UInt>(toKeep.begin(), toKeep.end()); 
	}
	
	
	template<typename SHAPE>
	vector<UInt> meshInfo<SHAPE, MeshType::DATA>::getDataModifiedInEdgeCollapsing
		(const vector<UInt> & invElems, const vector<UInt> & invData) const
	{
		// Keep only the data whose connected elements are all 
		// involved in the collapsing, i.e. disregard the data 
		// which lie on the border of the region involved in the collapsing
		set<UInt> toKeep;
		for (auto datum : invData)
		{
			auto s = set_difference(this->connectivity.data2elem[datum], graphItem(invElems));
			if (s.size() == 0)
				toKeep.insert(datum);
		}
		
		return vector<UInt>(toKeep.begin(), toKeep.end()); 
	}
	
	
	template<typename SHAPE>
	Real meshInfo<SHAPE, MeshType::DATA>::getQuantityOfInformation(const UInt & Id) const
	{
		assert(Id < this->connectivity.elem2data.size());
		
		Real Nt = 0.;
		
		// Loop over all data associated with the triangle
		auto data = this->connectivity.getElem2Data(Id).getConnected();
		for (auto datum : data)
		{
			// Extract number of elements the datum is associated with
			auto patch = this->connectivity.getData2Elem(datum).size();
			if (patch == 1)
				Nt += 1.;
			else
				Nt += 1./patch;
		}
		
		return Nt;
	}
	
	
	template<typename SHAPE>
	INLINE bool meshInfo<SHAPE, MeshType::DATA>::isEmpty(const UInt & Id) const
	{
		return (this->connectivity.elem2data[Id].size() == 0);
	}
}

#endif
