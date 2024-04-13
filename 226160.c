TEST_F(QueryPlannerTest, NoSplitLimitedSortAsCommandBatchSize) {
    params.options = QueryPlannerParams::NO_TABLE_SCAN;
    params.options |= QueryPlannerParams::SPLIT_LIMITED_SORT;
    addIndex(BSON("a" << 1));
    addIndex(BSON("b" << 1));

    runQueryAsCommand(fromjson("{find: 'testns', filter: {a: 1}, sort: {b: 1}, batchSize: 3}"));

    assertNumSolutions(2U);
    assertSolutionExists(
        "{fetch: {filter: {a: 1}, node: {ixscan: "
        "{filter: null, pattern: {b: 1}}}}}");
    assertSolutionExists(
        "{sort: {pattern: {b: 1}, limit: 0, node: {sortKeyGen: {node: {fetch: {filter: null,"
        "node: {ixscan: {pattern: {a: 1}}}}}}}}}");
}