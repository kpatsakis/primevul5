TEST_F(QueryPlannerTest, MergeSortReverseSubtreeContainedOr) {
    addIndex(BSON("a" << 1 << "e" << 1));
    addIndex(BSON("c" << 1 << "e" << -1));
    addIndex(BSON("d" << 1 << "e" << -1));
    runQueryAsCommand(fromjson(
        "{find: 'testns', filter: {$or: [{a: 1}, {b: 1, $or: [{c: 1}, {d: 1}]}]}, sort: {e: 1}}"));

    assertNumSolutions(2U);
    assertSolutionExists(
        "{sort: {pattern: {e: 1}, limit: 0, node: {sortKeyGen: {node: "
        "{cscan: {dir: 1}}}}}}");
    assertSolutionExists(
        "{fetch: {node: {mergeSort: {nodes: "
        "[{ixscan: {pattern: {a: 1, e: 1}, dir: 1}}, {fetch: {node: {mergeSort: {nodes: "
        "[{ixscan: {pattern: {c: 1, e: -1}, dir: -1}}, {ixscan: {pattern: {d: 1, e: -1}, dir: "
        "-1}}]}}}}]}}}}");
}