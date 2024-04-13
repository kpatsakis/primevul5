TEST_F(QueryPlannerTest, InCompoundIndexLast) {
    addIndex(fromjson("{a: 1, b: 1}"));
    runQuery(fromjson("{a: 3, b: {$in: [1, 2]}}"));

    assertNumSolutions(2U);
    // TODO: update filter in cscan solution when SERVER-12024 is implemented
    assertSolutionExists("{cscan: {dir: 1, filter: {a: 3, b: {$in: [1, 2]}}}}");
    assertSolutionExists(
        "{fetch: {filter: null, "
        "node: {ixscan: {pattern: {a: 1, b: 1}}}}}");
}