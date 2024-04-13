TEST_F(QueryPlannerTest, NoBlockingSortsAllowedTest) {
    params.options = QueryPlannerParams::NO_BLOCKING_SORT;
    runQuerySortProj(BSONObj(), BSON("x" << 1), BSONObj());
    assertNumSolutions(0U);

    addIndex(BSON("x" << 1));

    runQuerySortProj(BSONObj(), BSON("x" << 1), BSONObj());
    assertNumSolutions(1U);
    assertSolutionExists(
        "{fetch: {filter: null, node: {ixscan: "
        "{filter: null, pattern: {x: 1}}}}}");
}