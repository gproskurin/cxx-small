/**/

#include <vector>
#include <set>

typedef short Cell_Value; // 0-9

template <class T>
class Iter {
public:
	virtual Iter& operator++() = 0;
	virtual T& operator*() = 0;
	virtual T* operator->() = 0;
};

template <class C>
class Matrix {
	const int size;
	C data[size][size];
public:
	typedef void (*action)();
	Matrix(const int sz) : size(sz) {}
};

class Cell {
	bool solved;
	Cell_Value value;
	std::set<Cell_Value> values;
	Cell();
	Cell(const int);
public:
	bool is_solved() const { return solved; };
	bool remove_from_values(const Cell_Value);
	Cell_Value get_value() { return value; };
};

class Sudoku {
	std::vector<std::vector<Cell>> data;
public:
	std::vector<Cell>& operator[] (const int row) { return data[row]; };
	void print() const;
};


