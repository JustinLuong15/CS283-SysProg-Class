1. Can you think of why we use `fork/execvp` instead of just calling `execvp` directly? What value do you think the `fork` provides?

    > **Answer**:  We use 'fork/execvp' instead of just calling 'execvp' directly because without fork, calling execvp would replace the entirety of the shell process with a new command, terminating the shell. However, by using fork, the child executes the 'execvp' command, while the parent, the shell, remains to handle other commands. In otherwords, because we use 'fork/execvp' we are able to ensure that the shell continuously runs after executing commands.

2. What happens if the fork() system call fails? How does your implementation handle this scenario?

    > **Answer**:  If fork() fails my implementation returns -1, using perror("fork") to print the reason. Additionally, the parent process would skip the commands and return the error code, ERR_EXEC_CMD to make sure the shell still continues to accept responses.

3. How does execvp() find the command to execute? What system environment variable plays a role in this process?

    > **Answer**:  execvp() finds the command by searching for the executables in directories listed in the PATH environment variable.

4. What is the purpose of calling wait() in the parent process after forking? What would happen if we didn’t call it?

    > **Answer**:  wait() ensures that the parent process pauses until the child is terminated. If not called, the child would continue to use resourses and the shell would ask for a new prompt before the child process is finished which could lead to unpredictable outputs.

5. In the referenced demo code we used WEXITSTATUS(). What information does this provide, and why is it important?

    > **Answer**:  The WEXITSTATUS() retrieves the exit code of the child process, which is important for reporting if the command succeeded or failed.

6. Describe how your implementation of build_cmd_buff() handles quoted arguments. Why is this necessary?

    > **Answer**:  My implementation uses character-by-character parsing to detect quotes. Arguments inside of the quotes are treated as single tokens. For example, [echo "hello world"] is treated as ["echo" "hello world"]. This implementation ensures that the command is executed as intended.

7. What changes did you make to your parsing logic compared to the previous assignment? Were there any unexpected challenges in refactoring your old code?

    > **Answer**:  The previous assignment split commands using pipes and not quotes. The current assingment now parses using quotes and no longer uses pipes.

8. For this quesiton, you need to do some research on Linux signals. You can use [this google search](https://www.google.com/search?q=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&oq=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&gs_lcrp=EgZjaHJvbWUyBggAEEUYOdIBBzc2MGowajeoAgCwAgA&sourceid=chrome&ie=UTF-8) to get started.

- What is the purpose of signals in a Linux system, and how do they differ from other forms of interprocess communication (IPC)?

    > **Answer**:  The purpose of signals in a Linux system is to interupt process execution by terminating, pausing or reporting errors for processes. Unlike IPC, which typically involves exchanging larger or structured amounts of data between processes.

- Find and describe three commonly used signals (e.g., SIGKILL, SIGTERM, SIGINT). What are their typical use cases?

    > **Answer**:  SIGKILL - Forceful termination, used as a last resort termination if nothing else works, SIGTERM - a signal that requests a program to terminate, used to close resources and shutting down the program cleanly, SIGINT - graceful termination of a process, used to ensure that processes can be suspended reliably

- What happens when a process receives SIGSTOP? Can it be caught or ignored like SIGINT? Why or why not?

    > **Answer**:  When a process receives SIGSTOP, the os stops execution, pausing the process for later resumption. Unlike SIGINT, it cannot be catched and ignored. This is because SIGSTOP is designed to help tools like debuggers the pauce processes regardless of signals or internal state.
