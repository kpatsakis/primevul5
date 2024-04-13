TEST_F(QueryPlannerTest, ShardFilterBasicCovered) {
    params.options = QueryPlannerParams::INCLUDE_SHARD_FILTER;
    params.shardKey = BSON("a" << 1);
    addIndex(BSON("a" << 1));

    runQuery(fromjson("{a: 1}"));

    assertNumSolutions(1U);
    assertSolutionExists(
        "{fetch: {node: "
        "{sharding_filter: {node: "
        "{ixscan: {pattern: {a: 1}}}}}}}");
}