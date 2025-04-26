# Contributing to Serdec

Thank you for considering contributing to **Serdec**! 🚀

We welcome contributions of all kinds: new features, bug fixes, documentation improvements, and tests.

## 📚 How to Contribute

1. **Fork** the repository.
2. **Clone** your fork to your local machine.
3. **Create a new branch** for your changes:

```bash
git checkout -b feature/my-new-feature
```

4. **Make your changes**.
5. **Commit your changes** using [Conventional Commits](https://www.conventionalcommits.org/en/v1.0.0/).
6. **Push** to your fork.
7. **Open a Pull Request** with a clear description of what you changed and why.

## 📆 Commit Message Guidelines

We follow the [Conventional Commits](https://www.conventionalcommits.org/en/v1.0.0/) specification.

Commit messages should look like this:

```
feat(json): add serdec_json_new_array function

Add a new function to create JSON arrays, with safe memory initialization.
```

### Common Commit Types

| Type       | Purpose                                                     |
| ---------- | ----------------------------------------------------------- |
| `feat`     | Add a new feature                                           |
| `fix`      | Fix a bug                                                   |
| `docs`     | Documentation only changes                                  |
| `style`    | Changes that do not affect meaning (formatting, whitespace) |
| `refactor` | Code change that neither fixes a bug nor adds a feature     |
| `perf`     | Code change that improves performance                       |
| `test`     | Adding or updating tests                                    |
| `chore`    | Maintenance tasks, like updating build scripts              |

## 🔧 Code Guidelines

- Keep functions small and focused.
- Always check for `NULL` before dereferencing pointers.
- Free any allocated memory properly to avoid leaks.
- Keep public APIs clean and consistent.
- Write defensive code where appropriate (e.g., validate inputs).
- Match the existing code style (tabs, spacing, function names).

## 📈 Improving Documentation

If you find any part of the documentation unclear or missing:

- Feel free to open a pull request.
- Suggest better examples or improvements.
- Fix typos or improve formatting.

## 📥 Submitting Pull Requests

- Link related issues if applicable.
- Provide a clear summary of the changes.
- Make sure your code builds and passes all tests (if tests exist).
- Be respectful and open to feedback during review.

## 💪 Thank You

Your help makes Serdec better for everyone!
We appreciate every contribution, no matter how big or small. ✨
