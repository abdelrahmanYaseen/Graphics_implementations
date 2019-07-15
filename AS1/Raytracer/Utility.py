from Ray import Ray
from Vector import Vector
from Sphere import Sphere
from Triangle import Triangle
from Color import Color
import math
class Utility:
    @classmethod
    def floatToColor(cls,f):
        c = f * 255
        if  c > 255:
            return  int(255)
        elif c < 0:
            return int(0)
        return int(c)
    @classmethod
    def convert(cls,c):
        if  c > 255:
            return  int(255)
        elif c < 0:
            return int(0)
        return round(c)

    @classmethod
    def getClosestIntersection(cls,ray,scene,threshold):
        tmin=float('inf')
        closestObj=None
        for sphere in scene.spheres:
            t = ray.intersect(sphere)
            if t>threshold :
                if(t<tmin):
                    tmin=t
                    closestObj=sphere
        for triangle in scene.triangles:
            t = ray.intersect(triangle)
            if t>threshold :
                if(t<tmin):
                    tmin=t
                    closestObj=triangle
        return [tmin,closestObj]

    @classmethod
    def shading(cls,p,closestObj,ray,scene,light):
        ldiffuse=0
        lspecular=0
        toLight = light.position - p
        sr=Ray(p,toLight)
        lightDistance = toLight.length()
        toLight = toLight.normalize()
        shadow = 0

        for sphere in scene.spheres:
            t = sr.intersect(sphere)
            if(t>0.00001 and t<2):
                shadow=1
                break
        if not shadow:
            for triangle in scene.triangles:
                t = sr.intersect(triangle)
                if(t>0.000001 and t<1):
                    shadow=1
                    break

        if not shadow :

            normal = None
            view = ray.b*-1
            view = view.normalize()


            half = view+toLight

            if isinstance(closestObj,Triangle):
                normal = closestObj.normal
            elif isinstance(closestObj,Sphere):
                normal = p - closestObj.center

            normal = normal.normalize()
            half= half.normalize()

            lspecular= normal.dot(half)
            ldiffuse = normal.dot(toLight)

            if lspecular < 0 : lspecular = 0
            else:
                lspecular = lspecular**100

        return [lspecular,ldiffuse]

    @classmethod
    def getRef1(cls,ray,p,obj,scene,factor,color):

        # print("Call  : ",factor)
        if(factor<0.25):
            return color

        if isinstance(obj,Triangle):
            normal = obj.normal
        elif isinstance(obj,Sphere):
            normal = p - obj.center
        normal=normal.normalize()
        d = p-ray.a
        reflectionRay = Ray(p,d - (normal*d.dot(normal)*2))

        tminRef, closestObjRef = Utility.getClosestIntersection(reflectionRay,scene,0.001)
        diffuseRef=0
        specularRef=0
        rRef=0
        pRef=None
        if not closestObjRef==None :
            for light in scene.lights:
                pRef = reflectionRay.a + reflectionRay.b*tminRef
                lightDistance=Vector.distance(pRef,light.position)
                lrspecular,lrdiffuse = Utility.shading(pRef,closestObjRef,reflectionRay,scene,light)
                specularRef+=lrspecular*(light.intensity**0.001)
                diffuseRef+=light.intensity*lrdiffuse/(lightDistance**2)
            rRef=closestObjRef.getTexture(pRef)*diffuseRef + Color(1,1,1)*(specularRef)*closestObjRef.specularFactor + closestObjRef.getTexture(pRef)*scene.ambient
            color= rRef*factor + color
            return cls.getRef1(reflectionRay,pRef,closestObjRef,scene,closestObjRef.reflectionFactor*0.8,color)
        return color


    #one iteration
    @classmethod
    def getRef2(cls,ray,p,obj,scene,factor,color):

        # print("Call  : ",factor)
        if(factor<0.25):
            return color

        if   isinstance(obj,Triangle):
            normal = obj.normal
        elif isinstance(obj,Sphere):
            normal = p - obj.center
        normal=normal.normalize()
        d = p-ray.a
        reflectionRay = Ray(p,d - (normal*d.dot(normal)*2))

        tminRef, closestObjRef = Utility.getClosestIntersection(reflectionRay,scene,0.001)
        diffuseRef=0
        specularRef=0
        rRef=0
        pRef=None
        if not closestObjRef==None :
            for light in scene.lights:
                pRef = reflectionRay.a + reflectionRay.b*tminRef
                lightDistance=Vector.distance(pRef,light.position)
                lrspecular,lrdiffuse = Utility.shading(pRef,closestObjRef,reflectionRay,scene,light)
                specularRef+=lrspecular*(light.intensity**0.001)
                diffuseRef+=light.intensity*lrdiffuse/(lightDistance**2)
            rRef=closestObjRef.getTexture(pRef)*diffuseRef + Color(1,1,1)*(specularRef)*closestObjRef.specularFactor + closestObjRef.getTexture(pRef)*scene.ambient
            color= rRef*factor + color
            return cls.getRef2(reflectionRay,pRef,closestObjRef,scene,0,color)
        return color
