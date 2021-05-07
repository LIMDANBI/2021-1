import cv2
import matplotlib.pyplot as plt
import numpy as np

def myImread(path):
    bgr_img = cv2.imread(path)
    assert bgr_img is not None, "이미지 로드 실패"
    rgb_img = cv2.cvtColor(bgr_img, cv2.COLOR_BGR2RGB)
    return rgb_img

def Harris(img, blockSize=2, apertureSize=3, k=0.04, thresh=0.1):
    radian = 3
    color = (255,0,0)
    thickness = 4

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

original = myImread("./original.png")
light = myImread("./light.png")
size = myImread("./size.png")
turn = myImread("./turn.png")

original_harris = Harris(original)
light_harris =  Harris(light)
size_harris = Harris(size)
turn_harris = Harris(turn)

plt.figure(figsize=(6,6))
plt.imshow(original_harris)

plt.figure(figsize=(6,6))
plt.imshow(light_harris)

plt.figure(figsize=(6,6))
plt.imshow(size_harris)

plt.figure(figsize=(6,6))
plt.imshow(turn_harris)

def SIFT(img, nfeatures=0, nOctaveLayers=3, contrastThreshold=0.03, edgeThreshold=10, sigma=1.6, draw_rich=False):
    radian = 3
    color = (255,0,0)
    thickness = 4
    
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

original = myImread("./original.png")
size = myImread("./size.png")
light = myImread("./light.png")
turn = myImread("./turn.png")

original_sift = SIFT(original,nfeatures=50)
size_sift = SIFT(size,nfeatures=50)
light_sift = SIFT(light,nfeatures=50)
turn_sift = SIFT(turn,nfeatures=50)

plt.figure(figsize=(6,6))
plt.imshow(original_sift)

plt.figure(figsize=(6,6))
plt.imshow(size_sift)

plt.figure(figsize=(6,6))
plt.imshow(light_sift)

plt.figure(figsize=(6,6))
plt.imshow(turn_sift)

def Compare(path):
    img = myImread(path)
    img_harris = Harris(img,)
    img_sift = SIFT(img, nfeatures=50, draw_rich=False)
    
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
    
Compare("./original.png")
Compare("./size.png")
Compare("./light.png")
Compare("./turn.png")