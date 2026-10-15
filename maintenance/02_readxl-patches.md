# Re-vendoring libxls and readxl's patches

readxl embeds a subset of [libxls](https://github.com/libxls/libxls) in `src/libxls/`.
We modify the vendored code as little as possible.
Every readxl modification is wrapped in fences:

```c
// --- Start readxl ---
...
// --- End readxl ---
```

so this lists all of them:

```sh
grep -rn "Start readxl" src/libxls
```

Everything outside the fences should be byte-for-byte identical to upstream.

## Workflow

Do this as a sequence of commits, so that upstream changes and readxl patches can be reviewed separately:

1. **Revert to pristine upstream.**
   Run `01_pull-from-libxls-upstream.R` with `target_version` set to the *currently* vendored version (see `Note` in `DESCRIPTION`).
   Commit.
   The diff is exactly our current patches, inverted.
   If it contains anything that isn't fenced, find out why before going further.
2. **Vendor the new version, unmodified.**
   Run `01_pull-from-libxls-upstream.R` with `target_version` set to the new version.
   Commit.
   The diff is exactly upstream's changes to the files we embed.
3. **Re-apply readxl patches.**
   Work through the inventory below.
   For each patch, first check whether upstream has made it unnecessary; if so, retire it rather than re-applying it.
   Commit, explaining anything that was retired.
4. **Housekeeping.**
   Update the libxls version in `src/unix/config-*.h` and `src/windows/config.h` (if `configure.ac` changed in more than the version number, regenerate these; see notes in `01_pull-from-libxls-upstream.R`), the `Note` field in `DESCRIPTION`, `NEWS.md`, and this file.

Commits 1 and 2 don't compile, and that's fine.

Watch out for editors (or tools) that strip trailing whitespace on save: upstream libxls has plenty, and stripping it creates noise in the diff against upstream.
Compare `git diff --stat` with `git diff --ignore-space-at-eol --stat` to check.

## Current patches

| Patch | Files | Why |
|---|---|---|
| filepaths | `brdb.h`, `endian.c`, `endian.h`, `locale.c`, `ole.c`, `ole.h`, `xls.c`, `xlsstruct.h`, `xlstool.c`, `xlstool.h` | Upstream's includes look like `"../include/libxls/foo.h"`; we flatten everything into `src/libxls/`, so these become `"libxls/foo.h"`. |
| cran | `xlstypes.h` | Include `src/cran.h`, which masks `printf()`, `fprintf()`, and `stderr`, which R CMD check doesn't allow in compiled code. Every libxls file includes `xlstypes.h`, directly or indirectly. |

## Retired patches

| Patch | Retired in | Why |
|---|---|---|
| strict-prototypes | libxls v1.6.3 | Upstream adopted `f(void)` prototypes. |
| snprintf | libxls v1.6.3 | Upstream switched `sprintf()` to `snprintf()`. |
| Solaris locale typedef | readxl, alongside libxls v1.6.4 | `locale.h` had a Solaris-only typedef for `xls_locale_t` (#663). CRAN no longer checks on Solaris, and readxl stopped shipping a Solaris config with libxls v1.6.3 (#764). |
| Rtools < 4.2 locale shim | readxl requiring R >= 4.2 (#808) | On Windows with GCC < 10 (Rtools40 and earlier, i.e. R <= 4.1), `locale.c` used `setlocale()` and `wcstombs()` instead of `_create_locale()` and `_wcstombs_l()` (#663, #694). |
| stdint | libxls v1.6.4 | We included `<stdint.h>` and `<sys/types.h>` in `xls.h` before opening `namespace xls`, so their typedefs didn't get trapped in `xls::` (#775, #790, #792). Upstream now does this (libxls/libxls#141). |
| bounds-sanitizer accessors | libxls v1.6.4 | We added `memcpy()`-based accessors for the variable-length members of `MULRK`, `MULBLANK`, and `LABEL` to avoid `-fsanitize=bounds` errors (#771). Upstream now uses C99 flexible array members for these records (libxls/libxls#144, #806). |
