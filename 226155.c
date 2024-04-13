TEST_F(QueryPlannerTest, CantExplodeWithEmptyBounds2) {
    addIndex(BSON("a" << 1 << "b" << 1 << "c" << 1));
    runQuerySortProj(fromjson("{a: {$gt: 3, $lt: 0}}"), BSON("b" << 1), BSONObj());

    assertNumSolutions(2U);
    assertSolutionExists(
        "{sort: {pattern: {b:1}, limit: 0, node: {sortKeyGen: {node: "
        "{cscan: {dir: 1}}}}}}");
    assertSolutionExists(
        "{sort: {pattern: {b:1}, limit: 0, node: {sortKeyGen: {node: "
        "{fetch: {node: {ixscan: {pattern: {a:1,b:1,c:1}}}}}}}}}");
}