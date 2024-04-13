TEST_F(QueryPlannerTest, CantExplodeOrForSort) {
    addIndex(BSON("a" << 1 << "b" << 1 << "c" << 1));
    addIndex(BSON("d" << 1 << "c" << 1));

    runQuerySortProj(fromjson("{$or: [{a: {$in: [1, 2]}}, {d: 3}]}"), BSON("c" << 1), BSONObj());

    assertNumSolutions(2U);
    assertSolutionExists(
        "{sort: {pattern: {c: 1}, limit: 0, node: {sortKeyGen: {node: "
        "{cscan: {dir: 1}}}}}}");
    assertSolutionExists(
        "{sort: {pattern: {c: 1}, limit: 0, node: {sortKeyGen: {node: "
        "{fetch: {filter: null, node: {or: {nodes: ["
        "{ixscan: {pattern: {a: 1, b: 1, c: 1}}},"
        "{ixscan: {pattern: {d: 1, c: 1}}}]}}}}}}}}");
}