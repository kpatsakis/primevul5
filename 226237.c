TEST_F(QueryPlannerTest, ContainedOrCombineLeadingFieldsMoveToAnd) {
    addIndex(BSON("a" << 1 << "b" << 1));
    addIndex(BSON("c" << 1 << "a" << 1));

    runQuery(
        fromjson("{$and: [{a: {$gte: 0}}, {$or: [{$and: [{a: {$lte: 10}}, {b: 6}]}, {c: 7}]}]}"));
    assertNumSolutions(3);
    assertSolutionExists(
        "{fetch: {filter: null, node: {or: {nodes: ["
        "{ixscan: {pattern: {a: 1, b: 1}, bounds: {a: [[0, 10, true, true]], b: [[6, 6, true, "
        "true]]}}},"
        "{ixscan: {pattern: {c: 1, a: 1}, bounds: {c: [[7, 7, true, true]], a: [[0, Infinity, "
        "true, true]]}}}"
        "]}}}}");
    assertSolutionExists(
        "{fetch: {filter: {$or: [{$and: [{a: {$lte: 10}}, {b: 6}]}, {c: 7}]}, node: "
        "{ixscan: {pattern: {a: 1, b: 1}, bounds: {a: [[0, Infinity, true, true]], b: [['MinKey', "
        "'MaxKey', true, true]]}}}"
        "}}");
    assertSolutionExists("{cscan: {dir: 1}}}}");
}