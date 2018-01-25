# Storytime
Final Project for Systems

***

Kenny Chen, Fabiola Radosav, Jan Kowalski

***

### Purpose:
	This program allows multiple clients to manage to contents of a story collection without running into read/write errors.
	Note this has only been tested in UNIX environments, specifically Ubuntu 14/16


### How to use:
	Once the repository is cloned, `make` is run to create executable files

	`./client` - run the client
	`./server` - run the server

	Only one server should be running at a time. However, an arbitrarily large amount of clients can be maintained.
	Once a client has connected with the server, the client will be prompted to enter 'help' to learn about available commands.
		`help` - returns a list of available commands
		`list` - lists the stories stored in the program
		`create [filename]` - creates the new story
		`read [filename]` - reads the contents of the chosen file
		`edit [filename]` - allows the user to edit the contents of a file through nano
		`remove [filename]` - removes the chosen file

	File manipulation is limited to the stories directory, which does not include subdirectories.

  To end a client session use the session interrupt key (Ctrl - c).


### Known Bugs:
	Ending the server program before all instances of client will likely result in an `error[98] Address already in use` that prevents the recreation of server.
	This error can be circumvented by either changing the port number or waiting a few minutes before attempting to restart the server.

	The nano environment created  when the edit command is run does not display the correct file.

	Editing a file may spawn a buffer file (ex. ".nfs00000000011b0fba0000000d")
