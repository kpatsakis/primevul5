TEST_F(QueryPlannerTest, HintElemMatch) {
    // true means multikey
    addIndex(fromjson("{'a.b': 1}"), true);
    runQueryHint(fromjson("{'a.b': 1, a: {$elemMatch: {b: 2}}}"), fromjson("{'a.b': 1}"));

    assertNumSolutions(2U);
    assertSolutionExists(
        "{fetch: {filter: {$and: [{a:{$elemMatch:{b:2}}}, {'a.b': 1}]}, "
        "node: {ixscan: {filter: null, pattern: {'a.b': 1}, bounds: "
        "{'a.b': [[2, 2, true, true]]}}}}}");
    assertSolutionExists(
        "{fetch: {filter: {a:{$elemMatch:{b:2}}}, "
        "node: {ixscan: {filter: null, pattern: {'a.b': 1}, bounds: "
        "{'a.b': [[1, 1, true, true]]}}}}}");
}