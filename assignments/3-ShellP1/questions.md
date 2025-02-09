1. In this assignment I suggested you use `fgets()` to get user input in the main while loop. Why is `fgets()` a good choice for this application?

    > **Answer**:  'fgets()' is a good choice to get user input for this application because it reads inputs one line at a time, uses a size parameter to prevent buffer overflows, handles newlines, handles EOF and finally detects read errors.

2. You needed to use `malloc()` to allocte memory for `cmd_buff` in `dsh_cli.c`. Can you explain why you needed to do that, instead of allocating a fixed-size array?

    > **Answer**:  I needed to use 'malloc()' to allocate memory for 'cmd_buff' in 'dsh_cli.c' instead of allocating a fixed array because properly manage memory. If our code were to ever need to change the maximum input size or if the buffer needs more space, I am not able to do that with a fixed array. Since I used malloc, I am able to allocate memory dynamically based on changing requirements.


3. In `dshlib.c`, the function `build_cmd_list(`)` must trim leading and trailing spaces from each command before storing it. Why is this necessary? If we didn't trim spaces, what kind of issues might arise when executing commands in our shell?

    > **Answer**:  This is necessary because if we didn't commands can be parse commands incorrectly, create extra unintended arguments and potentially commands not being found as a result of command names having extra spaces. Trimming the spaces is crucial to ensure that commands are as intended and to reduce errors during execution of these commands.

4. For this question you need to do some research on STDIN, STDOUT, and STDERR in Linux. We've learned this week that shells are "robust brokers of input and output". Google _"linux shell stdin stdout stderr explained"_ to get started.

- One topic you should have found information on is "redirection". Please provide at least 3 redirection examples that we should implement in our custom shell, and explain what challenges we might have implementing them.

    > **Answer**:  One example of redirection is output redirection. Where an example of this can be "ls > output.txt" where the output of ls is redirected from STDOUT to outputfile.txt. A challenge that can happen is opening/creating the file due to permission issues. Another example of redirection is input redirection. Where an example of this can be "sort < unsortedfile.txt" where the the txt file is used as STDIN for the sort command. A challenge that can happen is the file not existing and the file not able to read. Another example of redirection is error redirection. Where an example of this can be "grep pattern file 2> error.txt" where the STDERR is now inputted into error.txt to get error messages. A challenge that can happen is that normal outputs can be mixed in with error messages and seperating STDERR from STDOUT.

- You should have also learned about "pipes". Redirection and piping both involve controlling input and output in the shell, but they serve different purposes. Explain the key differences between redirection and piping.

    > **Answer**:  The key differences between redirection and piping is that redirection is used to redirect output/input to files, while piping is used to redirect commmands to other commands. In other words, redirection allows the ability to connect commands to files, while piping allows the ability to connect commands together.
- STDERR is often used for error messages, while STDOUT is for regular output. Why is it important to keep these separate in a shell?

    > **Answer**:  It is important to keep these seperate for clarity and confusion. Mixing both can make it hard to make a distinction between normal output and error messages, which in turn makes it hard to debug code.

- How should our custom shell handle errors from commands that fail? Consider cases where a command outputs both STDOUT and STDERR. Should we provide a way to merge them, and if so, how?

    > **Answer**:  Our custom shell should handle errors from commands that fail be keeping STDOUT and SDERR seperate so that users could see normal output and error messages clearly. Additionally, errors should be displayed as STDERR and normal outputs should be displayed as STDOUT. In the case of merging, we could provide a way to merge them by adding command line commands that allow merging to occur in the same output stream. This can be done by adding code to allow merging once a command is called in our shell.