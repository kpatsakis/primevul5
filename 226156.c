TEST_F(QueryPlannerTest, OrElemMatchObject) {
    // true means multikey
    addIndex(BSON("a.b" << 1), true);
    runQuery(
        fromjson("{$or: [{a: {$elemMatch: {b: {$lte: 1}}}},"
                 "{a: {$elemMatch: {b: {$gte: 4}}}}]}"));

    assertNumSolutions(2U);
    assertSolutionExists("{cscan: {dir: 1}}");
    assertSolutionExists(
        "{or: {nodes: ["
        "{fetch: {filter: {a:{$elemMatch:{b:{$gte:4}}}}, node: "
        "{ixscan: {filter: null, pattern: {'a.b': 1}}}}},"
        "{fetch: {filter: {a:{$elemMatch:{b:{$lte:1}}}}, node: "
        "{ixscan: {filter: null, pattern: {'a.b': 1}}}}}]}}");
}