TEST_F(QueryPlannerTest, NaturalHint) {
    addIndex(BSON("a" << 1));
    addIndex(BSON("b" << 1));
    runQuerySortHint(BSON("a" << 1), BSON("b" << 1), BSON("$natural" << 1));

    assertNumSolutions(1U);
    assertSolutionExists(
        "{sort: {pattern: {b: 1}, limit: 0, node: {sortKeyGen: {node: "
        "{cscan: {filter: {a: 1}, dir: 1}}}}}}");
}