TEST_F(QueryPlannerTest, PlansForMultipleIndexesOnTheSameKeyPatternAreGenerated) {
    CollatorInterfaceMock reverseCollator(CollatorInterfaceMock::MockType::kReverseString);
    CollatorInterfaceMock equalCollator(CollatorInterfaceMock::MockType::kAlwaysEqual);
    addIndex(BSON("a" << 1), &reverseCollator, "reverse"_sd);
    addIndex(BSON("a" << 1), &equalCollator, "forward"_sd);

    runQuery(BSON("a" << 1));

    assertNumSolutions(3U);
    assertSolutionExists("{fetch: {node: {ixscan: {name: 'reverse'}}}}");
    assertSolutionExists("{fetch: {node: {ixscan: {name: 'forward'}}}}");
    assertSolutionExists("{cscan: {dir: 1}}}}");
}