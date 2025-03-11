## Joystick Servo test - see data aggregator read me for data aggregtor documentation
Created by Quinton Yusi

# Status of tests in "joystick_servotest folder"
    The data aggregator servo test works with its dummy flight control partition as expected and the partitions are set up with an expected sampling rate of 50 hz 
  
      # Running the Tests
    Open 4 separate terminal windows in terminals in the Flight Computer folder. 
    In each terminal enter the virtual environment using the command: 
    source .venv/bin/activate
    
    In the first terminal window run: 
    python3 joystick_servotest/dataAggregator.py
    
    In the second terminal window run:
    python3 joystick_servotest/FC_demo.py

    In the third terminal window run: 
    python3 DataAggregator/joystick_servotest/joystickPartition.py

    In the fourth terminal window run:
    python3 DataAggregator/joystick_servotest/servotest.py     
    
    The setup json being used is joystick_servo.json in the FlightComputer folder 

    The target behavoir for running these scripts, the joystick should control the servo as demonstrated by the existing and functioning joystick and servo test that does not use the data aggregator but a direct UDP communication between the joystick and servo however this does not happen. 

    # Observed Behavior: 
    The data aggregator successfully saves the correct data from each partition, but the servo does not respond to joystick inputs.
    The current issue is believed to be with the getRecentData method.
     
    In the FCdemo Partition, the data aggregator is not properly differentiating between the joystick and servo data which is believed to be an issue with the getRecentData method reporting data from the joystick partition when it is being asked for the data from the servo partition. The symptom is that the servo partitions recieved joystick command is identical to the reported servo position, and different from the joystick partitions reported joystick position. As the command being passed to the servo partition
    Three different team members have checked the setup files and partitions for mixups in the dictionary setups in the setup json that could cause this error, but none have been found. The current understanding is that this is likely a bug in the getRecentData method in the DataProcessor class. 

