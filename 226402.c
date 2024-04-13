TEST_F(QueryPlannerTest, InWithSort) {
    addIndex(BSON("a" << 1 << "b" << 1));
    runQuerySortProjSkipNToReturn(fromjson("{a: {$in: [1, 2]}}"), BSON("b" << 1), BSONObj(), 0, 1);

    assertSolutionExists(
        "{sort: {pattern: {b: 1}, limit: 1, node: {sortKeyGen: "
        "{node: {cscan: {dir: 1}}}}}}");
    assertSolutionExists(
        "{fetch: {node: {mergeSort: {nodes: "
        "[{ixscan: {pattern: {a: 1, b: 1}}}, {ixscan: {pattern: {a: 1, b: 1}}}]}}}}");
}