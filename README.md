#rshell
rshell server and client, allowing user to run terminal commands remotely

Instructions:
First, start the server with:
 ```
 ./server 1234
 ```
Then, start the client with:
 ```
 ./client localhost 1234
 ```

When you are prompted to enter a command you have several options:
 - Enter a linux command to be processed.
 - Enter "stats", which will print out how many commands have been entered and memory usage statistics.
 - Enter "reset", which will reset the command count to zero.
 - Enter "exit", which will exit the program.
 ![alt tag](https://github.com/christopher18/rshell/blob/master/example_input.png "Example Input")
