# Git Branching Strategy

**Project:** Apex Compiler  
**Strategy:** Git Flow (Modified)  
**Last Updated:** November 22, 2025

---

## Branch Structure

### Main Branches

#### `main`
- **Purpose:** Production-ready code
- **Protection:** Protected, requires PR + review
- **Deployments:** Stable releases only (alpha, beta, stable)
- **Version Tags:** All releases tagged here (v1.0.0-alpha.2, etc.)
- **Rules:**
  - ✅ No direct commits
  - ✅ All merges via Pull Request
  - ✅ All tests must pass
  - ✅ Documentation must be updated
  - ✅ CHANGELOG must be updated

#### `develop`
- **Purpose:** Integration branch for features
- **Protection:** Semi-protected, requires tests to pass
- **Contains:** Latest development changes
- **Merges from:** Feature branches
- **Merges to:** `main` (for releases)
- **Rules:**
  - ✅ Direct commits allowed for small fixes
  - ✅ Feature branches merge here first
  - ✅ Must maintain green CI/tests

---

## Supporting Branches

### Feature Branches

**Naming:** `feature/<feature-name>`  
**Branch from:** `develop`  
**Merge to:** `develop`  
**Lifetime:** Temporary (deleted after merge)

**Examples:**
- `feature/arrays-implementation`
- `feature/string-type`
- `feature/type-checker`
- `feature/io-library`

**Workflow:**
```bash
# Create feature branch
git checkout develop
git pull origin develop
git checkout -b feature/arrays-implementation

# Work on feature
git add .
git commit -m "feat: implement array type codegen"

# Keep up-to-date with develop
git fetch origin
git rebase origin/develop

# Push to remote
git push origin feature/arrays-implementation

# Create PR to develop
# After merge, delete branch
git checkout develop
git branch -d feature/arrays-implementation
```

---

### Bug Fix Branches

**Naming:** `bugfix/<bug-name>` or `fix/<bug-name>`  
**Branch from:** `develop` (or `main` for hotfixes)  
**Merge to:** `develop` (or `main` for hotfixes)  
**Lifetime:** Temporary (deleted after merge)

**Examples:**
- `bugfix/parser-infinite-loop`
- `fix/match-alloca-placement`
- `hotfix/segfault-in-codegen`

**Workflow:**
```bash
# For regular bug fixes (from develop)
git checkout develop
git checkout -b bugfix/parser-crash

# For critical hotfixes (from main)
git checkout main
git checkout -b hotfix/critical-segfault

# Fix, commit, and create PR
```

---

### Release Branches

**Naming:** `release/<version>`  
**Branch from:** `develop`  
**Merge to:** `main` AND `develop`  
**Lifetime:** Temporary (deleted after release)

**Examples:**
- `release/1.0.0-alpha.3`
- `release/1.0.0-beta.1`
- `release/1.0.0`

**Workflow:**
```bash
# Create release branch
git checkout develop
git checkout -b release/1.0.0-alpha.3

# Update version numbers, CHANGELOG, docs
git commit -m "chore: prepare release v1.0.0-alpha.3"

# Merge to main
git checkout main
git merge --no-ff release/1.0.0-alpha.3
git tag -a v1.0.0-alpha.3 -m "Release version 1.0.0-alpha.3"
git push origin main --tags

# Merge back to develop
git checkout develop
git merge --no-ff release/1.0.0-alpha.3
git push origin develop

# Delete release branch
git branch -d release/1.0.0-alpha.3
```

---

### Experimental Branches

**Naming:** `experiment/<experiment-name>`  
**Branch from:** `develop`  
**Merge to:** `develop` (if successful) or discarded  
**Lifetime:** Temporary or long-lived

**Examples:**
- `experiment/llvm-optimization-passes`
- `experiment/alternative-syntax`
- `experiment/jit-compilation`

**Purpose:** Test risky or exploratory features without affecting main development.

---

## Commit Message Convention

Following **Conventional Commits** specification:

### Format
```
<type>(<scope>): <subject>

<body>

<footer>
```

### Types
- `feat:` New feature
- `fix:` Bug fix
- `docs:` Documentation changes
- `style:` Code style changes (formatting, no logic change)
- `refactor:` Code refactoring (no feature change)
- `perf:` Performance improvements
- `test:` Adding or updating tests
- `chore:` Build process, tooling, dependencies
- `ci:` CI/CD configuration changes

### Examples
```
feat(parser): add support for array indexing

Implemented array index expression parsing with proper
precedence handling. Supports multi-dimensional arrays.

Closes #42

---

fix(codegen): resolve match alloca placement issue

Match result allocas now created at function entry block
instead of inside loop bodies, fixing LLVM verification errors.

Fixes #38

---

test(arrays): add comprehensive array operation tests

Added 15 tests covering array literals, indexing, bounds
checking, and multi-dimensional arrays.

---

docs(readme): update test suite statistics

Updated README to reflect 43/43 tests passing (100%).
```

---

## Workflow Examples

### Working on a New Feature (Arrays)

```bash
# 1. Start from latest develop
git checkout develop
git pull origin develop

# 2. Create feature branch
git checkout -b feature/arrays-implementation

# 3. Implement incrementally with commits
git add src/apexc/codegen/LLVMCodeGen.cpp
git commit -m "feat(codegen): add array type translation"

git add src/apexc/codegen/LLVMCodeGen.cpp
git commit -m "feat(codegen): implement array literal codegen"

git add tests/array_*.apx
git commit -m "test(arrays): add array operation tests"

# 4. Keep branch updated
git fetch origin
git rebase origin/develop

# 5. Push to remote
git push origin feature/arrays-implementation

# 6. Create Pull Request on GitHub
#    - Title: "feat: Implement array type and operations"
#    - Description: Details, test results, closes #issues
#    - Request review

# 7. After PR approval and merge
git checkout develop
git pull origin develop
git branch -d feature/arrays-implementation
git push origin --delete feature/arrays-implementation
```

---

### Making a Release

```bash
# 1. Ensure develop is stable and tested
git checkout develop
git pull origin develop
# Run full test suite
cd tests && ./run_tests.sh

# 2. Create release branch
git checkout -b release/1.0.0-alpha.3

# 3. Update version information
# - Update version in CMakeLists.txt
# - Update CHANGELOG.md with release notes
# - Update docs/STATUS.md
# - Create RELEASE_NOTES_v1.0.0-alpha.3.md

git add .
git commit -m "chore: prepare release v1.0.0-alpha.3"

# 4. Final testing on release branch
cd tests && ./run_tests.sh

# 5. Merge to main
git checkout main
git pull origin main
git merge --no-ff release/1.0.0-alpha.3 -m "Release v1.0.0-alpha.3"

# 6. Tag the release
git tag -a v1.0.0-alpha.3 -m "Release version 1.0.0-alpha.3

Features:
- Array type implementation
- String operations
- Logical operators

Bug fixes:
- Fixed parser crash on nested arrays
- Resolved type checker issues

Test results: 58/58 tests passing (100%)"

git push origin main --tags

# 7. Merge back to develop
git checkout develop
git merge --no-ff release/1.0.0-alpha.3
git push origin develop

# 8. Clean up
git branch -d release/1.0.0-alpha.3

# 9. Create GitHub Release
# Go to https://github.com/ShubhamPhapale/apex/releases
# Create release from tag with release notes
```

---

### Hotfix for Critical Bug in Production

```bash
# 1. Branch from main
git checkout main
git pull origin main
git checkout -b hotfix/critical-segfault

# 2. Fix the bug
git add src/apexc/codegen/LLVMCodeGen.cpp
git commit -m "fix(codegen): prevent segfault in struct field access"

# 3. Test thoroughly
cd tests && ./run_tests.sh

# 4. Merge to main
git checkout main
git merge --no-ff hotfix/critical-segfault
git tag -a v1.0.0-alpha.2.1 -m "Hotfix: Critical segfault in codegen"
git push origin main --tags

# 5. Merge to develop
git checkout develop
git merge --no-ff hotfix/critical-segfault
git push origin develop

# 6. Clean up
git branch -d hotfix/critical-segfault
```

---

## Branch Protection Rules (GitHub)

### For `main` branch:
- ✅ Require pull request before merging
- ✅ Require status checks to pass
- ✅ Require branches to be up to date
- ✅ Require signed commits (optional)
- ✅ Include administrators (optional)
- ✅ Restrict push access

### For `develop` branch:
- ✅ Require status checks to pass
- ⚠️ Allow direct pushes for maintainers
- ✅ Require branches to be up to date before merge

---

## Best Practices

### DO:
- ✅ Always branch from `develop` for features
- ✅ Keep feature branches small and focused
- ✅ Write descriptive commit messages
- ✅ Rebase regularly to stay up-to-date
- ✅ Run tests before pushing
- ✅ Delete merged branches
- ✅ Tag all releases on `main`
- ✅ Update CHANGELOG for significant changes

### DON'T:
- ❌ Commit directly to `main`
- ❌ Merge broken code to `develop`
- ❌ Create feature branches from other features
- ❌ Leave stale branches for weeks
- ❌ Force push to shared branches
- ❌ Commit without testing
- ❌ Use vague commit messages

---

## Quick Reference

```bash
# Check current branch
git branch

# Switch branches
git checkout develop

# Create and switch to new feature branch
git checkout -b feature/my-feature

# Update current branch
git pull origin $(git branch --show-current)

# Rebase on develop
git fetch origin
git rebase origin/develop

# View commit history
git log --oneline --graph --all

# View branch status
git status

# View remote branches
git branch -r

# Delete local branch
git branch -d feature/my-feature

# Delete remote branch
git push origin --delete feature/my-feature

# Show changes
git diff

# Stash changes
git stash
git stash pop
```

---

## Current Branch Status

- **main:** Production releases only (v1.0.0-alpha.2)
- **develop:** Active development (all new work here)
- **feature/*:** Individual features in progress
- **bugfix/*:** Bug fixes in progress
- **hotfix/*:** Critical production fixes

---

## Version Numbering

Following **Semantic Versioning 2.0.0**:

**Format:** `MAJOR.MINOR.PATCH-PRERELEASE`

**Examples:**
- `1.0.0-alpha.1` - First alpha release
- `1.0.0-alpha.2` - Second alpha release
- `1.0.0-beta.1` - First beta release
- `1.0.0-rc.1` - First release candidate
- `1.0.0` - First stable release
- `1.1.0` - Minor version with new features
- `1.1.1` - Patch version with bug fixes
- `2.0.0` - Major version with breaking changes

---

**Maintained by:** Shubham Phapale  
**Repository:** https://github.com/ShubhamPhapale/apex  
**Questions:** Open an issue or discussion on GitHub
