## partitonManager.py
Written by Ajay Parikh

## Objective
Start and ensure running of all partitons

## How to use
Run the following in terminal:
```
cd
cd Desktop/FlightComputer
source .venv/bin/activate
python3 PartitonManager/partitonManager.py
```

You may also use the shortcut named Partition Manager on the desktop. This can be run by simply double clicking the file and it will do the rest for you.

## Order of operations
Read .json file with the partitons you want to launch and some additonal relevant information

Close all open ports

Start partiton in a new terminal window
Ensure that said partiton has initilzed, see example client code in clientExample.py
Repeat last 2 steps until all partitons are open 

Checks that partitons are still running
If specifed in .json file, restarts partiton if it is no longer running
Repeat last 2 steps until program ended

## json file fields:
* name: string that gives what the program is called in outputs
* path: relative filepath from FlightComputer folder to partiton
* priority: integer that gives the order to start program, lower number starts sooner
* restart: string that says what to do upon partiton close, can be "True" (restarts program automatically) or "Ask" (creates dialog box asking if program should be restarted) with all other options (blank, "False", et cetera) not restarting the program

## Known Issues
If a partiton has any sort of error, the partiton closes and does not report the error. This can be solved by putting the entire partiton in a try expect statement or you can troubleshoot my taking a slo mo video of the terminal to see error messages.

If a partition crashes during certian phases of startup/before it initializes, the program will hang.

## Debugging help
If you have an error that Address already in use for the socket after trying to restart the progarm, type "fg" then "ctrl+c". Then, try running the program again.

Slo motion video can help you find errors before the terminal closes.

You can run partitions on their own by commenting out initialize.initialize() if one partition seems to be acting up.