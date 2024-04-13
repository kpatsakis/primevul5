TEST_F(QueryPlannerTest, LockstepOrEnumerationSanityCheckTwoChildrenTwoIndexesEach) {
    params.options =
        QueryPlannerParams::NO_TABLE_SCAN | QueryPlannerParams::ENUMERATE_OR_CHILDREN_LOCKSTEP;
    addIndex(BSON("a" << 1 << "b" << 1));
    addIndex(BSON("a" << 1 << "c" << 1));

    runQueryAsCommand(
        fromjson("{find: 'testns', filter: {a: 1, $or: [{b: 1, c: 1}, {b: 2, c: 2}]}}"));

    assertNumSolutions(6U);

    assertSolutionExists(
        "{or: {nodes: [{fetch: {filter: {c: {$eq: 1} }, node: {ixscan: {pattern: {a: 1, b: 1}}}}}, "
        "{fetch: {filter: {c: {$eq: 2}}, node: {ixscan: {pattern: {a: 1, b: 1}}}}}]}}");
    assertSolutionExists(
        "{or: {nodes: [{fetch: {filter: {b: {$eq: 1} }, node: {ixscan: {pattern: {a: 1, c: 1}}}}}, "
        "{fetch: {filter: {b: {$eq: 2}}, node: {ixscan: {pattern: {a: 1, c: 1}}}}}]}}");
    assertSolutionExists(
        "{or: {nodes: [{fetch: {filter: {c: {$eq: 1} }, node: {ixscan: {pattern: {a: 1, b: 1}}}}}, "
        "{fetch: {filter: {b: {$eq: 2}}, node: {ixscan: {pattern: {a: 1, c: 1}}}}}]}}");
    assertSolutionExists(
        "{or: {nodes: [{fetch: {filter: {b: {$eq: 1} }, node: {ixscan: {pattern: {a: 1, c: 1}}}}}, "
        "{fetch: {filter: {c: {$eq: 2}}, node: {ixscan: {pattern: {a: 1, b: 1}}}}}]}}");
    assertSolutionExists(
        "{fetch: {filter: {$or: [{b: {$eq: 1}, c: {$eq: 1}}, {b: {$eq: 2}, c: {$eq: 2}}]}, node: "
        "{ixscan: {pattern: {a: 1, b: 1}}}}}}}");
    assertSolutionExists(
        "{fetch: {filter: {$or: [{b: {$eq: 1}, c: {$eq: 1}}, {b: {$eq: 2}, c: {$eq: 2}}]}, node: "
        "{ixscan: {pattern: {a: 1, c: 1}}}}}}}");
}