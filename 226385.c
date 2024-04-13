TEST_F(QueryPlannerTest, BasicSortWithIndexablePred) {
    addIndex(BSON("a" << 1));
    addIndex(BSON("b" << 1));
    runQuerySortProj(fromjson("{ a : 5 }"), BSON("b" << 1), BSONObj());

    ASSERT_EQUALS(getNumSolutions(), 3U);
    assertSolutionExists(
        "{sort: {pattern: {b: 1}, limit: 0, node: {sortKeyGen: "
        "{node: {cscan: {dir: 1, filter: {a: 5}}}}}}}");
    assertSolutionExists(
        "{sort: {pattern: {b: 1}, limit: 0, node: {sortKeyGen: "
        "{node: {fetch: {filter: null, node: "
        "{ixscan: {filter: null, pattern: {a: 1}}}}}}}}}");
    assertSolutionExists(
        "{fetch: {filter: {a: 5}, node: {ixscan: "
        "{filter: null, pattern: {b: 1}}}}}");
}