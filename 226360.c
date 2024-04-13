TEST_F(QueryPlannerTest, OrInexactWithExact) {
    addIndex(BSON("name" << 1));
    runQuery(fromjson("{$or: [{name: 'thomas'}, {name: /^alexand(er|ra)/}]}"));

    assertNumSolutions(2U);
    assertSolutionExists("{cscan: {dir: 1}}");
    assertSolutionExists(
        "{fetch: {node: {ixscan: {filter:"
        "{$or: [{name: 'thomas'}, {name: /^alexand(er|ra)/}]},"
        "pattern: {name: 1}}}}}");
}