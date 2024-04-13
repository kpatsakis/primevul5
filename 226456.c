TEST_F(QueryPlannerTest, BasicCovering) {
    addIndex(BSON("x" << 1));
    // query, sort, proj
    runQuerySortProj(fromjson("{ x : {$gt: 1}}"), BSONObj(), fromjson("{_id: 0, x: 1}"));

    ASSERT_EQUALS(getNumSolutions(), 2U);
    assertSolutionExists(
        "{proj: {spec: {_id: 0, x: 1}, node: {ixscan: "
        "{filter: null, pattern: {x: 1}}}}}");
    assertSolutionExists(
        "{proj: {spec: {_id: 0, x: 1}, node: "
        "{cscan: {dir: 1, filter: {x:{$gt:1}}}}}}");
}