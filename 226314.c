TEST_F(QueryPlannerTest, ExistsTrue) {
    addIndex(BSON("x" << 1));

    runQuery(fromjson("{x: {$exists: true}}"));

    assertNumSolutions(2U);
    assertSolutionExists("{cscan: {dir: 1}}");
    assertSolutionExists("{fetch: {node: {ixscan: {pattern: {x: 1}}}}}");
}