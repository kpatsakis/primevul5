TEST_F(QueryPlannerTest, ExplodeIxscanWithFilter) {
    addIndex(BSON("a" << 1 << "b" << 1));

    runQuerySortProj(fromjson("{$and: [{b: {$regex: 'foo', $options: 'i'}},"
                              "{a: {$in: [1, 2]}}]}"),
                     BSON("b" << 1),
                     BSONObj());

    assertNumSolutions(2U);
    assertSolutionExists(
        "{sort: {pattern: {b: 1}, limit: 0, node: {sortKeyGen: {node: "
        "{cscan: {dir: 1}}}}}}");
    assertSolutionExists(
        "{fetch: {node: {mergeSort: {nodes: "
        "[{ixscan: {pattern: {a:1, b:1},"
        "filter: {b: {$regex: 'foo', $options: 'i'}}}},"
        "{ixscan: {pattern: {a:1, b:1},"
        "filter: {b: {$regex: 'foo', $options: 'i'}}}}]}}}}");
}