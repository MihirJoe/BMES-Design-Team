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

def track_object(frame, tracker):
  # Update tracker
  ret, bounding_box = tracker.update(frame)

  if ret:
    # Extract bounding box coordinates
    (x, y, w, h) = bounding_box
    center_x = int(x + w / 2)
    center_y = int(y + h / 2)
    return (center_x, center_y)
  else:
    return None

def main():
  # Capture video from webcam
  cap = cv2.VideoCapture(0)

  # Create CSRT tracker
  tracker = cv2.TrackerCSRT_create()

  # Initialization flag
  init = False

  while True:
    # Capture frame-by-frame
    ret, frame = cap.read()

    # Check if frame captured successfully 
    if not ret:
      print("Error: Could not capture frame")
      break

    if not init:
      # Select object using mouse selection (modify as needed)
      selection = cv2.selectROI("Select Object", frame)
      init = True
      # Initialize tracker with bounding box
      tracker.init(frame, selection)

    # Track object
    object_point = track_object(frame, tracker)

    if object_point is not None:
      # Calculate moving vector
      moving_vector = np.array(object_point) - np.array(anchor_point)

      # Calculate fixed vector endpoint
      fixed_vector_endpoint = (int(anchor_point[0] + fixed_vector_length * np.cos(np.pi/4)), 
                               int(anchor_point[1] + fixed_vector_length * np.sin(np.pi/4)))
      fixed_vector = np.array(fixed_vector_endpoint) - np.array(anchor_point)

      # Draw vectors
      cv2.arrowedLine(frame, anchor_point, tuple(fixed_vector_endpoint), (0, 255, 0), 2)
      cv2.arrowedLine(frame, anchor_point, object_point, (0, 0, 255), 2)

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
