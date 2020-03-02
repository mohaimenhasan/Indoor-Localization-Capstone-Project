class Receiver:
    def __init__(self, receiverId = None, receiverPosition = None, angleOffset = 0):
        self.__receiverId = receiverId
        self.__receiverPosition = receiverPosition
        self.__angleOffset = angleOffset
    
    def __str__(self):
        return "{0}: Position -> {1}; Angle Offset -> {2}".format(str(self.__receiverId), str(self.__receiverPosition), str(self.__angleOffset))
    
    def get_receiverId(self):
        return self.__receiverId
        
    def get_receiverPosition(self):
        return self.__receiverPosition
    
    def get_angleOffset(self):
        return self.__angleOffset