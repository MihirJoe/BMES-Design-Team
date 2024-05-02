import cv2
import numpy as np
import math

def nothing(x):
   pass

def slope(p1, p2):
    return (p2[1] - p1[1]) / (p2[0] - p1[0])

def findangle(vertices, img):
    a = vertices[-2]
    b = vertices[-3]
    c = vertices[-1]
    m1 = slope(b, a)
    m2 = slope(b, c)
    Calc_angle = (m2 - m1) / (1 + m1 * m2)
    rad_angle = math.atan(Calc_angle)
    degree_angle = round(math.degrees(rad_angle))
    print("degree angle is ", degree_angle)
    if degree_angle < 0:
        degree_angle = 180 + degree_angle
    cv2.putText(img, str(degree_angle), (b[0] + 40, b[1] + 40), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 255, 255), 1, cv2.LINE_AA)

cap = cv2.VideoCapture(0)
font = cv2.FONT_HERSHEY_SIMPLEX

while True:
   _,frame = cap.read()
   hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
   lower_red = np.array([0,153,82])
   upper_red = np.array([255,255,180])
   mask = cv2.inRange(hsv,lower_red,upper_red)
   kernel = np.ones((5,5),np.uint8)
   mask = cv2.erode(mask, kernel)

   # Contours detection
   contours, _ = cv2.findContours(mask, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
   for cnt in contours:
      area = cv2.contourArea(cnt)
      approx = cv2.approxPolyDP(cnt, 0.02*cv2.arcLength(cnt, True),True)
      # Coordinate
      x = approx.ravel()[0]
      y = approx.ravel()[1]
      print(x)
      if area > 50:
         cv2.drawContours(frame,[approx],0,(0,0,0),5) # 5 is thickness
         print(len(approx))
         if len(approx) == 3:
            cv2.putText(frame, "Triangle", (x, y), font, 1, (0, 0, 0))
         if len(approx) == 4:
            cv2.putText(frame, "Rectangle", (x, y), font, 1, (0, 0, 0))
         elif 10 < len(approx) < 20:
            cv2.putText(frame, "Circle", (x, y), font, 1, (0, 0, 0))

   # Used to store coordinates of vertices
   vertices = []
   
   # Extract vertices
   n = approx.ravel()
   i = 0

   for j in n:
      if (i % 2 == 0):
         x = n[i]
         y = n[i + 1]
         
         # String containing the coordinates.
         string = str(x) + " " + str(y)
         cv2.putText(frame, string, (x, y), font, 0.5, (0, 255, 0))
         
         # Append coordinates to the list
         vertices.append((x, y))
      i = i + 1

   # Pass the list of vertices to the findangle() function
   findangle(vertices, frame)


   cv2.imshow('Frame', frame)
   k = cv2.waitKey(5) & 0xFF
   if k == 27:
      break

cv2.destroyAllWindows()
cap.release()
