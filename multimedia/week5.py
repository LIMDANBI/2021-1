import cv2 #영상처리 라이브러
import numpy as np #과학 계산을 위한 라이브러리로서 다차원 배열을 처리하는데 필요한 여러 유용한 기능 제공
import matplotlib.pyplot as plt #영상출력라이브러리

# grayscale 이미지를 입력 받아 histogram(배열)을 만들어주는 함수 
def make_hist(image):

    #입력 영상이 None이거나 3차원(h,w,c)이면 assert
    assert len(image.shape) == 2,  "grayscale 영상을 입력해주세요."
    
    histogram = np.zeros((256), dtype=np.int32) #2^32 개수 만큼 표현 가능 
    h, w = image.shape
    
    for i in range(h):
        for j in range(w):
            histogram[image[i,j]]+=1 # 모든 픽셀을 탐색하며 histogram 계산
    return histogram

# lena image와 histogram 출력
lena_image = cv2.imread('lena.png', 0) # 0 - grayscale / 1 - color 로 읽음
plt.imshow(lena_image, cmap='gray', vmin=0, vmax=255)
plt.figure()
lena_hist = make_hist(lena_image)

x = np.arange(256) # 범위 : 0~255
plt.bar(x, height=lena_hist) # 막대 그래프 그리기
plt.title("Histogram")
plt.xlabel("Value") # 밝기값
plt.ylabel("Frequency") # 도수
plt.show()

# lena_low 이미지/히스토그램 출력
lena_before = cv2.imread('lena_low.png', 0)
h, w = np.shape(lena_before)
plt.imshow(lena_before, cmap='gray', vmin=0, vmax=255)

plt.figure()
lena_hist = make_hist(lena_before)
x = np.arange(256)
plt.bar(x, height=lena_hist)
plt.title("Low contrast image")
plt.show()

# 영상의 최소, 최대 밝기 계산
max_value = 0 # 영상의 최대 밝기  
min_value = 0 # 영상의 최소 밝기 

for i in np.arange(0,256,1): # 0->255로 탐색하며 최소 밝기 찾기
    freq = lena_hist[i]
    if freq != 0 :
        min_value = i
        break
for i in np.arange(255, -1, -1): # 255->0 로 탐색하며 최대 밝기 찾기
    freq = lena_hist[i]
    if freq != 0 :
        max_value = i
        break
print("최소 밝기: ", min_value)
print("최대 밝기: ", max_value)

# 노이즈에 강건한 최소, 최대 밝기 계산 (픽셀 개수 5% 미만까지는 skip)
max_value = 0 # 영상의 최대 밝기 
min_value = 0 # 영상의 최소 밝기 

sum_low = 0
sum_high = 0
num_pixel = w * h

for i in np.arange(0,256,1):
    freq = lena_hist[i]
    sum_low += freq
    if sum_low > num_pixel * 0.05:
        min_value = i
        break

for i in np.arange(255, -1, -1):
    freq = lena_hist[i]
    sum_high += freq
    if sum_high > num_pixel * 0.05:
        max_value = i
        break

print("최소 밝기: ", min_value)
print("최대 밝기: ", max_value)

# slope 계산, histogram stretching 수행
slope = 255/(max_value-min_value)
print("slope: ", slope)
print( "(max-min) * slope : ", (max_value-min_value)*slope)

lena_after = np.zeros((h,w), dtype=np.uint8)

for i in range(h):
    for j in range(w):
        if lena_before[i, j] < min_value:
            lena_after[i,j] = 0
        elif lena_before[i, j] > max_value:
            lena_after[i,j] = 255
        else : 
            lena_after[i,j] = (lena_before[i,j]-min_value)*slope

# 결과 영상 출력
plt.figure(figsize=(12,12)) # 출력 크기 조절 

plt.subplot(2,2,1)
plt.title("Before")
plt.imshow(lena_before, cmap='gray', vmin=0, vmax=255)

plt.subplot(2,2,2)
plt.title("After")
plt.imshow(lena_after, cmap='gray', vmin=0, vmax=255)

plt.subplot(2,2,3)
before_hist = make_hist(lena_before)
x=np.arange(256)
plt.bar(x, height=before_hist)

plt.subplot(2,2,4)
after_hist = make_hist(lena_after)
x = np.arange(256)
plt.bar(x, height=after_hist)

plt.show()

# Histogram Equalization
lena_before = cv2.imread("lena_low.png", 0)
h, w = np.shape(lena_before)
lena_after = np.zeros((h,w), dtype=np.uint8)

unit = h*w/256
print("영상크기: ", h, w)
print("unit: ", unit)

lena_hist = make_hist(lena_before)

T = np.zeros((256))
hist_sum = 0
for i in range(256):
    hist_sum += lena_hist[i]
    T[i] = np.uint8(hist_sum/unit)
print("T: ", T)

for i in range(h):
    for j in range(w):
        lena_after[i,j] = T[lena_before[i,j]]

# 이미지, 히스토그램 출력
plt.figure(figsize=(12,12))

plt.subplot(2,2,1)
plt.title("Before")
plt.imshow(lena_before, cmap='gray', vmin=0, vmax=255)

plt.subplot(2,2,2)
plt.title("After")
plt.imshow(lena_after, cmap='gray', vmin=0, vmax=255)

plt.subplot(2,2,3)
before_hist = make_hist(lena_before)
x=np.arange(256)
plt.bar(x, height=before_hist)

plt.subplot(2,2,4)
after_hist = make_hist(lena_after)
x=np.arange(256)
plt.bar(x, height=after_hist)

plt.show()


# Threshold
finger_image = cv2.imread("finger_print.png",  0)
h,w = np.shape(finger_image)

thresh_image = np.zeros((h,w), dtype=np.uint8)

thresh_value = 120

for i in range(h):
    for j in range(w):
        if(finger_image[i, j]<thresh_value):
            thresh_image[i,j] = 0
        else : 
            thresh_image[i,j] = 255
            
plt.figure()
plt.subplot(1,2,1)
plt.imshow(finger_image, cmap='gray', vmin=0, vmax=255)

plt.subplot(1,2,2)
plt.imshow(thresh_image, cmap='gray', vmin=0, vmax=255)

# method 적용 
finger_image = cv2.imread("finger_print.png", 0)
th, thresh_image = cv2.threshold(finger_image, 0, 255, cv2.THRESH_OTSU)
print("Otsu 알고리즘이 찾아낸 임계치: ", th)

plt.figure()
plt.subplot(1,2,1)
plt.imshow(finger_image, cmap='gray', vmin=0, vmax=255)

plt.subplot(1,2,2)
plt.imshow(thresh_image, cmap='gray', vmin= 0, vmax=255)


