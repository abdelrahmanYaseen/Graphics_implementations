class Color:
	def __init__(self,r=0,g=0,b=0):
		self.r=r
		self.g=g
		self.b=b

	def __str__(self):
		return "r:{} g:{} b:{}".format(self.r,self.g,self.b)

	def __mul__(self,i):

		if i<0:
			i=0
		return Color(self.r*i,self.g*i,self.b*i)

	def __eq__(self,i):
		return self.r==i.r and self.g==i.g and self.b==i.b

	def __add__(self,i):
		if isinstance(i,float):
			if(i>1):
				i=1
			elif i<0:
				i=0
			return Color(self.r+i,self.g+i,self.b+i)
		elif isinstance(i,Color):
			r = self.r+i.r
			g = self.g+i.g
			b = self.b+i.b

			if(r>1):
				r=1
			elif r<0:
				r=0
			if(g>1):
				g=1
			elif g<0:
				g=0
			if(b>1):
				b=1
			elif b<0:
				b=0
			return Color(r,g,b)
	def __sub__(self,i):
		r=g=b=0
		if isinstance(i,float):
			r = self.r-i
			g = self.g-i
			b = self.b-i
		elif isinstance(i,Color):
			r = self.r-i.r
			g = self.g-i.g
			b = self.b-i.b
		if(r<0):
			r=0
		if(g<0):
			g=0
		if(b<0):
			b=0
		return Color(r,g,b)
	def __iadd__(self,i):
		if isinstance(i,float):
			if(i>1):
				i=1
			elif i<0:
				i=0
			self.r+=i
			self.g+=i
			self.b+=i

		elif isinstance(i,Color):
			r = self.r+i.r
			g = self.g+i.g
			b = self.b+i.b

			if(r>1):
				r=1
			elif r<0:
				r=0
			if(g>1):
				g=1
			elif g<0:
				g=0
			if(b>1):
				b=1
			elif b<0:
				b=0
			self.r=r
			self.g=g
			self.b=b
		return self

	def __isub__(self,i):
		if isinstance(i,float):
			self.r-=i
			self.g-=i
			self.b-=i
		elif isinstance(i,Color):
			r = self.r-i.r
			g = self.g-i.g
			b = self.b-i.b

			self.r=r
			self.g=g
			self.b=b

		if(self.r<0):
			self.r=0
		if(self.g<0):
			self.g=0
		if(self.b<0):
			self.b=0
		return self
