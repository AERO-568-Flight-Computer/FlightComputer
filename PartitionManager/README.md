## partitonManager.py
Written by Ajay Parikh

## Objective
Start and ensure running of all partitons

## Order of operations
Read .json file (eventually sync with DataAggregator) with the partitons you want to launch and some additonal relevant information

Close all open ports

Start partiton in a new terminal window
Ensure that said partiton has initilzed, see example client code in clientExample.py
Repeat last 2 steps until all partitons are open 

Checks that partitons are still running
If specifed in .json file, restarts partiton if it is no longer running
Repeat last 2 steps until program ended

## json file feilds:
* name: string that gives what the program is called in outputs
* path: relative filepath from FlightComputer folder to partiton
* priority: integer that gives the order to start program, lower number starts sooner
* restart: string that says what to do upon partiton close, can be "True" (restarts program automatically) or "Ask" (creates dialog box asking if program should be restarted) with all other options (blank, "False", et cetera) not restarting the program


## Debugging help

If you have an error that Address already in use for the socket after trying to restart the progarm, type "fg" then "ctrl+c". Then, try running the program again.

## Reinstalling commands

Create a bootable ubuntu installer flashdrive (this is tested with 24.04)
Turn computer off
Turn computer on
Mash escape
Go to boot menu
Select flashdrive
Press enter
Select try/install
Wait a few seconds
next -> next -> next -> next -> use wired connection -> update if prompted (reopen if needed and repeat steps) -> interactive installation -> default selection -> check install third party software and additonal media formats -> erase disk and install Ubuntu

Your name: cp-opa
Your computer's name: cp-opa
Your username: cp-opa
Password: 1234

Uncheck require my password to login and use active directory

next -> next

Review choices (make sure it's on the right drive and that everything looks "good")

Click install, sit back, and relax

Follow the instructions on the screen (restart, remove install media, let it reboot)

Once rebooted, click next, skip ubuntu pro, next, no don't share system data, finish

Go to app center and intsall sublime text, sublime merge, and code

Run:
* cd
* sudo apt update
* sudo apt upgrade
 * Takes awhile, be patient
* sudo apt install git

Restart

On sublime merge, add a repo with source URL https://github.com/AERO-568-Flight-Computer/FlightComputer, repository name FlightComputer, and path /home/cp-opa/Desktop/FlightComputer
Clone repo
Click push button
Duplicate a file, stage it, and commit it
When prompted, enter name and email from onedrive sandbox 2024 and apply to global configuration
Push
When prompted, enter username from onedrive sandbox 2024 and access token from onedrive sandbox 2025 (do NOT pt in the git under any circumstance) (it's useful to have an extra flash drive to transfer files)
__THIS IS NOT WORKING, I CANNOT PUSH WILL COME BACK TO THIS LATER (MAY NEED A NEW ACCESS TOKEN EACH TIME)__

Run:
* cd 
* sudo apt install python3-pip
* sudo apt install python3-tk
* sudo apt install python3-venv

Now we want to create a virtual enviroment, run the following commands. Whenever you try to run 
* cd
* cd Desktop/FlightComputer
* python3 -m venv .venv
* source .venv/bin/activate
* pip install psutil
* pip install colorama
* pip install pyzmq
* pip install pyserial
Add any other packages you need

Then close out of terminal

Note that whenever you want to run python code you will need to enter our virtual enviroment (called venv) using source .venv/bin/activate

Install your drivers:
Download the drivers you need (DM35424 and SER25330 (installed by default on newer kernals)) from https://www.rtd.com/software_drivers.htm
Unzip drivers
Follow instructions in the readme (something like make then sudo make load, located in the Driver section of the README.txt)

Set permissions on your serial ports by running:
* cd
* sudo usermod -a -G dialout $USER
Reboot your system. If this does not work, you can set in one port at a time (and every time you reboot) similar to:
* sudo chmod 666 /dev/ttyS4

Configure your network settings:
Go to settings -> Network
Enable enp13s0 and select the little gear icon next to it
Go to ipv4 and set to manual with IP address 192.168.10.116 and netmask to 255.255.0.0

Set up DHCP server:
* cd
* sudo apt install isc-dhcp-server
* cd /etc/dhcp/
* sudo nano dhcpd.conf
 * in nano, append to the end of the .conf file
    ```
    subnet 192.168.10.0 netmask 255.255.255.0 {
    range 192.168.10.102 192.168.10.110;
    option routers 192.168.10.1;
    option broadcast-address 192.168.10.255;
    default-lease-time 600;
    max-lease-time  7200;
    }



    # This is the configuration for the Stirling Joystick
    # It assigns a specific IP address to the MAC address of the joystick
    host myClient {
    hardware ethernet 00:01:c0:03:e7:dd;
    fixed-address 192.168.10.101;
    }
    ```
* sudo systemctl restart isc-dhcp-server.service
 * wait 30 seconds
* ping 192.168.10.101 -c 5
 * make sure joystick is connected on the crossover cable