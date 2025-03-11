# Reinstalling Ubuntu Guide

## Create a Bootable Ubuntu Installer USB Drive
This guide is tested with **Ubuntu 24.04**.

Follow this tutorial to create a bootable USB drive:
[Create a USB Stick on Windows](https://ubuntu.com/tutorials/create-a-usb-stick-on-windows#1-overview)

---

## Installing Ubuntu
### Booting into the USB Installer
1. Turn **off** the flight computer.
2. Turn **on** the flight computer.
3. Press **ESC** rapidly until you see a screen with multiple options.
4. Click **Boot Menu**.
5. Select the **USB installer** you created.
6. Press **Enter**.

### Running the Ubuntu Installer
1. Select **Try/Install Ubuntu** (use arrow keys to navigate) and press **Enter**.
2. Wait for Ubuntu to load and launch the installer.
3. Follow the installer prompts:
   - Click **Next** multiple times.
   - Choose **Use Wired Connection**.
   - **Update Installer** if prompted. If it closes, reopen it and restart installation.
   - Choose **Interactive Installation**.
   - Select **Default Options**.
   - Check **Install third-party software and additional media formats**.
   - Choose **Erase disk and install Ubuntu**.

### Set the Following User Information:
- **Your Name:** `cp-opa`
- **Your Computer's Name:** `cp-opa`
- **Your Username:** `cp-opa`
- **Password:** `1234`
- **Uncheck** "Require my password to login" and "Use Active Directory"

### Finalizing Installation
1. Click **Next** until you reach the final review screen.
2. Ensure the correct drive is selected.
3. Click **Install** and wait for completion.
4. Follow on-screen instructions:
   - Restart when prompted.
   - Remove the USB installer.
   - Let the system reboot.
   - Click **Next** after reboot.
   - **Skip Ubuntu Pro**.
   - **Do not share system data**.
   - Click **Finish**.

---

## Initial Setup After Installation
### Update System
Open the **Terminal** (`Ctrl + Alt + T`) and run the following commands **one at a time**:
```bash
cd
sudo apt update
sudo apt upgrade
```
> *This may take a while. Run these two commands every few days to keep the system updated.*

### Install Git
```bash
sudo apt install git
```

### Install Essential Applications
Go to **App Center** and install:
- **Sublime Text**
- **Sublime Merge**
- **VS Code**

Restart the flight computer after installation.

---

## Setting Up Sublime Merge & GitHub Account  
1. Open **Sublime Merge**.  
2. Add a repository with:  
   - **Source URL:** `https://github.com/AERO-568-Flight-Computer/FlightComputer`  
   - **Repository Name:** `FlightComputer`  
   - **Path:** `/home/cp-opa/Desktop/FlightComputer`  
3. Click **Clone Repo**.  
4. Log in to GitHub on the browser (ask Iscold for 2FA, login info is in OneDrive).  
5. Run:  

   ```bash
   sudo apt install gh
   gh auth login
   ```  
6. Select the following options:  
   - **What account?** GitHub.com  
   - **Preferred protocol?** HTTPS  
   - **Authenticate with GitHub credentials?** Yes  
   - **Authenticate with GitHub CLI?** Login with a web browser  
7. Copy your first-time code and follow the instructions in the browser.

---

## Install Python and Virtual Environment
```bash
cd
sudo apt install python3-pip
sudo apt install python3-tk
sudo apt install python3-venv
sudo apt install xterm
```

### Create and Activate Virtual Environment
```bash
cd
cd Desktop/FlightComputer
python3 -m venv .venv
source .venv/bin/activate
pip install psutil 
pip install colorama
pip install pyzmq
pip install pyserial
pip install numpy
```
>Add any additional packages with:
```bash
pip install <package_name>
```
Whenever running Python code, activate the virtual environment:
```bash
source .venv/bin/activate
```

---

## Install Drivers
1. Download the required drivers:
   - **DM35424**
   - **SER25330** (*installed by default on newer kernels*)
   - [RTD Drivers](https://www.rtd.com/software_drivers.htm)
2. Unzip the drivers.
3. Follow the README instructions (**typically:** `make` then `sudo make load`).

### Set Serial Port Permissions
```bash
cd
sudo usermod -a -G dialout $USER
```
Reboot your system.
If needed, manually set permissions each time using:
```bash
sudo chmod 666 /dev/ttyS4
```

---

## Configure Network Settings
1. Open **Settings** → **Network**.
2. Enable **enp13s0** and click the **gear icon** next to it.
3. In the **IPv4** tab, set to **Manual**:
   - **IP Address:** `192.168.10.116`
   - **Netmask:** `255.255.0.0`

---

## Set Up DHCP Server
```bash
cd
sudo apt install isc-dhcp-server
cd /etc/dhcp/
sudo nano dhcpd.conf
```
Append the following lines to the end of `dhcpd.conf`:
```bash
subnet 192.168.10.0 netmask 255.255.255.0 {
  range 192.168.10.102 192.168.10.110;
  option routers 192.168.10.1;
  option broadcast-address 192.168.10.255;
  default-lease-time 600;
  max-lease-time  7200;
}

# Assigns a specific IP to the Stirling Joystick
host myClient {
  hardware ethernet 00:01:c0:03:e7:dd;
  fixed-address 192.168.10.101;
}
```
Restart the DHCP server:
```bash
sudo systemctl restart isc-dhcp-server.service
```
Wait 30 seconds and verify connectivity:
```bash
ping 192.168.10.101 -c 5
```
Ensure the joystick is connected via the crossover cable.

Note that the joystick does not have a static IP, it has a reserved IP that we reserve for it using the DHCP server. As it is directly connected and is not designed to have a static IP, we must create the DHCP server and reserve an IP address in order to connect to it.

---

## Set Up SSH
```bash
sudo apt update
sudo apt upgrade
sudo apt install openssh-server
sudo systemctl start ssh
sudo systemctl status ssh
sudo systemctl enable ssh
```
Modify the SSH configuration:
```bash
sudo nano /etc/ssh/sshd_config
```
Uncomment the line:
```bash
Port 22
```
Restart SSH service:
```bash
sudo systemctl restart ssh
```

---

## Set Up Wireless Casting (Still Debugging)
```bash
sudo apt install gnome-network-displays
```

---