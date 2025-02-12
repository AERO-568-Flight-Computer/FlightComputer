from SimpleJoysticInteface import SimpleJoysticInteface
from DataProcessor import DataProcessor
import time


JoysticInteface = SimpleJoysticInteface()
time.sleep(1)
#Trying to print out positions every half second
t_delay = 0.5
count = 0

    # Consider making filepath a command line argument
name = "name1joystick"
filepath = "joystick_DA_i.json"

# Create an instance of the DataProcessor class specified by the name and filepath
processor = DataProcessor(name, filepath)

# Display the attributes of the processor
print(processor.name)
print(processor.portSend)
print(processor.portReceive)

dataDictionaryList = [
    {
        "timeRec": None,
        "pitchCommand": None
    }
]

while True:
    int_time = time.time()
    processor.receiveData()
    recentData = processor.getRecentData("fc_demo", 1)
    timeRecRecieved = recentData[0, 0]
    iasReceived = recentData[0, 1]
    ias = iasReceived  # Placeholder for IAS

    pitchPosition, rollPosition, err_code = JoysticInteface.get_pitch_roll()
    
    count = count+1
    if count > 20:
        print("Trying to adjust the force")
        JoysticInteface.adjustForce(ias)
        count = 0

    dataDictionaryList[0]["timeRec"] = int_time
    dataDictionaryList[0]["pitchCommand"] = pitchPosition
    print(dataDictionaryList)
    processor.sendData(dataDictionaryList)