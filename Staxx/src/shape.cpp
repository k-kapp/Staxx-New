#include "../include/shape.h"
#include <climits>
#include <vector>
#include <iostream>
#include <utility>
#include <deque>
#include <stdexcept>
#include <string>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/deque.hpp>

using namespace std;


class invalid_input : public runtime_error
{
public:
	invalid_input(const string &func_name) : runtime_error(func_name)
	{
		this->func_name = func_name;
	}

	virtual const char * what() const throw()
	{
		string msg = "invalid input to function " + func_name;
		return msg.c_str();
	}
private:
	string func_name;

};

bool equal_bool2d(bool_2d first, bool_2d second)
{
	if (first.size() != second.size())
		return false;
	if (first.size() == 0)
		return true;
	if (first.at(0).size() != second.at(0).size())
		return false;
	for (int i = 0; i < first.size(); i++)
	{
		for (int j = 0; j < first.at(0).size(); j++)
		{
			if (first.at(i).at(j) != second.at(i).at(j))
			{
				return false;
			}
		}
	}
	return true;
}



shape::shape(bool_2d shape_arr, unsigned num_cols, unsigned num_rows) 
	: orig_arr(shape_arr), up(this->shape_arr)
{
	fix_shape_arr();
	make_orients();
	set_padding(UP, num_cols, num_rows);
}

shape::shape(bool_2d shape_arr) 
	: orig_arr(shape_arr), up(this->shape_arr)
{
	fix_shape_arr();
	make_orients();
}

shape::shape(boost::archive::text_iarchive &ia)
	: up(this->shape_arr)
{
	ia >> *this;
}

bool shape::operator == (const shape &other) const
{
	if (equal_bool2d(this->up, other.up))
		return true;
	if (equal_bool2d(this->up, other.down))
		return true;
	if (equal_bool2d(this->up, other.right))
		return true;
	if (equal_bool2d(this->up, other.left))
		return true;
	return false;
}

void shape::print_orig_arr()
{
	for (auto &row : orig_arr)
	{
		for (auto &bool_val : row)
		{
			cout << bool_val << " ";
		}
		cout << endl;
	}
}

int shape::first_true_idx(const deque<bool> &bool_vec)
{
	int count = 0;
	for (const auto &bool_val : bool_vec)
	{
		if (bool_val)
			return count;
		count++;
	}
	return -1;
}

int shape::last_true_idx(const deque<bool> &bool_vec)
{
	int count = 0;
	int highest_true = -1;
	for (const auto &bool_val : bool_vec)
	{
		if (bool_val)
		{
			if (count > highest_true)
			{
				highest_true = count;
			}
		}
		count++;
	}
	return highest_true;
}

bool shape::shape_valid()
{
	vector<int> one_neg = {1, -1};
	for (int i = 0; i < orig_arr.size(); i++)
	{
		for (int j = 0; j < orig_arr[0].size(); j++)
		{
			if (orig_arr.at(i).at(j))
			{
				bool shape_valid = false;
				for (auto one_iter = one_neg.begin(); one_iter != one_neg.end(); advance(one_iter, 1))
				{
					if (check_bounds(orig_arr, i + *one_iter, j) && orig_arr.at(i + *one_iter).at(j))
					{
						shape_valid = true;
						break;
					}
				}
				if (shape_valid)
					continue;
				for (auto one_iter = one_neg.begin(); one_iter != one_neg.end(); advance(one_iter, 1))
				{
					if (check_bounds(orig_arr, i, j + *one_iter) && orig_arr.at(i).at(j + *one_iter))
					{
						shape_valid = true;
						break;
					}
				}
				if (!shape_valid)
					return false;
			}
		}
	}
	return true;
}
				

void shape::fix_shape_arr()
{
	int leftmost_idx = INT_MAX;
	int rightmost_idx = 0;

	int empty_above = 0;
	bool above_done = false;
	int last_row = orig_arr.size() - 1;
	int curr_row = 0;

	for (auto &bool_vec : orig_arr)
	{
		int first_idx, last_idx;
		first_idx = first_true_idx(bool_vec);
		curr_row++;
		if (first_idx == -1)
		{
			if (above_done)
			{
				last_row = curr_row - 2;
				break;
			}
			else
			{
				empty_above++;
				continue;
			}
		}
		else
			above_done = true;
		if (first_idx < leftmost_idx)
		{
			leftmost_idx = first_idx;
		}
		last_idx = last_true_idx(bool_vec);
		if (last_idx > rightmost_idx)
		{
			rightmost_idx = last_idx;
		}
	}

	if (!above_done)
	{
		throw invalid_input("fix_shape_arr() (shape contains no blocks)");
	}

	shape_arr = orig_arr;

	print_orig_arr();

	int pop_back_num = shape_arr.size() - last_row - 1;
	int pop_front_num = empty_above;

	for (int i = 0; i < pop_front_num; i++)
	{
		shape_arr.pop_front();
	}

	for (int i = 0; i < pop_back_num; i++)
	{
		shape_arr.pop_back();
	}

	pop_front_num = leftmost_idx;
	pop_back_num = shape_arr.at(0).size() - rightmost_idx - 1;

	for (auto &row : shape_arr)
	{
		for (int i = 0; i < pop_front_num; i++)
		{
			row.pop_front();
		}
		for (int i = 0; i < pop_back_num; i++)
		{
			row.pop_back();
		}
	}
}


bool_2d shape::rotate_right(const bool_2d &arr)
{
	if (arr.size() == 0)
		throw invalid_input("bool_2d rotate_right(const bool_2d &)");;
	bool_2d new_arr = bool_2d(arr.at(0).size(), deque<bool>(arr.size()));
	for (int row_num = 0; row_num < arr.size(); row_num++)
	{
		for (int col_num = 0; col_num < arr.at(row_num).size(); col_num++)
		{
			int dest_row = col_num;
			int dest_col = arr.size() - 1 - row_num;
			new_arr.at(dest_row).at(dest_col) = arr.at(row_num).at(col_num);
		}
	}
	return new_arr;
}


void shape::make_orients()
{
	right = rotate_right(up);
	down = rotate_right(right);
	left = rotate_right(down);
}


bool_2d shape::get_shape(orient spec)
{
	return get_shape_ref(spec);
}

bool_2d & shape::get_shape_ref(orient spec)
{
	switch (spec)
	{
		case (UP):
			return up;
		case (RIGHT):
			return right;
		case (DOWN):
			return down;
		case (LEFT):
			return left;
	}
}

void shape::set_padding(orient spec, unsigned num_cols, unsigned num_rows)
{
	int size_across;
	switch (spec)
	{
		case (UP):
		case (DOWN):
			size_across = up.at(0).size();
			break;
		case (RIGHT):
		case (LEFT):
			size_across = right.at(0).size();
			break;
	}
	if ((num_cols - size_across) % 2 == 1)
	{
		padding.first = (num_cols - size_across + 1)/2;
		padding.second = (num_cols - size_across - 1)/2;
	}
	else
	{
		padding.first = padding.second = (num_cols - size_across)/2;
	}
}

int shape::get_height(orient spec)
{
	return get_shape_ref(spec).size();
}

int shape::get_width(orient spec)
{
	return get_shape_ref(spec).at(0).size();
}

pair<int, int> shape::get_padding()
{
	return padding;
}

bool shape::get_coords(int row, int col, orient spec)
{
	return get_shape_ref(spec).at(row).at(col);
}

deque<bool> & shape::get_row(int row, orient spec)
{
	return get_shape_ref(spec).at(row);
}
