TEST_F(QueryPlannerTest, NegationOr) {
    addIndex(BSON("a" << 1));
    runQuerySortProj(fromjson("{$or: [{a: 1}, {b: {$ne: 1}}]}"), BSONObj(), BSONObj());

    assertNumSolutions(1U);
    assertSolutionExists("{cscan: {dir: 1}}");
}