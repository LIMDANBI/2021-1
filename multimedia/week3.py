import cv2 #영상처리 라이브러
import matplotlib.pyplot as plt #영상출력라이브러리
img_lena = cv2.imread("./Lena.jpg")
img_desert = cv2.imread("./Desert.jpg")
img_lena = cv2.cvtColor(img_lena, cv2.COLOR_BGR2RGB) # BGR format -> RGB format
img_desert = cv2.cvtColor(img_desert, cv2.COLOR_BGR2RGB)
plt.imshow(img_lena)
plt.figure()
plt.imshow(img_desert)
