import cv2
import os
from glob import glob
import numpy as np
import matplotlib.pyplot as plt


####  OpenCV 함수 사용 
img_names = ['hotel-01.png', 'hotel-02.png','hotel-03.png','hotel-04.png']

imgs = []
for i, name in enumerate(img_names):
    img = cv2.imread(name)
    plt.figure(figsize=(15,15))
    plt.subplot(len(img_names)//3+1,3,i+1)
    plt.imshow(cv2.cvtColor(img, cv2.COLOR_BGR2RGB))
    imgs.append(img)

stitcher = cv2.Stitcher_create()

status, dst = stitcher.stitch(imgs)

s=0
plt.figure(figsize=(20,20))
plt.imshow(cv2.cvtColor(dst, cv2.COLOR_BGR2RGB))


#### OpenCV 함수 미사용 
import cv2
import os
from glob import glob
import numpy as np
import matplotlib.pyplot as plt

# 특징점 매칭 및 변환행렬 탐색
def KeypointsMatching(keyPoints_L, keyPoints_R, descriptors_L, descriptors_R):

    bf = cv2.BFMatcher() # 가능한 모든 경우에 대해 다 계산해본 후 결과 반환 (무식하게 전부 다 확인, BF = brute force)
    raw_matches = bf.knnMatch(descriptors_L, descriptors_R, k=2) # k개의 가장 근접한 매칭

    matches = []
    for m in raw_matches:
        if len(m) == 2 and m[0].distance < m[1].distance * 0.79:
            matches.append((m[0].trainIdx, m[0].queryIdx))

    if len(matches) >= 4:

        keyPoints_L = np.float32([keyPoints_L[i] for (_, i) in matches])
        keyPoints_R = np.float32([keyPoints_R[i] for (i, _) in matches])

        H, status = cv2.findHomography(keyPoints_L, keyPoints_R, cv2.RANSAC, 4.0)
        # H - 결과 변환 행렬 / status - 정상치 판별 결과, N x 1 배열 (0: 비정상치, 1: 정상치) / keyPoints_L - 원본 좌표 배열 / keyPoints_R - 결과 좌표 배열 
        # cv2.RANSAC - 근사 계산 알고리즘 / 4.0 - 정상치 거리 임계 값


    return matches, H, status

# RANSAC을 이용해 찾은 기하 변환을 적용하여 파노라마 생성
image_1 = cv2.imread('hotel-01.png')
image_2 = cv2.imread('hotel-02.png')
image_3 = cv2.imread('hotel-03.png')
image_4 = cv2.imread('hotel-04.png')

# 이미지를 BGR -> GRAY
gray_1 = cv2.cvtColor(image_1, cv2.COLOR_BGR2GRAY)
gray_2 = cv2.cvtColor(image_2, cv2.COLOR_BGR2GRAY)
gray_3 = cv2.cvtColor(image_3, cv2.COLOR_BGR2GRAY)
gray_4 = cv2.cvtColor(image_4, cv2.COLOR_BGR2GRAY)


detector = cv2.xfeatures2d.SIFT_create() # 객체 생성 => SIFT의 특징점, descriptor들을 계산하는 함수 제공
# 4개의 이미지의 key point, descriptor 계산
keyPoints_1, descriptors_1 = detector.detectAndCompute(gray_1, None) # detectAndCompute =>  특징점과 descriptor를 한번에 계산해서 리턴
keyPoints_2, descriptors_2 = detector.detectAndCompute(gray_2, None)
keyPoints_3, descriptors_3 = detector.detectAndCompute(gray_3, None)
keyPoints_4, descriptors_4 = detector.detectAndCompute(gray_4, None)

keyPoints_1 = np.float32([keypoint.pt for keypoint in keyPoints_1])
keyPoints_2 = np.float32([keypoint.pt for keypoint in keyPoints_2])
keyPoints_3 = np.float32([keypoint.pt for keypoint in keyPoints_3])
keyPoints_4 = np.float32([keypoint.pt for keypoint in keyPoints_4])

matches, H, status = KeypointsMatching(keyPoints_1, keyPoints_2, descriptors_1, descriptors_2)

# 원근 변환 함수 (원근 맵 행렬에 대한 기하학적 변환을 수행)
tmp1 = cv2.warpPerspective(image_1, H,
                             (image_1.shape[1] + image_2.shape[1], image_1.shape[0]))

tmp1[0:image_2.shape[0], 0:image_2.shape[1]] = image_2

keyPoints_tmp1, descriptors_tmp1 = detector.detectAndCompute(tmp1, None)
keyPoints_tmp1 = np.float32([keypoint.pt for keypoint in keyPoints_tmp1])

matches, H, status = KeypointsMatching(keyPoints_tmp1, keyPoints_3, descriptors_tmp1, descriptors_3)
tmp2 = cv2.warpPerspective(tmp1, H,
                             (tmp1.shape[1] + image_3.shape[1], tmp1.shape[0]))

tmp2[0:image_3.shape[0], 0:image_3.shape[1]] = image_3

keyPoints_tmp2, descriptors_tmp2 = detector.detectAndCompute(tmp2, None)
keyPoints_tmp2 = np.float32([keypoint.pt for keypoint in keyPoints_tmp2])

matches, H, status = KeypointsMatching(keyPoints_tmp2, keyPoints_4, descriptors_tmp2, descriptors_4)
result = cv2.warpPerspective(tmp2, H,
                             (tmp2.shape[1] + image_4.shape[1], tmp2.shape[0]))

result[0:image_4.shape[0], 0:image_4.shape[1]] = image_4


# 결과 이미지 출력
plt.figure(figsize=(20,20))
plt.imshow(cv2.cvtColor(result, cv2.COLOR_BGR2RGB))

