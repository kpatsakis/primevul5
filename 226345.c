TEST_F(QueryPlannerTest, InBasic) {
    addIndex(fromjson("{a: 1}"));
    runQuery(fromjson("{a: {$in: [1, 2]}}"));

    assertNumSolutions(2U);
    assertSolutionExists("{cscan: {dir: 1, filter: {a: {$in: [1, 2]}}}}");
    assertSolutionExists(
        "{fetch: {filter: null, "
        "node: {ixscan: {pattern: {a: 1}}}}}");
}