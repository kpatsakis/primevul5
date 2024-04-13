TEST_F(QueryPlannerTest, ContainedOrPathLevelMultikeyCompoundTrailingFields) {
    MultikeyPaths multikeyPaths{{}, {0U}, {}};
    addIndex(BSON("b" << 1 << "a" << 1 << "c" << 1), multikeyPaths);
    addIndex(BSON("d" << 1));

    runQuery(fromjson("{$and: [{a: 5}, {$or: [{$and: [{b: 6}, {c: 7}]}, {d: 8}]}]}"));
    assertNumSolutions(2);
    assertSolutionExists(
        "{fetch: {filter: {a: 5}, node: {or: {nodes: ["
        "{ixscan: {pattern: {b: 1, a: 1, c: 1}, bounds: {b: [[6, 6, true, true]], a: [[5, 5, true, "
        "true]], c: [[7, 7, true, true]]}}},"
        "{ixscan: {pattern: {d: 1}, bounds: {d: [[8, 8, true, true]]}}}"
        "]}}}}");
    assertSolutionExists("{cscan: {dir: 1}}}}");
}