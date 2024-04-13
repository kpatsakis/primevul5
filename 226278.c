TEST_F(QueryPlannerTest, ExistsFalse) {
    addIndex(BSON("x" << 1));

    runQuery(fromjson("{x: {$exists: false}}"));

    assertNumSolutions(2U);
    assertSolutionExists("{cscan: {dir: 1}}");
    assertSolutionExists("{fetch: {node: {ixscan: {pattern: {x: 1}}}}}");
}