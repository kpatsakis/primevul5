TEST_F(QueryPlannerTest, LockstepOrEnumerationDoesPrioritizeLockstepIterationMixedChildren) {
    params.options =
        QueryPlannerParams::NO_TABLE_SCAN | QueryPlannerParams::ENUMERATE_OR_CHILDREN_LOCKSTEP;
    ASSERT_EQ(internalQueryEnumerationMaxOrSolutions.load(), 10);
    addIndex(BSON("a" << 1 << "b" << 1));
    addIndex(BSON("a" << 1 << "c" << 1));
    addIndex(BSON("a" << 1 << "d" << 1));
    addIndex(BSON("a" << 1 << "e" << 1));

    // For this query and the above indexes, each clause of the $or has a varying number options to
    // choose from, for a total of 2 * 3 * 4 * 2 = 48 possible enumerations for just that $or
    // sub-branch.
    runQueryAsCommand(
        fromjson("{find: 'testns', filter: {"
                 " a: 1,"
                 " $or: ["
                 " {b: 2.1, c: 2.1},"
                 " {b: 3, c: 3, d: 3},"
                 " {b: 4, c: 4, d: 4, e: 4},"
                 " {b: 2.2, c: 2.2}"
                 "]}}"));

    // The $or enumeration is limited to 10, and then we have four plans where just the {a: 1}
    // predicate is indexed.
    assertNumSolutions(14U);

    // Lockstep enumerations. Definitely expected.
    assertSolutionExists(
        "{or: {nodes: ["
        "{fetch: {filter: {c: {$eq: 2.1}}, node: {ixscan: {pattern: {a: 1, b: 1}}}}}, "
        "{fetch: {filter: {c: {$eq: 3}, d: {$eq: 3}}, node: {ixscan: {pattern: {a: 1, b: 1}}}}}, "
        "{fetch: {filter: {c: {$eq: 4}, d: {$eq: 4}, e: {$eq: 4}},"
        " node: {ixscan: {pattern: {a: 1, b: 1}}}}}, "
        "{fetch: {filter: {c: {$eq: 2.2}}, node: {ixscan: {pattern: {a: 1, b: 1}}}}}"
        "]}}");
    assertSolutionExists(
        "{or: {nodes: ["
        "{fetch: {filter: {b: {$eq: 2.1}}, node: {ixscan: {pattern: {a: 1, c: 1}}}}}, "
        "{fetch: {filter: {b: {$eq: 3}, d: {$eq: 3}}, node: {ixscan: {pattern: {a: 1, c: 1}}}}}, "
        "{fetch: {filter: {b: {$eq: 4}, d: {$eq: 4}, e: {$eq: 4}},"
        " node: {ixscan: {pattern: {a: 1, c: 1}}}}}, "
        "{fetch: {filter: {b: {$eq: 2.2}}, node: {ixscan: {pattern: {a: 1, c: 1}}}}}"
        "]}}");
    // Everyone advances one more time, no longer lock step.
    assertSolutionExists(
        "{or: {nodes: ["
        "{fetch: {filter: {c: {$eq: 2.1}}, node: {ixscan: {pattern: {a: 1, b: 1}}}}}, "
        "{fetch: {filter: {b: {$eq: 3}, c: {$eq: 3}}, node: {ixscan: {pattern: {a: 1, d: 1}}}}}, "
        "{fetch: {filter: {b: {$eq: 4}, c: {$eq: 4}, e: {$eq: 4}},"
        " node: {ixscan: {pattern: {a: 1, d: 1}}}}}, "
        "{fetch: {filter: {c: {$eq: 2.2}}, node: {ixscan: {pattern: {a: 1, b: 1}}}}}"
        "]}}");
    // Normal enumeration. Here we observe an interesting phenomena. Before we get into plan
    // enumeration, the query is parsed and "normalized". This process involves putting the query in
    // a canonical order, in part so that similar queries can be recognized as such for caching. In
    // this case, it orders the $or children by their respective number of children. So our original
    // query will be enumerated as if it were typed in this order:
    // {a: 1,
    //  $or: [
    //    {b: 2.1, c: 2.1},
    //    {b: 2.2, c: 2.2},
    //    {b: 3, c: 3, d: 3},
    //    {b: 4, c: 4, d: 4, e: 4}
    //  ]
    // }
    // Here are the exact plans:
    assertSolutionExists(
        "{or: {nodes: ["
        "{fetch: {filter: {b: {$eq: 2.1}}, node: {ixscan: {pattern: {a: 1, c: 1}}}}}, "
        "{fetch: {filter: {c: {$eq: 2.2}}, node: {ixscan: {pattern: {a: 1, b: 1}}}}}, "
        "{fetch: {filter: {b: {$eq: 3}, c: {$eq: 3}}, node: {ixscan: {pattern: {a: 1, d: 1}}}}}, "
        "{fetch: {filter: {b: {$eq: 4}, c: {$eq: 4}, e: {$eq: 4}},"
        " node: {ixscan: {pattern: {a: 1, d: 1}}}}}"
        "]}}");
    assertSolutionExists(
        "{or: {nodes: ["
        "{fetch: {filter: {c: {$eq: 2.1}}, node: {ixscan: {pattern: {a: 1, b: 1}}}}}, "
        "{fetch: {filter: {b: {$eq: 2.2}}, node: {ixscan: {pattern: {a: 1, c: 1}}}}}, "
        "{fetch: {filter: {b: {$eq: 3}, c: {$eq: 3}}, node: {ixscan: {pattern: {a: 1, d: 1}}}}}, "
        "{fetch: {filter: {b: {$eq: 4}, c: {$eq: 4}, e: {$eq: 4}},"
        " node: {ixscan: {pattern: {a: 1, d: 1}}}}}"
        "]}}");
    assertSolutionExists(
        "{or: {nodes: ["
        "{fetch: {filter: {b: {$eq: 2.1}}, node: {ixscan: {pattern: {a: 1, c: 1}}}}}, "
        "{fetch: {filter: {b: {$eq: 2.2}}, node: {ixscan: {pattern: {a: 1, c: 1}}}}}, "
        "{fetch: {filter: {b: {$eq: 3}, c: {$eq: 3}}, node: {ixscan: {pattern: {a: 1, d: 1}}}}}, "
        "{fetch: {filter: {b: {$eq: 4}, c: {$eq: 4}, e: {$eq: 4}},"
        " node: {ixscan: {pattern: {a: 1, d: 1}}}}}"
        "]}}");
    assertSolutionExists(
        "{or: {nodes: ["
        "{fetch: {filter: {c: {$eq: 2.1}}, node: {ixscan: {pattern: {a: 1, b: 1}}}}}, "
        "{fetch: {filter: {c: {$eq: 2.2}}, node: {ixscan: {pattern: {a: 1, b: 1}}}}}, "
        "{fetch: {filter: {c: {$eq: 3}, d: {$eq: 3}}, node: {ixscan: {pattern: {a: 1, b: 1}}}}}, "
        "{fetch: {filter: {b: {$eq: 4}, c: {$eq: 4}, d: {$eq: 4}},"
        " node: {ixscan: {pattern: {a: 1, e: 1}}}}}"
        "]}}");
    assertSolutionExists(
        "{or: {nodes: ["
        "{fetch: {filter: {b: {$eq: 2.1}}, node: {ixscan: {pattern: {a: 1, c: 1}}}}}, "
        "{fetch: {filter: {c: {$eq: 2.2}}, node: {ixscan: {pattern: {a: 1, b: 1}}}}}, "
        "{fetch: {filter: {c: {$eq: 3}, d: {$eq: 3}}, node: {ixscan: {pattern: {a: 1, b: 1}}}}}, "
        "{fetch: {filter: {b: {$eq: 4}, c: {$eq: 4}, d: {$eq: 4}},"
        " node: {ixscan: {pattern: {a: 1, e: 1}}}}}"
        "]}}");
    assertSolutionExists(
        "{or: {nodes: ["
        "{fetch: {filter: {c: {$eq: 2.1}}, node: {ixscan: {pattern: {a: 1, b: 1}}}}}, "
        "{fetch: {filter: {b: {$eq: 2.2}}, node: {ixscan: {pattern: {a: 1, c: 1}}}}}, "
        "{fetch: {filter: {c: {$eq: 3}, d: {$eq: 3}}, node: {ixscan: {pattern: {a: 1, b: 1}}}}}, "
        "{fetch: {filter: {b: {$eq: 4}, c: {$eq: 4}, d: {$eq: 4}},"
        " node: {ixscan: {pattern: {a: 1, e: 1}}}}}"
        "]}}");
    assertSolutionExists(
        "{or: {nodes: ["
        "{fetch: {filter: {b: {$eq: 2.1}}, node: {ixscan: {pattern: {a: 1, c: 1}}}}}, "
        "{fetch: {filter: {b: {$eq: 2.2}}, node: {ixscan: {pattern: {a: 1, c: 1}}}}}, "
        "{fetch: {filter: {c: {$eq: 3}, d: {$eq: 3}}, node: {ixscan: {pattern: {a: 1, b: 1}}}}}, "
        "{fetch: {filter: {b: {$eq: 4}, c: {$eq: 4}, d: {$eq: 4}},"
        " node: {ixscan: {pattern: {a: 1, e: 1}}}}}"
        "]}}");

    // Now to the solutions which don't index the $or.
    assertSolutionExists(
        "{fetch: {filter: {$or: ["
        "{b: {$eq: 2.1}, c: {$eq: 2.1}}, "
        "{b: {$eq: 2.2}, c: {$eq: 2.2}}, "
        "{b: {$eq: 3}, c: {$eq: 3}, d: {$eq: 3}}, "
        "{b: {$eq: 4}, c: {$eq: 4}, d: {$eq: 4}, e: {$eq: 4}} "
        "]}, node: {ixscan: {pattern: {a: 1, b: 1}}}}}}");
    assertSolutionExists(
        "{fetch: {filter: {$or: ["
        "{b: {$eq: 2.1}, c: {$eq: 2.1}}, "
        "{b: {$eq: 2.2}, c: {$eq: 2.2}}, "
        "{b: {$eq: 3}, c: {$eq: 3}, d: {$eq: 3}}, "
        "{b: {$eq: 4}, c: {$eq: 4}, d: {$eq: 4}, e: {$eq: 4}} "
        "]}, node: {ixscan: {pattern: {a: 1, c: 1}}}}}}");
    assertSolutionExists(
        "{fetch: {filter: {$or: ["
        "{b: {$eq: 2.1}, c: {$eq: 2.1}}, "
        "{b: {$eq: 2.2}, c: {$eq: 2.2}}, "
        "{b: {$eq: 3}, c: {$eq: 3}, d: {$eq: 3}}, "
        "{b: {$eq: 4}, c: {$eq: 4}, d: {$eq: 4}, e: {$eq: 4}} "
        "]}, node: {ixscan: {pattern: {a: 1, d: 1}}}}}}");
    assertSolutionExists(
        "{fetch: {filter: {$or: ["
        "{b: {$eq: 2.1}, c: {$eq: 2.1}}, "
        "{b: {$eq: 2.2}, c: {$eq: 2.2}}, "
        "{b: {$eq: 3}, c: {$eq: 3}, d: {$eq: 3}}, "
        "{b: {$eq: 4}, c: {$eq: 4}, d: {$eq: 4}, e: {$eq: 4}} "
        "]}, node: {ixscan: {pattern: {a: 1, e: 1}}}}}}");
}