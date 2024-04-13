TEST_F(QueryPlannerTest, NoTableScanBasic) {
    params.options = QueryPlannerParams::NO_TABLE_SCAN;
    runQuery(BSONObj());
    assertNumSolutions(0U);

    addIndex(BSON("x" << 1));

    runQuery(BSONObj());
    assertNumSolutions(0U);

    runQuery(fromjson("{x: {$gte: 0}}"));
    assertNumSolutions(1U);
    assertSolutionExists(
        "{fetch: {filter: null, node: {ixscan: "
        "{filter: null, pattern: {x: 1}}}}}");
}