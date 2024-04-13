TEST_F(QueryPlannerTest, MaxMinSelectCorrectlyOrderedIndex) {
    // There are both ascending and descending indices on 'a'.
    addIndex(BSON("a" << 1));
    addIndex(BSON("a" << -1));

    // The ordering of min and max means that we *must* use the descending index.
    runQueryFull(
        BSONObj(), BSONObj(), BSONObj(), 0, 0, BSONObj(), fromjson("{a: 8}"), fromjson("{a: 2}"));

    assertNumSolutions(1);
    assertSolutionExists("{fetch: {node: {ixscan: {filter: null, dir: 1, pattern: {a: -1}}}}}");

    // If we switch the ordering, then we use the ascending index.
    // The ordering of min and max means that we *must* use the descending index.
    runQueryFull(
        BSONObj(), BSONObj(), BSONObj(), 0, 0, BSONObj(), fromjson("{a: 2}"), fromjson("{a: 8}"));

    assertNumSolutions(1);
    assertSolutionExists("{fetch: {node: {ixscan: {filter: null, dir: 1, pattern: {a: 1}}}}}");
}