TEST_F(QueryPlannerTest, ShardFilterCompoundProjCovered) {
    params.options = QueryPlannerParams::INCLUDE_SHARD_FILTER;
    params.shardKey = BSON("a" << 1 << "b" << 1);
    addIndex(BSON("a" << 1 << "b" << 1));

    runQuerySortProj(fromjson("{a: 1}"), BSONObj(), fromjson("{_id: 0, a: 1, b: 1}"));

    assertNumSolutions(1U);
    assertSolutionExists(
        "{proj: {spec: {_id: 0, a: 1, b: 1 }, type: 'coveredIndex', node: "
        "{sharding_filter: {node: "
        "{ixscan: {pattern: {a: 1, b: 1}}}}}}}");
}