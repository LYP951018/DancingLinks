#pragma once

#include "Objects.hpp"
#include <gsl.h>
#include <cstdint>
#include <vector>
#include <memory>
#include <utility>

class Dlx
{
public:
	using SolutionType = std::vector<std::vector<std::uint32_t>>;

	Dlx() noexcept = default;
	Dlx(const Dlx&) = delete;
	Dlx& operator= (const Dlx&) = delete;

	Dlx(Dlx&& other) noexcept
		: root_{other.root_},
		dataPool_{std::move(other.dataPool_)},
		columnPool_{std::move(other.columnPool_)}
	{
		other.root_ = {};
	}

	Dlx& operator= (Dlx&& other) noexcept
	{
		Dlx{ std::move(other) }.swap(*this);
		return *this;
	}

	void swap(Dlx& other) noexcept;

	template<typename ContainerOfContainer>
	static Dlx FromMatrix(ContainerOfContainer& matrix, std::uint32_t secondaryColumnOffset)
	{
		Dlx dlx;
		const auto rowCount = std::size(matrix);
		Expects(rowCount > 0);
		const auto columnCount = std::size(matrix[0]);
		const auto lastNodes = std::make_unique<Ptr<DataObject>[]>(columnCount);
		{
			const auto firstColumnObject = dlx.AllocateColumn();
			dlx.root_ = firstColumnObject;
			Ptr<ColumnObject> last = firstColumnObject;
			for (std::size_t i = 0; i < secondaryColumnOffset; ++i)
			{
				const auto newColumnObject = dlx.AllocateColumn();
				last->Right = newColumnObject;
				newColumnObject->Left = last;
				firstColumnObject->Left = newColumnObject->Left;
				newColumnObject->Right = firstColumnObject;
				last = newColumnObject;
				lastNodes[i] = newColumnObject;
				newColumnObject->Index = i;
			}
			//从 secondaryColumnOffset 起，所有的 ColumnObject Header 不互相相连。
			for (std::size_t i = secondaryColumnOffset; i < columnCount; ++i)
			{
				const auto newColumnObject = dlx.AllocateColumn();
				newColumnObject->Index = i;
				lastNodes[i] = newColumnObject;
			}
		}

		for (std::size_t i = 0; i < rowCount; ++i)
		{
			Ptr<DataObject> rowLast = {};
			std::size_t columnIndex = {};
			for (auto element : matrix[i])
			{
				if (element)
				{
					const auto data = dlx.AllocateData();
					auto& last = lastNodes[columnIndex];
					auto column = last->Down;
					Expects(column->IsColumnObject());
					++reinterpret_cast<Ptr<ColumnObject>>(column)->Size;
					last->Down = data;
					data->Up = last;
					data->Down = column;
					column->Up = data;
					last = data;
					data->Column = column;
					data->RowNumber = i;
					//横着插
					if (rowLast != nullptr)
					{
						const auto rowStart = rowLast->Right;
						rowLast->Right = data;
						data->Left = rowLast;
						data->Right = rowStart;
						rowStart->Left = data;
					}
					rowLast = data;
				}
				++columnIndex;
			}
		}
		return dlx;
	}

	void Solve(SolutionType& solution);

private:
	void Cover(Ptr<DataObject> data) noexcept;
	void UnCover(Ptr<DataObject> data) noexcept;
	Ptr<ColumnObject> SelectColumn() noexcept;
	void SolveImpl(std::vector<std::uint32_t>& current, SolutionType& solution);

	void RemoveLR(Ptr<DataObject> data) noexcept;
	void RecoverLR(Ptr<DataObject> data) noexcept;
	void RemoveUD(Ptr<DataObject> data) noexcept;
	void RecoverUD(Ptr<DataObject> data) noexcept;

	Ptr<DataObject> AllocateData();
	Ptr<ColumnObject> AllocateColumn();

	Ptr<ColumnObject> root_ = {};
	std::vector<std::unique_ptr<DataObject>> dataPool_;
	std::vector<std::unique_ptr<ColumnObject>> columnPool_;
};