#include <gtest/gtest.h>
#include "solver.hpp"

TEST(Edge, BasicTests) {
    using namespace Edges;

    Edge e = RT;

    // Edge matching
    EXPECT_EQ(e.get_match(), RH);
    EXPECT_TRUE(e.is_match(RH));
    EXPECT_FALSE(e.is_match(RT));

    // Edges can be used as indices
    EXPECT_EQ(edges[RT], RT);
}

TEST(Tile, BasicTests) {
    using namespace Edges;

    Tile t1 = {RT, BH, YH, GT};
    auto e1 = t1.edges();

    EXPECT_EQ(e1.top(), RT);
    EXPECT_EQ(e1.right(), BH);
    EXPECT_EQ(e1[1], BH);
    EXPECT_EQ(e1[3], GT);

    Tile t2 = t1;
    EXPECT_EQ(t1, t2);

    Tile t3 = {RT, BH, GT, YH};
    EXPECT_NE(t1, t3);

    auto tv = rotate(t1,2);
    auto ev = tv.edges();
    EXPECT_EQ(e1.top(),  ev.bottom());
    EXPECT_EQ(e1.left(), ev.right());

}