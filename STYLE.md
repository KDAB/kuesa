# Coding Style

We are using a coding style similar to the Qt one. To ease reviews we use
clang-format and we advise you to do the same. The easiest way to work with it
is to install clang-format and then add something like this in your
.git/hooks/pre-commit:

    #!/usr/bin/env python

    import subprocess
    output = subprocess.check_output(["git", "clang-format", "--diff"])

    if 'clang-format did not modify any files' not in output and 'no modified files to format' not in output:
        print "Run git clang-format, add the modified files, then commit.\n"
        exit(1)
    else:
        exit(0)

Note that .git/hooks/pre-commit must be made executable.
