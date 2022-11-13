# puzz

My mother-in-law recently gifted my son two obnoxious puzzles from [Lagoon Games](www.lagoongames.com):

| Fabulous Frogs | Leapin' Lizards |
|----------------|-----------------|
| <img src="https://user-images.githubusercontent.com/12869747/201507904-2ea420bb-7589-4b25-86d5-8886fce6ab54.png" width=256> | <img src="https://user-images.githubusercontent.com/12869747/201507803-c20bc712-8284-45be-a066-96ab7dd74b41.png" width=256> |

The idea of these puzzles is to arrange the nine tiles into a 3-by-3 grid such that all edges "match". This is essentially a combinatorial search problem, and while there are a few tricks you can use to limit the search, I couldn't find the solution despite spending more hours trying than I care to admit.

So, I did what any good geek would do an wrote program to solve it. I chose to write the solver in C++ both for speed (potentially testing millions of edge combinations in a CPU-bound loop) and so I could play around with C++20's range algorithms and the `mdarray` type proposed for C++26. It can solve both puzzles in about ~0.1sec with the `-O2` flag.

### Implementation Notes

* The output of the solver routine is two 3x3 arrays: one specifying the position of each input tile in final arrangement, and another specifying the number of quarter-turns in clockwise direction needed to orient each tile.

* The solution process consists of two main steps. First, an outer loop generates a sequence of possible tile layouts using `std::next_permutation`. Then, a series of nine inner loops perfom a brute force search of possible tile orientations for a given layout, exiting when either a solution is found or the layout is determined to be inconsistent.

* During the orientation search, tiles in the layout are "rotated" in place by applying `std::rotate` to a tile's edge list. The rotation count array keeps track of how many times a tile has been rotated and also serves as the loop control variable. This minimizes working memory used during these hot loops (9 32-bit `Tile` objects and 9 `uint8_t` counters). It also ensures we can use the same edge checking function at each level of the search, which results in a very clear and elegant implementation. 

* The edge check function itself simply verifies that a tile's left and top edges match its neighbors. This function is templated on the (i,j) location of the tile being tested so that appropriate checks can be deleted via `if constexpr` if the tile doesn't have, e.g. a top neighbor. The approprite `check<i,j>` function is called after each tile rotation, and the puzzle is solved when `check<i,j>(t)` returns true for all 9 tile positions.

### Usage

    g++ --std=c++20 -DNDEBUG -O2 puzz.cpp -o puzz
    ./puzz frogs.txt
    ./puzz lizards.txt
    
  
