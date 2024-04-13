TEST_F(QueryPlannerTest, InWithSortAndLimitTrailingField) {
    addIndex(BSON("a" << 1 << "b" << -1 << "c" << 1));
    runQuerySortProjSkipNToReturn(fromjson("{a: {$in: [1, 2]}, b: {$gte: 0}}"),
                                  fromjson("{b: -1}"),
                                  BSONObj(),  // no projection
                                  0,          // no skip
                                  -1);        // .limit(1)

    assertNumSolutions(2U);
    assertSolutionExists(
        "{sort: {pattern: {b:-1}, limit: 1, node: {sortKeyGen: "
        "{node: {cscan: {dir: 1}}}}}}");
    assertSolutionExists(
        "{limit: {n: 1, node: {fetch: {node: {mergeSort: {nodes: "
        "[{ixscan: {pattern: {a:1,b:-1,c:1}}}, "
        " {ixscan: {pattern: {a:1,b:-1,c:1}}}]}}}}}}");
}