from Vector import Vector
from Color import Color
import math
class Triangle:
	def __init__(self,a,b,c,color,texture=None,specularFactor=0.6,reflectionFactor=0.6):
		self.a=a
		self.b=b
		self.c=c
		self.color=color
		self.normal=((a-b).cross(a-c))
		self.texture = self.color
		self.specularFactor=specularFactor
		self.reflectionFactor=reflectionFactor

	def getTexture(self,p=None):
		if not isinstance(self.texture,Color):
			areaABC = self.normal.dot((self.b - self.a).cross(self.c - self.a))
			areaPBC = self.normal.dot((self.b - p).cross(self.c - p))
			areaPCA = self.normal.dot((self.c - p).cross(self.a - p))

			# exit()
			bary_a = areaPBC / areaABC
			bary_b = areaPCA / areaABC
			bary_c = 1.0 - bary_a - bary_b

			a = Vector(0,0,0)
			b = Vector(self.texture.sizeX,0,0)
			c = Vector(0,self.texture.sizeY,0)

			u = a * bary_a + b * bary_b + c * bary_c

			return self.texture.colors[math.floor(u.x)][math.floor(u.y)]
		else :
			return self.color

	def __str__(self):
		return "Triangle : a=[{},{},{}] b=[{},{},{}] c=[{},{},{}] color=[{},{},{}]".format(self.a.x,self.a.y,self.a.z,self.b.x,self.b.y,self.b.z,self.c.x,self.c.y,self.c.z,self.color.r,self.color.g,self.color.b)
