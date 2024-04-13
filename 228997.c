TEST_F(QueryPlannerTest, MaxMinReverseIndexDirSort) {
    addIndex(BSON("a" << -1));

    // Min/max specifies a forward scan with bounds [{a: 8}, {a: 2}]. Asking for
    // an ascending sort reverses the direction of the scan to [{a: 2}, {a: 8}].
    runQueryFull(BSONObj(),
                 fromjson("{a: 1}"),
                 BSONObj(),
                 0,
                 0,
                 BSONObj(),
                 fromjson("{a: 8}"),
                 fromjson("{a: 2}"));

    assertNumSolutions(1);
    assertSolutionExists(
        "{fetch: {node: {ixscan: {filter: null, dir: -1,"
        "pattern: {a: -1}}}}}");
}