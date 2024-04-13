TEST_F(QueryPlannerTest, PrefixRegexCovering) {
    addIndex(BSON("a" << 1));
    runQuerySortProj(fromjson("{a: /^foo/}"), BSONObj(), fromjson("{_id: 0, a: 1}"));

    ASSERT_EQUALS(getNumSolutions(), 2U);
    assertSolutionExists(
        "{proj: {spec: {_id: 0, a: 1}, node: "
        "{cscan: {dir: 1, filter: {a: /^foo/}}}}}");
    assertSolutionExists(
        "{proj: {spec: {_id: 0, a: 1}, node: "
        "{ixscan: {filter: null, pattern: {a: 1}}}}}");
}