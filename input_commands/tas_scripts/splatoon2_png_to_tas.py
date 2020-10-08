
from PIL import Image
import numpy as np
import sys
import struct

def packBits(bits):
	out = 0
	for b in bits:
		out <<= 1
		if b:
			out |= 1
	return out

class USB_JoystickReport_Input:
	button16 = 0
	button15 = 0
	capture = 0
	home = 0
	rclick = 0
	lclick = 0
	start = 0
	select = 0
	zr = 0
	zl = 0
	r = 0
	l = 0
	x = 0
	a = 0
	b = 0
	y = 0
	hat = 8
	hat2 = 8
	leftx = 128
	lefty = 128
	rightx = 128
	righty = 128
	vendorSpec = 0

	def packButtons(self):
		return packBits([self.button16, self.button15, self.capture, self.home, self.rclick, self.lclick, self.start, self.select, self.zr, self.zl, self.r, self.l, self.x, self.a, self.b, self.y])

	def pack(self):
		return struct.pack("HBBBBBB", self.packButtons(), self.hat << 4 | self.hat2, self.leftx, self.lefty, self.rightx, self.righty, self.vendorSpec)

class Action:
	report = USB_JoystickReport_Input()
	mode = 0
	ms = 0
	frames = 0

	def pack(self):
		return self.report.pack() + struct.pack("IB", self.ms if self.mode else self.frames, self.mode)

post_shape = (320,120)


def translate(in_path, out_path):
	img_in = Image.open(in_path)
	if img_in.size != post_shape:
		img_in.thumbnail(post_shape)
	img_bool = img_in.convert('1', None,  Image.FLOYDSTEINBERG)
	img_bool.show()
	img_bool.save(in_path + "_1.png")
	img = np.array(img_bool, dtype=bool)
	img = np.rollaxis(img, 1)
	img = np.logical_not(img)
	out_tas = open(out_path, "wb")

	print("size:", img.shape)
	out = Action()

	def writeAction(name):
		print(name, out.frames)
		print(out.pack().hex())
		out_tas.write(out.pack())

	out.frames = 2
	print("initial", img[0,0])
	# move to the poition x y and ink it using the same input,
	# then pause for 2 polls to toggle the dpad and not overload the thing
	for y in range(0,img.shape[1]):
		if y > 0: # first px: move down and ink
			out.report.a = img[(y % 2) * (img.shape[0] - 1),y]
			out.report.hat2 = 4
			writeAction("move down")
		elif img[0,0]: # no need to move in the first row
			out.report.a = True
			out.report.hat2 = 8
			writeAction("initital")
		out.report.hat2 = 8
		writeAction("pause")
		# go left right depending on row
		for x in range(1, img.shape[0]) if y % 2 == 0 else range(img.shape[0] - 2, -1, -1):
			out.report.a = img[x,y]
			out.report.hat2 = 2 + (y % 2) * 4
			writeAction("move")
			out.report.hat2 = 8
			writeAction("pause")

translate(sys.argv[1], sys.argv[2])
