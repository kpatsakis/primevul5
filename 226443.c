TEST_F(QueryPlannerTest, ContainedOrPushdownIndexedExpr) {
    addIndex(BSON("a" << 1 << "b" << 1));

    runQuery(
        fromjson("{$expr: {$and: [{$eq: ['$d', 'd']}, {$eq: ['$a', 'a']}]},"
                 "$or: [{b: 'b'}, {b: 'c'}]}"));
    assertNumSolutions(3);
    // When we have path-level multikey info, we ensure that predicates are assigned in order of
    // index position.
    assertSolutionExists(
        "{fetch: {node: {or: {nodes: ["
        "{ixscan: {pattern: {a: 1, b: 1}, filter: null, bounds: {a: [['a', 'a', true, true]], b: "
        "[['b', 'b', true, true]]}}},"
        "{ixscan: {pattern: {a: 1, b: 1}, filter: null, bounds: {a: [['a', 'a', true, true]], b: "
        "[['c', 'c', true, true]]}}}]}}}}");
    assertSolutionExists(
        "{fetch: {node: {ixscan: {pattern: {a: 1, b: 1}, filter: null,"
        "bounds: {a: [['a', 'a', true, true]], b: [['MinKey', 'MaxKey', true, true]]}}}}}");
    assertSolutionExists("{cscan: {dir: 1}}}}");
}