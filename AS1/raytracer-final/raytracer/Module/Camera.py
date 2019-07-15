from Module.Vector import Vector

class Camera:
        def __init__(self,position,gaze,v,u,l,r,b,t,d,sizeX ,sizeY,outFile="out.ppm"):
            self.position=position
            self.gaze=gaze
            self.v=v
            self.u=u
            self.r=r
            self.l=l
            self.b=b
            self.t=t
            self.d=d
            self.sizeX=int(sizeX)
            self.sizeY=int(sizeY)
            self.pixelW= abs(l-r)/float(sizeX)
            self.pixelH=abs(t-b)/float(sizeY)
            self.outFile=outFile
        @classmethod
        def fromFile(cls,filename):
            file = open(filename,"r")
            lines = file.readlines()

            position = Vector()
            gaze = Vector()
            v = Vector()
            u = Vector()

            input=[]
            input.extend(lines[0].split(' '))
            position.x=float(input[0])
            position.y=float(input[1])
            position.z=float(input[2])


            input=[]
            input.extend(lines[1].split(' '))
            gaze.x=float(input[0])
            gaze.y=float(input[1])
            gaze.z=float(input[2])


            input=[]
            input.extend(lines[2].split(' '))
            v.x=float(input[0])
            v.y=float(input[1])
            v.z=float(input[2])


            u=gaze.cross(v)

            u=u.normalize()
            v=v.normalize()
            gaze=gaze.normalize()


            input=[]
            input.extend(lines[3].split(' '))
            l=float(input[0])
            r=float(input[1])
            b=float(input[2])
            t=float(input[3])
            d=float(input[4])
            sizeX=float(input[5])
            sizeY=float(input[6])

            outFile = lines[4].replace("\n","")
            camera =  cls(position,gaze,v,u,l,r,b,t,d,sizeX,sizeY,outFile)
            return camera

        def __str__(self):
            return "[Camera]\n position:{}\n gaze:{}\n v:{}\n u:{}\n r:{}\n l:{}\n b:{}\n t:{}\n d:{}\n sizeX:{}\n sizeY:{}\n pixelW:{}\n pixelH :{}\n ".format(self.position,self.gaze,self.v,self.u,self.r,self.l,self.b,self.t,self.d,self.sizeX,self.sizeY,self.pixelW,self.pixelH)
