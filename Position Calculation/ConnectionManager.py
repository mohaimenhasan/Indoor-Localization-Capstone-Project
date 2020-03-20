import requests
import json
from  enum import Enum

class RequestType(Enum):
    GET = 1
    POST = 2
    
class ConnectionManager:
    def getData(self, year=None, month=None, day=None, timeStart=None, timeEnd=None):
        rawData = None
        
        if year != None and month != None and day != None and timeFrom != None and timeTo != None:
            data = [
            ('year', year),
            ('month', month),
            ('day', day),
            ('timeFrom', timeFrom),
            ('timeTo', timeTo)
            ]
            url = "http://localhost:8888/analysis/byTime"
            rawData = self.__executeApiCall(RequestType.GET, url, data)
        else:
            url = "http://localhost:8888/analysis/allData"
            rawData = self.__executeApiCall(RequestType.GET, url)
            
        return rawData
    
    def sendPositionData(self, data):
        url = "http://localhost:8888/analysis/storeAnalysisData"
        response = self.__executeApiCall(RequestType.POST, url, {'data':data})
    
    def __executeApiCall(self, callType, url, data=None):
        if callType == RequestType.GET:
            if data != None:
                req = requests.get(url = url, data = data)
            else:
                req = requests.get(url = url)
        elif callType == RequestType.POST:
            return 0 # here just to prevent call in testing
            req = requests.post(url = url, data = data)
        
        if req:
            
            return req.json()
        else:
            print("Error in exectuting api call: {}".format(str(url)))
            return None