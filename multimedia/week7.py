import cv2
import matplotlib.pyplot as plt
import numpy as np

def myImread(path):
    bgr_img = cv2.imread(path)
    assert bgr_img is not None, "이미지 로드 실패"
    rgb_img = cv2.cvtColor(bgr_img, cv2.COLOR_BGR2RGB)
    return rgb_img

# Harris Corner Detector 
# blockSize-윈도우사이즈, apertureSize-미분kernel사이즈(소벨 마스크 크기)
def Harris(img, blockSize=2, apertureSize=3, k=0.04, thresh=0.1):
    radian=3
    color = (255,0,0)
    thickness = 2

    h, w, c = img.shape
    result_img = img.copy()

    gray_img = cv2.cvtColor(img, cv2.COLOR_RGB2GRAY)
    corner_response = cv2.cornerHarris(gray_img, blockSize, apertureSize, k)

    max_value = np.max(corner_response)
    corner_response /= max_value

    for y in range(h):
        for x in range(w):
            if(corner_response[y,x] > thresh):
                cv2.circle(result_img, (x,y), radian, color, 2) # 원 그리기 (이미지, 원의 중심, 반지름, 색상, 선두께)
    
    return result_img

hotel1 = myImread("./hotel-00.png")
hotel15 = myImread("./hotel-15.png")
hotel2 = myImread("./hotel-01.png")

hotel_harris1 = Harris(hotel1,thresh=0.1)
hotel_harris15 =  Harris(hotel15,thresh=0.1)
hotel_harris2 = Harris(hotel2,thresh=0.1)

plt.figure(figsize=(6,6))
plt.imshow(hotel_harris1)

plt.figure(figsize=(6,6))
plt.imshow(hotel_harris15)

plt.figure(figsize=(6,6))
plt.imshow(hotel_harris2)

# SIFT 알고리즘
# 이미지, 점수가 높은 n개의 특징점 리턴, 몇개의 옥타브를 사용할지, 약한 특징점 제거를 위한 threshold값, edge-like한 특징점을 제거하기 위한 threshold값, 첫 가우시안블러의 시그마 값
def SIFT(img, nfeatures=0, nOctaveLayers=3, contrastThreshold=0.04, edgeThreshold=10, sigma=1.6, draw_rich=False):
    radian = 3
    color = (255,0,0)
    thickness = 2
    
    h,w,c = img.shape
    result_img = img.copy()    
    gray_img = cv2.cvtColor(img, cv2.COLOR_RGB2GRAY)
    
    sift = cv2.xfeatures2d.SIFT_create(nfeatures, nOctaveLayers, contrastThreshold, edgeThreshold, sigma) # SIFT 객체 생성자
    kp = sift.detect(gray_img, None) #이미지 내의 키포인트를 찾아줌

    if draw_rich:
        cv2.drawKeypoints(result_img, kp, result_img, flags=cv2.DRAW_MATCHES_FLAGS_DRAW_RICH_KEYPOINTS) 
        # 특징점을 표시해주는 함수 (이미지, 표시할 특징점 리스트, 결과 이미지, 표시방법-동그라미의 크기를 size와 angle을 반영해서 그림)
    else:
        kp_xy = cv2.KeyPoint_convert(kp)
        for x,y in kp_xy:
            cv2.circle(result_img, (x,y), radian, color, thickness)
                
    return result_img

hotel1 = myImread("./hotel-00.png")
hotel15 = myImread("./hotel-15.png")
hotel2 = myImread("./hotel-01.png")

hotel_SIFT1 = SIFT(hotel1, nfeatures=50)
hotel_SIFT15 = SIFT(hotel15, nfeatures=50)
hotel_SIFT2 = SIFT(hotel2, nfeatures=50)

plt.figure(figsize=(6,6))
plt.imshow(hotel_SIFT1)

plt.figure(figsize=(6,6))
plt.imshow(hotel_SIFT15)

plt.figure(figsize=(6,6))
plt.imshow(hotel_SIFT2)

# Harris Corner 알고리즘과 SIFT 알고리즘 비교
def Compare(path):
    img = myImread(path)
    img_harris = Harris(img,)
    img_sift = SIFT(img, draw_rich=True)
    
    plt.figure(figsize=(12,12))
    plt.subplot(1,3,1)
    plt.title("Original")
    plt.imshow(img)
    
    plt.subplot(1,3,2)
    plt.title("harris")
    plt.imshow(img_harris)
    
    plt.subplot(1,3,3)
    plt.title("SIFT")
    plt.imshow(img_sift)
Compare("./korea.png")
Compare("./checkerboard.png")
Compare("./triangle.jpg")
Compare("./test1.jpg")
Compare("./test2.jpg")