TEST_F(QueryPlannerTest, ExplodeMustReverseScans2) {
    addIndex(BSON("a" << 1 << "b" << 1 << "c" << -1));
    runQuerySortProj(fromjson("{a: {$in: [1, 2]}, b: {$in: [3, 4]}}"), BSON("c" << 1), BSONObj());

    assertNumSolutions(2U);
    assertSolutionExists(
        "{sort: {pattern: {c: 1}, limit: 0, node: {sortKeyGen: {node: "
        "{cscan: {dir: 1}}}}}}");
    assertSolutionExists(
        "{fetch: {node: {mergeSort: {nodes: "
        "[{ixscan: {pattern: {a:1, b:1, c:-1}}},"
        "{ixscan: {pattern: {a:1, b:1, c:-1}}},"
        "{ixscan: {pattern: {a:1, b:1, c:-1}}},"
        "{ixscan: {pattern: {a:1, b:1, c:-1}}}]}}}}");
}