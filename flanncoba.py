import cv2 as cv
import numpy as np
img1 = cv.imread('F:/SEMANGAT/AAAAMILIKKU/sample/1/ori.jpg',0);
img2 = cv.imread('F:/SEMANGAT/AAAAMILIKKU/sample/1/ori_2.jpg',0);
minHessian = 400
detector = cv.xfeatures2d_SURF.create(hessianThreshold=minHessian)
keypoints1, descriptors1 = detector.detectAndCompute(img1, None)
keypoints2, descriptors2 = detector.detectAndCompute(img2, None)
matcher = cv.DescriptorMatcher_create(cv.DescriptorMatcher_FLANNBASED)
knn_matches = matcher.knnMatch(descriptors1, descriptors2, 2)
ratio_thresh = 0.8
good_matches = []

for m,n in knn_matches:
    if m.distance < ratio_thresh * n.distance:
        good_matches.append(m)

        
img_matches = np.empty((max(img1.shape[0], img2.shape[0]), img1.shape[1]+img2.shape[1], 3), dtype=np.uint8)
cv.drawMatches(img1, keypoints1, img2, keypoints2, good_matches, img_matches, flags=cv.DrawMatchesFlags_NOT_DRAW_SINGLE_POINTS)
cv.imshow('Good Matches', img_matches)
dist = (1 - len(good_matches) / (max(len(descriptors1), len(descriptors2))))*100
print(dist)