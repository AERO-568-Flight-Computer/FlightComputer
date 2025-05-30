## configuration .json
Written by Ajay Parikh

## Objective
Give the partition manager what to open (implemented) and the data aggregator information of how to talk to them (not implemented).

## json file feilds:
* name: string that gives what the program is called in outputs
* path: relative filepath from FlightComputer folder to partiton
* priority: integer that gives the order to start program, lower number starts sooner
* restart: string that says what to do upon partiton close, can be "True" (restarts program automatically) or "Ask" (creates dialog box asking if program should be restarted) with all other options (blank, "False", et cetera) not restarting the program