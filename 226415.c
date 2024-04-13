TEST_F(QueryPlannerTest, TooManyToExplode) {
    addIndex(BSON("a" << 1 << "b" << 1 << "c" << 1 << "d" << 1));
    runQuerySortProjSkipNToReturn(fromjson("{a: {$in: [1,2,3,4,5,6]},"
                                           "b:{$in:[1,2,3,4,5,6,7,8]},"
                                           "c:{$in:[1,2,3,4,5,6,7,8]}}"),
                                  BSON("d" << 1),
                                  BSONObj(),
                                  0,
                                  1);

    // We cap the # of ixscans we're willing to create.
    assertNumSolutions(2);
    assertSolutionExists(
        "{sort: {pattern: {d: 1}, limit: 1, node: {sortKeyGen: "
        "{node: {cscan: {dir: 1}}}}}}");
    assertSolutionExists(
        "{sort: {pattern: {d: 1}, limit: 1, node: {sortKeyGen: {node: "
        "{fetch: {node: {ixscan: {pattern: {a: 1, b: 1, c:1, d:1}}}}}}}}}");
}