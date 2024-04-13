TEST_F(QueryPlannerTest, ContainedOrPathLevelMultikeyCannotCompoundTrailingFields) {
    MultikeyPaths multikeyPaths{{}, {0U}, {0U}};
    addIndex(BSON("d" << 1 << "a.b" << 1 << "a.c" << 1), multikeyPaths);
    addIndex(BSON("e" << 1));

    runQuery(fromjson("{$and: [{'a.b': 5}, {$or: [{$and: [{'a.c': 6}, {d: 7}]}, {e: 8}]}]}"));
    assertNumSolutions(2);
    // When we have path-level multikey info, we ensure that predicates are assigned in order of
    // index position.
    assertSolutionExists(
        "{fetch: {filter: {'a.b': 5}, node: {or: {nodes: ["
        "{fetch: {filter: {'a.c': 6}, node: {ixscan: {pattern: {d: 1, 'a.b': 1, 'a.c': 1}, bounds: "
        "{d: [[7, 7, true, true]], 'a.b': [[5, 5, true, true]], 'a.c': [['MinKey', 'MaxKey', true, "
        "true]]}}}}},"
        "{ixscan: {pattern: {e: 1}, bounds: {e: [[8, 8, true, true]]}}}"
        "]}}}}");
    assertSolutionExists("{cscan: {dir: 1}}}}");
}