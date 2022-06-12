#include <array>
#include <cassert>
#include <cstdint>
#include <fstream>
#include <limits>
#include <span>
#include <string_view>
#include <initializer_list>

using UInt = uint8_t;

//------------------------------------------------------------------------
// Edges
//------------------------------------------------------------------------
namespace edge {

class Edge {

    static constexpr UInt red    = 0;
    static constexpr UInt blue   = 1;
    static constexpr UInt green  = 2;
    static constexpr UInt yellow = 3;
    static constexpr UInt head   = 4;
    static constexpr UInt max_value = 7;

    UInt value_;
    Edge(UInt value) : value_(value) {}

  public:
    constexpr Edge() : value_(red) {}
    constexpr Edge(const std::string_view digraph) {
        assert(digraph.length() == 2);
        switch (digraph[0]) {
            case 'R': value_ = red;    break;
            case 'B': value_ = blue;   break;
            case 'G': value_ = green;  break;
            case 'Y': value_ = yellow; break;
            assert(false);
        }
        switch (digraph[1]) {
            case 'T': /* no-op */     break;
            case 'H': value_ |= head; break;
            assert(false);
        }
    }

    bool operator==(const Edge&) const = default;
    Edge get_match() const { return { static_cast<UInt>(value_ ^ head) }; }
    bool is_match(const Edge e) const { return *this == e.get_match(); }

    constexpr operator UInt() const { return value_; }

};

constexpr Edge RT{"RT"}, red_tail = RT;
constexpr Edge BT{"BT"}, blue_tail = BT;
constexpr Edge GT{"GT"}, green_tail = GT;
constexpr Edge YT{"YT"}, yellow_tail = YT;
constexpr Edge RH{"RH"}, red_head = RH;
constexpr Edge BH{"BH"}, blue_head = BH;
constexpr Edge GH{"GH"}, green_head = GH;
constexpr Edge YH{"YH"}, yellow_head = YH;
constexpr std::array edges = {RT, BT, GT, YT, RH, BH, GH, YH};

}

using edge::Edge;


//------------------------------------------------------------------------
// Tiles
//------------------------------------------------------------------------
namespace tile {

constexpr UInt edges_per_tile = 4;

using TileEdgeSpan = std::span<const Edge, edges_per_tile>;
auto edge_span(const Edge* first) { return TileEdgeSpan(first, edges_per_tile); }
auto wrap(const UInt n) { return n % edges_per_tile; }

class Tile {
    std::array<Edge, 2*edges_per_tile> edges_;
  public:
    Tile() = default;
    Tile(const TileEdgeSpan es) {
        UInt n = 0;
        for (auto edge : es) edges_[n++] = edge;
        for (auto edge : es) edges_[n++] = edge;
    }
    Tile(const std::initializer_list<Edge> il) : Tile(edge_span(std::data(il))) {}
    Edge edge(const UInt n) const { return edges_[n]; }
    auto edges() const { return edge_span(std::data(edges_)); }
    bool operator==(const Tile&) const = default;
    friend class TileView;
};

class TileView {
    const Tile& tile_;
    UInt rotation_;
  public:
    TileView(const Tile& tile) : tile_(tile), rotation_(0) {}
    TileView(const Tile& tile, UInt rotation) : tile_(tile), rotation_(wrap(rotation)) {}
    TileView(const TileView& tv, UInt rotation) : tile_(tv.tile_), rotation_(wrap(tv.rotation_ + rotation)) {}
    Edge edge(const UInt n) const { return tile_.edges_[rotation_ + n]; }
    auto edges() const { return edge_span(&tile_.edges_[rotation_]); }
    bool operator==(const TileView&) const = default;
    bool is_same_tile(const TileView& tv) const { return &tile_ == &tv.tile_; }
};

}

using tile::Tile;
using tile::TileView;

auto top   (const TileView& tv) { return tv.edge(0); }
auto right (const TileView& tv) { return tv.edge(1); }
auto bottom(const TileView& tv) { return tv.edge(2); }
auto left  (const TileView& tv) { return tv.edge(3); }
TileView rotate(const TileView& tv, UInt rotations=1) { return TileView(tv, rotations); }


//------------------------------------------------------------------------
// Utilities
//------------------------------------------------------------------------
std::array<Tile,9> load_tiles(const std::string& input_file) {
    std::array<Tile, 9> tiles;
    std::array<Edge, tile::edges_per_tile> edges;
    std::string line, digraph;
    std::ifstream inp;
    inp.exceptions(std::ios::failbit | std::ios::badbit);
    inp.open(input_file);
    for(int n = 0; n < tiles.size(); ) {
        if (inp.peek() == '#') {
            std::getline(inp, line);
            continue;
        }
        for (auto& edge : edges) {
            inp >> digraph;
            edge = Edge(digraph);
        }
        tiles[n++] = Tile(edges);
    }
    return tiles;
}


/*
class Deck {

    static constexpr uint8_t num_tiles = 9;
    static constexpr uint8_t offsets_size = edges.size()+1;
    using OffsetArray = std::array<uint8_t, offset_size>;

    std::array<Tile,    num_tiles> tiles;
    std::array<Tile,  4*num_tiles> rotated_tiles;
    std::array<Tile, 16*num_tiles> edge_to_tile_buffer;
    OffsetArray top_edge_offsets;
    OffsetArray right_edge_offsets;
    OffsetArray bottom_edge_offsets;
    OffsetArray left_edge_offsets;

    uint8_t fill_edge_to_tile_buffer(uint8_t side, uint8_t start, OffsetArray* offsets) {
        for (auto e : edges) {
            offsets[e] = start;
            for (auto t : rotated_tiles) {
                if (t.side(side) == e)
                    edge_to_tile_buffer[start++] = t;
            }
        }
        offsets[offsets_size] = start;
        return start;
    }

    Deck(const std::span<const Tiles> t) : tiles(t) {

        size_t n = 0;
        for (auto tile : tiles) {
            rotated_tiles[n++] = tile
            rotated_tiles[n++] = rotate(rotated_tiles[n]);
            rotated_tiles[n++] = rotate(rotated_tiles[n]);
            rotated_tiles[n++] = rotate(rotated_tiles[n]);
        }

        uint8_t m = 0;
        m = fill_edge_to_tile_buffer(0, m, &top_edge_offsets);
        m = fill_edge_to_tile_buffer(1, m, &right_edge_offsets);
        m = fill_edge_to_tile_buffer(2, m, &bottom_edge_offsets);
        m = fill_edge_to_tile_buffer(3, m, &left_edge_offsets);

    }

    std::span<Tile> tiles_with_top_edge(Edge e) {
        auto begin = top_edge_offsets(e);
        auto end   = top_edge_offsets(e+1);
        return {&edge_to_tile_buffer[begin], end-begin};
    }
    std::span<Tile> tiles_with_right_edge(Edge e) {
        auto begin = right_edge_offsets(e);
        auto end   = right_edge_offsets(e+1);
        return {&edge_to_tile_buffer[begin], end-begin};
    }
    std::span<Tile> tiles_with_bottom_edge(Edge e) {
        auto begin = bottom_edge_offsets(e);
        auto end   = bottom_edge_offsets(e+1);
        return {&edge_to_tile_buffer[begin], end-begin};
    }
    std::span<Tile> tiles_with_left_edge(Edge e) {
        auto begin = left_edge_offsets(e);
        auto end   = left_edge_offsets(e+1);
        return {&edge_to_tile_buffer[begin], end-begin};
    }
}

std::vector<Quads> find_quads(const Deck& d) {

    // Algorithm 1
    for (auto ul : d.rotated_tiles()) {
        for (auto ur : d.tiles_with_left_edge(ul.right.match())) {
            for (auto ll : d.tiles_with_top_edge(ul.bottom.match())) {
                for (auto lr : d.tiles_with_left_edge(ll.right.match()) {
                    if lr.top.matches(ur.bottom)
                        quads.push_back({ul, ur, ll, lr});
                }
            }
        }
    }

    // Algorithm 2
    for (uint8_t iul = 0; iul <= d.num_tiles-4; ++iul) {
    for (auto ul : d.rotated_tiles(iul)) {
        for (uint8_t iur = iul+1; iur < d.num_tiles; ++iur) {
        for (ur : d.rotated_tiles(iur)) {
            if (!ul.right.matches(ur.left)) continue;
            for (uint8_t ill = iul+1; ill < d.num_tiles; ++ill) {
            for (ll : d.rotated_tiles(ill)) {
                if (!ul.bottom.matches(ll.top)) continue;
                for (uint8_t ilr = iul+1; ilr < d.num_tiles; ++ilr) {
                for (lr : d.rotated_tiles(ilr)) {
                    if (!ll.right.matches(lr.left)) continue;
                    if (!ur.bottom.matches(lr.top)) continue;
                    if (iur == ill | iur == ilr | ill == ilr) continue;
                    quads.push_back({ul, ur, ll, lr});
    }}}}}}}}

}
*/