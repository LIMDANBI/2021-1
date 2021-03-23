import numpy as np #과학 계산을 위한 라이브러리로서 다차원 배열을 처리하는데 필요한 여러 유용한 기능 제공
import cv2 #영상처리 라이브러
import matplotlib.pyplot as plt #영상출력라이브러리

#1
img = np.zeros(shape=(512,512), dtype=np.uint8) #512x512 이미지 생성, 0으로 초기화 (zeros), uint:2^8만큼 양수만 표현 가능 (0~255)
print("img의 shape: ", img.shape) #img가 어떤 형태의 행렬인지 확인
print("img[0,0]의 값: ", img[0,0])
plt.imshow(img, cmap='gray', vmin=0, vmax=255) #이미지를 사이즈에 맞게 보여줌, cmpa='gray'->회색조 이미지 표현

#2
for i in range(0,200):
    for j in range(0,200):
        img[i,j]=120
print("img[0,0]의 값: ", img[0,0])
plt.imshow(img, cmap='gray', vmin=0, vmax=255)

#3
lena_gray = cv2.imread("./Lena_gray.jpg", cv2.IMREAD_GRAYSCALE) # 이미지를 Grayscale로 읽어 들임. 실제 이미지 처리시 중간단계로 많이 사용
print("영상의 data type: ", lena_gray.dtype)
print("영상의 shpae: ", lena_gray.shape)
print("lena_gray[0,0]의 값: ", lena_gray[0,0])
plt.imshow(lena_gray, cmap='gray') #cmap=colormap

#4
img = np.zeros(shape=(512,512,3), dtype=np.uint8)
print("img의 shape: ", img.shape)
plt.subplot(1,3,1) #한줄에 3개의 이미지 출력, 첫번째 이미지
plt.title("All zero")
plt.imshow(img)
for i in range(512):
    for j in range(512):
        img[i,j,0] = 255
plt.subplot(1,3,2)
plt.title("Red color")
plt.imshow(img)
for i in range(100):
    for j in range(100):
        img[i,j,0]=0
        img[i,j,1]=128
        img[i,j,2]=128
plt.subplot(1,3,3)
plt.title("ohter color")
plt.imshow(img)

#5 Color이미지 출력
flowers_RGB = cv2.imread("./Flowers.jpg", cv2.IMREAD_COLOR) #이미지 파일을 Color로 읽음 / cv2.imread()함수에서는 채널이 BGR의 순서로 변환 -> cv2.cvtColor
flowers_RGB = cv2.cvtColor(flowers_RGB, cv2.COLOR_BGR2RGB)
print("영상의 data type: ", flowers_RGB.dtype)
print("영상의 shpae: ", flowers_RGB.shape)
print("flowers_RGB[0,0]의 값: ", flowers_RGB[0,0])
plt.imshow(flowers_RGB)

#6 RGB를 채널별로 출력 (grayscale)
plt.imshow(flowers_RGB)
flowers_R = flowers_RGB[:,:,0]
flowers_G = flowers_RGB[:,:,1]
flowers_B = flowers_RGB[:,:,2]
print("flowers_RGB의 shape: ", flowers_RGB.shape)
print("flowers_R(단일채널)의 shape: ", flowers_R.shape)
plt.figure() #  새로운 figure 생성
plt.subplot(1,3,1)
plt.title("R channel")
plt.imshow(flowers_R, cmap='gray')
plt.subplot(1,3,2)
plt.title("G channel")
plt.imshow(flowers_G, cmap='gray')
plt.subplot(1,3,3)
plt.title("B channel")
plt.imshow(flowers_B, cmap='gray')

#7 산술연산(+)
lena_gray = cv2.imread("./Lena_gray.jpg", cv2.IMREAD_GRAYSCALE)
plt.subplot(1,3,1)
plt.title("Original Image")
plt.imshow(lena_gray, cmap='gray')
h,w = lena_gray.shape
lena_add = np.zeros((h,w), np.uint8)
for i in range(h):
    for j in range(w):
        lena_add[i,j] = lena_gray[i,j]+50
plt.subplot(1,3,2)
plt.title("Add 50")
plt.imshow(lena_add, cmap='gray')
lena_add = lena_gray+100 #위의 이중 for문과 동일한 효과
plt.subplot(1,3,3)
plt.title("Add 100")
plt.imshow(lena_add, cmap='gray')

#8 산술연산(/)
lena_div = lena_gray/2
plt.figure()
plt.subplot(1,2,1)
plt.title("Origianl")
plt.imshow(lena_gray, cmap='gray')
plt.subplot(1,2,2)
plt.title("Div")
plt.imshow(lena_div, cmap='gray', vmin=0, vmax=255)

#9 산술연산 (*, -)
lena_gray = cv2.imread("./Lena_gray.jpg", cv2.IMREAD_GRAYSCALE)
plt.subplot(1,3,1)
plt.title("Original Image")
plt.imshow(lena_gray, cmap='gray')
lena_mul = np.int32(lena_gray)*2 #int32 : 2^32만큼 표현 가능, 왜변경?? -> 강의 다시
lena_mul[lena_mul>255]=255
lena_mul = np.uint8(lena_mul)
plt.subplot(1,3,2)
plt.title("Mul (Saturated)")
plt.imshow(lena_mul, cmap='gray')
lena_sub = np.int32(lena_gray)-100
lena_sub[lena_sub<0]=0
lena_sub = np.uint8(lena_sub)
plt.subplot(1,3,3)
plt.title("Sub (Saturated)")
plt.imshow(lena_sub, cmap='gray')

#10 두 영상의 합성
lena = cv2.imread("./lena_color.jpg")
lena = cv2.cvtColor(lena, cv2.COLOR_BGR2RGB)
pattern = cv2.imread("./Pattern1.png")
pattern = cv2.cvtColor(pattern, cv2.COLOR_BGR2RGB)
alpha=0.5
beta=1-alpha
mixed = (alpha*lena) + (beta*pattern)
mixed = np.uint8(mixed)
plt.subplot(1,3,1)
plt.imshow(lena)
plt.subplot(1,3,2)
plt.imshow(pattern)
plt.subplot(1,3,3)
plt.imshow(mixed)

#11 두 영상의 차
img1 = cv2.imread("./Pattern1.png", 0)
img2 = cv2.imread("./Pattern2.png", 0)
img_diff = np.abs(img1-img2)
plt.subplot(1,2,1)
plt.imshow(img1, cmap='gray', vmin=0, vmax=255)
plt.subplot(1,2,2)
plt.imshow(img2, cmap='gray', vmin=0, vmax=255)
plt.figure()
plt.imshow(img_diff, cmap='gray', vmin=0, vmax=255)
plt.colorbar() #colorbar() 함수 사용 -> 그래프 영역에 컬러바를 포함

#12 영상의 반전
lena = cv2.imread("./Lena_color.jpg")
lena = cv2.cvtColor(lena, cv2.COLOR_BGR2RGB)
plt.title("Lena")
plt.imshow(lena)
lena_inverted = 255-lena #영상 반전 
plt.figure()
plt.title("Inverted")
plt.imshow(lena_inverted)