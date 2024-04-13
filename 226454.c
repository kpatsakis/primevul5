TEST_F(QueryPlannerTest, PlannerUsesCoveredIxscanForCountWhenIndexSatisfiesQuery) {
    params.options = QueryPlannerParams::IS_COUNT;
    addIndex(BSON("x" << 1));
    runQuery(BSON("x" << 5));
    ASSERT_EQUALS(getNumSolutions(), 1U);
    assertSolutionExists("{ixscan: {pattern: {x: 1}, bounds: {x: [[5,5,true,true]]}}}");
}