class Line:
    def __init__(self, slope = 0, intercept = 0):
        self.slope = slope
        self.intercept = intercept
    
    def __str__(self):
        return "y = {0}*x + {1}".format(str(self.slope), str(self.intercept))
        
    def calculateX(self, y):
        return (y - self.intercept)/self.slope
    
    def calculateY(self, x):
        return self.slope * x + self.intercept

    def returnLineProperties(self):
        return {
            "slope": self.slope,
            "intercept": self.intercept
        }