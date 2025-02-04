class NoDecrementorError(Exception):
    pass

class Printer:
    def __init__(self,val1,val2):
        self.val1 = val1
        self.val2 = val2
        self.decrementor = 3

    def print(self):
        if self.decrementor <= 0:
            raise NoDecrementorError("No decrementor available") 
        print("Val1:",self.val1)
        print("Val2:",self.val2)
        print("Decrementor:",self.decrementor)
        print("-------------------------------")
        self.decrementor = self.decrementor-1

printer1 = Printer(0,1)
try:
    printer1.print()
    printer1.print()
    printer1.print(465798)
    printer1.print()
except NoDecrementorError:
    print("No decrementor!")
    raise
except:
    print("Some other error?")
    raise