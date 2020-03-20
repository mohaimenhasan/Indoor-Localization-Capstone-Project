class Position:
    def __init__(self, x = None, y = None, z = 0):
        self.x = x
        self.y = y
        self.z = z
    
    def get_x(self):
        return self.x
    
    def get_y(self):
        return self.y
    
    def get_z(self):
        return self.z
    
    def __str__(self):
        return "({0},{1},{2})".format(str(self.x), str(self.y), str(self.z))