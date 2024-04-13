TEST_F(QueryPlannerTest, ExplodeRootedOrForSortWorksWithShardingFilter) {
    params.options = QueryPlannerParams::NO_TABLE_SCAN;
    params.options |= QueryPlannerParams::INCLUDE_SHARD_FILTER;
    params.shardKey = BSON("c" << 1);

    addIndex(BSON("a" << 1 << "b" << 1));
    runQuerySortProj(fromjson("{$or: [{a: 1}, {a: 3}]}"), fromjson("{b: 1}"), BSONObj());

    assertNumSolutions(1U);
    assertSolutionExists(
        "{sharding_filter: {node: {fetch: {filter: null, node: {mergeSort: {nodes: ["
        "{ixscan:  {pattern: {a:1,b:1}, filter: null, bounds: {a: [[1,1,true,true]], b: "
        "[['MinKey','MaxKey',true,true]]}}},"
        "{ixscan:  {pattern: {a:1,b:1}, filter: null, bounds: {a: [[3,3,true,true]], b: "
        "[['MinKey','MaxKey',true,true]]}}}]}}}}}}");
}