# Execute Git command to retrieve the change list number.
Execute_Process(COMMAND ${GIT_EXECUTABLE} rev-list --count --first-parent HEAD OUTPUT_STRIP_TRAILING_WHITESPACE OUTPUT_VARIABLE GIT_CHANGE_LIST)

# Execute Git command to retrieve the commit hash.
Execute_Process(COMMAND ${GIT_EXECUTABLE} log -1 --format=%h OUTPUT_STRIP_TRAILING_WHITESPACE OUTPUT_VARIABLE GIT_COMMIT_HASH)

# Execute Git command to retrieve the commit date.
Execute_Process(COMMAND ${GIT_EXECUTABLE} log -1 --format=%ci OUTPUT_STRIP_TRAILING_WHITESPACE OUTPUT_VARIABLE GIT_COMMIT_DATE)

# Execute Git command to retrieve the branch name.
Execute_Process(COMMAND ${GIT_EXECUTABLE} rev-parse --abbrev-ref HEAD OUTPUT_STRIP_TRAILING_WHITESPACE OUTPUT_VARIABLE GIT_BRANCH_NAME)

# Create a configuration file.
Configure_File(${INPUT_FILE} ${OUTPUT_FILE})
