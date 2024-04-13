TEST_F(QueryPlannerTest, ShardFilterNoIndexNotCovered) {
    params.options = QueryPlannerParams::INCLUDE_SHARD_FILTER;
    params.shardKey = BSON("a"
                           << "hashed");
    addIndex(BSON("b" << 1));

    runQuerySortProj(fromjson("{b: 1}"), BSONObj(), fromjson("{_id : 0, a : 1}"));

    assertNumSolutions(1U);
    assertSolutionExists(
        "{proj: {spec: {_id: 0,a: 1}, type: 'simple', node: "
        "{sharding_filter : {node: "
        "{fetch: {node: "
        "{ixscan: {pattern: {b: 1}}}}}}}}}");
}