TEST_F(QueryPlannerTest, NoSplitLimitedSortAsCommand) {
    params.options = QueryPlannerParams::NO_TABLE_SCAN;
    params.options |= QueryPlannerParams::SPLIT_LIMITED_SORT;
    addIndex(BSON("a" << 1));
    addIndex(BSON("b" << 1));

    runQueryAsCommand(fromjson("{find: 'testns', filter: {a: 1}, sort: {b: 1}, limit: 3}"));

    assertNumSolutions(2U);
    assertSolutionExists(
        "{limit: {n: 3, node: {fetch: {filter: {a:1}, node: "
        "{ixscan: {filter: null, pattern: {b: 1}}}}}}}");
    assertSolutionExists(
        "{sort: {pattern: {b: 1}, limit: 3, node: {sortKeyGen: {node: {fetch: {filter: null,"
        "node: {ixscan: {pattern: {a: 1}}}}}}}}}");
}