TEST_F(QueryPlannerTest, NonPrefixRegexAndCovering) {
    addIndex(BSON("a" << 1 << "b" << 1));
    runQuerySortProj(fromjson("{a: /foo/, b: 2}"), BSONObj(), fromjson("{_id: 0, a: 1, b: 1}"));

    ASSERT_EQUALS(getNumSolutions(), 2U);
    assertSolutionExists(
        "{proj: {spec: {_id: 0, a: 1, b: 1}, node: "
        "{cscan: {dir: 1, filter: {$and: [{b: 2}, {a: /foo/}]}}}}}");
    assertSolutionExists(
        "{proj: {spec: {_id: 0, a: 1, b: 1}, node: "
        "{ixscan: {filter: {a: /foo/}, pattern: {a: 1, b: 1}}}}}");
}