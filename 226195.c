TEST_F(QueryPlannerTest, ContainedOrMultikeyCannotCompoundTrailingFields) {
    const bool multikey = true;
    addIndex(BSON("d" << 1 << "a.b" << 1 << "a.c" << 1), multikey);
    addIndex(BSON("e" << 1));

    runQuery(fromjson("{$and: [{'a.b': 5}, {$or: [{$and: [{'a.c': 6}, {d: 7}]}, {e: 8}]}]}"));
    assertNumSolutions(2);
    std::vector<std::string> alternates;
    alternates.push_back(
        "{fetch: {filter: {'a.b': 5}, node: {or: {nodes: ["
        "{ixscan: {pattern: {d: 1, 'a.b': 1, 'a.c': 1}, bounds: {d: [[7, 7, true, true]], 'a.b': "
        "[['MinKey', 'MaxKey', true, true]], 'a.c': [[6, 6, true, true]]}}},"
        "{ixscan: {pattern: {e: 1}, bounds: {e: [[8, 8, true, true]]}}}"
        "]}}}}");
    alternates.push_back(
        "{fetch: {filter: {'a.b': 5}, node: {or: {nodes: ["
        "{ixscan: {pattern: {d: 1, 'a.b': 1, 'a.c': 1}, bounds: {d: [[7, 7, true, true]], 'a.b': "
        "[[5, 5, true, true]], 'a.c': [['MinKey', 'MaxKey', true, true]]}}},"
        "{ixscan: {pattern: {e: 1}, bounds: {e: [[8, 8, true, true]]}}}"
        "]}}}}");
    assertHasOneSolutionOf(alternates);
    assertSolutionExists("{cscan: {dir: 1}}}}");
}