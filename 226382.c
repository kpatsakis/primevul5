TEST_F(QueryPlannerTest, CompoundMissingField) {
    addIndex(BSON("x" << 1 << "y" << 1 << "z" << 1));
    runQuery(fromjson("{ x : 5, z: 10}"));

    ASSERT_EQUALS(getNumSolutions(), 2U);
    assertSolutionExists("{cscan: {dir: 1}}");
    assertSolutionExists(
        "{fetch: {filter: null, node: "
        "{ixscan: {filter: null, pattern: {x: 1, y: 1, z: 1}}}}}");
}