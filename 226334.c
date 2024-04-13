TEST_F(QueryPlannerTest, ExistsTrueOnUnindexedField) {
    addIndex(BSON("x" << 1));

    runQuery(fromjson("{x: 1, y: {$exists: true}}"));

    assertNumSolutions(2U);
    assertSolutionExists("{cscan: {dir: 1}}");
    assertSolutionExists("{fetch: {node: {ixscan: {pattern: {x: 1}}}}}");
}