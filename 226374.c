TEST_F(QueryPlannerTest, EquivalentAndsTwo) {
    addIndex(BSON("a" << 1 << "b" << 1));
    runQuery(fromjson("{$and: [{a: 1, b: 10}, {a: 1, b: 20}]}"));

    ASSERT_EQUALS(getNumSolutions(), 2U);
    assertSolutionExists("{cscan: {dir: 1, filter: {$and:[{a:1},{a:1},{b:10},{b:20}]}}}");
    assertSolutionExists(
        "{fetch: {filter: null, node: {ixscan: "
        "{filter: null, pattern: {a: 1, b: 1}}}}}");
}