#pragma once

#include "Basis.hpp"
#include <cstddef>
#include <cstdint>
#include <climits>

struct DataObject
{
	Ptr<DataObject> Left, Right, Up, Down, Column;
	std::uint32_t RowNumber;

	DataObject() noexcept
		: Left{this}, Right{this}, Up{this}, Down{this}, Column{this}, RowNumber{UINT32_MAX}
	{}

	bool IsColumnObject() const noexcept
	{
		return RowNumber == 0;
	}
};

struct ColumnObject : DataObject
{
	ColumnObject() noexcept
		: DataObject{}
	{
		RowNumber = {};
	}
	std::uint32_t Size = {};
	std::uint32_t Index;
};