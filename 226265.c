TEST_F(QueryPlannerTest, ContainedOrPathLevelMultikeyCombineTrailingFields) {
    MultikeyPaths multikeyPaths{{0U}, {}};
    addIndex(BSON("b" << 1 << "a" << 1), multikeyPaths);
    addIndex(BSON("c" << 1));

    runQuery(
        fromjson("{$and: [{a: {$gte: 0}}, {$or: [{$and: [{a: {$lte: 10}}, {b: 6}]}, {c: 7}]}]}"));
    assertNumSolutions(2);
    assertSolutionExists(
        "{fetch: {filter: {a: {$gte: 0}}, node: {or: {nodes: ["
        "{ixscan: {pattern: {b: 1, a: 1}, bounds: {b: [[6, 6, true, true]], a: [[0, 10, true, "
        "true]]}}},"
        "{ixscan: {pattern: {c: 1}, bounds: {c: [[7, 7, true, true]]}}}"
        "]}}}}");
    assertSolutionExists("{cscan: {dir: 1}}}}");
}