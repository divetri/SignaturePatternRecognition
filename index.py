import serial
import os
import glob
import cv2 as cv
import numpy as np
from datetime import datetime
now = datetime.now()
tgl = now.strftime("%m%d%H%M")
box = ''
stat = ''
nim = ''
koordinat = []
ep = []
sp = []
path = ''
imgpre = ''
imgreg= ''
if __name__ == '__main__':
    ser = serial.Serial('/dev/ttyUSB0',9600,timeout=1)
    ser.flush()
    while True:
        if ser.in_waiting > 0:
            line = ser.readline().decode('utf-8').rstrip()
            box += line
            if line[-3:]=='sub':
                for baris in box.split("#"):
                    stat = (box.split("#")[0])
                    nim = (box.split("#")[1])
                    if len(baris.split(","))==3:
                        S,X,Y = baris.split(",")
                        koordinat.append([int(S),int(X),int(Y)])
                blank_image = np.zeros((340,280,1), np.uint8)
                blank_image.fill(0)
                for titik in koordinat:
                    S,X,Y = titik
                    blank_image[Y,X]=255
                    if S==0:
                        sp=X,Y
                    elif S==1:
                        ep=sp
                        sp=X,Y
                        blank_image = cv.line(blank_image, sp, ep, 255, 1)
                path = '/home/pi/Documents/'+stat
                if stat=='reg':
                    i = 1
                    for img in glob.glob('/home/pi/Documents/reg/'+nim+'*.png'):
                        i=i+1
                    print(i)
                    cv.imwrite(os.path.join(path, nim+'-'+str(i)+".png"),blank_image)
                    box = ''
                    stat = ''
                    nim = ''
                    koordinat = []
                    path = ''
                elif stat == 'pre':
                    cv.imwrite(os.path.join(path, nim+"-"+tgl+".png"),blank_image)
                    imgpre = path+'/'+nim+'-'+tgl+'.png'
                    imgreg = '/home/pi/Documents/reg/'+nim+'.png'
                    print(imgpre)
                    print(imgreg)
                    box = ''
                    stat = ''
                    nim = ''
                    koordinat = []
                    path = ''
                    img1 = cv.imread(imgpre,0);
                    img2 = cv.imread(imgreg,0);
                    minHessian = 800
                    detector = cv.xfeatures2d_SURF.create(hessianThreshold=minHessian)
                    keypoints1, descriptors1 = detector.detectAndCompute(img1, None)
                    keypoints2, descriptors2 = detector.detectAndCompute(img2, None)
                    matcher = cv.DescriptorMatcher_create(cv.DescriptorMatcher_FLANNBASED)
                    knn_matches = matcher.knnMatch(descriptors1, descriptors2, 2)
                    ratio_thresh = 0.85
                    good_matches = []

                    for m,n in knn_matches:
                        if m.distance < ratio_thresh * n.distance:
                            good_matches.append(m)
                            
                    img_matches = np.empty((max(img1.shape[0], img2.shape[0]), img1.shape[1]+img2.shape[1], 3), dtype=np.uint8)
                    cv.drawMatches(img1, keypoints1, img2, keypoints2, good_matches, img_matches, flags=cv.DrawMatchesFlags_NOT_DRAW_SINGLE_POINTS)

                    dist = (len(good_matches) / (min(len(descriptors1), len(descriptors2))))*100
                    print(len(good_matches))
                    print(len(descriptors1))
                    print(len(descriptors2))
                    print(dist)

                    if dist >= 75:
                        print("Cocok")
                        
                    else:
                        print("Tidak cocok");
                    imgpre = ''
            elif line[-3:]=='res':
                box = ''

            


