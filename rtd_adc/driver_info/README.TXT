Linux Software (DM35424)

SWP-700010144 rev E

Version v04.00.01.130736

Copyright (C) RTD Embedded Technologies, Inc.  All Rights Reserved.

This software package is dual-licensed.  Source code that is compiled for
kernel mode execution is licensed under the GNU General Public License
version 2.  For a copy of this license, refer to the file
LICENSE_GPLv2.TXT (which should be included with this software) or contact
the Free Software Foundation.  Source code that is compiled for user mode
execution is licensed under the RTD End-User Software License Agreement.
For a copy of this license, refer to LICENSE.TXT or contact RTD Embedded
Technologies, Inc.  Using this software indicates agreement with the
license terms listed above.


------------------
Supported Hardware
------------------

This software supports the RTD DM35424.


-------------------------
Supported Kernel Versions
-------------------------

This software has been tested with the following Linux distributions and kernel
versions:

    * openSUSE 12.2 (unmodified kernel 3.2)
    * Ubuntu 16.04 LTS (unmodified kernel 4.4)
    * Ubuntu 16.04 LTS (4.10 kernel built from kernel.org source)

 Due to API differences between kernel versions, RTD cannot guarantee compatibility
 with kernels and distributions not listed above.  If a user wishes to use an 
 unsupported kernel/distribution, it may be necessary to modify the driver module 
 code and/or Makefiles for the specific Linux environment.
 
--------------------------
Supported CPU Architecture
--------------------------
  
 This software has been validated on the following CPU architectures.
 
    * i586 (32-bit) dual-core
    * i586 (64-bit) single-core
    * i586 (64-bit) multi-core


-------------------
Supported Compilers
-------------------

The driver software and example programs were compiled using the GNU gcc 
compiler under openSUSE 12.2 and Ubuntu 16.04


-------------
Driver Design
-------------



        +------------------+        +----------------------------+
        | Example Programs |   OR   | User-Developed Application |
        | Provided by RTD  |        |                            |
        +------------------+        +----------------------------+
                 |                                |
                 |                                |
                 +--------------------------------+
                               |
                               | High-level C Functions
                               |
                          +---------+
                          | Library |
                          +---------+
                               |
                               | Low-level IOCTLs
    User Mode                  |
    ==========================================================================
    Kernel Mode                |
                           +--------+
                           | Driver |
                           +--------+
                               |
                               | Kernel Services
                               |
    ==========================================================================
                            Hardware
                            
                            
------
Driver
------

The directory driver/ contains source code related to the driver.


In order to use a driver, one must first compile it, load it into the kernel,
and create device files for the board(s).  To do this, issue the following
commands while sitting in the driver/ directory:

    * make
    * sudo make load



The driver module must be loaded before running any program which accesses a
DM35424 device.



-----------------
Library Interface
-----------------

The directory lib/ contains source code related to the user library.


The DM35424 library is created with a file name of librtd-dm35424.a and is
statically linked.


Please refer to the software manual for details on using the user level library
functions.  These functions are prototyped in the file include/dm35424_library.h;
this header file must be included in any code which wishes to call library
functions.


The library must be built before compiling the example programs or your
application.


To build the library, issue the command "make" within lib/.


------------
Header Files
------------

The directory include/ contains all header files needed by the driver, example
programs, library, and user applications.


----------------
Example Programs
----------------

The directory examples/ contains source code related to the example programs,
which demonstrate how to use features of the DM35424 boards, test the driver, or
test the library.  In addition to source files, examples/ holds other files as
well; the purpose of these files will be explained below.


To build the example programs, issue the command "make" within examples/.


The following files are provided in examples/:

    * Makefile
	    Make description file for building example programs.

    * dm35424_adc.c
            This example program demonstrates the use of the ADC and interrupt
            handling.  An interrupt is generated each time an ADC has taken a 
            sample.  When that interrupt happens, the programs gets the value
            of the last sample and displays it on the screen.
            
            As a convenience, the onboard DAC will be setup to generate a
            suitable signal for the ADC to read.
            
            Setup: Connect the signal of interest to the ADC Channel+ pin,  
            Channel- pin and GND.  If using the onboard DAC instead, connect
            DAC Channel 0 to ADC Channel 0+ and ADC Channel 1-,
            and DAC Channel 1 to ADC Channel 0- and ADC Channel 1+.  Repeat 
            for remaining ADC channels.
            
            Usage: Display the command-line options by executing 
                           ./dm35424_adc --help
            Hit CTRL-C to exit.

    * dm35424_adc_continuous_dma.c
            This example program demonstrates the use of the ADC and DMA.  The 
            example will collect data from the ADC via DMA, and then write the
            data out to a file on disk.  The data can then be plotted using 
            gnuplot and the plot_adc_dma file.

            As a convenience, the onboard DAC will be setup to generate a
            suitable signal for the ADC to read.
            
            Setup: Connect the signal of interest to the ADC Channel+ pin,  
            Channel- pin and GND.  If using the onboard DAC to generate the
            signal, connect DAC Channel 0 to ADC Channel 0+ and ADC Channel 1-,
            and DAC Channel 1 to ADC Channel 0- and ADC Channel 1+.  Repeat 
            for remaining ADC channels.
            
            Usage: Display the command-line options by executing
                           ./dm35424_adc_continuous --help
                
            Hit CTRL-C to exit.

    * dm35424_dac.c
            This example program demonstrates the use of the DAC.  A voltage is
            put out on the pin corresponding to the input from the user.  The voltage
            can be easily changed to cover the full range of -5V to 5V.
            
            Setup: Connect an oscilloscope to the DACx Channel 0-3 pins associated with
            the DAC in use.
            
            Usage: Usage: Display the command-line options by executing
            		./dm35424_dac --help
            		
            Follow the prompts on screen for changing the voltage.

    * dm35424_dac_dma.c
            This example program demonstrates the use of the DAC and DMA.  Wave
            pattern data is generated in the program, written to a DMA buffer,
            and then sent to the DAC in a repeating loop, thus providing a
            continuous cycling signal.
            
            The data for each odd-numbered channel will be a half-cycle out of
            phase with the even-numbered channels, for viewability on the scope.
            
            Setup: Connect an oscilloscope to the DACx Channel 0-3 pins associated with
            the DAC in use.
            
            Usage: Display the command-line options by executing
            		./dm35424_dac_dma --help
            
    * dm35424_dio.c
            This example program demonstrates the use of the DIO.  It configures
            half of the pins as output, and the other half as input.  Then, 
            using a loopback connector, data is written to the output pins and 
            read from the input pins, and then verified to be correct.
            
            Setup: This example requires a loopback from DIO Port 0:0 to
            Port 0:7, Port 0:1 to Port 0:8, etc.
            
            Usage: ./dm35424_dio --minor 0

    * dm35424_list_fb.c
            This example program demonstrates accessing the board-level 
            registers to access the function blocks on the board.  The 
            program will query every function block location to see if a 
            valid function block type exists there, and if it does, it will
            display that type on the screen.  In this way, it will give an
            inventory of the function blocks on the board.
            
            Setup: No setup required.
            
            Usage: ./dm35424_list_fb --minor 0

    * dm35424_ref_adjust.c
            This example program demonstrates using the reference adjustment 
            value to adjust the value reported by the ADC or sent by the DAC.
            The example allows for setting an adjustment value, and then
            writing that value to the appropriate memory location.
            
            The reference voltage source should be attached to the differential
            inputs of ADC0 or ADC1 (depending on which reference adjustment you
            are using).  The voltage reading from the ADC will be displayed on
            the screen, allowing you to view the effect reference value changes has
            on the measured voltage.
            
            For purposes of running the example, and for convenience, the 
            onboard DACs will be set to supply a value of 2 volts.  You can then
            loopback the DAC outputs into the ADC inputs, as described below.
            
            Note that the supplied DAC outputs should not be considered a 
            reference voltage source for the purposes of calibrating the
            board.
            
            WARNING: This example will allow you to change the preset
            calibration values on the board.  Do not do so unless you
            understand the risks of permanently changing that value.
            
            Setup: Attach a reference voltage source to the ADC Channel 0-
            and Channel 0+ pins.  If using the onboard DACs, connect DAC
            Channel 0 to ADC Channel 0+ and DAC Channel 1 to ADC Channel 0-.
            
            Usage: Display the command-line options by executing
            		./dm35424_ref_adjust --help.

    * dm35424_temperature.c
            This example program demonstrates the temperature sensor on the board.
            It will continually display the temperature on the screen until
            stopped.

            Press CTRL-C to exit.

            Setup: No setup required.

            Usage: ./dm35424_temperature --minor 0


-----------------
Known Limitations
-----------------

 1. This software was tested only on little-endian processors.  If you are using
    a big-endian CPU, you will need to examine the driver, example, and library
    source code for endianness issues and resolve them.

 2. Many conditions affect board throughput and interrupt performance.  For a
    discussion of these issues, please see the Application Note SWM-640000021
    (Linux Interrupt Performance) available on our web site.

 3. If you are using the interrupt wait mechanism, be aware that signals
    delivered to the application can cause the sleep to awaken prematurely.
    Interrupts may be missed if signals are delivered rapidly enough or at
    inopportune times.



-------------------------
Getting Technical Support
-------------------------

If you require additional support with this product, or any other products from
RTD Embedded Technologies, contact us using the information below:

	RTD Embedded Technologies, Inc.
	103 Innovation Boulevard
	State College, PA 16803 USA

	Telephone: (814) 234-8087
	Fax: (814) 234-5218
	Sales Information and Quotes: sales@rtd.com
	Technical Assistance: techsupport@rtd.com
	Web Site: http://www.rtd.com
