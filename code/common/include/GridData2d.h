// ==========================================================================
// Copyright (C) 2008 Aline Normoyle
// ==========================================================================

#ifndef __GRID_DATA_2D_H__
#define __GRID_DATA_2D_H__

#pragma warning(disable: 4244 4267 4996)
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <glm/glm.hpp>

namespace Glb {

	using namespace boost::numeric;

	// GridData2d 能够存储网格中的任何数据
	// 列由 i 索引，随 x 增加而增加
	// 行由 j 索引，随 y 增加而增加
	//
	// GridData2d 通过全局变量 theDim2d 和 theCellSize2d 进行初始化
	// theDim2d 定义每个X、Y、Z方向上的单元格数
	// GridData2d 的世界空间尺寸从(0,0)延伸到 mMax
	// 其中 mMax 是 ( theCellSize * theDim[0], theCellSize * theDim[1])
	class GridData2d
	{
	public:
		GridData2d();
		GridData2d(const GridData2d& orig);
		virtual ~GridData2d();
		virtual GridData2d& operator=(const GridData2d& orig);

		// 用默认值初始化数据
		virtual void initialize(double dfltValue = 0.0);

		// 返回（i，j）位置上的可更改数据
		// 因此你可以使用 GridData2d(i, j) = num 来进行赋值
		virtual double& operator()(int i, int j);

		// 根据坐标，返回插值后得到的值
		// 对于超出范围的点，返回默认值
		virtual double interpolate(const glm::vec2& pt);	// 插值函数，根据给定的世界坐标返回网格数据

		// 返回数据，即 mData。通常用于和其他ublas类进行操作
		ublas::vector<double>& data();

		// 根据世界坐标，返回该点所在的网格索引
		virtual void getCell(const glm::vec2& pt, int& i, int& j);

		virtual glm::vec2 worldToSelf(const glm::vec2& pt) const;
		double mDfltValue;				// 默认值，用于初始化数据
		glm::vec2 mMax;					// 二维空间中的最大坐标，表示网格的尺寸
		ublas::vector<double> mData;	// 存储网格数据的一维数组
	};

	class GridData2dX : public GridData2d
	{
	public:
		GridData2dX();
		virtual ~GridData2dX();
		virtual void initialize(double dfltValue = 0.0);
		virtual double& operator()(int i, int j);
		virtual glm::vec2 worldToSelf(const glm::vec2& pt) const;
	};

	class GridData2dY : public GridData2d
	{
	public:
		GridData2dY();
		virtual ~GridData2dY();
		virtual void initialize(double dfltValue = 0.0);
		virtual double& operator()(int i, int j);
		virtual glm::vec2 worldToSelf(const glm::vec2& pt) const;
	};

	class CubicGridData2d : public GridData2d
	{
	public:
		CubicGridData2d();
		CubicGridData2d(const CubicGridData2d& orig);
		virtual ~CubicGridData2d();
		virtual double interpolate(const glm::vec2& pt);

	protected:

		double cubic(double q1, double q2, double q3, double q4, double t);
		double interpX(int i, int j, double fracty, double fractx);
		double interpY(int i, int j, double fracty);
	};
}

#endif