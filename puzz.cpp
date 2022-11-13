#include <algorithm>
#include <array>
#include <cstdio>
#include <cstdint>
#include <fstream>
#include <optional>
#include <string>
#include "include/experimental/mdarray"

namespace stdx = std::experimental;
namespace rng  = std::ranges;


//---------------------------------------------------------------------
// Magic Numbers
//---------------------------------------------------------------------
constexpr size_t num_rows  = 3;
constexpr size_t num_cols  = 3;
constexpr size_t num_tiles = num_rows*num_cols;
constexpr size_t num_edges = 4;


//---------------------------------------------------------------------
// Edge
//---------------------------------------------------------------------
struct Edge {
    uint8_t value;
};
namespace Edges {
    constexpr Edge RedTail   {0};
    constexpr Edge BlueTail  {1};
    constexpr Edge GreenTail {2};
    constexpr Edge YellowTail{3};
    constexpr Edge RedHead   {4};
    constexpr Edge BlueHead  {5};
    constexpr Edge GreenHead {6};
    constexpr Edge YellowHead{7};
}
bool is_match(Edge e1, Edge e2) {
    return (e1.value ^ e2.value) == 4;
}


//---------------------------------------------------------------------
// Tile
//---------------------------------------------------------------------
using Tile = std::array<Edge, num_edges>;
Edge top   (Tile t)  { return t[0]; }
Edge right (Tile t)  { return t[1]; }
Edge bottom(Tile t)  { return t[2]; }
Edge left  (Tile t)  { return t[3]; }
void rotate(Tile& t) { rng::rotate(t, &t[1]); }  // Rotates tile CCW


//---------------------------------------------------------------------
// Solution Algorithm
//---------------------------------------------------------------------
using TExtent       = stdx::extents<uint8_t, num_rows, num_cols>;
using TileList      = std::array<Tile, num_tiles>;
using TileArray     = stdx::mdarray<Tile, TExtent>;
using TilePositions = stdx::mdarray<uint8_t, TExtent>;
using TileRotations = stdx::mdarray<uint8_t, TExtent>;
struct SolveResult {
    TilePositions positions;
    TileRotations rotations;
};

template<size_t I, size_t J>
bool check(const TileArray& t) {
    bool all_match = true;
    if constexpr (I > 0) all_match &= is_match(bottom(t(I-1,J)), top(t(I,J)));
    if constexpr (J > 0) all_match &= is_match(right(t(I,J-1)), left(t(I,J)));
    return all_match;
}

bool orient(TileArray& t, TileRotations& r) {
    constexpr size_t N = num_edges;
    for (r(0,0)=N; r(0,0)--;) { if (check<0,0>(t)) {
    for (r(0,1)=N; r(0,1)--;) { if (check<0,1>(t)) {
    for (r(0,2)=N; r(0,2)--;) { if (check<0,2>(t)) {
    for (r(1,0)=N; r(1,0)--;) { if (check<1,0>(t)) {
    for (r(1,1)=N; r(1,1)--;) { if (check<1,1>(t)) {
    for (r(1,2)=N; r(1,2)--;) { if (check<1,2>(t)) {
    for (r(2,0)=N; r(2,0)--;) { if (check<2,0>(t)) {
    for (r(2,1)=N; r(2,1)--;) { if (check<2,1>(t)) {
    for (r(2,2)=N; r(2,2)--;) { if (check<2,2>(t)) {
        for (size_t i = 0; i < num_rows; ++i) {
        for (size_t j = 0; j < num_cols; ++j) {
            // Countdown => #CW rotations
            r(i,j) = (r(i,j) + 1) % N;
        }}
        return true;
    } rotate(t(2,2)); }
    } rotate(t(2,1)); }
    } rotate(t(2,0)); }
    } rotate(t(1,2)); }
    } rotate(t(1,1)); }
    } rotate(t(1,0)); }
    } rotate(t(0,2)); }
    } rotate(t(0,1)); }
    } rotate(t(0,0)); }
    return false;
}

std::optional<SolveResult> solve(const TileList& tiles) {
    TileArray arranged;
    SolveResult res {
        TilePositions({0, 1, 2, 3, 4, 5, 6, 7, 8}),
        TileRotations({0, 0, 0, 0, 0, 0, 0, 0, 0}),
    };

    auto& p = res.positions.container();
    do {
        for (size_t i = 0; i < num_rows; ++i) {
        for (size_t j = 0; j < num_cols; ++j) {
            arranged(i,j) = tiles[res.positions(i,j)];
        }}
        if (orient(arranged, res.rotations)) return res;
    } while(rng::next_permutation(p).found);

    return std::nullopt;
}


//---------------------------------------------------------------------
// Tile Reader
//---------------------------------------------------------------------
TileList load_tiles(const std::string& input_file) {
    TileList tiles;
    std::string digraph;
    std::ifstream inp(input_file);
    for (auto& tile : tiles) {
        while (inp.peek() == '#')
            inp.ignore(10000, '\n');
        for (auto& edge : tile) {
            inp >> digraph;
            if (digraph == "RH") edge = Edges::RedHead;
            if (digraph == "BH") edge = Edges::BlueHead;
            if (digraph == "GH") edge = Edges::GreenHead;
            if (digraph == "YH") edge = Edges::YellowHead;
            if (digraph == "RT") edge = Edges::RedTail;
            if (digraph == "BT") edge = Edges::BlueTail;
            if (digraph == "GT") edge = Edges::GreenTail;
            if (digraph == "YT") edge = Edges::YellowTail;
        }
    }
    return tiles;
}


//---------------------------------------------------------------------
// Main
//---------------------------------------------------------------------
int main(int argc, char* argv[]) {

    if (argc != 2) {
        std::printf("usage: puzz <puzzle_file>\n");
        return 1;
    }

    auto result = solve(load_tiles(argv[1]));

    if (result) {
        const auto& [p,r] = *result;
        std::printf("----------------------\n");
        std::printf("Puzzle solution found!\n");
        std::printf("----------------------\n");
        std::printf("  Pos  Tile  Rot(CW)  \n");
        for (size_t i = 0; i < num_rows; ++i) {
        for (size_t j = 0; j < num_cols; ++j) {
            std::printf("%3zu,%1zu%5d%5d\n", i, j, p(i,j)+1, r(i,j));
        }}
    }
    else {
        std::printf("Puzzle has no solution.\n");
    }

    return 0;
}
