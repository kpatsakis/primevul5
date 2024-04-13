TEST_F(QueryPlannerTest, Mod) {
    addIndex(BSON("a" << 1));

    runQuery(fromjson("{a: {$mod: [2, 0]}}"));

    assertNumSolutions(2U);
    assertSolutionExists("{cscan: {dir: 1, filter: {a: {$mod: [2, 0]}}}}");
    assertSolutionExists(
        "{fetch: {filter: null, node: {ixscan: "
        "{filter: {a: {$mod: [2, 0]}}, pattern: {a: 1}}}}}");
}