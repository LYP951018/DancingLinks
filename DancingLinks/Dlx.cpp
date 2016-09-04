#include "Dlx.hpp"

void Dlx::swap(Dlx& other) noexcept
{
    using std::swap;
    swap(root_, other.root_);
    swap(dataPool_, other.dataPool_);
    swap(columnPool_, other.columnPool_);
}

Ptr<ColumnObject> Dlx::AllocateColumn()
{
    auto column = std::make_unique<ColumnObject>();
    const auto ret = column.get();
    columnPool_.push_back(std::move(column));
    return ret;
}

void Dlx::Solve(SolutionType& solution)
{
    std::vector<std::uint32_t> current;
    SolveImpl(current, solution);
}

void Dlx::Cover(Ptr<DataObject> data) noexcept
{
    const auto column = data->Column;
    RemoveLR(column);
    for (auto cur = column->Down; cur != column; cur = cur->Down)
    {
        for (auto cur2 = cur->Right; cur2 != cur; cur2 = cur2->Right)
        {
            RemoveUD(cur2);
            --reinterpret_cast<Ptr<ColumnObject>>(cur2->Column)->Size;
        }
    }
}

void Dlx::UnCover(Ptr<DataObject> data) noexcept
{
    const auto column = data->Column;
    RecoverLR(column);
    //由于是在上下遍历中恢复上下结点，所以必须按照反顺序恢复。
    for (auto cur = column->Up; cur != column; cur = cur->Up)
        for (auto cur2 = cur->Right; cur2 != cur; cur2 = cur2->Right)
        {
            RecoverUD(cur2);
            const auto curColumn = cur2->Column;
            Expects(curColumn->IsColumnObject());
            ++reinterpret_cast<Ptr<ColumnObject>>(curColumn)->Size;
        }
}

Ptr<ColumnObject> Dlx::SelectColumn() noexcept
{
    std::uint32_t minimalSize = UINT32_MAX;
    Ptr<ColumnObject> columnSelected = {};
    for (auto cur = root_->Right; cur != root_; cur = cur->Right)
    {
        const auto column = reinterpret_cast<Ptr<ColumnObject>>(cur);
        const auto curSize = column->Size;
        if (curSize < minimalSize)
        {
            columnSelected = column;
            minimalSize = curSize;
        }
    }
    Ensures(columnSelected != nullptr);
    return columnSelected;
}

void Dlx::SolveImpl(std::vector<std::uint32_t>& current, SolutionType& solution)
{
    if (root_->Right == root_)
    {
        solution.push_back(current);
        return;
    }
    else
    {
        const auto columnSelected = SelectColumn();
        Cover(columnSelected);
        for (auto cur = columnSelected->Down; cur != columnSelected; cur = cur->Down)
        {
            //从中选择一行。
            const auto rowNumber = cur->RowNumber;
            current.push_back(rowNumber);
            //将该行元素不为 0 的列所在的行全部剔除掉。
            for (auto cur2 = cur->Right; cur2 != cur; cur2 = cur2->Right)
                Cover(cur2);
            SolveImpl(current, solution);
            //恢复上述被剔除的行（此处也必须按照反顺序因为在左右遍历时恢复的 ColumnObject 头节点是左右的）。
            for (auto cur2 = cur->Left; cur2 != cur; cur2 = cur2->Left)
                UnCover(cur2);
            current.pop_back();
        }
        UnCover(columnSelected);
    }
}

void Dlx::RemoveLR(Ptr<DataObject> data) noexcept
{
    data->Left->Right = data->Right;
    data->Right->Left = data->Left;
}

void Dlx::RecoverLR(Ptr<DataObject> data) noexcept
{
    data->Right->Left = data;
    data->Left->Right = data;
}

void Dlx::RemoveUD(Ptr<DataObject> data) noexcept
{
    data->Up->Down = data->Down;
    data->Down->Up = data->Up;
}

void Dlx::RecoverUD(Ptr<DataObject> data) noexcept
{
    data->Down->Up = data;
    data->Up->Down = data;
}

Ptr<DataObject> Dlx::AllocateData()
{
    auto data = std::make_unique<DataObject>();
    const auto ret = data.get();
    dataPool_.push_back(std::move(data));
    return ret;
}
