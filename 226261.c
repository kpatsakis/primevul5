TEST_F(QueryPlannerTest, LockstepOrEnumerationSanityCheckTwoChildrenOneIndexEach) {
    params.options =
        QueryPlannerParams::NO_TABLE_SCAN | QueryPlannerParams::ENUMERATE_OR_CHILDREN_LOCKSTEP;
    addIndex(BSON("a" << 1 << "b" << 1));
    addIndex(BSON("a" << 1 << "c" << 1));

    runQueryAsCommand(fromjson("{find: 'testns', filter: {a: 1, $or: [{b: 1}, {c: 2}]}}"));

    assertNumSolutions(3U);

    assertSolutionExists(
        "{fetch: {filter: null, node: {or: {nodes: [{ixscan: {pattern: {a: 1, b: 1}}}, {ixscan: "
        "{pattern: {a: 1, c: 1}}}]}}}}");
    assertSolutionExists(
        "{fetch: {filter: {$or: [{b: {$eq: 1}}, {c: {$eq: 2}}]}, node: {ixscan: {pattern: {a: 1, "
        "b: 1}}}}}}}");
    assertSolutionExists(
        "{fetch: {filter: {$or: [{b: {$eq: 1}}, {c: {$eq: 2}}]}, node: {ixscan: {pattern: {a: 1, "
        "c: 1}}}}}}}");
}