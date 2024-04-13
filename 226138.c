TEST_F(QueryPlannerTest, PlannerAddsFetchToIxscanForCountWhenFetchFilterNonempty) {
    params.options = QueryPlannerParams::IS_COUNT;
    addIndex(BSON("x" << 1));
    runQuery(BSON("y" << 3 << "x" << 5));
    ASSERT_EQUALS(getNumSolutions(), 1U);
    assertSolutionExists(
        "{fetch: {filter: {y: 3}, node: {ixscan: "
        "{pattern: {x: 1}, bounds: {x: [[5,5,true,true]]}}}}}");
}