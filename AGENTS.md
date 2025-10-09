# Repository Guidelines

## Project Structure & Module Organization
Core editor sources live in `src/`, grouped by responsibility: `main.cpp` boots the ncurses UI, `content_cursor.*`, `screen_cursor.*`, `tabs.*`, `screen.*`, and `position.*` coordinate text state and rendering. Place new runtime headers alongside their implementations and keep cross-module types in the corresponding header. Unit tests belong in `tests/`, one file per feature (`test_tabs.cpp`, `test_screen.cpp`), with helpers centralized in `tests/test_helpers.*`. `build/` is a generated CMake tree—treat it as ephemeral. The `Dockerfile` offers a reproducible environment when local ncurses tooling is unavailable.

## Build, Test, and Development Commands
Configure once with `cmake -S . -B build`. Rebuild after changes using `cmake --build build`. Run the full suite via `ctest --test-dir build --output-on-failure`; individual binaries such as `./build/test_tabs` help when iterating on a single module. The Docker workflow in the README (`docker build -t cppad .` then `docker run --rm -it -v "${PWD}:/app" -w /app cppad`) ensures the same toolchain and dependencies across platforms.

## Coding Style & Naming Conventions
Target C++17 throughout. Match the existing tab-based indentation and Allman brace placement (`screen.handle_left()` in `src/main.cpp`). Classes use PascalCase (`CPPAD`, `CSCREEN`); functions, variables, and files stick to snake_case (`handle_append`, `screen_cursor.cpp`); constants stay uppercase when added. Keep headers lean, group related declarations, and add concise comments only for non-obvious behaviour. Run `clang-format` if available, but align with the checked-in style first.

## Testing Guidelines
GoogleTest is fetched via CMake's `FetchContent`; prefer plain `TEST` cases mirroring the filename (`TEST(Tabs, ExpandsTab)` in `tests/test_tabs.cpp`). Add fixture helpers to `tests/test_helpers.{h,cpp}` instead of duplicating setup. New features require unit coverage and should extend `ctest` output; document intentionally skipped edge cases in the pull request for reviewer context.

## Commit & Pull Request Guidelines
Follow the short, imperative commit style already in history (`Clean up tabs`). Each PR should link related issues, describe user-visible changes, and note how to verify them (commands or screenshots for UI flows). Include the latest test results (`ctest` output or specific binaries) and call out migrations or manual steps so reviewers can reproduce them quickly.
