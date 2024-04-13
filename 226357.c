TEST_F(QueryPlannerTest, TwoRegexCompoundIndexCovering) {
    addIndex(BSON("a" << 1 << "b" << 1));
    runQuerySortProj(fromjson("{a: /0/, b: /1/}"), BSONObj(), fromjson("{_id: 0, a: 1, b: 1}"));

    ASSERT_EQUALS(getNumSolutions(), 2U);
    assertSolutionExists(
        "{proj: {spec: {_id: 0, a: 1, b: 1}, node: "
        "{cscan: {dir: 1, filter: {$and:[{a:/0/},{b:/1/}]}}}}}");
    assertSolutionExists(
        "{proj: {spec: {_id: 0, a: 1, b: 1}, node: "
        "{ixscan: {filter: {$and:[{a:/0/},{b:/1/}]}, pattern: {a: 1, b: 1}}}}}");
}