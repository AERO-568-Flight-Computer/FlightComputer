import abc


class InterfaceDC(abc.ABC):



    @abc.abstractmethod
    def internalInit(self):
        # Initialize the process 

        pass


    @abc.abstractmethod
    def checkin(self):
        # Send name to management class
        # Wait for response
        pass


    @abc.abstractmethod
    def regWAggregator(self):
        # Register the ports with the data aggregator
        pass

    @abc.abstractmethod
    def waitForManagerRun(self):
        # Wait for manager to ask to start the process
        pass

    @abc.abstractmethod
    def runStep(self):
        # Receive data from aggregator
        # Process data
        # Send data to aggregator
        pass

    @abc.abstractmethod
    def reportCompletion(self):
        # Send message to manager that process is complete
        pass

    # Helper methods, such as send piece of data


