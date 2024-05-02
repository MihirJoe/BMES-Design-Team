# import cv2
# import numpy as np
# import math

# points  = []
# cap = cv2.VideoCapture(0)  # Capturing video from the webcam

# def drawcircle(event, x, y, flags, params):
#     if event==cv2.EVENT_LBUTTONDOWN:
#         # cv2.circle(img, (x,y), 6, (255,0,0), -1)
#         cv2.circle(frame, (x, y), 6, (255, 0, 0), -1)
#         if(len(points) !=0):
#             # cv2.arrowedLine(img, tuple(points[0]), (x,y), (255,0,0), 3)
#             cv2.arrowedLine(frame, tuple(points[0]), (x,y), (255,0,0), 3)
#         points.append([x,y])
#         # cv2.imshow('image', img)
#         cv2.imshow('Webcam', frame)
#         print(points)
#         if(len(points)==3):
#             degrees = findangle()
#             print((degrees))
        

# # Y2-Y1 / X2-X1
# def slope(p1, p2):
#     return (p2[1] - p1[1])/(p2[0] - p1[0])
        
# def findangle():
#     a = points[-2]
#     b = points[-3]
#     c = points[-1]
#     m1 = slope(b, a)
#     m2 = slope(b, c)

#     # Calculate angle using tan inverse formula
#     angle = math.atan((m2-m1)/1+m1*m2)
#     angle = round(math.degrees((angle)))

#     # Obtuse and acute condition
#     if angle<0:
#         angle = 180+angle
#     # cv2.putText(img, str(angle), (b[0]-40, b[1]+40), cv2.FONT_HERSHEY_DUPLEX, 1, (0,0,225), 1, cv2.LINE_AA)
#     cv2.putText(frame, str(angle), (b[0]-40, b[1]+40), cv2.FONT_HERSHEY_DUPLEX, 1, (0,0,225), 1, cv2.LINE_AA)
#     # cv2.imshow('image', img)
#     cv2.imshow('Webcam', frame)

#     return angle


# # # img = np.zeros((512,512,3), np.uint8)
# # img = cv2.imread('prot.png')

# # while True:
# #     cv2.imshow('image', img)
# #     cv2.setMouseCallback('image', drawcircle)
# #     # cv2.waitKey(0)

# #     # refresh button is 'r'
# #     if  cv2.waitKey(1)&0xff==ord('r'):
# #         img = np.zeros((512,512,3), np.uint8)
# #         points = []
# #         cv2.imshow('image', img)
# #         cv2.setMouseCallback('image', drawcircle)

# #         # Hit q to end
# #     if  cv2.waitKey(1)&0xff==ord('q'):
# #         break

# while True:
#     ret, frame = cap.read()  # Capture frame-by-frame
#     if not ret:
#         break

#     cv2.imshow('Webcam', frame)
#     cv2.setMouseCallback('Webcam', drawcircle)

#     # Hit 'r' to reset points
#     if cv2.waitKey(1) & 0xFF == ord('r'):
#         frame = np.zeros(frame.shape, np.uint8)
#         points = []
#         cv2.imshow('Webcam', frame)
#         cv2.setMouseCallback('Webcam', drawcircle)

#     # Hit 'q' to quit
#     if cv2.waitKey(1) & 0xFF == ord('q'):
#         break

# cap.release()
# cv2.destroyAllWindows()


import cv2
import numpy as np
import math

points = []
cap = cv2.VideoCapture(0)  # Capturing video from the webcam

def drawcircle(event, x, y, flags, params):
    global points
    if event == cv2.EVENT_LBUTTONDOWN:
        cv2.circle(frame, (x, y), 6, (255, 0, 0), -1)
        points.append([x, y])
        cv2.imshow('Webcam', frame)
        print(points)
        if len(points) == 2:
            draw_arrow()
        elif len(points) == 3:
            degrees = findangle()
            print((degrees))

def slope(p1, p2):
    return (p2[1] - p1[1]) / (p2[0] - p1[0])

def findangle():
    a = points[-2]
    b = points[-3]
    c = points[-1]
    m1 = slope(b, a)
    m2 = slope(b, c)

    angle = math.atan((m2 - m1) / (1 + m1 * m2))
    angle = round(math.degrees(angle))

    if angle < 0:
        angle = 180 + angle
    cv2.putText(frame, str(angle), (b[0] - 40, b[1] + 40), cv2.FONT_HERSHEY_DUPLEX, 1, (0, 0, 225), 1, cv2.LINE_AA)
    cv2.imshow('Webcam', frame)

    return angle

def draw_arrow():
    if len(points) >= 2:
        cv2.arrowedLine(frame, tuple(points[0]), tuple(points[1]), (255, 0, 0), 3)
        cv2.imshow('Webcam', frame)

while True:
    ret, frame = cap.read()  # Capture frame-by-frame
    if not ret:
        break

    cv2.imshow('Webcam', frame)
    cv2.setMouseCallback('Webcam', drawcircle)

    # Hit 'r' to reset points
    if cv2.waitKey(1) & 0xFF == ord('r'):
        frame = np.zeros(frame.shape, np.uint8)
        points = []
        cv2.imshow('Webcam', frame)
        cv2.setMouseCallback('Webcam', drawcircle)

    # Hit 'q' to quit
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()


