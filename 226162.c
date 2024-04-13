TEST_F(QueryPlannerTest, OrWithExactAndInexact) {
    addIndex(BSON("name" << 1));
    runQuery(fromjson("{name: {$in: ['thomas', /^alexand(er|ra)/]}}"));

    assertNumSolutions(2U);
    assertSolutionExists("{cscan: {dir: 1}}");
    assertSolutionExists(
        "{fetch: {filter: null, node: {ixscan: "
        "{filter: {name: {$in: ['thomas', /^alexand(er|ra)/]}}, "
        "pattern: {name: 1}}}}}");
}