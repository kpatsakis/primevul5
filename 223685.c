void ExpressionReplaceBase::_doAddDependencies(DepsTracker* deps) const {
    _input->addDependencies(deps);
    _find->addDependencies(deps);
    _replacement->addDependencies(deps);
}