int ExpressionRegex::execute(RegexExecutionState* regexState) const {
    invariant(regexState);
    invariant(!regexState->nullish());
    invariant(regexState->pcrePtr);

    int execResult = pcre_exec(regexState->pcrePtr.get(),
                               nullptr,
                               regexState->input->c_str(),
                               regexState->input->size(),
                               regexState->startBytePos,
                               0,  // No need to overwrite the options set during pcre_compile.
                               &(regexState->capturesBuffer.front()),
                               regexState->capturesBuffer.size());
    // The 'execResult' will be -1 if there is no match, 0 < execResult <= (numCaptures + 1)
    // depending on how many capture groups match, negative (other than -1) if there is an error
    // during execution, and zero if capturesBuffer's capacity is not sufficient to hold all the
    // results. The latter scenario should never occur.
    uassert(51156,
            str::stream() << "Error occurred while executing the regular expression in " << _opName
                          << ". Result code: " << execResult,
            execResult == -1 || (execResult > 0 && execResult <= (regexState->numCaptures + 1)));
    return execResult;
}