
#include <array>
#include <vector>
#include <map>
#include <random>


typedef std::array<int, 2> IntPoint;
typedef std::array<float, 2> FloatPoint;
typedef std::mt19937 RGen;
typedef std::uniform_real_distribution<> NormalDist;


struct Cell
{
    IntPoint gridPosition;
    FloatPoint randomPosition;
};


struct Grid
{
    int width;
    int height;
    std::map<IntPoint, Cell> cells;
};


Grid BuildGrid(int width, int height)
{
    Grid grid;

    grid.width = width;
    grid.height = height;

    for (int w = 0; w < width; w++)
    {
        for (int h = 0; h < height; h++)
        {
            IntPoint position = {w, h};
            grid.cells[position] = {position};
        }
    }

    return grid;
}


void GenerateRandomPoints(Grid& grid, const int& seed)
{
    RGen gen;
    gen.seed(seed);

    NormalDist dist(0.0, 1.0);
    for (auto cell : grid.cells)
    {
        cell.second.randomPosition = {
                (float)dist(gen),
                (float)dist(gen)
        };
    }
}










