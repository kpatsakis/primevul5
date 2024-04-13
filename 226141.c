TEST_F(QueryPlannerTest, OrNaturalHint) {
    addIndex(BSON("a" << 1));
    runQueryHint(fromjson("{$or: [{a:1}, {a:3}]}"), fromjson("{$natural:1}"));

    assertNumSolutions(1U);
    assertSolutionExists("{cscan: {dir: 1}}");
}