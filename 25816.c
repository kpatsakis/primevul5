static bool hasSymbolicPart (const File& root, const File& f)
{
    jassert (root == f || f.isAChildOf (root));

    for (auto p = f; p != root; p = p.getParentDirectory())
    {
        if (p.isSymbolicLink())
            return true;
    }

    return false;
}