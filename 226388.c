TEST_F(QueryPlannerTest, IdCovering) {
    runQuerySortProj(fromjson("{_id: {$gt: 10}}"), BSONObj(), fromjson("{_id: 1}"));

    ASSERT_EQUALS(getNumSolutions(), 2U);
    assertSolutionExists(
        "{proj: {spec: {_id: 1}, node: "
        "{cscan: {dir: 1, filter: {_id: {$gt: 10}}}}}}");
    assertSolutionExists(
        "{proj: {spec: {_id: 1}, node: {ixscan: "
        "{filter: null, pattern: {_id: 1}}}}}");
}