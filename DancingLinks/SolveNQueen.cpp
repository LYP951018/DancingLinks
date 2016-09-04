#include "SolveNQueen.hpp"

std::uint32_t SolveNQueen(std::uint32_t n)
{
    const auto diagonalsCount = 2 * n - 1;
    const auto totalColumnCount = 2 * (diagonalsCount + n);
    std::vector<std::vector<bool>> matrix;
    for (std::uint32_t row = 0; row < n; ++row)
    {
        for (std::uint32_t column = 0; column < n; ++column)
        {
            const auto leftDiagonalNumber = row + column;
            const auto rightDiagonalNumber = n - 1 - row + column;
            std::vector<bool> r;
            r.resize(totalColumnCount);
            r[row] = true;
            r[column + n] = true;
            r[leftDiagonalNumber + 2 * n] = true;
            r[rightDiagonalNumber + 2 * n + diagonalsCount] = true;
            matrix.push_back(std::move(r));
        }
    }

    auto dlx = Dlx::FromMatrix(matrix, n * 2);
    using solution_t = typename Dlx::SolutionType;
    solution_t solution;
    dlx.Solve(solution);
    return static_cast<std::uint32_t>(solution.size());
}
