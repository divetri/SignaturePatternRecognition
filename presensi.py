import cv2
import numpy as np 
NIM = '123123123' #contohNIM
Koor = ''' ''' #koordinat dari arduino

def presensi(nim,koordinat):
	koordinatTTD = []
	for baris in koordinat.split("\n"):
		if len(baris.split(",")) == 2:
			X,Y = baris.split(",")
			koordinatTTD.append([int(X),int(Y)])
	print(koordinatTTD)
	# stackoverflow
	blank_image = np.zeros((240,340,1), np.uint8)
	for titik in koordinatTTD:
		X,Y = titik
		blank_image[Y,X] = 255
	cv2.imwrite("hasil.png",blank_image)
presensi(NIM,Koor)
