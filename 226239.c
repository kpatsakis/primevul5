TEST_F(QueryPlannerTest, CoveredSkipWithIndex) {
    addIndex(fromjson("{a: 1, b: 1}"));

    runQuerySortProjSkipNToReturn(
        fromjson("{a: 5}"), BSONObj(), fromjson("{_id: 0, a: 1, b: 1}"), 8, 0);

    ASSERT_EQUALS(getNumSolutions(), 2U);
    assertSolutionExists(
        "{proj: {spec: {_id: 0, a: 1, b: 1}, node: "
        "{skip: {n: 8, node: {cscan: {dir: 1, filter: {a: 5}}}}}}}");
    assertSolutionExists(
        "{proj: {spec: {_id: 0, a: 1, b: 1}, "
        "node: {skip: {n: 8, node: {ixscan: {filter: null, pattern: {a: 1, b: 1}}}}}}}");
}