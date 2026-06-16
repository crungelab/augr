// augr/fm/dx7_algorithm.cpp
#include <augr/fm/dx7_algorithm.h>

#include <cassert>

namespace augr::fm {

// Feedback on the DX7 always applies to the last operator in the feedback
// loop, which is the operator that feeds back into itself. For most
// algorithms this is OP1 (index 0). Where the spec designates a different
// operator it is noted per-entry.

static const Dx7AlgorithmDef kAlgorithms[32] = {
    // Algorithm 1: carriers 1, 3; 6->5 5->4 4->3 2->1; feedback OP6 (scale 0.2)
    {
        { true, false, true, false, false, false },
        { { {5,4}, {4,3}, {3,2}, {1,0}, {0,0} } },
        4, 5
    },
    // Algorithm 2: carriers 1, 3; 6->5 5->4 4->3 2->1; feedback OP2 (scale 0.2)
    {
        { true, false, true, false, false, false },
        { { {5,4}, {4,3}, {3,2}, {1,0}, {0,0} } },
        4, 1
    },
    // Algorithm 3: carriers 1, 4; 6->5 5->4 3->2 2->1; feedback OP6 (scale 0.2)
    {
        { true, false, false, true, false, false },
        { { {5,4}, {4,3}, {2,1}, {1,0}, {0,0} } },
        4, 5
    },
    // Algorithm 4: carriers 1, 4; 6->5 5->4 3->2 2->1; feedback none
    {
        { true, false, false, true, false, false },
        { { {5,4}, {4,3}, {2,1}, {1,0}, {0,0} } },
        4, -1
    },
    // Algorithm 5: carriers 1, 3, 5; 6->5 4->3 2->1; feedback OP6 (scale 0.1)
    {
        { true, false, true, false, true, false },
        { { {5,4}, {3,2}, {1,0}, {0,0}, {0,0} } },
        3, 5
    },
    // Algorithm 6: carriers 1, 3, 5; 6->5 4->3 2->1; feedback OP6 (scale 1.0)
    {
        { true, false, true, false, true, false },
        { { {5,4}, {3,2}, {1,0}, {0,0}, {0,0} } },
        3, 5
    },
    // Algorithm 7: carriers 1, 3; 6->5 4->3 5->3 2->1; feedback OP6 (scale 1.0)
    {
        { true, false, true, false, false, false },
        { { {5,4}, {3,2}, {4,2}, {1,0}, {0,0} } },
        4, 5
    },
    // Algorithm 8: carriers 1, 3; 6->5 4->3 5->3 2->1; feedback OP4 (scale 0.1)
    {
        { true, false, true, false, false, false },
        { { {5,4}, {3,2}, {4,2}, {1,0}, {0,0} } },
        4, 3
    },
    // Algorithm 9: carriers 1, 3; 6->5 4->3 5->3 2->1; feedback OP2 (scale 0.4)
    {
        { true, false, true, false, false, false },
        { { {5,4}, {3,2}, {4,2}, {1,0}, {0,0} } },
        4, 1
    },
    // Algorithm 10: carriers 1, 4; 5->4 6->4 3->2 2->1; feedback OP3 (scale 0.2)
    {
        { true, false, false, true, false, false },
        { { {4,3}, {5,3}, {2,1}, {1,0}, {0,0} } },
        4, 2
    },
    // Algorithm 11: carriers 1, 4; 5->4 6->4 3->2 2->1; feedback OP6 (scale 0.2)
    {
        { true, false, false, true, false, false },
        { { {4,3}, {5,3}, {2,1}, {1,0}, {0,0} } },
        4, 5
    },
    // Algorithm 12: carriers 1, 3; 4->3 5->3 6->3 2->1; feedback OP2 (scale 0.2)
    {
        { true, false, true, false, false, false },
        { { {3,2}, {4,2}, {5,2}, {1,0}, {0,0} } },
        4, 1
    },
    // Algorithm 13: carriers 1, 3; 4->3 5->3 6->3 2->1; feedback OP6 (scale 0.1)
    {
        { true, false, true, false, false, false },
        { { {3,2}, {4,2}, {5,2}, {1,0}, {0,0} } },
        4, 5
    },
    // Algorithm 14: carriers 1, 3; 5->4 6->4 4->3 2->1; feedback OP6 (scale 1.0)
    {
        { true, false, true, false, false, false },
        { { {4,3}, {5,3}, {3,2}, {1,0}, {0,0} } },
        4, 5
    },
    // Algorithm 15: carriers 1, 3; 5->4 6->4 4->3 2->1; feedback OP2 (scale 0.4)
    {
        { true, false, true, false, false, false },
        { { {4,3}, {5,3}, {3,2}, {1,0}, {0,0} } },
        4, 1
    },
    // Algorithm 16: carriers 1; 6->5 4->3 2->1 3->1 5->1; feedback OP6 (scale 1.0)
    {
        { true, false, false, false, false, false },
        { { {5,4}, {3,2}, {1,0}, {2,0}, {4,0} } },
        5, 5
    },
    // Algorithm 17: carriers 1; 6->5 4->3 2->1 3->1 5->1; feedback OP2 (scale 0.5)
    {
        { true, false, false, false, false, false },
        { { {5,4}, {3,2}, {1,0}, {2,0}, {4,0} } },
        5, 1
    },
    // Algorithm 18: carriers 1; 6->5 5->4 2->1 3->1 4->1; feedback OP3 (scale 1.0)
    {
        { true, false, false, false, false, false },
        { { {5,4}, {4,3}, {1,0}, {2,0}, {3,0} } },
        5, 2
    },
    // Algorithm 19: carriers 1, 4, 5; 6->5 6->4 3->2 2->1; feedback OP6 (scale 0.4)
    {
        { true, false, false, true, true, false },
        { { {5,4}, {5,3}, {2,1}, {1,0}, {0,0} } },
        4, 5
    },
    // Algorithm 20: carriers 1, 3, 5; 5->4 6->4 3->2 2->1; feedback OP3 (scale 0.2)
    {
        { true, false, true, false, true, false },
        { { {4,3}, {5,3}, {2,1}, {1,0}, {0,0} } },
        4, 2
    },
    // Algorithm 21: carriers 1, 2, 4, 5; 6->5 6->4 3->2 3->1; feedback OP3 (scale 0.2)
    {
        { true, true, false, true, true, false },
        { { {5,4}, {5,3}, {2,1}, {2,0}, {0,0} } },
        4, 2
    },
    // Algorithm 22: carriers 1, 3, 4, 5; 6->5 6->4 6->3 2->1; feedback OP6 (scale 0.1)
    {
        { true, false, true, true, true, false },
        { { {5,4}, {5,3}, {5,2}, {1,0}, {0,0} } },
        4, 5
    },
    // Algorithm 23: carriers 1, 2, 4, 5; 6->5 6->4 3->2; feedback OP6 (scale 0.2)
    {
        { true, true, false, true, true, false },
        { { {5,4}, {5,3}, {2,1}, {0,0}, {0,0} } },
        3, 5
    },
    // Algorithm 24: carriers 1, 2, 3, 4, 5; 6->5 6->4 6->3; feedback OP6 (scale 0.6)
    {
        { true, true, true, true, true, false },
        { { {5,4}, {5,3}, {5,2}, {0,0}, {0,0} } },
        3, 5
    },
    // Algorithm 25: carriers 1, 2, 3, 4, 5; 6->5 6->4; feedback OP6 (scale 0.2)
    {
        { true, true, true, true, true, false },
        { { {5,4}, {5,3}, {0,0}, {0,0}, {0,0} } },
        2, 5
    },
    // Algorithm 26: carriers 1, 2, 4; 5->4 6->4 3->2; feedback OP6 (scale 0.2)
    {
        { true, true, false, true, false, false },
        { { {4,3}, {5,3}, {2,1}, {0,0}, {0,0} } },
        3, 5
    },
    // Algorithm 27: carriers 1, 2, 4; 5->4 6->4 3->2; feedback OP3 (scale 0.2)
    {
        { true, true, false, true, false, false },
        { { {4,3}, {5,3}, {2,1}, {0,0}, {0,0} } },
        3, 2
    },
    // Algorithm 28: carriers 1, 3, 6; 5->4 4->3 2->1; feedback OP5 (scale 0.2)
    {
        { true, false, true, false, false, true },
        { { {4,3}, {3,2}, {1,0}, {0,0}, {0,0} } },
        3, 4
    },
    // Algorithm 29: carriers 1, 2, 3, 5; 6->5 4->3; feedback OP6 (scale 0.6)
    {
        { true, true, true, false, true, false },
        { { {5,4}, {3,2}, {0,0}, {0,0}, {0,0} } },
        2, 5
    },
    // Algorithm 30: carriers 1, 2, 3, 6; 5->4 4->3; feedback OP5 (scale 0.2)
    {
        { true, true, true, false, false, true },
        { { {4,3}, {3,2}, {0,0}, {0,0}, {0,0} } },
        2, 4
    },
    // Algorithm 31: carriers 1, 2, 3, 4, 5; 6->5; feedback OP6 (scale 0.6)
    {
        { true, true, true, true, true, false },
        { { {5,4}, {0,0}, {0,0}, {0,0}, {0,0} } },
        1, 5
    },
    // Algorithm 32: carriers 1, 2, 3, 4, 5, 6; ; feedback OP6 (scale 0.6)
    {
        { true, true, true, true, true, true },
        { { {0,0}, {0,0}, {0,0}, {0,0}, {0,0} } },
        0, 5
    },
};

const Dx7AlgorithmDef& GetDx7Algorithm(int index) {
    assert(index >= 0 && index < 32);
    return kAlgorithms[index];
}

} // namespace augr::fm