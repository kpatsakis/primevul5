TEST_F(QueryPlannerTest, MergeSortReverseIxscanBelowFetch) {
    addIndex(BSON("a" << 1 << "d" << 1));
    addIndex(BSON("b" << 1 << "d" << -1));
    runQueryAsCommand(
        fromjson("{find: 'testns', filter: {$or: [{a: 1}, {b: 1, c: 1}]}, sort: {d: 1}}"));

    assertNumSolutions(2U);
    assertSolutionExists(
        "{sort: {pattern: {d: 1}, limit: 0, node: {sortKeyGen: {node: "
        "{cscan: {dir: 1}}}}}}");
    assertSolutionExists(
        "{fetch: {node: {mergeSort: {nodes: "
        "[{ixscan: {pattern: {a: 1, d: 1}, dir: 1}}, {fetch: {node: {ixscan: {pattern: {b: 1, d: "
        "-1}, dir: -1}}}}]}}}}");
}