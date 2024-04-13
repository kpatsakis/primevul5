TEST_F(QueryPlannerTest, MaxMinReverseSort) {
    addIndex(BSON("a" << 1));

    // Run an empty query, sort {a: -1}, max/min arguments.
    runQueryFull(BSONObj(),
                 fromjson("{a: -1}"),
                 BSONObj(),
                 0,
                 0,
                 BSONObj(),
                 fromjson("{a: 2}"),
                 fromjson("{a: 8}"));

    assertNumSolutions(1);
    assertSolutionExists("{fetch: {node: {ixscan: {filter: null, dir: -1, pattern: {a: 1}}}}}");
}