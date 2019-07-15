from Module.Color import Color

class Image:
    def __init__(self,sizeX,sizeY,colors):
        self.sizeX=int(sizeX)
        self.sizeY=int(sizeY)
        self.colors=colors
    @classmethod
    def getBlankImage(cls,sizeX,sizeY,r=0,g=0,b=0):
        c = Color(r,g,b)
        colors = []
        for i in range (int(sizeX)):
            row = []
            for j in range(int(sizeY)):
                row.append(c)
            colors.append(row)

        return cls(sizeX,sizeY,colors)
    @classmethod
    def getCheckerImage(cls,ImageSize,sqSize):
        from math import floor
        nx = ImageSize/sqSize
        ny = ImageSize/sqSize
        colors = []
        for i in range (ImageSize):
            row = []
            for j in range(int(nx)):
                for px in range(sqSize):
                    if((floor(i/sqSize)+j)%2):
                        row.append(Color(1,1,1))
                    else:
                        row.append(Color(0,0,0))
            colors.append(row)
        return cls(ImageSize,ImageSize,colors)
    @classmethod
    def fromFile(cls,filename):
        file = open(filename,"r")
        lines = file.readlines()
        
        sizeX=int(lines[2].split(" ")[0])
        sizeY=int(lines[2].split(" ")[1])
        max=int(lines[3])
        linecounter=4
        colors = []
        for i in range(int(sizeY)):
            counter=0
            filecolors = lines[linecounter].split(" ")
            row = []
            for j in range(int(sizeX)):
                row.append(Color(float(filecolors[counter])/max,float(filecolors[counter+1])/max,float(filecolors[counter+2])/max))
                # print("",linecounter," ", len(filecolors)," ",filecolors[counter]," ",filecolors[counter+1]," ",filecolors[counter+2])
                counter+=3
            linecounter+=1
            colors.append(row)
        # print("DONE")
        image = cls(sizeX,sizeY,colors)
        # print(image)
        return image

    def __str__(self):
        return "X:{} Y:{} COLORS:{}".format(self.sizeX,self.sizeY,len(self.colors[0]))
