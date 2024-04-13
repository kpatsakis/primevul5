TEST_F(QueryPlannerTest, SimpleOr) {
    addIndex(BSON("a" << 1));
    runQuery(fromjson("{$or: [{a: 20}, {a: 21}]}"));

    ASSERT_EQUALS(getNumSolutions(), 2U);
    assertSolutionExists("{cscan: {dir: 1, filter: {$or: [{a: 20}, {a: 21}]}}}");
    assertSolutionExists(
        "{fetch: {filter: null, node: {ixscan: "
        "{filter: null, pattern: {a:1}}}}}");
}