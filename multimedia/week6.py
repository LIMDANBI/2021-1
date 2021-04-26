import numpy as np
import cv2
import matplotlib.pyplot as plt

# Image blurring
img = cv2.imread("lena.png")
img = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)

blur_mask1 = np.ones((3,3), np.float32)
blur_mask1 = blur_mask1/9

blur_mask2 = np.ones((7,7), np.float32)
blur_mask2 = blur_mask2/49

blur_img1 = cv2.filter2D(img, -1, blur_mask1) # 입력 영상, -1(영상과 동일한 데이터의 출력 영상 생성), 필터 마스크 행렬
blur_img2 = cv2.filter2D(img, -1, blur_mask2)

plt.figure()
plt.title("Original")
plt.imshow(img)

plt.figure()
plt.title("3x3 blurred")
plt.imshow(blur_img1)

plt.figure()
plt.title("7x7 blurred")
plt.imshow(blur_img2)


# Image sharpening
img = cv2.imread("lena.png")
img = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)

sharp_mask1 = np.array([
    [0, -1, 0],
    [-1, 5, -1],
    [0, -1, 0]
])

sharp_mask2 = np.array([
    [-1, -1, -1],
    [-1, 9, -1],
    [-1, -1, -1]
])

sharp_img1 = cv2.filter2D(img, -1, sharp_mask1)
sharp_img2 = cv2.filter2D(img, -1, sharp_mask2)


plt.figure()
plt.title("Original")
plt.imshow(img)

plt.figure()
plt.title("Sharpened 1")
plt.imshow(sharp_img1)

plt.figure()
plt.title("Sharpened 2")
plt.imshow(sharp_img2)


# Noise reduction (Gaussian noise)
def median_filtering(img, window_size=3): # 중간 값 filtering 결과 return
    assert len(img.shape) == 2, "Suppert grayscal image only"

    h, w = img.shape
    result_img = np.zeros((h,w), np.uint8)

    ws2 = window_size//2

    center_index = (window_size * window_size)//2

    for img_y in range(ws2, h-ws2):
        for img_x in range(ws2, w-ws2):
            pixel_list = []

            for win_y in range(-ws2, ws2+1):
                for win_x in range(-ws2, ws2+1):
                    # 현재 계산에 사용될 픽셀 위치 계산 
                    y = img_y + win_y
                    x = img_x + win_x
                    pixel_list.append(img[y, x])
            result_img[img_y, img_x] = np.median(pixel_list) # median 값 
    return result_img

img = cv2.imread("gaussian.png", 0)

avg_mask = np.ones((3,3), np.float32)
avg_mask = avg_mask/9

avg_img = cv2.filter2D(img, -1, avg_mask)

med_img = median_filtering(img, window_size=3)

plt.figure(figsize=(6,6))
plt.title("Gaussian noise")
plt.imshow(img, cmap='gray')

plt.figure(figsize=(6,6))
plt.title("Average")
plt.imshow(avg_img, cmap='gray')

plt.figure(figsize=(6,6))
plt.title("Median")
plt.imshow(med_img, cmap='gray')


# Noise reduction (impulse noise)
img = cv2.imread("impulse.png", 0)

avg_mask = np.ones((3,3), np.float32)
avg_mask = avg_mask/9

avg_img = cv2.filter2D(img, -1, avg_mask)

med_img = median_filtering(img, window_size=3)

plt.figure(figsize=(6,6))
plt.title("Impulse noise")
plt.imshow(img, cmap='gray')

plt.figure(figsize=(6,6))
plt.title("Average")
plt.imshow(avg_img, cmap='gray')

plt.figure(figsize=(6,6))
plt.title("Median")
plt.imshow(med_img, cmap='gray')


# Edge detection
img = cv2.imread("lena.png", 0)

edge_sobel =cv2.Sobel(img, -1, 1, 1)

edge_canny = cv2.Canny(img, 100, 200)

plt.figure()
plt.title("Original")
plt.imshow(img, cmap='gray')

plt.figure()
plt.title("Sobel")
plt.imshow(edge_sobel, cmap='gray')

plt.figure()
plt.title("Canny")
plt.imshow(edge_canny, cmap='gray')
