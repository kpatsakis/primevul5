TEST_F(QueryPlannerTest, TwoRegexSameFieldCovering) {
    addIndex(BSON("a" << 1));
    runQuerySortProj(
        fromjson("{$and: [{a: /0/}, {a: /1/}]}"), BSONObj(), fromjson("{_id: 0, a: 1}"));

    ASSERT_EQUALS(getNumSolutions(), 2U);
    assertSolutionExists(
        "{proj: {spec: {_id: 0, a: 1}, node: "
        "{cscan: {dir: 1, filter: {$and:[{a:/0/},{a:/1/}]}}}}}");
    assertSolutionExists(
        "{proj: {spec: {_id: 0, a: 1}, node: "
        "{ixscan: {filter: {$and:[{a:/0/},{a:/1/}]}, pattern: {a: 1}}}}}");
}