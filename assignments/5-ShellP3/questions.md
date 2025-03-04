1. Your shell forks multiple child processes when executing piped commands. How does your implementation ensure that all child processes complete before the shell continues accepting user input? What would happen if you forgot to call waitpid() on all child processes?

My implementation ensures that all child processes complete before the shell continues acceping user input by calling waitpid() for each process created in the pipeline. If I forgot to call waitpid() on all child processes, the child process would become a zombie process after termination as the exit statuses wouldn't be collected, the parent shell may accept commands while the child process is still running and/or the child processes can accumulate preventing new processes from being created or processed correctly.

2. The dup2() function is used to redirect input and output file descriptors. Explain why it is necessary to close unused pipe ends after calling dup2(). What could go wrong if you leave pipes open?

dup2() functions are necessary to close unused pipe ends after calling dup2() because if not there could be too many files open leading to resource leaks, there could be an infinite wait if data is expected, and it is needed to signal EOF correctly.

3. Your shell recognizes built-in commands (cd, exit, dragon). Unlike external commands, built-in commands do not require execvp(). Why is cd implemented as a built-in rather than an external command? What challenges would arise if cd were implemented as an external process?

cd is implemented as a build-in rather than an external command because cs modifies the current working directory of the shell process, which must continue across all commands. Built-in commands run within the shell itself, allowing it to be possible to be run externally. If cd were implemented externally, new processes would change the working directory but not the parent shell. Additionally, it is likely that the parent shell would remain in the original directory, making cd useless.

4. Currently, your shell supports a fixed number of piped commands (CMD_MAX). How would you modify your implementation to allow an arbitrary number of piped commands while still handling memory allocation efficiently? What trade-offs would you need to consider?

Dynamic allocation instead of a fixed-size array using malloc and realloc. Trade-offs would be increased complexity from handling memory leaks and excessive memory consumption.
