General:

- [ ] Fixes #issuenumber (if someone opened an issue requesting the update)
- [ ] Supersedes #issuenumber - optional: only if this pull request replaces another
- [ ] Description of the PR
- [ ] If this is your first PR for this package did you include yourself in the
      "docs/Authors.rst" file (it should be sorted lexically by last name)?

The following shows some rough checklists depending on which kind of pull
request this is:

For Bugfixes:

- [ ] Did you add a regression test
- [ ] Did you add a changelog entry in "docs/CHANGES.rst"?


For new functions/classes:

- [ ] Did you include the file and function/class in the c module setup
- [ ] Did you include it in the ``__all__`` of the python module?
- [ ] Did you include a meaningful docstring with Parameters, Returns and
      Examples?
- [ ] Does the docstring contain a ``.. versionadded:: {version}`` directive?
- [ ] Did you include tests to ensure 100% coverage (except for memoryerrors)?
- [ ] Did you add the new function/class to the narrative documentation?
- [ ] Did you add it to the appropriate `Iterable` class as method? If not why?
- [ ] Did you add a changelog entry in "docs/CHANGES.rst"?


For new options in existing functions/classes:

- [ ] Did you explain the new functionality in the existing docstring with
      examples?
- [ ] Does the docstring contain a ``.. versionchanged:: {version}`` directive?
- [ ] Did you include tests to ensure 100% coverage (except for memoryerrors)?
- [ ] Did you also change the appropriate `Iterable` method? If not why?
- [ ] Did you add a changelog entry in "docs/CHANGES.rst"?


For documentation updates:

- [ ] If this is your first PR for this package did you include yourself in the
      "docs/Authors.rst" file (it should be sorted lexically by last name)?
