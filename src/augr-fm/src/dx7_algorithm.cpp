// augr/fm/dx7_algorithm.cpp
#include <augr/fm/dx7_algorithm.h>

#include <cassert>

namespace augr::fm {

// Feedback on the DX7 always applies to the last operator in the feedback
// loop, which is the operator that feeds back into itself. For most
// algorithms this is OP1 (index 0). Where the spec designates a different
// operator it is noted per-entry.

static const Dx7AlgorithmDef kAlgorithms[32] = {

    // Algorithm 1: 6→5→4→3→2→1
    {
        { true, false, false, false, false, false },
        { {{5,4}, {4,3}, {3,2}, {2,1}, {1,0}} },
        5, 1
    },
    // Algorithm 2: 6→5→4→3, 6→5→4→2→1
    {
        { true, false, false, false, false, false },
        { {{5,4}, {4,3}, {4,1}, {3,2}, {1,0}} },
        5, 1
    },
    // Algorithm 3: 6→5→4, 3→2→1
    {
        { true, false, false, false, false, false },
        { {{5,4}, {4,3}, {2,1}, {1,0}, {3,0}} },
        5, 1
    },
    // Algorithm 4: 6→5→4→3→2→1, 3 also feeds 1
    {
        { true, false, false, false, false, false },
        { {{5,4}, {4,3}, {3,2}, {2,1}, {3,0}} },
        5, 1
    },
    // Algorithm 5: 6→5→1, 4→3→1, 2→1
    {
        { true, false, false, false, false, false },
        { {{5,4}, {4,0}, {3,2}, {2,0}, {1,0}} },
        5, 1
    },
    // Algorithm 6: 6→5→1, 4→3→2→1
    {
        { true, false, false, false, false, false },
        { {{5,0}, {4,3}, {3,2}, {2,1}, {1,0}} },
        5, 1
    },
    // Algorithm 7: 6→5, 4→3→2→1, 4 also feeds 1
    {
        { true, false, false, false, false, false },
        { {{5,4}, {3,2}, {2,1}, {1,0}, {3,0}} },
        4, 1
    },
    // Algorithm 8: 6→5→4→3, 2→1 (two carriers: OP3, OP1)
    {
        { true, false, true, false, false, false },
        { {{5,4}, {4,3}, {1,0}, {0,0}, {0,0}} },
        3, 0
    },
    // Algorithm 9: 6→5→4→3, 3→2, 3→1
    {
        { true, false, false, false, false, false },
        { {{5,4}, {4,3}, {3,2}, {2,0}, {3,0}} },
        5, 1
    },
    // Algorithm 10: 6→5, 4→3→2→1, 6 also feeds 4
    {
        { true, false, false, false, false, false },
        { {{5,4}, {5,3}, {3,2}, {2,1}, {1,0}} },
        5, 1
    },
    // Algorithm 11: 6→5→4→3→2, 6→1 (two carriers: OP2, OP1)
    {
        { true, true, false, false, false, false },
        { {{5,4}, {4,3}, {3,2}, {2,1}, {5,0}} },
        5, 0
    },
    // Algorithm 12: 6→5→4→3, 6→5→2→1 (two carriers: OP3, OP1)
    {
        { true, false, true, false, false, false },
        { {{5,4}, {4,3}, {5,1}, {1,0}, {0,0}} },
        4, 0
    },
    // Algorithm 13: 6→5→4, 3→2, 3→1 (three carriers: OP4, OP2, OP1)
    {
        { true, true, false, true, false, false },
        { {{5,4}, {4,3}, {2,1}, {2,0}, {0,0}} },
        4, 0
    },
    // Algorithm 14: 6→5, 4→3→2, 4→1 (two carriers: OP2, OP1)
    {
        { true, true, false, false, false, false },
        { {{5,4}, {3,2}, {2,1}, {3,0}, {0,0}} },
        4, 0
    },
    // Algorithm 15: 6→5, 4→3, 4→2→1 (two carriers: OP3, OP1)
    {
        { true, false, true, false, false, false },
        { {{5,4}, {3,2}, {3,1}, {1,0}, {0,0}} },
        4, 0
    },
    // Algorithm 16: 6→5→4→3, 2→1 (two carriers: OP3, OP1)
    {
        { true, false, true, false, false, false },
        { {{5,4}, {4,3}, {3,2}, {1,0}, {0,0}} },
        4, 0
    },
    // Algorithm 17: 6→5→4, 3→2, 6→1 (three carriers: OP4, OP2, OP1)
    {
        { true, true, false, true, false, false },
        { {{5,4}, {4,3}, {3,2}, {5,0}, {0,0}} },
        4, 0
    },
    // Algorithm 18: 6→5→4→3, 5 also feeds 1 (two carriers: OP3, OP1)
    {
        { true, false, true, false, false, false },
        { {{5,4}, {4,3}, {3,2}, {4,0}, {0,0}} },
        4, 0
    },
    // Algorithm 19: 6→5, 4→3, 2→1 (three carriers: OP5, OP3, OP1)
    {
        { true, false, true, false, true, false },
        { {{5,4}, {3,2}, {1,0}, {0,0}, {0,0}} },
        3, 0
    },
    // Algorithm 20: 6→5→4, 3→2, 3→1 (three carriers: OP4, OP2, OP1)
    {
        { true, true, false, true, false, false },
        { {{5,4}, {4,3}, {2,1}, {2,0}, {0,0}} },
        4, 0
    },
    // Algorithm 21: 6→5, 4→3→2→1 (two carriers: OP5, OP1)
    {
        { true, false, false, false, true, false },
        { {{5,4}, {3,2}, {2,1}, {1,0}, {0,0}} },
        4, 0
    },
    // Algorithm 22: 6→5, 4→3, 2→1, 6 also→1 (three carriers: OP5, OP3, OP1)
    {
        { true, false, true, false, true, false },
        { {{5,4}, {5,0}, {3,2}, {1,0}, {0,0}} },
        4, 0
    },
    // Algorithm 23: 6→5, 4→3, 2→1 (three carriers: OP5, OP3, OP1)
    {
        { true, false, true, false, true, false },
        { {{5,4}, {3,2}, {1,0}, {0,0}, {0,0}} },
        3, 0
    },
    // Algorithm 24: 6→5→4, 3→2 (three carriers: OP4, OP2, OP1)
    {
        { true, true, false, true, false, false },
        { {{5,4}, {4,3}, {2,1}, {0,0}, {0,0}} },
        3, 0
    },
    // Algorithm 25: 6→5, 4→3 (four carriers: OP5, OP3, OP2, OP1)
    {
        { true, true, true, false, true, false },
        { {{5,4}, {3,2}, {0,0}, {0,0}, {0,0}} },
        2, 0
    },
    // Algorithm 26: 6→5, 4→3 (four carriers: OP5, OP3, OP2, OP1 — OP6 mods OP5 only)
    {
        { true, true, true, false, true, false },
        { {{5,4}, {3,2}, {0,0}, {0,0}, {0,0}} },
        2, 0
    },
    // Algorithm 27: 6→1 (five carriers: OP5, OP4, OP3, OP2, OP1)
    {
        { true, true, true, true, true, false },
        { {{5,0}, {0,0}, {0,0}, {0,0}, {0,0}} },
        1, 0
    },
    // Algorithm 28: all carriers — additive synthesis
    {
        { true, true, true, true, true, true },
        { {{0,0}, {0,0}, {0,0}, {0,0}, {0,0}} },
        0, 0
    },
    // Algorithm 29: 6→5→4→3 (four carriers: OP3, OP2, OP1, and branch)
    {
        { true, true, true, true, false, false },
        { {{5,4}, {4,3}, {3,2}, {0,0}, {0,0}} },
        3, 0
    },
    // Algorithm 30: 6→5, 4→3 (four carriers: OP5, OP3, OP2, OP1)
    {
        { true, true, true, false, true, false },
        { {{5,4}, {3,2}, {0,0}, {0,0}, {0,0}} },
        2, 0
    },
    // Algorithm 31: 6→5→4→3→2 (two carriers: OP2, OP1)
    {
        { true, true, false, false, false, false },
        { {{5,4}, {4,3}, {3,2}, {2,1}, {0,0}} },
        4, 0
    },
    // Algorithm 32: 6→5→4→3→2→1
    {
        { true, false, false, false, false, false },
        { {{5,4}, {4,3}, {3,2}, {2,1}, {1,0}} },
        5, 1
    },
};

const Dx7AlgorithmDef& GetDx7Algorithm(int index) {
    assert(index >= 0 && index < 32);
    return kAlgorithms[index];
}

} // namespace augr::fm