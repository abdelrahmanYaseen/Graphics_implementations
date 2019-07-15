from Module.Camera import Camera
from Module.Color import Color
from Module.Ray import Ray
from Module.Scene import Scene
from Module.Sphere import Sphere
from Module.Triangle import Triangle
from Module.Vector import Vector
from Module.Image import Image
from Module.Utility import Utility
import sys, time

if len(sys.argv) < 4 :
    print("Usage: raytracer <camera file> <scene file> <option>\n")
    print("Options : \n")
    print("     1: (-) diffuse, (-) shadows, (-) specular, (-) reflection \n")
    print("     2: (+) diffuse, (-) specular, (-) shadows, (-) reflection \n")
    print("     3: (+) diffuse, (+) specular, (-) shadows, (-) reflection \n")
    print("     4: (+) diffuse, (+) specular, (+) shadows, (-) reflection \n")
    print("     5: (+) diffuse, (+) specular, (+) shadows, (+) reflection [one iteration] \n")
    print("     6: (+) diffuse, (+) specular, (+) shadows, (+) reflection [realistic]\n")
    exit()

def render(op,camera,scene,image):
    start_time = time.time()
    progress=0
    if op==1:
        for i in range(image.sizeX):
            for j in range(image.sizeY):
                progress+=1
                sys.stdout.write("\r" + "{:.1f}%".format(100*(progress)/(image.sizeX*image.sizeY)))
                sys.stdout.flush()
                ray = Ray.generate(camera,i,j)
                tmin=float('inf')
                closestObj=None
                for sphere in scene.spheres:
                    t = ray.intersect(sphere)
                    if t>1 :
                        if(t<tmin):
                            tmin=t
                            closestObj=sphere
                for triangle in scene.triangles:
                    t = ray.intersect(triangle)
                    if t>1 :
                        if(t<tmin):
                            tmin=t
                            closestObj=triangle
                if not closestObj==None :
                    # if isinstance(closestObj,Sphere):
                    p = ray.a + ray.b*tmin
                    image.colors[i][j]= closestObj.getTexture(p)
                    # else :
                        # image.colors[i][j]= closestObj.color
                    # image.colors[i][j]= closestObj.color
    #diffuse
    if op==2:
        for i in range(image.sizeX):
            for j in range(image.sizeY):
                progress+=1
                sys.stdout.write("\r" + "{:.1f}%".format(100*(progress)/(image.sizeX*image.sizeY)))
                sys.stdout.flush()
                ray = Ray.generate(camera,i,j)
                tmin=float('inf')
                closestObj=None
                for sphere in scene.spheres:
                    t = ray.intersect(sphere)
                    if t>1 :
                        if(t<tmin):
                            tmin=t
                            closestObj=sphere
                for triangle in scene.triangles:
                    t = ray.intersect(triangle)
                    if t>1 :
                        if(t<tmin):
                            tmin=t
                            closestObj=triangle

                diffuse=1
                if not closestObj==None :
                    for light in scene.lights:
                        normal = None
                        p = ray.a + ray.b*tmin
                        toLight = light.position - p
                        if isinstance(closestObj,Triangle):
                            normal = closestObj.normal
                        elif isinstance(closestObj,Sphere):
                            normal = p - closestObj.center
                            flag=0
                        normal = normal.normalize()
                        toLight = toLight.normalize()
                        diffuse = normal.dot(toLight)

                    image.colors[i][j]= closestObj.color * diffuse + closestObj.color*scene.ambient
    #specular
    if op==3:
        for i in range(image.sizeX):
            for j in range(image.sizeY):
                progress+=1
                sys.stdout.write("\r" + "{:.1f}%".format(100*(progress)/(image.sizeX*image.sizeY)))
                sys.stdout.flush()
                ray = Ray.generate(camera,i,j)
                tmin=float('inf')
                closestObj=None
                for sphere in scene.spheres:
                    t = ray.intersect(sphere)
                    if t>1 :
                        if(t<tmin):
                            tmin=t
                            closestObj=sphere
                for triangle in scene.triangles:
                    t = ray.intersect(triangle)
                    if t>1 :
                        if(t<tmin):
                            tmin=t
                            closestObj=triangle

                diffuse=0
                specular=0
                if not closestObj==None :
                    for light in scene.lights:
                        # print(light.position)
                        ldiffuse=0
                        lspecular=0
                        normal = None
                        p = ray.a + ray.b*tmin
                        toLight = light.position - p
                        view = ray.b*-1
                        view = view.normalize()
                        toLight = toLight.normalize()

                        half = view+toLight

                        if isinstance(closestObj,Triangle):
                            normal = closestObj.normal
                        elif isinstance(closestObj,Sphere):
                            normal = p - closestObj.center
                            flag=0

                        normal = normal.normalize()
                        half= half.normalize()

                        lspecular= normal.dot(half)
                        ldiffuse = normal.dot(toLight)

                        if lspecular < 0 : lspecular = 0
                        else:
                            lspecular = lspecular**150

                        specular+=lspecular*(light.intensity**0.005)
                        diffuse+=ldiffuse
                    image.colors[i][j]= closestObj.color*diffuse  + Color(1,1,1)*(specular)
    #+shadows
    if op==4:
        for i in range(image.sizeX):
            for j in range(image.sizeY):
                ray = Ray.generate(camera,i,j)
                tmin, closestObj = Utility.getClosestIntersection(ray,scene,1)
                progress+=1
                sys.stdout.write("\r" + "{:.1f}%".format(100*(progress)/(image.sizeX*image.sizeY)))
                sys.stdout.flush()
                diffuse=0
                specular=0
                if not closestObj==None :
                    for light in scene.lights:
                        p = ray.a + ray.b*tmin
                        lightDistance=Vector.distance(p,light.position)
                        lspecular,ldiffuse = Utility.shading(p,closestObj,ray,scene,light)
                        specular+=lspecular*(light.intensity**0.005)
                        diffuse+=light.intensity*ldiffuse/(lightDistance**2)
                    r=closestObj.color*diffuse + Color(1,1,1)*(specular) + closestObj.color*scene.ambient

                    image.colors[i][j] = r
    #+reflection (1 iteration)
    if op==5:
        for i in range(image.sizeX):
            for j in range(image.sizeY):
                ray = Ray.generate(camera,i,j)
                tmin, closestObj = Utility.getClosestIntersection(ray,scene,1)
                progress+=1
                sys.stdout.write("\r" + "{:.1f}%".format(100*(progress)/(image.sizeX*image.sizeY)))
                sys.stdout.flush()
                diffuse=0
                specular=0
                if not closestObj==None :
                    for light in scene.lights:
                        p = ray.a + ray.b*tmin
                        lightDistance=Vector.distance(p,light.position)
                        lspecular,ldiffuse = Utility.shading(p,closestObj,ray,scene,light)
                        specular+=lspecular*(light.intensity**0.005)
                        diffuse+=light.intensity*ldiffuse/(lightDistance**2)
                        # print(scene.ambient)
                    r=closestObj.color*diffuse + Color(1,1,1)*(specular) + closestObj.color*scene.ambient

                    image.colors[i][j] = r
                    #REFLECTION
                    closestObj.texture=closestObj.color # to remove the texture
                    rRef= Utility.getRef2(ray,p,closestObj,scene,closestObj.reflectionFactor,r)
                    image.colors[i][j] = rRef*closestObj.reflectionFactor + r

    if op==6:
        for i in range(image.sizeX):
            for j in range(image.sizeY):
                ray = Ray.generate(camera,i,j)
                tmin, closestObj = Utility.getClosestIntersection(ray,scene,1)
                progress+=1
                sys.stdout.write("\r" + "{:.1f}%".format(100*(progress)/(image.sizeX*image.sizeY)))
                sys.stdout.flush()
                diffuse = 0
                specular = 0
                if not closestObj == None :
                    for light in scene.lights:
                        p = ray.a + ray.b*tmin
                        lightDistance=Vector.distance(p,light.position)
                        lspecular,ldiffuse = Utility.shading(p,closestObj,ray,scene,light)
                        specular+=lspecular
                        diffuse+=light.intensity*ldiffuse/(lightDistance**2)
                    r=closestObj.getTexture(p)*diffuse + Color(1,1,1)*(specular)*closestObj.specularFactor + closestObj.getTexture(p)*scene.ambient

                    rRef= Utility.getRef1(ray,p,closestObj,scene,closestObj.reflectionFactor,r)
                    image.colors[i][j] = rRef*closestObj.reflectionFactor + r


    print("\nimage rendered in {:.2f} seconds".format(time.time()-start_time))
    Scene.writeImage(camera.outFile,image)
    print("image file is ready [{}]".format(camera.outFile))

    import subprocess
    opener ="open" if sys.platform == "darwin" else "xdg-open"
    subprocess.call([opener, camera.outFile])


camera = Camera.fromFile(sys.argv[1])
scene = Scene.fromFile(sys.argv[2])

if (len(sys.argv)>3):
    op=int(sys.argv[3])
print(camera)
print(scene)
print([str(v) for v in scene.lights])

image = Image.getBlankImage(camera.sizeX,camera.sizeY,0.7,0.7,0.7)
render(op,camera,scene,image)



# Uncomment for animation
# import time
# for i in range(100):
#     image = Image.getBlankImage(camera.sizeX,camera.sizeY,0.7,0.7,0.7)
#     # string = "image"+str(i)+".ppm"
#     render(1,camera,scene,image,"animation.ppm")
#     camera.position=camera.position.rotate(-0.1,'y')
#     camera.gaze=camera.gaze.rotate(-0.1,'y')
#     camera.gaze=camera.gaze.normalize()
    # time.sleep(0.5)
