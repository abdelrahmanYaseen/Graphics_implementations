class Vector:
	def __init__(self,x=0,y=0,z=0):
		self.x=x
		self.y=y
		self.z=z


	def length(self):
	 	return (self.x**2+self.y**2+self.z**2)**0.5


	def cross(self, v):
		x = self.y*v.z-self.z*v.y
		y = self.x*v.z-self.z*v.x
		z = self.x*v.y-self.y*v.x
		return Vector(x,-y,z)


	def dot(self,v):
			return self.x*v.x+self.y*v.y+self.z*v.z


	def normalize(self):
		v = Vector(self.x,self.y,self.z)
		l = v.length()
		v.x=v.x/l
		v.y=v.y/l
		v.z=v.z/l
		return v

	def __add__(self,v):
		new = Vector()
		new.x=self.x+v.x
		new.y=self.y+v.y
		new.z=self.z+v.z
		return new

	def __sub__(self,v):
		new = Vector()
		new.x=self.x-v.x
		new.y=self.y-v.y
		new.z=self.z-v.z
		return new

	def __mul__(self,v):
		if isinstance(v,Vector):
			new = Vector()
			new.x=self.x*v.x
			new.y=self.y*v.y
			new.z=self.z*v.z
			return new
		else:
			new = Vector()
			new.x=self.x*v
			new.y=self.y*v
			new.z=self.z*v
			return new


	def __equ__(self,v):
		segma = 0.0000001
		return abs(self.x-v.x)<segma and abs(self.y-v.y)<segma and abs(self.z-v.z)<segma

	def __str__(self):
		return "[{},{},{}]".format(self.x,self.y,self.z)

	@classmethod
	def distance(cls, v1,v2):
		return ( (v1.x-v2.x)**2 + (v1.y-v2.y)**2 + (v1.z-v2.z)**2 )**0.5

	def rotate(self,rad,axis='y'):
		import math
		if axis == 'z' :
			v = [[math.cos(rad),-math.sin(rad),0,0],[math.sin(rad),math.cos(rad),0,0],[0,0,1,0],[0,0,0,1]]
		elif axis == 'y' :
			v = [[math.cos(rad),0,math.sin(rad),0],[0,1,0,0],[-math.sin(rad),0,math.cos(rad),0],[0,0,0,1]]
		else :
			v = [[1,0,0,0],[math.cos(rad),-math.sin(rad),0,0],[0,math.sin(rad),math.cos(rad),0],[0,0,0,1]]


		matrix = [[self.x,self.y,self.z,1]]
		result = [[0,0,0,0]]

		for i in range(len(matrix)):
		   # iterate through columns of v
		   for j in range(len(v[0])):
		       # iterate through rows of v
		       for k in range(len(v)):
		           result[i][j] += matrix[i][k] * v[k][j]

		return Vector(result[0][0],result[0][1],result[0][2])
