import cv2
import numpy as np

img = np.zeros((128, 128), np.uint8)

while True:
    cv2.imshow("image", img)
    if cv2.waitKey(1) == ord("q"):
        break

cv2.destroyAllWindows()
