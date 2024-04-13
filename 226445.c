TEST_F(QueryPlannerTest, NegationRegexWithIndexablePred) {
    addIndex(BSON("i" << 1));
    runQuery(fromjson("{$and: [{i: {$not: /o/}}, {i: 2}]}"));

    assertNumSolutions(2U);
    assertSolutionExists("{cscan: {dir: 1}}");
    assertSolutionExists(
        "{fetch: {node: {ixscan: {pattern: {i:1}, "
        "bounds: {i: [[2,2,true,true]]}}}}}");
}