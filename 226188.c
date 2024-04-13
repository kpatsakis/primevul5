TEST_F(QueryPlannerTest, CompoundIndexWithEqualityPredicatesProvidesSort) {
    params.options = QueryPlannerParams::NO_TABLE_SCAN;
    addIndex(BSON("a" << 1 << "b" << 1));
    runQuerySortProj(fromjson("{a: 1, b: 1}"), fromjson("{b: 1}"), BSONObj());

    assertNumSolutions(1U);
    assertSolutionExists(
        "{fetch: {filter: null, node: {ixscan: {filter: null,"
        "pattern: {a: 1, b: 1}, "
        "bounds: {a:[[1,1,true,true]], b:[[1,1,true,true]]}}}}}");
}