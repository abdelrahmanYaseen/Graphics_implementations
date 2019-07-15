from Vector import Vector
from Triangle import Triangle
from Sphere import Sphere
class Ray:
	def __init__(self,a,b):
		self.a=a
		self.b=b

	def intersect(self,obj):
		if isinstance(obj,Triangle):
			ma = obj.a
			mb = obj.b
			mc = obj.c

			a = ma.x-mb.x
			b = ma.y-mb.y
			c = ma.z-mb.z

			d = ma.x-mc.x
			e = ma.y-mc.y
			f = ma.z-mc.z

			g = self.b.x
			h = self.b.y
			i = self.b.z

			j = ma.x-self.a.x
			k = ma.y-self.a.y
			l = ma.z-self.a.z

			eimhf = e*i-h*f
			gfmdi = g*f-d*i
			dhmeg = d*h-e*g
			akmjb = a*k-j*b
			jcmal = j*c-a*l
			blmkc = b*l-k*c

			M = a*eimhf+b*gfmdi+c*dhmeg
			if M==0 :
				return -1

			t = -(f*akmjb+e*jcmal+d*blmkc)/M

			gamma = (i*akmjb+h*jcmal+g*blmkc)/M

			if (gamma<0 or gamma>1) :
				return -1

			beta = (j*eimhf+k*gfmdi+l*dhmeg)/M

			if (beta<0 or beta>(1-gamma)) :
				return -1

			return t

		elif isinstance(obj,Sphere):
			C = (self.a.x-obj.center.x)*(self.a.x-obj.center.x)+(self.a.y-obj.center.y)*(self.a.y-obj.center.y)+(self.a.z-obj.center.z)*(self.a.z-obj.center.z)-obj.r*obj.r

			B = 2*self.b.x*(self.a.x-obj.center.x)+2*self.b.y*(self.a.y-obj.center.y)+2*self.b.z*(self.a.z-obj.center.z)

			A = self.b.x*self.b.x+self.b.y*self.b.y+self.b.z*self.b.z

			delta = B*B-4*A*C

			if delta<0 :
				return -1
			elif delta==0:
				t = -B / (2*A)
			else:
				delta = (delta)**0.5
				A = 2*A
				t1 = (-B + delta) / A
				t2 = (-B - delta) / A

				if t2<t1:
					tmp = t2
					t2 = t1
					t1 = tmp

				t = t1

			return t

	def __str__(self):
		return "a:{} b:{}".format(self.a,self.b)

	@classmethod
	def generate(cls,camera,i,j):
		#different than the c
		""" :
			Ray tmp;

				Vec3 su,sv,s;

				tmp.a = cam.pos;

				su = mult(cam.u,cam.l+(i*pixelW)+halfPixelW);
				sv = mult(cam.v,cam.b+(j*pixelH)+halfPixelH);

				s = add(su,sv);

				tmp.b = add(mult(cam.gaze,cam.d),s);

				//printVec(add(tmp.a,tmp.b));

				return tmp;
		"""

		a = camera.position
		m = camera.position+camera.gaze*camera.d
		q = m + camera.u*camera.l + camera.v*camera.t
		su = (i+0.5)*camera.pixelW
		sv = (j+0.5)*camera.pixelH
		s = q + (camera.u*su) - (camera.v*sv)
		b= s-camera.position
		b.y=-b.y

		return cls(a,b)
