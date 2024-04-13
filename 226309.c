TEST_F(QueryPlannerTest, PrefixRegex) {
    addIndex(BSON("a" << 1));
    runQuery(fromjson("{a: /^foo/}"));

    ASSERT_EQUALS(getNumSolutions(), 2U);
    assertSolutionExists("{cscan: {dir: 1, filter: {a: /^foo/}}}");
    assertSolutionExists(
        "{fetch: {filter: null, node: {ixscan: "
        "{filter: null, pattern: {a: 1}}}}}");
}