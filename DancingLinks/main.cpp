#include "Dlx.hpp"
#include "SolveNQueen.hpp"

int main()
{
    /*constexpr bool data[][7] = {
        {false, false, true, false, true, true, false},
        {true, false, false, true, false, false, true},
        {false, true, true, false, false, true, false},
        {true, false, false, true, false, false, false},
        {false, true, false, false, false, false, true},
        {false, false, false, true, true, false, true}
    };
    auto dlx = Dlx::FromMatrix(data, 7);
    using solution_t = typename Dlx::SolutionType;
    solution_t solution;
    dlx.Solve(solution);*/

    auto x = SolveNQueen(8);
}