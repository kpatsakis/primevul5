TEST_F(QueryPlannerTest, ExistsTrueSparseIndexOnOtherField) {
    addIndex(BSON("x" << 1), false, true);

    runQuery(fromjson("{x: 1, y: {$exists: true}}"));

    assertNumSolutions(2U);
    assertSolutionExists("{cscan: {dir: 1}}");
    assertSolutionExists("{fetch: {node: {ixscan: {pattern: {x: 1}}}}}");
}