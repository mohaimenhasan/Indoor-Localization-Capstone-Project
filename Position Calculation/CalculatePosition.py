from Position import Position
from Receiver import Receiver
from ReceiverData import ReceiverData
from Line import Line
from ConnectionManager import ConnectionManager
import datetime
# import Position
# import Receiver
# import ReceivedData

import numpy as np
import matplotlib.pyplot as plt
import math
import json
from  enum import Enum

class Resolution(Enum):
    Centimeters = 1
    Decimeters = 10
    Meters = 100

TESTING = True
DEFAULT_SLOPE = (math.pi / 4)
connectionMgr = ConnectionManager()

def main():
    startTime = str(datetime.datetime.now())
    config()

    if TESTING:
        r1Data = ReceiverData(0, 1, math.radians(30.96))
        r2Data = ReceiverData(1, 10, math.radians(-63.43))
        r3Data = ReceiverData(2, 2, (math.radians(50.31) - math.pi))

        # r1Data =  ReceiverData(0, 1, math.radians(47))
        # r2Data =  ReceiverData(1, 1, math.radians(-45))
        # r3Data =  ReceiverData(2, 1, (math.radians(44) - math.pi))
        # r4Data =  ReceiverData(3, 1, (math.radians(-25) + math.pi))
        receiverData = np.array([
        r1Data,
        r2Data,
        r3Data,
        # r4Data
        ])
        
        positionData, linesOfIntersection = calculatePosition(receiverData, Resolution.Meters)
        
        # print(positionReturn)
    else:
        receiverData = fetchReceiverData()
        positionData, linesOfIntersection = calculatePosition(receiverData, Resolution.Meters)

        endTime = str(datetime.datetime.now())
        response = constructResponse(positionData, receiverData, startTime, endTime, linesOfIntersection)
        #print(response)
        connectionMgr.sendPositionData(response)

# function: calculatePosition
# input(s):
# 1. receiverData
#       structure: [[timeStamp, AoA], ]
#       note: AoA is in range of [-pi, pi]
# output(s): data matrix representing location estimation of transmitter


def calculatePosition(receiverData, resolution=Resolution.Centimeters):
    positionMatrix = np.zeros((yDimension, xDimension))
    intersectionPoints = np.array(np.shape(0))

    # Update Lines
    linesOfIntersection = {}
    directionLines = np.array(np.shape(0))
    for i in range(len(receiverData)):
        data = receiverData[i]
        receiverId = data.get_receiverId()
        equivalentSlope = math.tan(data.get_angleOfArrival())
        receiverPosition = receivers[receiverId].get_receiverPosition()

        updatedLine = Line(equivalentSlope, findYIntercept(
            equivalentSlope, receiverPosition))
        linesOfIntersection[receiverId] = str(updatedLine)
        print(str(updatedLine))
        directionLines = np.append(directionLines, updatedLine)

    # Calculate Intersection points
    numLines = len(directionLines)
    avgPosition = Position(0, 0)
    for i in range(numLines - 1):
        for j in range(i + 1, (numLines)):
            # TODO: Think more about case where the object is outside the 10x10 meter grid
            line1 = directionLines[i]
            line2 = directionLines[j]

            # Check if parallel lines
            if (line1.slope - line2.slope) == 0:
                continue

            xIntersection = (
                (line2.intercept - line1.intercept) / (line1.slope - line2.slope))
            intersectionPoint = Position(
                xIntersection, (line1.slope * xIntersection + line1.intercept))
            
            if isInBounds(intersectionPoint):
                intersectionPoints = np.append(
                    intersectionPoints, intersectionPoint)

                numIntersectionPoints = len(intersectionPoints)
                if numIntersectionPoints == 1:
                    avgPosition = intersectionPoint
                else:
                    avgPosition = Position((avgPosition.x * (numIntersectionPoints - 1)) / numIntersectionPoints + intersectionPoint.x / numIntersectionPoints,
                                           (avgPosition.y * (numIntersectionPoints - 1)) / numIntersectionPoints + intersectionPoint.y / numIntersectionPoints)

    # print(str(intersectionPoints[0]))
    # print("Avg intersection point: ", str(avgPosition))
    # Estimate position of transmitter
    maxRange = 100
    sigma = (maxRange / 3.0)
    # print ("sigma", sigma)
    for i in range(maxRange + 1):
        for j in range(maxRange + 1):
            distance = calculateMagnitude(Position(i, j))
            density = calculateGaussian(sigma, distance)

            for point in intersectionPoints:
                for update in range(4):
                    if update == 0:
                        y = int(point.y) + j
                        x = int(point.x) + i
                    elif update == 1:
                        y=int(point.y) - j
                        x=int(point.x) - i
                    elif update == 2:
                        y=int(point.y) + j
                        x=int(point.x) - i
                    elif update == 3:
                        y=int(point.y) - j
                        x=int(point.x) + i
                    
                    if isInBounds(Position(x,y)) and (update == 0 or (update == 1 and (i != 0 or j != 0)) or (update > 1 and (i != 0 and j != 0))):
                        positionMatrix[y, x] += density
                    
    maxVal = positionMatrix.max()
    # Take top 90%
    for i in range(positionMatrix.shape[0]):
        for j in range(positionMatrix.shape[1]):
            if positionMatrix[i][j] < 0.1*maxVal:
                positionMatrix[i][j] = 0

    # Show plot of position estimation
    generateHeatmap(positionMatrix)
    generateReceiverLines(directionLines)
    generateReceivers(receiverData)
    plt.show()
    positionMatrix = changeResolution(positionMatrix, resolution)
    positionMatrix = normalize(positionMatrix)
    print("changed resolution from Centimeters to ", resolution)
    generateHeatmap(positionMatrix)
    generateReceiverLines(directionLines, resolution)
    generateReceivers(receiverData, resolution)
    plt.show()
    
    return positionMatrix, linesOfIntersection

# retrieved data from the database
def fetchReceiverData():
    rawData = connectionMgr.getData()
    receiverData = []
    
    for data in reversed(rawData):
        # parsedData = json.loads(data)
        parsedData = data
        receiverId = parsedData["access_point"]
        timeStamp = parsedData["timestamp"]
        angleOfArrival = parsedData["angle_of_arrival"]
        
        # Escape if receiver data has been added already; can also add a time check to ensure "validity"
        if containsReceiverData(receiverData, receiverId):
            continue
        
        print("Adding new receiverData -> receiver: {0}  timeStamp: {1}  AoA: {2}".format(receiverId, timeStamp, angleOfArrival))
        receiverData.append(ReceiverData(int(receiverId)-1, timeStamp, math.radians(float(angleOfArrival))))
        

    return receiverData

def containsReceiverData(receiverData, receiverId):
    for data in receiverData:
        if (data.get_receiverId() == int(receiverId)-1):
            return True
    
    return False

def constructResponse(positionData, receiverData, startTime, endTime, linesOfIntersection):
    jsonData = {}
    jsonData['position'] = positionData.tolist()
    
    receiverInfo = {}
    for receiver in receiverData:
        receiverId = receiver.get_receiverId()
        receiverPositionObj = receivers[receiverId].get_receiverPosition()
        receiverPosition = [receiverPositionObj.get_x(), receiverPositionObj.get_y()]
        angleOfArrival = receiver.get_angleOfArrival()
        
        receiverKey = "receiver{0}".format(str(receiverId))
        receiverValue = {"position": receiverPosition, "angle_of_arrival": angleOfArrival, "line": linesOfIntersection[receiverId]}

        receiverInfo[receiverKey] = receiverValue
    
    jsonData['receivers'] = receiverInfo
    
    gridDimensions = [xDimension/100.0, yDimension/100.0]
    jsonData['gridDim'] = gridDimensions
    
    jsonData['timefrom'] = startTime
    jsonData['timeto'] = endTime
    
    with open('sampleOut.json', 'w') as outFile:
        json.dump(jsonData, outFile)
    
    return json.dumps(jsonData)
    
def isInBounds(position):
    return (position.x >= 0 and position.y >= 0) and (position.x < xDimension and position.y < yDimension)

def changeResolution(positionMatrix, units=Resolution.Meters):
    if units == Resolution.Centimeters:
        return positionMatrix
    
    resolution = int(units.value)
    newPositionMatrix = np.zeros(shape=(int(yDimension/resolution), int(xDimension/resolution)))
    numberPoints = int(yDimension/resolution) * int(xDimension/resolution)
    
    for i in range(int(yDimension/resolution)):
        for j in range(int(xDimension/resolution)):
            x = j * resolution
            y = i * resolution
            subMatrix = positionMatrix[y:(y+resolution-1), x:(x+resolution-1)]
            # print("imin", i , " imax", (i+resolution-1))
            # print("jmin", j , " jmax", (j+resolution-1))
            newPositionMatrix[i][j] = subMatrix.mean()
    
    return newPositionMatrix
    
def generateHeatmap(matrix):
    # print(positionMatrix)
    plt.imshow(matrix, cmap='jet')
    plt.gca().invert_yaxis()
    # plt.axis([0,10,0,10]) # Limit size of axis to 10x10
    plt.xlabel('x', fontsize=18)
    plt.ylabel('y', fontsize=18)
    plt.grid()
    plt.colorbar()

def generateReceiverLines(lines, resolution=Resolution.Centimeters):
    for line in lines:
        (x,y) = generateLine(line, resolution)
        plt.plot(x,y, 'w--', label=str(line))

def generateLine(line, resolution):
    resolutionScale = int(resolution.value)
    x = []; y= []
    
    for i in range(int(xDimension/resolutionScale)):
        yVal = line.calculateY(i*resolutionScale)/resolutionScale
        if yVal <= (yDimension/resolutionScale) and yVal >= 0:
            x.append(i)
            y.append(yVal)

    return (x,y)
    
def generateReceivers(receiverData, resolution=Resolution.Centimeters):
    resolutionScale = int(resolution.value)
    for data in receiverData:
        receiver = receivers[data.get_receiverId()]
        # print(str(receiver))
        receiverPositon = receiver.get_receiverPosition()
        x = receiverPositon.get_x()/resolutionScale
        y = receiverPositon.get_y()/resolutionScale
        plt.plot(x=x, y=y, style='wx', markersize = 100)
    pass

def findYIntercept(slope, position):
    return position.y - (slope * position.x)

def calculateMagnitude(point1, point2=Position(0, 0)):
    return math.sqrt(math.pow((point1.x - point2.x), 2) + math.pow((point1.y - point2.y), 2) + math.pow((point1.z - point2.z), 2))

def calculateGaussian(sigma, distanceFromIntersection):
    power = -math.pow(distanceFromIntersection, 2) / (2.0 * math.pow(sigma, 2.0))
    exponential = math.pow(math.e, power)
    divisor = (sigma * math.sqrt(2.0 * math.pi))
    if (divisor != 0):
        multiplier = 1.0 / divisor
    else:
        return 0
    return multiplier * exponential

def normalize(matrix):
    sum = np.sum(matrix)
    return matrix/sum
def config():
    # Dimension given in cm
    global xDimension
    global yDimension
    if TESTING == True:
        xDimension = 1000
        yDimension = 1000
        receiver1 = Receiver(0, Position(0, 0), 0)
        receiver2 = Receiver(1, Position(0, yDimension), 0)
        receiver3 = Receiver(2, Position(xDimension, yDimension), 0)
        receiver4 = Receiver(3, Position(xDimension, 0), 0)
    else:
        xDimension = 518
        yDimension = 335
        receiver1 = Receiver(0, Position(0, yDimension), 0)
        receiver2 = Receiver(1, Position(0, 0), 0)
        receiver3 = Receiver(2, Position(xDimension, yDimension), 0)
        receiver4 = Receiver(3, Position(xDimension, 0), 0)


    global receivers
    receivers = np.array([receiver1, receiver2, receiver3, receiver4])

    # global directionLines
    # directionLines = np.array([
    #     Line(DEFAULT_SLOPE, findYIntercept(DEFAULT_SLOPE, receivers[0].get_receiverPosition())),
    #     Line(DEFAULT_SLOPE, findYIntercept(DEFAULT_SLOPE, receivers[1].get_receiverPosition())),
    #     Line(DEFAULT_SLOPE, findYIntercept(DEFAULT_SLOPE, receivers[2].get_receiverPosition()))
    #     ])

main()
