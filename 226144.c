TEST_F(QueryPlannerTest, ShardFilterNestedProjCovered) {
    params.options = QueryPlannerParams::INCLUDE_SHARD_FILTER;
    params.shardKey = BSON("a" << 1 << "b.c" << 1);
    addIndex(BSON("a" << 1 << "b.c" << 1));

    runQuerySortProj(fromjson("{a: 1}"), BSONObj(), fromjson("{_id: 0, a: 1, 'b.c': 1}"));

    assertNumSolutions(1U);
    assertSolutionExists(
        "{proj: {spec: {_id: 0, a: 1, 'b.c': 1 }, type: 'default', node: "
        "{sharding_filter: {node: "
        "{ixscan: {filter: null, pattern: {a: 1, 'b.c': 1}}}}}}}");
}