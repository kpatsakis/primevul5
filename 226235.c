TEST_F(QueryPlannerTest, ExistsTrueSparseIndex) {
    addIndex(BSON("x" << 1), false, true);

    runQuery(fromjson("{x: {$exists: true}}"));

    assertNumSolutions(2U);
    assertSolutionExists("{cscan: {dir: 1}}");
    assertSolutionExists("{fetch: {node: {ixscan: {pattern: {x: 1}}}}}");
}