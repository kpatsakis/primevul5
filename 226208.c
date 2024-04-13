TEST_F(QueryPlannerTest, OrWithExactAndInexact2) {
    addIndex(BSON("name" << 1));
    runQuery(
        fromjson("{$or: [{name: {$in: ['thomas', /^alexand(er|ra)/]}},"
                 "{name: {$exists: false}}]}"));

    assertNumSolutions(2U);
    assertSolutionExists("{cscan: {dir: 1}}");
    assertSolutionExists(
        "{fetch: {filter: {$or: [{name: {$in: ['thomas', /^alexand(er|ra)/]}},"
        "{name: {$exists: false}}]}, "
        "node: {ixscan: {filter: null, pattern: {name: 1}}}}}");
}