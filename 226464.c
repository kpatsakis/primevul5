TEST_F(QueryPlannerTest, NonPrefixRegexMultikey) {
    // true means multikey
    addIndex(BSON("a" << 1), true);
    runQuery(fromjson("{a: /foo/}"));

    ASSERT_EQUALS(getNumSolutions(), 2U);
    assertSolutionExists("{cscan: {filter: {a: /foo/}, dir: 1}}");
    assertSolutionExists(
        "{fetch: {filter: {a: /foo/}, node: {ixscan: "
        "{pattern: {a: 1}, filter: null}}}}");
}