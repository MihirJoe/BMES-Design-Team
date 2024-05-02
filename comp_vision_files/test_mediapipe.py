import cv2
import mediapipe as mp

mp_hand = mp.solutions.hands
hands = mp_hand.Hands()
cap = cv2.VideoCapture(0)


while True:
        success, img = cap.read()

        if not success:
            break

        result = hands.process(cv2.cvtColor(img, cv2.COLOR_BGR2LAB))

        print(result.multi_hand_landmarks)

        cv2.imshow("Image", img)
        cv2.waitKey(1)

cap.release()
cv2.destroyAllWindows()
