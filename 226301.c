TEST_F(QueryPlannerTest, ProjNonCovering) {
    addIndex(BSON("x" << 1));
    runQuerySortProj(fromjson("{ x : {$gt: 1}}"), BSONObj(), fromjson("{x: 1}"));

    ASSERT_EQUALS(getNumSolutions(), 2U);
    assertSolutionExists(
        "{proj: {spec: {x: 1}, node: {cscan: "
        "{dir: 1, filter: {x: {$gt: 1}}}}}}");
    assertSolutionExists(
        "{proj: {spec: {x: 1}, node: {fetch: {filter: null, node: "
        "{ixscan: {filter: null, pattern: {x: 1}}}}}}}");
}