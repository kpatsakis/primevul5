TEST_F(QueryPlannerTest, ShardFilterHashProjNotCovered) {
    params.options = QueryPlannerParams::INCLUDE_SHARD_FILTER;
    params.shardKey = BSON("a"
                           << "hashed");
    addIndex(BSON("a"
                  << "hashed"));

    runQuerySortProj(fromjson("{a: 1}"), BSONObj(), fromjson("{_id : 0, a : 1}"));

    assertNumSolutions(1U);
    assertSolutionExists(
        "{proj: {spec: {_id: 0,a: 1}, type: 'simple', node: "
        "{sharding_filter : {node: "
        "{fetch: {node: "
        "{ixscan: {pattern: {a: 'hashed'}}}}}}}}}");
}