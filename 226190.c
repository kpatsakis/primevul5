TEST_F(QueryPlannerTest, BasicSortElim) {
    addIndex(BSON("x" << 1));
    // query, sort, proj
    runQuerySortProj(fromjson("{ x : {$gt: 1}}"), fromjson("{x: 1}"), BSONObj());

    ASSERT_EQUALS(getNumSolutions(), 2U);
    assertSolutionExists(
        "{sort: {pattern: {x: 1}, limit: 0, node: {sortKeyGen: "
        "{node: {cscan: {dir: 1, filter: {x: {$gt: 1}}}}}}}}");
    assertSolutionExists(
        "{fetch: {filter: null, node: {ixscan: {filter: null, pattern: {x: 1}}}}}");
}