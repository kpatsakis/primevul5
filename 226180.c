TEST_F(QueryPlannerTest, MergeSortReverseScanOneIndex) {
    addIndex(BSON("a" << 1 << "c" << 1));
    addIndex(BSON("b" << 1 << "c" << -1));
    runQueryAsCommand(fromjson("{find: 'testns', filter: {$or: [{a: 1}, {b: 1}]}, sort: {c: 1}}"));

    assertNumSolutions(2U);
    assertSolutionExists(
        "{sort: {pattern: {c: 1}, limit: 0, node: {sortKeyGen: {node: "
        "{cscan: {dir: 1}}}}}}");
    assertSolutionExists(
        "{fetch: {node: {mergeSort: {nodes: "
        "[{ixscan: {pattern: {a: 1, c: 1}, dir: 1}}, {ixscan: {pattern: {b: 1, c: -1}, dir: "
        "-1}}]}}}}");
}