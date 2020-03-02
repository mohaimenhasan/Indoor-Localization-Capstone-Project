from Position import Position
from Receiver import Receiver
from ReceiverData import ReceiverData
from Line import Line
# import Position
# import Receiver
# import ReceivedData

import numpy as np
import matplotlib.pyplot as plt
import math

TESTING = True
DEFAULT_SLOPE = (math.pi / 4)

def main():
    config()
    
    if TESTING:
        r1Data =  ReceiverData(0, 1, math.radians(30.96))
        r2Data =  ReceiverData(1, 1, math.radians(-63.43))
        r3Data =  ReceiverData(2, 1, (math.radians(56.31) - math.pi))
        
        # r1Data =  ReceiverData(0, 1, math.radians(47))
        # r2Data =  ReceiverData(1, 1, math.radians(-45))
        # r3Data =  ReceiverData(2, 1, (math.radians(44) - math.pi))
        calculatePosition(np.array([r1Data, r2Data, r3Data]))
    else:
        calculatePosition(fetchReceiverData())
        
# function: calculatePosition
# input(s):
# 1. receiverData
#       structure: [[timeStamp, AoA], ]
#       note: AoA is in range of [-pi, pi]
# output(s): data matrix representing location estimation of transmitter

def calculatePosition(receiverData):    
    positionMatrix = np.zeros((yDimension,xDimension))
    intersectionPoints = np.array(np.shape(0))
    
    # Update Lines 
    directionLines = np.array(np.shape(0))
    for i in range(len(receiverData)):
        data = receiverData[i]
        receiverId = data.get_receiverId()
        equivalentSlope = math.tan(data.get_angleOfArrival())
        receiverPosition = receivers[receiverId].get_receiverPosition()
        
        updatedLine = Line(equivalentSlope, findYIntercept(equivalentSlope, receiverPosition))
        print(str(updatedLine))
        directionLines = np.append(directionLines, updatedLine)
    
    # Calculate Intersection points
    numLines = len(directionLines)
    avgPosition = Position(0,0)
    for i in range(numLines-1):
        for j in range(i+1, (numLines)):
            # TODO: Think more about case where the object is outside the 10x10 meter grid
            line1 = directionLines[i]
            line2 = directionLines[j]
            
            # Check if parallel lines
            if (line1.slope - line2.slope) == 0:
                continue
                
            xIntersection = ((line2.intercept - line1.intercept)/(line1.slope - line2.slope))
            intersectionPoint = Position(xIntersection,(line1.slope * xIntersection + line1.intercept))
            
            intersectionPoints = np.append(intersectionPoints, intersectionPoint)
            
            numIntersectionPoints = len(intersectionPoints)
            if numIntersectionPoints == 1:
                avgPosition = intersectionPoint
            else:
                avgPosition = Position((avgPosition.x * (numIntersectionPoints-1))/numIntersectionPoints + intersectionPoint.x/numIntersectionPoints, (avgPosition.y * (numIntersectionPoints-1))/numIntersectionPoints + intersectionPoint.y/numIntersectionPoints)
    
    print(str(intersectionPoints[0]))
    print("Avg intersection point: ", str(avgPosition))
    # Estimate position of transmitter
    maxRange = 250
    sigma = (maxRange/3)
    # print ("sigma", sigma)
    for i in range(-maxRange, maxRange+1):
        for j in range(-maxRange, maxRange+1):
            distance = calculateMagnitude(Position(i, j))
            density = calculateGaussian(sigma, distance)
            
            for point in intersectionPoints:
                y=int(point.y + j)
                x=int(point.x + i)
                if (x >= 0 and y >= 0) and (x < xDimension and y < yDimension):
                    positionMatrix[y,x] += density
                # y=int(point.y - j)
                # x=int(point.x - i)
                # if (((i == 0) and (j ==0)) == False and x < 0 or y < 0):
                #     y=int(point.y - j)
                #     x=int(point.x - i)
                #     positionMatrix[y,x] += density
    
    # Show plot of position estimation
    # print(positionMatrix)
    plt.imshow(positionMatrix, cmap='hot')
    plt.gca().invert_yaxis()
    # plt.axis([0,10,0,10]) # Limit size of axis to 10x10
    plt.xlabel('x coordinate', fontsize=18)
    plt.ylabel('y coordinate', fontsize=18)
    plt.show()
    return 0

# retrieved data from the database
def fetchReceiverData():
    # In here we can use the receiver angle offset to standardize all 0 degree angles to be parallel to the x axis
    return 0
    
def findYIntercept(slope, position):
    return position.y - (slope * position.x)
    
def calculateMagnitude(point1, point2 = Position(0,0)):
    return math.sqrt(math.pow((point1.x-point2.x),2)+math.pow((point1.y-point2.y),2)+math.pow((point1.z-point2.z),2))

def calculateGaussian(sigma, distanceFromIntersection):
     power = -math.pow(distanceFromIntersection, 2) / (2.0*math.pow(sigma,2.0))
     exponential = math.pow(math.e, power)
     multiplier = 1.0/(sigma * math.sqrt(2.0*math.pi))
     return multiplier * exponential

def config():
    # Dimension given in dm
    global xDimension
    global yDimension
    xDimension = 1000
    yDimension = 1000
    
    receiver1 = Receiver(0, Position(0,0), 0)
    receiver2 = Receiver(1, Position(0,yDimension), 0)
    receiver3 = Receiver(2, Position(xDimension,yDimension), 0)
    
    global receivers
    receivers = np.array([receiver1, receiver2, receiver3])
    
    # global directionLines
    # directionLines = np.array([
    #     Line(DEFAULT_SLOPE, findYIntercept(DEFAULT_SLOPE, receivers[0].get_receiverPosition())), 
    #     Line(DEFAULT_SLOPE, findYIntercept(DEFAULT_SLOPE, receivers[1].get_receiverPosition())),
    #     Line(DEFAULT_SLOPE, findYIntercept(DEFAULT_SLOPE, receivers[2].get_receiverPosition()))
    #     ])
    
    
main()