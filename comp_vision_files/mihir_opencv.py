import cv2
import numpy as np

# Define vector lengths and anchor point
fixed_vector_length = 100
moving_vector_length = 50
anchor_point = (100, 100)

def calculate_angle(v1, v2):
  # Calculate dot product
  dot_product = np.dot(v1, v2)
  # Calculate magnitudes
  mag_v1 = np.linalg.norm(v1)
  mag_v2 = np.linalg.norm(v2)
  # Handle division by zero
  if mag_v1 * mag_v2 == 0:
    return 0
  # Radian angle using acos
  angle_rad = np.arccos(dot_product / (mag_v1 * mag_v2))
  # Convert to degrees
  angle_deg = np.rad2deg(angle_rad)
  return angle_deg

def main():
  # Capture video from webcam
  cap = cv2.VideoCapture(0)

  while True:
    # Capture frame-by-frame
    ret, frame = cap.read()

    # Check if frame captured successfully 
    if not ret:
      print("Error: Could not capture frame")
      break

    # Get object coordinates (replace with your object detection logic)
    # Here, we'll simulate with random coordinates
    object_point = (anchor_point[0] + 50, anchor_point[1] + 20)

    # Calculate moving vector
    moving_vector = np.array(object_point) - np.array(anchor_point)

    # Calculate fixed vector endpoint
    fixed_vector_endpoint = (int(anchor_point[0] + fixed_vector_length * np.cos(np.pi/4)), 
                             int(anchor_point[1] + fixed_vector_length * np.sin(np.pi/4)))
    fixed_vector = np.array(fixed_vector_endpoint) - np.array(anchor_point)

    # Draw vectors
    cv2.arrowedLine(frame, anchor_point, tuple(fixed_vector_endpoint), (0, 255, 0), 2)
    cv2.arrowedLine(frame, anchor_point, tuple(object_point), (0, 0, 255), 2)

    # Calculate angle
    angle = calculate_angle(fixed_vector, moving_vector)

    # Display angle text
    cv2.putText(frame, f"Angle: {angle:.2f} degrees", (10, 30), cv2.FONT_HERSHEY_SIMPLEX, 
                1, (255, 255, 255), 2)

    # Display the resulting frame
    cv2.imshow('Live Stream', frame)

    # Exit loop on 'q' key press
    if cv2.waitKey(1) == ord('q'):
      break

  # Release capture
  cap.release()
  cv2.destroyAllWindows()

if __name__ == "__main__":
  main()







# import cv2
# import math

# def angle_between_points(p1, p2):
#   """
#   Calculates the angle in radians between two points.

#   Args:
#       p1: A tuple representing the first point (x, y).
#       p2: A tuple representing the second point (x, y).

#   Returns:
#       The angle between the two points in radians.
#   """
#   dx = p2[0] - p1[0]
#   dy = p2[1] - p1[1]
#   # Calculate the dot product
#   dot_product = dx * dy
#   # Calculate the magnitude of the vectors
#   magnitude1 = math.sqrt(dx**2 + dy**2)
#   magnitude2 = math.sqrt(p1[0]**2 + p1[1]**2)
#   # Avoid division by zero
#   if magnitude1 == 0 or magnitude2 == 0:
#     return 0
#   # Cosine of the angle
#   cosine = dot_product / (magnitude1 * magnitude2)
#   # Clamp to avoid errors due to floating point precision
#   cosine = max(-1.0, min(1.0, cosine))
#   # Radians between the vectors
#   angle_rad = math.acos(cosine)
#   return angle_rad

# # # Capture video from webcam
# cap = cv2.VideoCapture(0)

# # Object tracking parameters (adjust as needed)
# tracker = cv2.TrackerCSRT()  # You can choose a different tracker type
# tracker_init = False

# def click_event(event, x, y, flags, param):
#   global tracker_init, selected_point
#   if event == cv2.EVENT_LBUTTONDOWN:
#     selected_point = (x, y)
#     tracker_init = True

# cv2.namedWindow("Live Stream")
# cv2.setMouseCallback("Live Stream", click_event)

# while True:
#   # Capture frame-by-frame
#   ret, frame = cap.read()

#   # Initialize tracker if a point is selected
#   if tracker_init:
#     success, bounding_box = tracker.update(frame)
#     if success:
#       x, y, w, h = bounding_box
#       # Track the point based on the bounding box center
#       tracked_point = (int(x + w/2), int(y + h/2))
#       cv2.rectangle(frame, (x, y), (x + w, y + h), (255, 0, 0), 2)
#       cv2.circle(frame, tracked_point, 5, (0, 255, 0), -1)

#       # Display coordinates
#       cv2.putText(frame, f"Selected Point: ({selected_point[0]}, {selected_point[1]})", (10, 60), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (255, 255, 255), 2)
#       cv2.putText(frame, f"Tracked Point: ({tracked_point[0]}, {tracked_point[1]})", (10, 90), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (255, 255, 255), 2)

#       # Calculate angle if there's a second selected point
#       if selected_point:
#         angle = angle_between_points(selected_point, tracked_point)
#         angle_deg = math.degrees(angle)
#         cv2.putText(frame, f"Angle: {angle_deg:.2f} degrees", (10, 30), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 0, 255), 2)
#     else:
#       # Tracking failed, re-initialize
#       tracker_init = False

#   # Display the resulting frame
#   cv2.imshow('Live Stream', frame)

#   # Exit if 'q' key is pressed
#   if cv2.waitKey(1) == ord('q'):
#     break

# # Release capture
# cap.release()
# cv2.destroyAllWindows()



# # # Define point selection variables (modify as needed)
# # point1_selected = False
# # point2_selected = False
# # selected_point1 = None
# # selected_point2 = None

# # def click_event(event, x, y, flags, param):
# #   global point1_selected, point2_selected, selected_point1, selected_point2
# #   if event == cv2.EVENT_LBUTTONDOWN:
# #     if not point1_selected:
# #       selected_point1 = (x, y)
# #       point1_selected = True
# #     elif not point2_selected:
# #       selected_point2 = (x, y)
# #       point2_selected = True

# # cv2.namedWindow("Live Stream")
# # cv2.setMouseCallback("Live Stream", click_event)

# # while True:
# #   # Capture frame-by-frame
# #   ret, frame = cap.read()

# #   # Draw circles on selected points
# #   if selected_point1:
# #     cv2.circle(frame, selected_point1, 5, (0, 255, 0), -1)
# #   if selected_point2:
# #     cv2.circle(frame, selected_point2, 5, (0, 0, 255), -1)

# #   # Calculate angle if both points are selected
# #   if point1_selected and point2_selected:
# #     angle = angle_between_points(selected_point1, selected_point2)
# #     angle_deg = math.degrees(angle)
# #     cv2.putText(frame, f"Angle: {angle_deg:.2f} degrees", (10, 30), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 0, 255), 2)

# #   # Display the resulting frame
# #   cv2.imshow('Live Stream', frame)

# #   # Exit if 'q' key is pressed
# #   if cv2.waitKey(1) == ord('q'):
# #     break

# # # Release capture
# # cap.release()
# # cv2.destroyAllWindows()
