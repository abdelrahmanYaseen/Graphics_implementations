from Image import Image
from Color import Color
import math
class Sphere:
	def __init__(self,center,r,color=Color(0.5,0.5,0.5),texture=None,specularFactor=1,reflectionFactor=0):
		self.center=center
		self.r=r
		self.color=color
		self.specularFactor=specularFactor
		self.reflectionFactor=reflectionFactor
		if not texture==None:
			self.texture= Image.fromFile(texture)
		else:
			self.texture = self.color

	def getTexture(self,p=None):
		if not isinstance(self.texture,Color):
			p=p-self.center
			val = p.y/self.r
			if val < -1:
				val = -1
			elif val > 1:
				val = 1
			theta = math.acos(val)

			if p.x ==0 :
				if p.z > 0:
					phi = math.pi/2
				else :
					phi = -1*math.pi/2
			else :
				phi = math.atan(p.z/p.x)

			u = (-phi + math.pi) / (2 * math.pi)
			v = theta/math.pi

			j = math.floor(v*self.texture.sizeY)
			i = math.floor(u*self.texture.sizeX)

			if i > self.texture.sizeX-1:
				i = self.texture.sizeX -1
			if j > self.texture.sizeY-1:
				j = self.texture.sizeY -1
			return self.texture.colors[i][j]
		else :
			return self.color
	def __str__(self):
		return "Sphere : c=[{},{},{}] r={} color=[{},{},{}]".format(self.center.x,self.center.y,self.center.z,self.r,self.color.r,self.color.g,self.color.b)
