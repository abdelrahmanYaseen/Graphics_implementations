from Module.Color import Color
from Module.Vector import Vector
from Module.Sphere import Sphere
from Module.Triangle import Triangle
from Module.Light import Light
from Module.Utility import Utility


class Scene:
        def __init__(self,spheres,triangles,lights,ambient=0.1):
            self.spheres = spheres
            self.triangles = triangles
            self.lights = lights
            self.ambient=ambient

        @classmethod
        def fromFile(cls,filename):
            file = open(filename,"r")
            lines = file.readlines()
            counter=0
            ambient=float(lines[counter].replace(" ","").replace("\n",""))
            counter+=1
            numerOfSpheres=int(lines[counter])

            spheres=[]
            for i in range(numerOfSpheres):
                center = Vector()
                color = Color()
                radius = 0

                counter+=1
                center.x=float(lines[counter].split(" ")[0])
                center.y=float(lines[counter].split(" ")[1])
                center.z=float(lines[counter].split(" ")[2])

                counter+=1
                radius=float(lines[counter])
                texture = None
                counter+=1

                if str.isdigit(lines[counter].replace(" ","").replace("\n","")):
                    color.r=float(lines[counter].split(" ")[0])
                    color.g=float(lines[counter].split(" ")[1])
                    color.b=float(lines[counter].split(" ")[2])
                else:
                    texture = lines[counter].replace("\n","").strip()

                counter+=1
                specFactor=float(lines[counter].split(" ")[0])
                refFactor=float(lines[counter].split(" ")[1])
                spheres.append(Sphere(center,radius,color,texture,specFactor,refFactor))


            counter+=1
            triangles=[]
            numberOfTriangles=int(lines[counter])
            for i in range(numberOfTriangles):
                a = Vector()
                b = Vector()
                c = Vector()
                color = Color()

                counter+=1
                a.x=float(lines[counter].split(" ")[0])
                a.y=float(lines[counter].split(" ")[1])
                a.z=float(lines[counter].split(" ")[2])

                counter+=1
                b.x=float(lines[counter].split(" ")[0])
                b.y=float(lines[counter].split(" ")[1])
                b.z=float(lines[counter].split(" ")[2])

                counter+=1
                c.x=float(lines[counter].split(" ")[0])
                c.y=float(lines[counter].split(" ")[1])
                c.z=float(lines[counter].split(" ")[2])

                counter+=1
                color.r=float(lines[counter].split(" ")[0])
                color.g=float(lines[counter].split(" ")[1])
                color.b=float(lines[counter].split(" ")[2])

                triangles.append(Triangle(a,b,c,color))

            counter+=1
            lights=[]
            numberOfLights=int(lines[counter])
            counter+=1
            for i in range(numberOfLights):
                position = Vector()
                position.x=float(lines[counter].split(" ")[0])
                position.y=float(lines[counter].split(" ")[1])
                position.z=float(lines[counter].split(" ")[2])
                counter+=1
                intensity=int(lines[counter])
                light = Light(position,intensity)
                lights.append(light)
                counter+=1

            return Scene(spheres,triangles,lights,ambient)

        @classmethod
        def writeImage(cls,filename,image):

            outFile = open(filename,"w")
            outFile.write("P3\n")
            outFile.write("# {}\n".format(filename))

            outFile.write("{} {}\n".format(image.sizeX,image.sizeY))

            outFile.write("255\n")
            for j in range(image.sizeY-1,-1,-1):
                for i in range(image.sizeX):
                    outFile.write("{} {} {} ".format(str(Utility.floatToColor(image.colors[i][j].r)),str(Utility.floatToColor(image.colors[i][j].g)),str(Utility.floatToColor(image.colors[i][j].b))))
                outFile.write("\n")

            outFile.close()

        def __str__(self):
            return "[Scene]\n #triangles:{}\n#shperes:{}\n#lights:{}".format(len(self.triangles),len(self.spheres),len(self.lights))
