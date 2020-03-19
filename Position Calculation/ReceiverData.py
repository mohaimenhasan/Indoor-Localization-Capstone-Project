class ReceiverData:
    
    def __init__(self, receiverId = None, timeStamp = None, angleOfArrival = None):
        self.__receiverId = receiverId
        self.__timeStamp = timeStamp
        self.__angleOfArrival = angleOfArrival
        
    def __str__(self):
        return "{0}.{1}: {2}".format(str(self.__receiverId), str(self.__timeStamp), str(self.__angleOfArrival))
    
    def get_receiverId(self):
        return self.__receiverId
        
    def get_timeStamp(self):
        return self.__timeStamp
    
    def get_angleOfArrival(self):
        return self.__angleOfArrival