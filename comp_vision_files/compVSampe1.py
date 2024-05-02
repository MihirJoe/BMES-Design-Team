import cv2
import numpy as np

# Global variables to store selected points
selected_points = []

# Function to calculate angle between three points
def calculate_angle(point1, center_point, point2):
    vector1 = point1 - center_point
    vector2 = point2 - center_point

    # Ensure the vectors are not zero
    if np.linalg.norm(vector1) == 0 or np.linalg.norm(vector2) == 0:
        return 0.0  # Return 0 if one of the vectors has zero magnitude

    cosine_angle = np.dot(vector1, vector2) / (np.linalg.norm(vector1) * np.linalg.norm(vector2))
    angle = np.arccos(np.clip(cosine_angle, -1.0, 1.0))  # Clip to ensure the value is within [-1, 1] range
    return np.degrees(angle)

# Mouse callback function to select points
def select_points(event, x, y, flags, param):
    global selected_points

    if event == cv2.EVENT_LBUTTONDOWN:
        if len(selected_points) < 2:
            selected_points.append((x, y))

# Initialize camera
cap = cv2.VideoCapture(0)

# Create a window and set mouse callback function
cv2.namedWindow('Frame')
cv2.setMouseCallback('Frame', select_points)

while True:
    ret, frame = cap.read()
    if not ret:
        break

    # Display the frame
    frame_with_points = frame.copy()  # Create a copy of the frame to draw points on
    for point in selected_points:
        cv2.circle(frame_with_points, point, 5, (0, 0, 255), -1)
    cv2.imshow('Frame', frame_with_points)

    # Check if two points are selected
    if len(selected_points) == 2:
        # Calculate the angle between the two selected points and the center of the frame
        center_point = np.array([frame.shape[1] // 2, frame.shape[0] // 2])  # Center of the frame
        point1 = np.array(selected_points[0])
        point2 = np.array(selected_points[1])
        angle = calculate_angle(point1, center_point, point2)
        print("Angle between points: {:.2f} degrees".format(angle))
        selected_points = []  # Reset selected points

    # Press 'q' to exit
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# Release the camera and close all OpenCV windows
cap.release()
cv2.destroyAllWindows()
