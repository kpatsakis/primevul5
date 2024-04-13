TEST_F(QueryPlannerTest, OrTwoInexactFetch) {
    // true means multikey
    addIndex(BSON("names" << 1), true);
    runQuery(
        fromjson("{$or: [{names: {$elemMatch: {$eq: 'alexandra'}}},"
                 "{names: {$elemMatch: {$eq: 'thomas'}}}]}"));

    assertNumSolutions(2U);
    assertSolutionExists("{cscan: {dir: 1}}");
    assertSolutionExists(
        "{fetch: {filter: "
        "{$or: [{names: {$elemMatch: {$eq: 'alexandra'}}},"
        "{names: {$elemMatch: {$eq: 'thomas'}}}]}, "
        "node: {ixscan: {filter: null, pattern: {names: 1}}}}}");
}