TEST_F(QueryPlannerTest, MaxMinSortEqualityFirstSortSecond) {
    addIndex(BSON("a" << 1 << "b" << 1));

    // Run an empty query, sort {b: 1}, max/min arguments.
    runQueryFull(BSONObj(),
                 fromjson("{b: 1}"),
                 BSONObj(),
                 0,
                 0,
                 BSONObj(),
                 fromjson("{a: 1, b: 1}"),
                 fromjson("{a: 1, b: 2}"),
                 false);

    assertNumSolutions(1);
    assertSolutionExists("{fetch: {node: {ixscan: {filter: null, pattern: {a: 1, b: 1}}}}}");
}