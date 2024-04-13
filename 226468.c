TEST_F(QueryPlannerTest, ContainedOrPathLevelMultikeyCompoundDottedFields) {
    MultikeyPaths multikeyPaths{{1U}, {1U}};
    addIndex(BSON("a.c" << 1 << "a.b" << 1), multikeyPaths);
    addIndex(BSON("d" << 1));

    runQuery(fromjson("{$and: [{'a.b': 5}, {$or: [{'a.c': 6}, {d: 7}]}]}"));
    assertNumSolutions(2);
    assertSolutionExists(
        "{fetch: {filter: {'a.b': 5}, node: {or: {nodes: ["
        "{ixscan: {pattern: {'a.c': 1, 'a.b': 1}, bounds: {'a.c': [[6, 6, true, true]], 'a.b': "
        "[[5, 5, true, true]]}}},"
        "{ixscan: {pattern: {d: 1}, bounds: {d: [[7, 7, true, true]]}}}"
        "]}}}}");
    assertSolutionExists("{cscan: {dir: 1}}}}");
}