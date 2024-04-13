TEST_F(QueryPlannerTest, NegationOrNotIn) {
    addIndex(BSON("a" << 1));
    runQuerySortProj(fromjson("{$or: [{a: 1}, {b: {$nin: [1]}}]}"), BSONObj(), BSONObj());

    assertNumSolutions(1U);
    assertSolutionExists("{cscan: {dir: 1}}");
}