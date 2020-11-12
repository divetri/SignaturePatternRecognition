from flask import Flask,request
import cv2
import numpy as np 
from datetime import datetime
app = Flask(__name__)

@app.route('/presensi', methods=['POST'])
def presensi():
	nim = 0
	now = datetime.now()
	current_time = now.strftime("%m%d%H%M")
	print("Current Time =", current_time)
	data_mentah = str(request.get_data())[2:-1]
	koordinatTTD = []
	ep = []
	sp = []
	print(data_mentah)
	for baris in data_mentah.split("#"):
		print(baris)
		if len(baris.split(",")) == 3:
			S,X,Y = baris.split(",")
			koordinatTTD.append([int(S),int(X),int(Y)])
		elif len(baris) >= 9:
			nim = baris
	print(koordinatTTD)
	
	
	
	# stackoverflow
	blank_image = np.zeros((340,240,1), np.uint8)
	for titik in koordinatTTD:
		S,X,Y = titik
		blank_image[Y,X] = 255
		if S==0:
			sp=X,Y
		elif S==1:
			ep=sp
			sp=X,Y
			blank_image = cv2.line(blank_image, sp, ep, 255, 1)
	cv2.imwrite(f"{nim}-{current_time}.png",blank_image)
	return 'ok'

app.run("0.0.0.0",port=6666)
