TEST_F(QueryPlannerTest, Snapshot) {
    addIndex(BSON("a" << 1));
    runQuerySnapshot(fromjson("{a: {$gt: 0}}"));

    assertNumSolutions(1U);
    assertSolutionExists("{cscan: {filter: {a: {$gt: 0}}, dir: 1}}");
}