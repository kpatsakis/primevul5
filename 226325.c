TEST_F(QueryPlannerTest, ShardFilterCollScan) {
    params.options = QueryPlannerParams::INCLUDE_SHARD_FILTER;
    params.shardKey = BSON("a" << 1);
    addIndex(BSON("a" << 1));

    runQuery(fromjson("{b: 1}"));

    assertNumSolutions(1U);
    assertSolutionExists(
        "{sharding_filter: {node: "
        "{cscan: {dir: 1}}}}}}}");
}