TEST_F(QueryPlannerTest, NonPrefixRegexAnd) {
    addIndex(BSON("a" << 1 << "b" << 1));
    runQuery(fromjson("{a: /foo/, b: 2}"));

    ASSERT_EQUALS(getNumSolutions(), 2U);
    assertSolutionExists("{cscan: {dir: 1, filter: {$and: [{b: 2}, {a: /foo/}]}}}");
    assertSolutionExists(
        "{fetch: {filter: null, node: {ixscan: "
        "{filter: {a: /foo/}, pattern: {a: 1, b: 1}}}}}");
}