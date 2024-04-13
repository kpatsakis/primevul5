TEST_F(QueryPlannerTest, OrWithoutEnoughIndices) {
    addIndex(BSON("a" << 1));
    runQuery(fromjson("{$or: [{a: 20}, {b: 21}]}"));
    ASSERT_EQUALS(getNumSolutions(), 1U);
    assertSolutionExists("{cscan: {dir: 1, filter: {$or: [{a: 20}, {b: 21}]}}}");
}