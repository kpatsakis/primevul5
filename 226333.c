TEST_F(QueryPlannerTest, BasicSortBooleanIndexKeyPattern) {
    addIndex(BSON("a" << true));
    runQuerySortProj(fromjson("{ a : 5 }"), BSON("a" << 1), BSONObj());

    ASSERT_EQUALS(getNumSolutions(), 2U);
    assertSolutionExists(
        "{sort: {pattern: {a: 1}, limit: 0, node: {sortKeyGen: "
        "{node: {cscan: {dir: 1, filter: {a: 5}}}}}}}");
    assertSolutionExists(
        "{fetch: {filter: null, node: {ixscan: "
        "{filter: null, pattern: {a: true}}}}}");
}