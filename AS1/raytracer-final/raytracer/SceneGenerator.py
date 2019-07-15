outFile=input("Enter the name of the desired output file:")
file = open(outFile,"w")
userInput=input("Enter the value of the ambient (0-1 scale): ")
file.write("{}\n".format(userInput))
numberOfSpheres=int(input("Enter the number of spheres:"))
file.write("{}\n".format(numberOfSpheres))
for i in range(numberOfSpheres):
    print("~~~~~~~~~~~~~~~~({})~~~~~~~~~~~~~~~~".format(i+1))
    userInput = input("Enter a space-seperated center coordinates of shpere #{}: ".format(i+1))
    file.write("{}\n".format(userInput))
    userInput=float(input("Enter the radius: "))
    file.write("{}\n".format(userInput))
    userInput = input("Enter the name of the file of the texture, or a sapace-separated rgb color values (0-1 scale): ")
    file.write("{}\n".format(userInput))
    userInput=input("Enter the specular factor, reflection factor (space-spearated): ")
    file.write("{}\n".format(userInput))

numberOfTriangles=int(input("Enter the number of triangles: "))
file.write("{}\n".format(numberOfTriangles))

for i in range(numberOfTriangles):
    print("~~~~~~~~~~~~~~~~({})~~~~~~~~~~~~~~~~".format(i+1))
    userInput = input("Enter a space-seperated center coordinates of the first vertex of triangle #{}: ".format(i+1))
    file.write("{}\n".format(userInput))
    userInput = input("Enter a space-seperated center coordinates of the second vertex of triangle #{}: ".format(i+1))
    file.write("{}\n".format(userInput))
    userInput = input("Enter a space-seperated center coordinates of the third vertex of triangle #{}: ".format(i+1))
    file.write("{}\n".format(userInput))
    userInput = input("Enter the name of the file of the texture, or a sapace-separated rgb color values (0-1 scale): ")
    file.write("{}\n".format(userInput))


numberOfLights=int(input("Enter the number of lights: "))
file.write("{}\n".format(numberOfLights))
for i in range(numberOfLights):
    print("~~~~~~~~~~~~~~~~({})~~~~~~~~~~~~~~~~".format(i+1))
    userInput = input("Enter a space-seperated center coordinates of light #{}: ".format(i+1))
    file.write("{}\n".format(userInput))
    userInput = input("Enter the intenssity: ")
    file.write("{}\n".format(userInput))


print("File '{}' has been generated successfully.".format(outFile))
