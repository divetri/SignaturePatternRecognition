import serial
import os
import glob
import cv2 as cv
import numpy as np
from datetime import datetime
box = ''
stat = ''
nim = ''
koordinat = []
ep = []
sp = []
path = ''
imgpre = ''
imgreg= ''
def matching(img1, img2):
    minHessian = 800 #hessian threshold you can set it depends the type of your image, usually will on default at 400
    detector = cv.xfeatures2d_SURF.create(hessianThreshold=minHessian) 
    keypoints1, descriptors1 = detector.detectAndCompute(img1, None)
    keypoints2, descriptors2 = detector.detectAndCompute(img2, None)
    matcher = cv.DescriptorMatcher_create(cv.DescriptorMatcher_FLANNBASED)
    knn_matches = matcher.knnMatch(descriptors1, descriptors2, 2)
    ratio_thresh = 0.85 #you can set it too as well like hessian threshold
    good_matches = []
    for m, n in knn_matches:
        if m.distance < ratio_thresh * n.distance:
            good_matches.append(m)
    #img_matches = np.empty((max(img1.shape[0], img2.shape[0]), img1.shape[1] + img2.shape[1], 3), dtype=np.uint8)
    #cv.drawMatches(img1, keypoints1, img2, keypoints2, good_matches, img_matches,
                   #flags=cv.DrawMatchesFlags_NOT_DRAW_SINGLE_POINTS)
    #return [len(good_matches) / len(knn_matches), len(good_matches), len(knn_matches), img_matches]
    return [round(len(good_matches) / len(knn_matches),6), len(good_matches), len(knn_matches)]
def thresholding(nim): #calculating class image threshold
    namefiles=[]
    acuan = []
    imgs=[]
    train=[]
    freg = '[REGISTER IMAGE FOLDER]' #change inside [] to your directory folder
    for x in glob.glob(os.path.join(freg, '*.png')):
        if (x.startswith('[REGISTER IMAGE FOLDER]'+str(nim))): #change inside [] to your directory folder
            namefiles.append(x)
            img=cv.imread(x)
            imgs.append(img)
    data = 0
    for i in range(0,len(imgs)):
        for j in range(0,len(imgs)):
            if j == i:
                continue
            else:
                ratio, good, knn = matching(img1=imgs[i], img2=imgs[j])
                print(namefiles[i], end=' ')
                print(namefiles[j], end=' ')
                print(ratio)
                acuan.append(ratio)
                data += 1
    avg = sum(acuan)/len(acuan)
    trains=np.load('thresh.npy') 
    train=[[str(nim)],[str(avg)]]
    trains=np.append([[trains]],[[train]])
    np.save('thresh.npy',trains)
    print(np.load('thresh.npy'))
    print("acuan =", sum(acuan)/len(acuan))
    return sum(acuan)/len(acuan)
def presensi(imgpre,nim):
    namefiles=[]
    imgs=[]
    freg = '[REGISTER IMAGE FOLDER]' #change inside [] to your directory folder
    for x in glob.glob(os.path.join(freg, '*.png')):
        if (x.startswith('[REGISTER IMAGE FOLDER]'+str(nim))): #change inside [] to your directory folder
            namefiles.append(x)
            img=cv.imread(x)
            imgs.append(img)
    local_avg = []
    y=0
    z=0
    for i in imgs:
        z+=1
        ratio, good, knn= matching(img1=i, img2=cv.imread(imgpre))
        #ratio, good, knn= matching(img1=i, img2=img)
        local_avg.append(ratio)
        print('Dicocokkan dengan ',namefiles[z-1],str(ratio))
    trains=np.load('thresh.npy')
    for i in range(len(trains)):
        if(trains[i]==nim):
            avgs = float(trains[i+1])
    avg=round(avgs,6)
    print(avg)
    print('Rerata Kecocokan= ',sum(local_avg),'/',len(local_avg),'= ',sum(local_avg) / len(local_avg))
    return [sum(local_avg) / len(local_avg) > avg, round((sum(local_avg) / len(local_avg)),6), avg]
if __name__ == '__main__':
    ser = serial.Serial('[PORT]','[BAUDRATE]',timeout=1) #change [PORT] & [BAUDRATE] to your port name and baudrate
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
                path = '[YOUR IMAGE FOLDER]' #change inside [] to your directory folder
                if stat=='reg':
                    i = 1
                    for img in glob.glob(path+nim+'*.png'):
                        i=i+1
                    print(i)
                    cv.imwrite(os.path.join(path, nim+'-'+str(i)+".png"),blank_image)
                    if (i==10):
                        acuan = thresholding(nim)
                        print(acuan)
                    box = ''
                    stat = ''
                    nim = ''
                    koordinat = []
                    path = ''
                    msg = "saved, "+str(i)
                    ser.write(msg.encode())
                elif stat == 'pre':
                    now = datetime.now()
                    tgl = now.strftime("%m%d%H%M")
                    cv.imwrite(os.path.join(path, nim+"-"+tgl+".png"),blank_image)
                    imgpre = path+nim+'-'+tgl+'.png'
                    box = ''
                    stat = ''
                    koordinat = []
                    path = ''
                    hasil, nilai, avg = presensi(imgpre,nim)
                    nim = ''
                    msg = str(avg)+", "+str(nilai)
                    print(msg)
                    ser.write(msg.encode())
                    if hasil:
                        print("MATCH")
                    imgpre = ''
                box = ''
                stat = ''
                nim = ''
                koordinat = []
                path = ''
                tgl= ''
            elif line[-3:]=='res':
                box = ''


    
