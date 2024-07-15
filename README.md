
# MissionConverter

MissionConverter is a tool designed to prepare routes for the Pixhawk 4 customized autopilot. It includes functionalities to view maps, check altitudes at specific coordinates, and configure various parameters of the Pixhawk such as FBWA motor power, stabilization motor power, flight height, GPS wait time, and more.

## Features

- Prepare and customize routes for Pixhawk 4 autopilot.
- View maps and check altitudes at specific coordinates.
- Configure Pixhawk settings:
  - Start delay (seconds)
  - FBWA motor power (%)
  - Stabilize motor power (%)
  - Flight height (m)
  - Flight height offset (m)
  - Waypoint radius (m)
  - Stabilize flight time (seconds)
  - GPS max wait time (seconds)

## Installation

To set up the project, follow these steps:

1. **Install dependencies**:
    The project requires Qt (either Qt5 or Qt6). Ensure you have Qt installed.

2. **Build the project**:
    ```mkdir build && cd build && cmake .. && cmake --build . -j$(nproc)```

## Usage

1. **Run the executable**:
    ```bash
    ./bin/MissionConverter
    ```

2. **Using the UI**:
    - Open the application.
    - Use the map interface to set waypoints and check altitudes.
    - Configure the Pixhawk settings through the provided UI elements.

## Project Structure

- `main.cpp`: Entry point of the application.
- `mainwindow.cpp` and `mainwindow.h`: Main window logic and UI handling.
- `mainwindow.ui`: Qt Designer UI file for the main window.
- `defines.h`: Header file for various definitions.
- `Leaflet.PolylineMeasure.css`, `Leaflet.PolylineMeasure.js`, `map.html`, `map.js`: Files related to the map interface and functionalities.

## Dependencies

- **Qt**: For UI and web integration (either Qt5 or Qt6).
- **Leaflet**: For map rendering.
- **jQuery**: For AJAX calls to fetch elevation data.

## License
This project is licensed under the MIT License.

## Acknowledgments

- [Leaflet](https://leafletjs.com/)
- [Qt](https://www.qt.io/)
- [jQuery](https://jquery.com/)