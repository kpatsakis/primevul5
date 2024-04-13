TEST_F(QueryPlannerTest, ExistsFalseOnUnindexedField) {
    addIndex(BSON("x" << 1));

    runQuery(fromjson("{x: 1, y: {$exists: false}}"));

    assertNumSolutions(2U);
    assertSolutionExists("{cscan: {dir: 1}}");
    assertSolutionExists("{fetch: {node: {ixscan: {pattern: {x: 1}}}}}");
}