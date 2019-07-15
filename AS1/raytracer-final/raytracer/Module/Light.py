class Light:
    def __init__(self,position,intensity):
        self.position=position
        self.intensity=intensity

    def __str__(self):
        return "[Light] position : {} intensity: {}".format(str(self.position),self.intensity)
