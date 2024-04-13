TEST_F(QueryPlannerTest, ContainedOrMultikeyCompoundFields) {
    const bool multikey = true;
    addIndex(BSON("b" << 1 << "a" << 1), multikey);
    addIndex(BSON("c" << 1));

    runQuery(fromjson("{$and: [{a: 5}, {$or: [{b: 6}, {c: 7}]}]}"));
    assertNumSolutions(2);
    assertSolutionExists(
        "{fetch: {filter: {a: 5}, node: {or: {nodes: ["
        "{ixscan: {pattern: {b: 1, a: 1}, bounds: {b: [[6, 6, true, true]], a: [[5, 5, true, "
        "true]]}}},"
        "{ixscan: {pattern: {c: 1}, bounds: {c: [[7, 7, true, true]]}}}"
        "]}}}}");
    assertSolutionExists("{cscan: {dir: 1}}}}");
}