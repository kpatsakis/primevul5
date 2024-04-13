TEST_F(QueryPlannerTest, EmptyQueryWithoutProjectionUsesCollscan) {
    addIndex(BSON("a" << 1));
    runQuery(BSONObj());
    assertNumSolutions(1);
    assertSolutionExists("{cscan: {dir: 1}}}");
}