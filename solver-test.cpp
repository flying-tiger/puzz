#include <gtest/gtest.h>
#include "solver.hpp"

TEST(Edge, BasicTests) {
    using namespace edge;

    Edge e = RT;

    // Edge matching
    EXPECT_EQ(e.get_match(), RH);
    EXPECT_TRUE(e.is_match(RH));
    EXPECT_FALSE(e.is_match(RT));

    // Edges can be used as indices
    EXPECT_EQ(edges[RT], RT);
}

TEST(Tile, BasicTests) {
    using namespace edge;

    Tile t1 = {RT, BH, YH, GT};
    auto e1 = t1.edges();

    EXPECT_EQ(top(t1), RT);
    EXPECT_EQ(right(t1), BH);
    EXPECT_EQ(e1[1], BH);
    EXPECT_EQ(e1[3], GT);

    Tile t2 = t1;
    EXPECT_EQ(t1, t2);

    std::array<Edge,4> e3 = {RT, BH, GT, YH};
    Tile t3(e3);
    EXPECT_NE(t1, t3);

    auto tv = rotate(t1,2);
    EXPECT_EQ(top(t1),  bottom(tv));
    EXPECT_EQ(left(t1), right(tv));

}

TEST(Tile, FileTests) {
    auto tiles = load_tiles("../puzzles/frogs.txt");
    EXPECT_EQ(top(tiles[3]), edge::yellow_tail);
    EXPECT_EQ(left(tiles[5]), edge::yellow_head);
}