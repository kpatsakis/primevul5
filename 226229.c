TEST_F(QueryPlannerTest, NonPrefixRegexOrCovering) {
    addIndex(BSON("a" << 1));
    runQuerySortProj(
        fromjson("{$or: [{a: /0/}, {a: /1/}]}"), BSONObj(), fromjson("{_id: 0, a: 1}"));

    ASSERT_EQUALS(getNumSolutions(), 2U);
    assertSolutionExists(
        "{proj: {spec: {_id: 0, a: 1}, node: "
        "{cscan: {dir: 1, filter: {$or: [{a: /0/}, {a: /1/}]}}}}}");
    assertSolutionExists(
        "{proj: {spec: {_id: 0, a: 1}, node: "
        "{ixscan: {filter: {$or: [{a: /0/}, {a: /1/}]}, pattern: {a: 1}}}}}");
}