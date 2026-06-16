static const Dx7AlgorithmDef kAlgorithms[32] = {
    // Algorithm 1: carriers 1, 3; 6->5 5->4 4->3 2->1; feedback OP6 (scale 0.2)
    {
        { true, false, true, false, false, false },
        { { {5,4}, {4,3}, {3,2}, {1,0}, {0,0} } },
        4, 5, 0.2f
    },
    // Algorithm 2: carriers 1, 3; 6->5 5->4 4->3 2->1; feedback OP2 (scale 0.2)
    {
        { true, false, true, false, false, false },
        { { {5,4}, {4,3}, {3,2}, {1,0}, {0,0} } },
        4, 1, 0.2f
    },
    // Algorithm 3: carriers 1, 4; 6->5 5->4 3->2 2->1; feedback OP6 (scale 0.2)
    {
        { true, false, false, true, false, false },
        { { {5,4}, {4,3}, {2,1}, {1,0}, {0,0} } },
        4, 5, 0.2f
    },
    // Algorithm 4: carriers 1, 4; 6->5 5->4 3->2 2->1; feedback none
    {
        { true, false, false, true, false, false },
        { { {5,4}, {4,3}, {2,1}, {1,0}, {0,0} } },
        4, -1, 0.0f
    },
    // Algorithm 5: carriers 1, 3, 5; 6->5 4->3 2->1; feedback OP6 (scale 0.1)
    {
        { true, false, true, false, true, false },
        { { {5,4}, {3,2}, {1,0}, {0,0}, {0,0} } },
        3, 5, 0.1f
    },
    // Algorithm 6: carriers 1, 3, 5; 6->5 4->3 2->1; feedback OP6 (scale 1.0)
    {
        { true, false, true, false, true, false },
        { { {5,4}, {3,2}, {1,0}, {0,0}, {0,0} } },
        3, 5, 1.0f
    },
    // Algorithm 7: carriers 1, 3; 6->5 4->3 5->3 2->1; feedback OP6 (scale 1.0)
    {
        { true, false, true, false, false, false },
        { { {5,4}, {3,2}, {4,2}, {1,0}, {0,0} } },
        4, 5, 1.0f
    },
    // Algorithm 8: carriers 1, 3; 6->5 4->3 5->3 2->1; feedback OP4 (scale 0.1)
    {
        { true, false, true, false, false, false },
        { { {5,4}, {3,2}, {4,2}, {1,0}, {0,0} } },
        4, 3, 0.1f
    },
    // Algorithm 9: carriers 1, 3; 6->5 4->3 5->3 2->1; feedback OP2 (scale 0.4)
    {
        { true, false, true, false, false, false },
        { { {5,4}, {3,2}, {4,2}, {1,0}, {0,0} } },
        4, 1, 0.4f
    },
    // Algorithm 10: carriers 1, 4; 5->4 6->4 3->2 2->1; feedback OP3 (scale 0.2)
    {
        { true, false, false, true, false, false },
        { { {4,3}, {5,3}, {2,1}, {1,0}, {0,0} } },
        4, 2, 0.2f
    },
    // Algorithm 11: carriers 1, 4; 5->4 6->4 3->2 2->1; feedback OP6 (scale 0.2)
    {
        { true, false, false, true, false, false },
        { { {4,3}, {5,3}, {2,1}, {1,0}, {0,0} } },
        4, 5, 0.2f
    },
    // Algorithm 12: carriers 1, 3; 4->3 5->3 6->3 2->1; feedback OP2 (scale 0.2)
    {
        { true, false, true, false, false, false },
        { { {3,2}, {4,2}, {5,2}, {1,0}, {0,0} } },
        4, 1, 0.2f
    },
    // Algorithm 13: carriers 1, 3; 4->3 5->3 6->3 2->1; feedback OP6 (scale 0.1)
    {
        { true, false, true, false, false, false },
        { { {3,2}, {4,2}, {5,2}, {1,0}, {0,0} } },
        4, 5, 0.1f
    },
    // Algorithm 14: carriers 1, 3; 5->4 6->4 4->3 2->1; feedback OP6 (scale 1.0)
    {
        { true, false, true, false, false, false },
        { { {4,3}, {5,3}, {3,2}, {1,0}, {0,0} } },
        4, 5, 1.0f
    },
    // Algorithm 15: carriers 1, 3; 5->4 6->4 4->3 2->1; feedback OP2 (scale 0.4)
    {
        { true, false, true, false, false, false },
        { { {4,3}, {5,3}, {3,2}, {1,0}, {0,0} } },
        4, 1, 0.4f
    },
    // Algorithm 16: carriers 1; 6->5 4->3 2->1 3->1 5->1; feedback OP6 (scale 1.0)
    {
        { true, false, false, false, false, false },
        { { {5,4}, {3,2}, {1,0}, {2,0}, {4,0} } },
        5, 5, 1.0f
    },
    // Algorithm 17: carriers 1; 6->5 4->3 2->1 3->1 5->1; feedback OP2 (scale 0.5)
    {
        { true, false, false, false, false, false },
        { { {5,4}, {3,2}, {1,0}, {2,0}, {4,0} } },
        5, 1, 0.5f
    },
    // Algorithm 18: carriers 1; 6->5 5->4 2->1 3->1 4->1; feedback OP3 (scale 1.0)
    {
        { true, false, false, false, false, false },
        { { {5,4}, {4,3}, {1,0}, {2,0}, {3,0} } },
        5, 2, 1.0f
    },
    // Algorithm 19: carriers 1, 4, 5; 6->5 6->4 3->2 2->1; feedback OP6 (scale 0.4)
    {
        { true, false, false, true, true, false },
        { { {5,4}, {5,3}, {2,1}, {1,0}, {0,0} } },
        4, 5, 0.4f
    },
    // Algorithm 20: carriers 1, 3, 5; 5->4 6->4 3->2 2->1; feedback OP3 (scale 0.2)
    {
        { true, false, true, false, true, false },
        { { {4,3}, {5,3}, {2,1}, {1,0}, {0,0} } },
        4, 2, 0.2f
    },
    // Algorithm 21: carriers 1, 2, 4, 5; 6->5 6->4 3->2 3->1; feedback OP3 (scale 0.2)
    {
        { true, true, false, true, true, false },
        { { {5,4}, {5,3}, {2,1}, {2,0}, {0,0} } },
        4, 2, 0.2f
    },
    // Algorithm 22: carriers 1, 3, 4, 5; 6->5 6->4 6->3 2->1; feedback OP6 (scale 0.1)
    {
        { true, false, true, true, true, false },
        { { {5,4}, {5,3}, {5,2}, {1,0}, {0,0} } },
        4, 5, 0.1f
    },
    // Algorithm 23: carriers 1, 2, 4, 5; 6->5 6->4 3->2; feedback OP6 (scale 0.2)
    {
        { true, true, false, true, true, false },
        { { {5,4}, {5,3}, {2,1}, {0,0}, {0,0} } },
        3, 5, 0.2f
    },
    // Algorithm 24: carriers 1, 2, 3, 4, 5; 6->5 6->4 6->3; feedback OP6 (scale 0.6)
    {
        { true, true, true, true, true, false },
        { { {5,4}, {5,3}, {5,2}, {0,0}, {0,0} } },
        3, 5, 0.6f
    },
    // Algorithm 25: carriers 1, 2, 3, 4, 5; 6->5 6->4; feedback OP6 (scale 0.2)
    {
        { true, true, true, true, true, false },
        { { {5,4}, {5,3}, {0,0}, {0,0}, {0,0} } },
        2, 5, 0.2f
    },
    // Algorithm 26: carriers 1, 2, 4; 5->4 6->4 3->2; feedback OP6 (scale 0.2)
    {
        { true, true, false, true, false, false },
        { { {4,3}, {5,3}, {2,1}, {0,0}, {0,0} } },
        3, 5, 0.2f
    },
    // Algorithm 27: carriers 1, 2, 4; 5->4 6->4 3->2; feedback OP3 (scale 0.2)
    {
        { true, true, false, true, false, false },
        { { {4,3}, {5,3}, {2,1}, {0,0}, {0,0} } },
        3, 2, 0.2f
    },
    // Algorithm 28: carriers 1, 3, 6; 5->4 4->3 2->1; feedback OP5 (scale 0.2)
    {
        { true, false, true, false, false, true },
        { { {4,3}, {3,2}, {1,0}, {0,0}, {0,0} } },
        3, 4, 0.2f
    },
    // Algorithm 29: carriers 1, 2, 3, 5; 6->5 4->3; feedback OP6 (scale 0.6)
    {
        { true, true, true, false, true, false },
        { { {5,4}, {3,2}, {0,0}, {0,0}, {0,0} } },
        2, 5, 0.6f
    },
    // Algorithm 30: carriers 1, 2, 3, 6; 5->4 4->3; feedback OP5 (scale 0.2)
    {
        { true, true, true, false, false, true },
        { { {4,3}, {3,2}, {0,0}, {0,0}, {0,0} } },
        2, 4, 0.2f
    },
    // Algorithm 31: carriers 1, 2, 3, 4, 5; 6->5; feedback OP6 (scale 0.6)
    {
        { true, true, true, true, true, false },
        { { {5,4}, {0,0}, {0,0}, {0,0}, {0,0} } },
        1, 5, 0.6f
    },
    // Algorithm 32: carriers 1, 2, 3, 4, 5, 6; ; feedback OP6 (scale 0.6)
    {
        { true, true, true, true, true, true },
        { { {0,0}, {0,0}, {0,0}, {0,0}, {0,0} } },
        0, 5, 0.6f
    },
};
