TEST_F(QueryPlannerTest, MaxMinSortInequalityFirstSortSecond) {
    addIndex(BSON("a" << 1 << "b" << 1));

    // Run an empty query, sort {b: 1}, max/min arguments.
    runQueryFull(BSONObj(),
                 fromjson("{b: 1}"),
                 BSONObj(),
                 0,
                 0,
                 BSONObj(),
                 fromjson("{a: 1, b: 1}"),
                 fromjson("{a: 2, b: 2}"),
                 false);

    assertNumSolutions(1);
    assertSolutionExists(
        "{sort: {pattern: {b: 1}, limit: 0, node: {sortKeyGen: {node: "
        "{fetch: {node: "
        "{ixscan: {filter: null, pattern: {a: 1, b: 1}}}}}}}}}");
}