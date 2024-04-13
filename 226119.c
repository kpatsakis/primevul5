TEST_F(QueryPlannerTest, CoveredOrUniqueIndexLookup) {
    params.options = QueryPlannerParams::NO_TABLE_SCAN;

    addIndex(BSON("a" << 1 << "b" << 1));
    addIndex(BSON("a" << 1),
             false,  // multikey
             false,  // sparse,
             true);  // unique

    runQuerySortProj(fromjson("{a: 1, b: 1}"), BSONObj(), fromjson("{_id: 0, a: 1}"));

    assertNumSolutions(2U);
    assertSolutionExists(
        "{proj: {spec: {_id: 0, a: 1}, node: "
        "{fetch: {filter: {b: 1}, node: {ixscan: {pattern: {a: 1}}}}}}}");
    assertSolutionExists(
        "{proj: {spec: {_id: 0, a: 1}, node: "
        "{ixscan: {filter: null, pattern: {a: 1, b: 1}}}}}");
}