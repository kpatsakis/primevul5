TEST_F(QueryPlannerTest, SnapshotUseId) {
    params.options = QueryPlannerParams::SNAPSHOT_USE_ID;

    addIndex(BSON("a" << 1));
    runQuerySnapshot(fromjson("{a: {$gt: 0}}"));

    assertNumSolutions(1U);
    assertSolutionExists(
        "{fetch: {filter: {a:{$gt:0}}, node: "
        "{ixscan: {filter: null, pattern: {_id: 1}}}}}");
}