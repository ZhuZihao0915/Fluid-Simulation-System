// ==========================================================================
// Copyright (C) 2008 Aline Normoyle
// ==========================================================================

#ifndef __GRID_DATA_3D_H__
#define __GRID_DATA_3D_H__

#pragma warning(disable: 4244 4267 4996)
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <glm/glm.hpp>

namespace Glb {

	using namespace boost::numeric;

	// GridData is capable of storing any data in a grid
	// Columns are indexed with i and increase with increasing x
	// Rows are indexed with j and increase with z
	// Stacks are indexed with k and incrase with y
	//
	// GridData is initialized by global variables theDim and theCellSize
	// defined in GridData.cpp.  theDim defines the number of cells in 
	// each X,Y,Z direction.  theCellSize defines the size of each cell.
	// GridData's world space dimensions extend from (0,0,0) to mMax, where mMax is
	// (theCellSize*theDim[0], theCellSize*theDim[1], theCellSize*theDim[2])
	class GridData3d
	{
	public:
		GridData3d();
		GridData3d(const GridData3d& orig);
		virtual ~GridData3d();
		virtual GridData3d& operator=(const GridData3d& orig);

		// Initialize underlying data structure with dlftValue
		virtual void initialize(double dfltValue = 0.0);

		// Returns editable data at index (i,j,k).
		// E.g. to set data on this object, call mygriddata(i,j,k) = newval
		virtual double& operator()(int i, int j, int k);

		// Given a point in world coordinates, return the corresponding
		// value from this grid. mDfltValue is returned for points
		// outside of our grid dimensions
		virtual double interpolate(const glm::vec3& pt);	// 插值函数，根据给定的世界坐标返回网格数据

		// Access underlying data structure (for use with other UBLAS objects)
		ublas::vector<double>& data();

		// Given a point in world coordinates, return the cell index (i,j,k)
		// corresponding to it
		virtual void getCell(const glm::vec3& pt, int& i, int& j, int& k);

	protected:

		virtual glm::vec3 worldToSelf(const glm::vec3& pt) const;
		double mDfltValue;	// 默认值，用于初始化数据
		glm::vec3 mMax;		// 三维空间中的最大坐标，表示网格的尺寸
		ublas::vector<double> mData;	// 存储网格数据的一维数组
	};

	class GridData3dX : public GridData3d
	{
	public:
		GridData3dX();
		virtual ~GridData3dX();
		virtual void initialize(double dfltValue = 0.0);
		virtual double& operator()(int i, int j, int k);
		virtual glm::vec3 worldToSelf(const glm::vec3& pt) const;
	};

	class GridData3dY : public GridData3d
	{
	public:
		GridData3dY();
		virtual ~GridData3dY();
		virtual void initialize(double dfltValue = 0.0);
		virtual double& operator()(int i, int j, int k);
		virtual glm::vec3 worldToSelf(const glm::vec3& pt) const;
	};

	class GridData3dZ : public GridData3d
	{
	public:
		GridData3dZ();
		virtual ~GridData3dZ();
		virtual void initialize(double dfltValue = 0.0);
		virtual double& operator()(int i, int j, int k);
		virtual glm::vec3 worldToSelf(const glm::vec3& pt) const;
	};

	class CubicGridData3d : public GridData3d
	{
	public:
		CubicGridData3d();
		CubicGridData3d(const CubicGridData3d& orig);
		virtual ~CubicGridData3d();
		virtual double interpolate(const glm::vec3& pt);

	protected:

		double cubic(double q1, double q2, double q3, double q4, double t);
		double interpX(int i, int j, int k, double fracty, double fractx);
		double interpY(int i, int j, int k, double fracty);
	};
}

#endif