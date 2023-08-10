/**/

Cell::Cell() : solved(false), value(0)
{
	for (Cell_Value v=1; v<=9; ++v) {
		values.insert(values.end(), v);
	}
}
Cell::Cell(const int val) : solved(true), value(val)
{
}

void Sudoku::print() const
{
	for (int row=0; row<9; ++row) {
		for (int col=0; col<9; ++col) {
			cout << data[row][col];
			if ((col+1)%3 == 0)
				cout << ' ';
		}
		cout << '\n';
		if ((row+1)%3 == 0)
			cout << '\n';
	}
}

bool Cell::remove_from_values(const Cell_Value val)
{
	if (solved)
		return false;
	std::set<Cell_Value>::iterator item = values.find(val.begin(), val);
	if (item == 0)
		return false;
	values.remove(item);
	if (values.size() == 1) {
		value = *values.begin();
		solved=true;
		values.erase();
	}
	return true;
}

bool Sudoku::column_remove_num(const int col, const Cell_Value num)
{
	for (int r=0; r<9; ++r)
		Cell* c = data[r][col];
		if (c->number == 0)
}

void Sudoku::row_get_solved_values(const int row, std::set<Cell_Value> &s)
{
	for (int col=0; col<9; ++col) {
		const Cell& c = data[row][col];
		if (c.is_solved()) {
			s.insert(s.end(), c.get_value());
		}
	}
}

bool Sudoku::process_row(const int row)
{
	cell_set row_nums;
	for (int col=0; col<9; ++col)
		if (is_solved(data[row][col])) {
		}
}

void Sudoku::solve()
{
}
