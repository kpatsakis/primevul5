TEST_F(QueryPlannerTest, OrAllThreeTightnesses) {
    addIndex(BSON("names" << 1));
    runQuery(
        fromjson("{$or: [{names: 'frank'}, {names: /^al(ice)|(ex)/},"
                 "{names: {$elemMatch: {$eq: 'thomas'}}}]}"));

    assertNumSolutions(2U);
    assertSolutionExists("{cscan: {dir: 1}}");
    assertSolutionExists(
        "{fetch: {filter: "
        "{$or: [{names: 'frank'}, {names: /^al(ice)|(ex)/},"
        "{names: {$elemMatch: {$eq: 'thomas'}}}]}, "
        "node: {ixscan: {filter: null, pattern: {names: 1}}}}}");
}