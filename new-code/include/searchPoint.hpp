/*! \file 	searchPoint.hpp
	\brief 	Class storing point information useful for structured data search. */
	
#ifndef HH_SEARCHPOINT_HH
#define HH_SEARCHPOINT_HH
	
#include "point.hpp"

namespace geometry 
{			
	/*! Class inheriting point and storing the indices for structured data search. 
		All methods are modified so to keep the indices updated after any operation. */
	class searchPoint final : public simplePoint
	{
		private:
			/*! ID. */
			UInt					Id;
			
			/*! Indices. */
			std::array<UInt,3> 		idx;
			
			/*! Static variables storing
				<ol>
				<li> the North-East point of the grid
				<li> the South-West point of the grid 
				<li> the cells sizes
				<li> the number of cells along each direction
				</ol>
				Note: this is done because the code is supposed to deal with one mesh at time. */
			static point			pNE;
			static point 			pSW;
			static array<Real,3> 	cellSize;
			static array<UInt,3> 	numCells;
			
		public:
			//
			// Constructors
			//
			
			/*! Constructor.
				\param idx	array with indices
				\param ID	point Id */
			searchPoint(const array<UInt,3> & idx, const UInt & ID = 0);
			
			/*! Constructor.
				\param x	first coordinate
				\param y	second coordinate
				\param z	third coordinate
				\param ID	point Id
				\param bond	boundary flag */
			searchPoint(const Real & x = 0.0, const Real & y = 0.0, const Real & z = 0.0, const UInt & ID = 0);
			
			/*! Constructor.
				\param c	array with coordinates
				\param ID	point Id
				\param bond	boundary flag */		
			searchPoint(const array<Real,3> & c, const UInt & ID = 0);
			
			/*! Constructor.
				\param p	a point */
			searchPoint(const point & p);
						
			/*! Copy constructor.
				\param p	a point */
			searchPoint(const searchPoint & p) = default;
					
			//
			// Operators
			//
			
			/*! The equality operator. 
				\param V	point */
			searchPoint & operator=(const searchPoint & V);

			/*! Less than operator: a searchPoint is "less" than another if its index is smaller than the other one. 
				\param pA	LHS
				\param pB	RHS 
				\return 	bool reporting the result */
			friend bool operator<(const searchPoint & pA, const searchPoint & pB);
			
			/*! Inequality operator: two points are equal if they fall within the same cell. 
				\param pA	LHS
				\param pB	RHS 
				\return 	bool reporting the result */ 
			friend bool operator!=(const searchPoint & pA, const searchPoint & pB);

			/*! Inequality operator: two points are equal if they fall within the same cell. 
				\param pA	LHS
				\param pB	RHS 
				\return 	bool reporting the result */
			friend bool operator==(const searchPoint & pA, const searchPoint & pB);
			
			/*! Access operator (non const version).
				\param i	component
				\return		index */
			UInt & operator[](const UInt & i);
			
			/*! Access operator (const version).
				\param i	component
				\return		index */
			UInt operator[](const UInt & i) const;
			
			/*! Output stream operator.
				\param out	output stream
				\param p	point to print
				\return update output stream */
			friend ostream & operator<<(ostream & out, const searchPoint & p);
					
			//
			// Get methods 
			//
			
			/*! Get the Id.
				\return		the point Id */
			UInt getId() const;
			
			/*! Get North-East point.
				\return 	the North-East point of the grid. */
			static point getPNE();
						
			/*! Get South-West point.
				\return 	the South-West point of the grid. */
			static point getPSW();
			
			/*! Get one size of the cells.
				\param i	component
				\return 	the size */
			static Real getCellSize(const UInt & i);
			
			/*! Get cells sizes.
				\return		array with cells sizes */
			static array<Real,3> getCellSize();
			
			/*! Get number of cells along one direction.
				\param i	direction
				\return		number of cells */
			static UInt getNumCells(const UInt & i);
			
			/*! Get number of cells along each direction.
				\return		array with number of cells along each direction */
			static array<UInt,3> getNumCells();
			
			//
			// Set methods 
			//
			
			/*! Set the Id.
				\param idNew	the new Id */
			void setId(const UInt & idNew);
			
			/*! Set the North-East point.
				\param p	the new North-East point */
			static void setPNE(const point & p);
			
			/*! Set the South-West point.
				\param p	the new South-West point */
			static void setPSW(const point & p);
			
			/*! Set one cells size.
				\param i	component
				\param val	value */
			static void setCellSize(const UInt & i, const Real & val);
			
			/*! Set all cells sizes.
				\param val	array with new cells sizes */
			static void setCellSize(const array<Real,3> & val);
			
			/*! Set number of cells along one direction.
				\param i	component
				\param val	value */
			static void setNumCells(const UInt & i, const UInt & val);
			
			/*! Set number of cells along each direction.
				\param val	array with new number of cells */
			static void setNumCells(const array<UInt,3> & val);
			
			/*! Initialize extrema of the grid and cells sizes.
				This method is supposed to be called ONLY ONCE and BEFORE any use of the class.
				\param pne	North-East point
				\param psw	South-West point
				\param dx	cells size along x
				\param dy	cells size along y
				\param dz	cells size along z */
			static void initialize(const point & pne = point(1.,1.,1.), const point & psw = point(0.,0.,0.), const Real & dx = 1., const Real & dy = 1., const Real & dz = 1.);
			
			//
			// Methods to keep static variables coherent
			//     
			
			/*! Update number of cells along one direction. 
				\param i	direction */
			static void updateNumCells(const UInt & i);            
			
			/*! Update number of cells along each direction. */
			static void updateNumCells();
			
			/*! Update cells size along one direction.
				\param i	direction */
			static void updateCellSize(const UInt & i);
			
			/*! Update cells sizes along each direction. */
			static void updateCellSize();
			
						
		private:      
			//
			// Print method
			//
			
			/*! Print to output the point data.
				\param out	the output string */
			void print(ostream & out) const;
	};
}

/*! Include definitions of inlined members and friend functions. */
#include "inline/inline_searchPoint.hpp"

#endif
