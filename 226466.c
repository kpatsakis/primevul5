TEST_F(QueryPlannerTest, ContainedOrPredicateIsLeadingFieldMoveToAnd) {
    addIndex(BSON("a" << 1 << "b" << 1 << "c" << 1));
    addIndex(BSON("a" << 1 << "d" << 1));

    runQuery(fromjson("{$and: [{a: 5}, {$or: [{$and: [{b: 6}, {c: 7}]}, {d: 8}]}]}"));
    assertNumSolutions(4);
    assertSolutionExists(
        "{fetch: {filter: null, node: {or: {nodes: ["
        "{ixscan: {pattern: {a: 1, b: 1, c: 1}, bounds: {a: [[5, 5, true, true]], b: [[6, 6, true, "
        "true]], c: [[7, 7, true, true]]}}},"
        "{ixscan: {pattern: {a: 1, d: 1}, bounds: {a: [[5, 5, true, true]], d: [[8, 8, true, "
        "true]]}}}"
        "]}}}}");
    assertSolutionExists(
        "{fetch: {filter: {$or: [{$and: [{b: 6}, {c: 7}]}, {d: 8}]}, node: "
        "{ixscan: {pattern: {a: 1, b: 1, c: 1}, bounds: {a: [[5, 5, true, true]], b: [['MinKey', "
        "'MaxKey', true, true]], c: [['MinKey', 'MaxKey', true, true]]}}}"
        "}}");
    assertSolutionExists(
        "{fetch: {filter: {$or: [{$and: [{b: 6}, {c: 7}]}, {d: 8}]}, node: "
        "{ixscan: {pattern: {a: 1, d: 1}, bounds: {a: [[5, 5, true, true]], d: [['MinKey', "
        "'MaxKey', true, true]]}}}"
        "}}");
    assertSolutionExists("{cscan: {dir: 1}}}}");
}