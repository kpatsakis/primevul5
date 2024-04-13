TEST_F(QueryPlannerTest, BasicCompound) {
    addIndex(BSON("x" << 1 << "y" << 1));
    runQuery(fromjson("{ x : 5, y: 10}"));

    ASSERT_EQUALS(getNumSolutions(), 2U);
    assertSolutionExists("{cscan: {dir: 1}}");
    assertSolutionExists(
        "{fetch: {filter: null, node: {ixscan: "
        "{filter: null, pattern: {x: 1, y: 1}}}}}");
}