#include <gtest/gtest.h>
#include "solver.hpp"

TEST(Edge, BasicTests) {

    Edge e = RT;

    // Edge matching
    EXPECT_EQ(e.get_match(), RH);
    EXPECT_TRUE(e.is_match(RH));
    EXPECT_FALSE(e.is_match(RT));

    // Edges can be used as indices
    EXPECT_EQ(edges[RT], RT);

}

TEST(Tile, BasicTests) {

    Tile t1 = {RT, BH, YH, GT};

    EXPECT_EQ(t1.top(), RT);
    EXPECT_EQ(t1.right(), BH);
    EXPECT_EQ(t1.edge(1), BH);
    EXPECT_EQ(t1.edges()[3], GT);

    Tile t2 = t1;
    EXPECT_EQ(t1, t2);

    Tile t3 = {RT, BH, GT, YH};
    EXPECT_NE(t1, t3);

}