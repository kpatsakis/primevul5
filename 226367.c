TEST_F(QueryPlannerTest, LockstepOrEnumerationApplysToEachOrInTree) {
    params.options =
        QueryPlannerParams::NO_TABLE_SCAN | QueryPlannerParams::ENUMERATE_OR_CHILDREN_LOCKSTEP;
    ASSERT_EQ(internalQueryEnumerationMaxOrSolutions.load(), 10);
    addIndex(BSON("a" << 1 << "b" << 1));
    addIndex(BSON("a" << 1 << "c" << 1));
    addIndex(BSON("a" << 1 << "x" << 1));
    addIndex(BSON("a" << 1 << "y" << 1));

    // For this query and the above indexes, each clause of the $or has 2 indexes to choose from,
    // for a total of 2 * 2 * 2 * 2 = 16 possible enumerations for just that $or sub-branch.
    runQueryAsCommand(
        fromjson("{find: 'testns', filter: {"
                 " a: 1,"
                 " $or: ["
                 " {b: 2.1, c: 2.1},"
                 " {b: 2.2, c: 2.2},"
                 " {$and: ["
                 "  {unindexed: 'thisPredicateToEnsureNestedOrsAreNotCombined'},"
                 "  {$or: ["
                 "    {x: 3.0, y: 3.0},"
                 "    {x: 3.1, y: 3.1}"
                 "  ]}"
                 " ]}"
                 "]}}"));

    // The $or enumeration is limited to 10, and then we have 4 plans where just the {a: 1}
    // predicate is indexed.
    assertNumSolutions(14U);

    // Both lockstep enumerations should be present.
    assertSolutionExists(
        "{or: {nodes: ["
        " {fetch: {filter: {c: {$eq: 2.1}}, node: {ixscan: {pattern: {a: 1, b: 1}}}}}, "
        " {fetch: {filter: {c: {$eq: 2.2}}, node: {ixscan: {pattern: {a: 1, b: 1}}}}}, "
        " {fetch: {"
        "  filter: {unindexed: {$eq: 'thisPredicateToEnsureNestedOrsAreNotCombined'}},"
        "  node: {"
        "   or: {nodes: ["
        "    {fetch: {filter: {y: {$eq: 3.0}}, node: {ixscan: {pattern: {a: 1, x: 1}}}}},"
        "    {fetch: {filter: {y: {$eq: 3.1}}, node: {ixscan: {pattern: {a: 1, x: 1}}}}}"
        "  ]}}"
        " }}"
        "]}}");
    assertSolutionExists(
        "{or: {nodes: ["
        " {fetch: {filter: {b: {$eq: 2.1}}, node: {ixscan: {pattern: {a: 1, c: 1}}}}}, "
        " {fetch: {filter: {b: {$eq: 2.2}}, node: {ixscan: {pattern: {a: 1, c: 1}}}}}, "
        " {fetch: {"
        "  filter: {unindexed: {$eq: 'thisPredicateToEnsureNestedOrsAreNotCombined'}},"
        "  node: {"
        "   or: {nodes: ["
        "    {fetch: {filter: {x: {$eq: 3.0}}, node: {ixscan: {pattern: {a: 1, y: 1}}}}},"
        "    {fetch: {filter: {x: {$eq: 3.1}}, node: {ixscan: {pattern: {a: 1, y: 1}}}}}"
        "  ]}}"
        " }}"
        "]}}");
}