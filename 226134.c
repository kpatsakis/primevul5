TEST_F(QueryPlannerTest, NoKeepWithIndexedSort) {
    params.options = QueryPlannerParams::KEEP_MUTATIONS;
    addIndex(BSON("a" << 1 << "b" << 1));
    runQuerySortProjSkipNToReturn(fromjson("{a: {$in: [1, 2]}}"), BSON("b" << 1), BSONObj(), 0, 1);

    // cscan solution exists but we didn't turn on the "always include a collscan."
    assertNumSolutions(1);
    assertSolutionExists(
        "{fetch: {node: {mergeSort: {nodes: "
        "[{ixscan: {pattern: {a: 1, b: 1}}}, {ixscan: {pattern: {a: 1, b: 1}}}]}}}}");
}