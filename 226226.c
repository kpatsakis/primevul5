TEST_F(QueryPlannerTest, NormalOrEnumerationDoesNotPrioritizeLockstepIteration) {
    params.options = QueryPlannerParams::NO_TABLE_SCAN;
    ASSERT_EQ(internalQueryEnumerationMaxOrSolutions.load(), 10);
    addIndex(BSON("a" << 1 << "b" << 1));
    addIndex(BSON("a" << 1 << "c" << 1));
    addIndex(BSON("a" << 1 << "d" << 1));

    // For this query and the above indexes, each clause of the $or has three options to choose
    // from, for a total of 3 * 3 * 3 = 27 possible enumerations for just that $or sub-branch.
    runQueryAsCommand(
        fromjson("{find: 'testns', filter: {a: 1, $or: [{b: 1, c: 1, d: 1}, {b: 2, c: 2, d: 2}, "
                 "{b: 3, c: 3, d: 3}]}}"));

    // The $or enumeration is limited to 10, and then we have three plans where just the {a: 1}
    // predicate is indexed.
    assertNumSolutions(13U);

    assertSolutionExists(
        "{or: {nodes: ["
        "{fetch: {filter: {c: {$eq: 1}, d: {$eq: 1}}, node: {ixscan: {pattern: {a: 1, b: 1}}}}}, "
        "{fetch: {filter: {c: {$eq: 2}, d: {$eq: 2}}, node: {ixscan: {pattern: {a: 1, b: 1}}}}}, "
        "{fetch: {filter: {c: {$eq: 3}, d: {$eq: 3}}, node: {ixscan: {pattern: {a: 1, b: 1}}}}} "
        "]}}");
    // Because we did not set the 'ENUMERATE_OR_CHILDREN_LOCKSTEP' flag, we don't expect this
    // solution to be generated. This is in contrast to the next test case.
    ASSERT_THROWS(
        assertSolutionExists(
            "{or: {nodes: ["
            "{fetch: {filter: {b: {$eq: 1}, c: {$eq: 1}}, node: {ixscan: {pattern: {a: 1, d: "
            "1}}}}}, "
            "{fetch: {filter: {b: {$eq: 2}, c: {$eq: 2}}, node: {ixscan: {pattern: {a: 1, d: "
            "1}}}}}, "
            "{fetch: {filter: {b: {$eq: 3}, c: {$eq: 3}}, node: {ixscan: {pattern: {a: 1, d: "
            "1}}}}} "
            "]}}"),
        unittest::TestAssertionFailureException);

    // We still expect to generate the solutions which don't index the $or.
    assertSolutionExists(
        "{fetch: {filter: {$or: ["
        "{b: {$eq: 1}, c: {$eq: 1}, d: {$eq: 1}}, "
        "{b: {$eq: 2}, c: {$eq: 2}, d: {$eq: 2}}, "
        "{b: {$eq: 3}, c: {$eq: 3}, d: {$eq: 3}} "
        "]}, node: {ixscan: {pattern: {a: 1, b: 1}}}}}}");
}