TEST_F(QueryPlannerTest, ManyInWithSort) {
    addIndex(BSON("a" << 1 << "b" << 1 << "c" << 1 << "d" << 1));
    runQuerySortProjSkipNToReturn(fromjson("{a: {$in: [1, 2]}, b:{$in:[1,2]}, c:{$in:[1,2]}}"),
                                  BSON("d" << 1),
                                  BSONObj(),
                                  0,
                                  1);

    assertSolutionExists(
        "{sort: {pattern: {d: 1}, limit: 1, node: {sortKeyGen: "
        "{node: {cscan: {dir: 1}}}}}}");
    assertSolutionExists(
        "{fetch: {node: {mergeSort: {nodes: "
        "[{ixscan: {pattern: {a: 1, b: 1, c:1, d:1}}},"
        "{ixscan: {pattern: {a: 1, b: 1, c:1, d:1}}},"
        "{ixscan: {pattern: {a: 1, b: 1, c:1, d:1}}},"
        "{ixscan: {pattern: {a: 1, b: 1, c:1, d:1}}},"
        "{ixscan: {pattern: {a: 1, b: 1, c:1, d:1}}},"
        "{ixscan: {pattern: {a: 1, b: 1, c:1, d:1}}},"
        "{ixscan: {pattern: {a: 1, b: 1, c:1, d:1}}},"
        "{ixscan: {pattern: {a: 1, b: 1, c:1, d:1}}}]}}}}");
}