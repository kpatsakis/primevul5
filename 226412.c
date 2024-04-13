TEST_F(QueryPlannerTest, LockstepOrEnumerationSanityCheckTwoChildrenDifferentNumSolutions) {
    params.options =
        QueryPlannerParams::NO_TABLE_SCAN | QueryPlannerParams::ENUMERATE_OR_CHILDREN_LOCKSTEP;
    addIndex(BSON("a" << 1 << "b" << 1));
    addIndex(BSON("a" << 1 << "c" << 1));

    runQueryAsCommand(fromjson("{find: 'testns', filter: {a: 1, $or: [{b: 1}, {b: 2, c: 2}]}}"));

    assertNumSolutions(4U);

    assertSolutionExists(
        "{fetch: {filter: null, node: {or: {nodes: [{ixscan: {pattern: {a: 1, b: 1}}}, {fetch: "
        "{filter: {c: {$eq: 2}}, node: {ixscan: {pattern: {a: 1, b: 1}}}}}]}}}}");
    assertSolutionExists(
        "{fetch: {filter: null, node: {or: {nodes: [{ixscan: {pattern: {a: 1, b: 1}}}, {fetch: "
        "{filter: {b: {$eq: 2}}, node: {ixscan: {pattern: {a: 1, c: 1}}}}}]}}}}");
    assertSolutionExists(
        "{fetch: {filter: {$or: [{b: {$eq: 1}}, {b: {$eq: 2}, c: {$eq: 2}}]}, node: {ixscan: "
        "{pattern: {a: 1, b: 1}}}}}}}");
    assertSolutionExists(
        "{fetch: {filter: {$or: [{b: {$eq: 1}}, {b: {$eq: 2}, c: {$eq: 2}}]}, node: {ixscan: "
        "{pattern: {a: 1, c: 1}}}}}}}");
}