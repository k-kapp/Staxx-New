#ifndef SHAPE_H
#define SHAPE_H

#include <vector>
#include <deque>
#include <iostream>
#include <utility>
#include <fstream>
#include "grid.h"
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

using namespace std;

using bool_2d = std::deque< std::deque<bool> >;

typedef enum _orient
{
	UP,
	RIGHT,
	DOWN,
	LEFT
} orient;


class shape
{
public:
	shape(bool_2d shape_arr, unsigned num_cols, unsigned num_rows);
	shape(bool_2d shape_arr);
	shape(boost::archive::text_iarchive &ia);

	bool operator == (const shape &other) const;
	void fix_shape_arr();
	bool shape_valid();
	bool get_coords(int row, int col, orient spec);
	int get_height(orient);
	int get_width(orient);
	bool_2d get_shape(orient spec);
	deque<bool> & get_row(int row, orient spec);
	pair<int, int> get_horiz_padding();
	pair<int, int> get_vert_padding();
	void set_padding(orient, unsigned num_cols, unsigned num_rows);
private:
	friend class boost::serialization::access;

	template<typename Archive>
	void serialize(Archive & ar, unsigned const version)
	{
		ar & orig_arr;
		ar & shape_arr;
		ar & right;
		ar & down;
		ar & left;
		ar & horiz_padding;
		ar & vert_padding;
	}

	bool_2d orig_arr;
	bool_2d shape_arr;
	bool_2d &up;
	bool_2d right;
	bool_2d down;
	bool_2d left;
	pair<int, int> horiz_padding;
	pair<int, int> vert_padding;

	void make_orients();
	int first_true_idx(const std::deque<bool> &bool_vec);
	int last_true_idx(const std::deque<bool> &bool_vec);
	bool_2d & get_shape_ref(orient);
	bool_2d rotate_right(const bool_2d &arr);
	void print_orig_arr();
};




template<template<typename, typename> class ContainerType, typename TObject>
void print_container(ContainerType<TObject, allocator<TObject> > container)
{
	cout << "[";
	for (auto iter = container.begin(); iter != container.end(); advance(iter, 1))
	{
		cout << *iter;
		if (next(iter, 1) == container.end())
		{
			cout << "]" << endl;
		}
		else
		{
			cout << ", ";
		}
	}
}

template<template<typename, typename> class ContainerType, typename TObject>
void print_arr(ContainerType<ContainerType<TObject, allocator<TObject> >, allocator<ContainerType<TObject, allocator<TObject> > > > container)
{
	for (auto &content : container)
	{
		print_container(content);
	}
}


template<template<typename T, typename Alloc> class ContainerType, typename TObject, typename idx_type>
bool check_bounds(const ContainerType<TObject, allocator<TObject>> &vec, idx_type row_idx, idx_type col_idx)
{
	if (row_idx < 0 || col_idx < 0)
		return false;
	if (row_idx >= vec.size() || col_idx >= vec[0].size())
		return false;
	return true;
}



#endif
