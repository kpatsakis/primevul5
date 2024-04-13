TEST_F(QueryPlannerTest, IndexFilterAppliedTrue) {
    params.indexFiltersApplied = true;

    addIndex(BSON("x" << 1));

    runQuery(BSON("x" << 5));

    ASSERT_EQUALS(getNumSolutions(), 2U);
    assertSolutionExists("{cscan: {dir: 1, filter: {x: 5}}}");
    assertSolutionExists("{fetch: {filter: null, node: {ixscan: {pattern: {x: 1}}}}}");

    // Check indexFilterApplied in query solutions;
    for (auto it = solns.begin(); it != solns.end(); ++it) {
        QuerySolution* soln = it->get();
        ASSERT_EQUALS(params.indexFiltersApplied, soln->indexFilterApplied);
    }
}