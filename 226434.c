TEST_F(QueryPlannerTest, NonPrefixRegex) {
    addIndex(BSON("a" << 1));
    runQuery(fromjson("{a: /foo/}"));

    ASSERT_EQUALS(getNumSolutions(), 2U);
    assertSolutionExists("{cscan: {dir: 1, filter: {a: /foo/}}}");
    assertSolutionExists(
        "{fetch: {filter: null, node: "
        "{ixscan: {filter: {a: /foo/}, pattern: {a: 1}}}}}");
}