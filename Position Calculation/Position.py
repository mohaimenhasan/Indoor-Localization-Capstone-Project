class Position:
    def __init__(self, x = None, y = None, z = 0):
        self.x = x
        self.y = y
        self.z = z
    
    def __str__(self):
        return "({0},{1},{2})".format(str(self.x), str(self.y), str(self.z))