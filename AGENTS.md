<!-- Code development was assisted by big AI. -->

# Agent guide for vrpRouting

These instructions apply throughout this repository. vrpRouting implements vehicle
routing algorithms as a PostgreSQL extension. Read the relevant code,
[CONTRIBUTING.md](CONTRIBUTING.md), and this guide before making changes.

## Coding standards

Use pgRouting's
[contribution conventions](https://github.com/pgRouting/pgrouting/blob/develop/CONTRIBUTING.md),
[EditorConfig](https://github.com/pgRouting/pgrouting/blob/develop/.editorconfig), and
[C/C++ lint configuration](https://github.com/pgRouting/pgrouting/blob/develop/tools/scripts/code_checker.sh)
as the baseline. Apply them through this repository's configuration; local
[.editorconfig](.editorconfig), [CMakeLists.txt](CMakeLists.txt), and
[code_checker.sh](tools/scripts/code_checker.sh) define vrpRouting's specific settings.

- Optimize for readability. Follow nearby naming, include ordering, namespace,
  header guard, and function layout conventions. Keep changes focused on the task.
- Use four spaces for C/C++ indentation and a 120-column C/C++ line limit. Run the
  repository's `cpplint` wrapper with its existing filters.
- Use UTF-8, LF line endings, a final newline, and no trailing whitespace.
  Follow local EditorConfig exceptions: two spaces for SQL, CMake, and HTML;
  three for reStructuredText; tabs for Makefiles.
- Use C11 and C++17, as configured locally. Changing language versions or build
  dependencies is a separate change requiring a concrete reason.
- Keep PostgreSQL-facing C wrappers, C++ drivers, and algorithm implementations
  in their existing layers. Follow existing allocation, cleanup, and error
  reporting patterns; do not let C++ exceptions escape into PostgreSQL C callers.
- Preserve SQL function signatures, result columns, and established behavior
  unless the task explicitly changes them. Update affected SQL, documentation,
  signatures, and tests together when an interface changes.
- Use comments and Doxygen documentation to explain contracts, constraints, and
  non-obvious reasoning. Keep documentation consistent with the implementation.
- Preserve licensing and attribution. Use the existing project license headers
  for new files and retain notices on reused code.

## Repository map

| Area | Location |
| --- | --- |
| PostgreSQL entry points, C++ drivers, algorithms | `src/` |
| Shared types, driver declarations, algorithm headers | `include/` |
| SQL API and extension packaging | `sql/` |
| API signature records | `sql/sigs/` |
| pgTAP regression tests | `pgtap/` |
| Executable documentation queries and expected results | `docqueries/` |
| User documentation | `doc/` |
| Build, lint, and test tooling | `cmake/`, `tools/`, `.github/workflows/` |

## Build and validation

Use an out-of-source build. Follow [README.md](README.md) and the relevant CI
workflow for dependency setup. With dependencies available, a typical build from
the repository root is:

```sh
mkdir -p build
cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake --build . --parallel 4
```

Run subsequent checks from the repository root:

| Change | Relevant validation |
| --- | --- |
| C/C++ | `bash tools/scripts/code_checker.sh` and a build |
| SQL or algorithm behavior | Regression coverage in `pgtap/`; run `bash tools/testers/pg_prove_tests.sh -d ___vrp___test___` |
| Documentation queries | `tools/testers/doc_queries_generator.pl`, following `.github/workflows/queries.yml` |
| SQL signatures | `bash tools/scripts/test_signatures.sh` and the signature generation check in `.github/workflows/queries.yml` |
| Shell scripts | `bash tools/scripts/test_shell.sh` |
| License headers | `bash tools/scripts/test_license.sh` |
| Any text change | `git diff --check` and review of new, untracked files |

The lint wrapper downloads `cpplint` on first use if it is absent. Database tests
need a matching installed extension, PostgreSQL, PostGIS, pgRouting, pgTAP, and
test data; follow `.github/workflows/ubuntu.yml` for setup, including unpacking
`tools/testers/matrix_new_values.tar.gz`. Use a disposable test database. The
documentation query runner drops and recreates its target database.

For behavior changes, add or update regression coverage for the affected behavior
and run the relevant checks. For documentation-only changes, review the text,
links, and whitespace. Report which checks ran and any that could not run; never
claim a passing build or test without observing it.

## Generative AI and contribution text

This policy adapts the human authorship and review approach in
[Valhalla's CLAUDE.md](https://github.com/valhalla/valhalla/blob/master/CLAUDE.md#pull-requests-and-generative-ai),
with broader disclosure requirements for vrpRouting.

Every repository contribution message you draft or submit must start with this
exact standalone line, before any heading or other text:

```text
This contribution had a little help from Big AI.
```

This includes pull request descriptions, issue bodies, comments, inline review
comments, review summaries, replies, discussions, commit messages, and similar
contribution text. Include the line in local drafts too. Keep it verbatim and
visible; do not translate it, paraphrase it, or hide it in a comment.

Write the remaining text in plain, concise English. Explain the problem, the
change, why it works, and relevant validation. Avoid filler and claims that have
not been checked.

Humans own the final contribution. Before submission, remind the author to review
the draft and rewrite it in their own words, retaining the disclosure for
AI-assisted material. They must also review generated code for correctness and
readability, and explain non-obvious decisions in review comments. Those comments
must use the same opening line.

## Required file header

Before any commit, every file added, modified, or renamed in that commit must have
a comment at the top containing this exact sentence:

```text
Code development was assisted by big AI.
```

This applies to source files, headers, SQL, tests, scripts, build files, configuration, and documentation
included in the change. Add the header while preparing the change so it is visible
during local review, even when no commit is requested.

Use the file format's native comment syntax:

| File type | Header comment |
| --- | --- |
| C / C++ | `/* Code development was assisted by big AI. */` |
| SQL / SQL-based `.pg` tests | `-- Code development was assisted by big AI.` |
| Shell / Python / Perl / CMake / YAML | `# Code development was assisted by big AI.` |
| Markdown / HTML | `<!-- Code development was assisted by big AI. -->` |
| reStructuredText | `.. Code development was assisted by big AI.` |

Place the disclosure in the first comment at the top, after a required shebang or
encoding declaration when necessary. Preserve existing copyright, license, and
author notices. Keep one disclosure header per file; do not duplicate an existing
one or stamp unrelated files outside the change.

For a format that cannot contain comments, such as strict JSON or a binary file,
flag the conflict to the maintainer before including it in a commit. Do not break
the format or silently exempt it from the disclosure requirement.

## Local review and delivery

Work on a task branch and preserve the user's existing changes. Inspect the final
diff and new files for scope, required headers, accidental formatting changes,
and generated artifacts. Summarize the change and validation for local review.

Commit, push, or publish repository messages only when explicitly authorized.
A request to edit files or draft contribution text does not authorize publication.
When asked for local review without committing, leave the changes uncommitted and
do not push anything to a remote.
