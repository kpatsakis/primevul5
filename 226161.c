TEST_F(QueryPlannerTest, OrWithExactAndInexact3) {
    addIndex(BSON("a" << 1));
    addIndex(BSON("b" << 1));
    runQuery(
        fromjson("{$or: [{a: {$in: [/z/, /x/]}}, {a: 'w'},"
                 "{b: {$exists: false}}, {b: {$in: ['p']}}]}"));

    assertNumSolutions(2U);
    assertSolutionExists("{cscan: {dir: 1}}");
    assertSolutionExists(
        "{fetch: {filter: null, node: {or: {nodes: ["
        "{ixscan: {filter: {$or:[{a:{$in:[/z/, /x/]}}, {a:'w'}]}, "
        "pattern: {a: 1}}}, "
        "{fetch: {filter: {$or:[{b:{$exists:false}}, {b:{$eq:'p'}}]},"
        "node: {ixscan: {filter: null, pattern: {b: 1}}}}}]}}}}");
}