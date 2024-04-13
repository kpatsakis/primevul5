TEST_F(QueryPlannerTest, NoMutationsForCollscan) {
    params.options = QueryPlannerParams::KEEP_MUTATIONS;
    runQuery(fromjson(""));
    assertSolutionExists("{cscan: {dir: 1}}");
}