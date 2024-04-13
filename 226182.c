TEST_F(QueryPlannerTest, ShardFilterBasicIndex) {
    params.options = QueryPlannerParams::INCLUDE_SHARD_FILTER;
    params.shardKey = BSON("a" << 1);
    addIndex(BSON("a" << 1));
    addIndex(BSON("b" << 1));

    runQuery(fromjson("{b: 1}"));

    assertNumSolutions(1U);
    assertSolutionExists(
        "{sharding_filter: {node: "
        "{fetch: {node: "
        "{ixscan: {pattern: {b: 1}}}}}}}");
}