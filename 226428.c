TEST_F(QueryPlannerTest, NoFetchNoKeep) {
    params.options = QueryPlannerParams::KEEP_MUTATIONS;
    addIndex(BSON("x" << 1));
    // query, sort, proj
    runQuerySortProj(fromjson("{ x : {$gt: 1}}"), BSONObj(), fromjson("{_id: 0, x: 1}"));

    // cscan is a soln but we override the params that say to include it.
    ASSERT_EQUALS(getNumSolutions(), 1U);
    assertSolutionExists(
        "{proj: {spec: {_id: 0, x: 1}, node: {ixscan: "
        "{filter: null, pattern: {x: 1}}}}}");
}