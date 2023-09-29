# System Requirements Specification (SRS)
**Team:** 0xC  
**Date:** 09 Sep 2023

## GUI Requirements

### Temperature Display

1. When the temperature box is on and the temperature sensor is connected, the GUI shall display real-time temperature data from the temperature sensor box.
2. The real-time temperature display must be prominent and updated once a second.
3. The real-time temperature display must include an option to convert the displayed value between degrees Fahrenheit and Celsius. 
4. When the temperature box is on, but the temperature sensor is disconnected, the GUI shall display “unplugged sensor.”
5. When the temperature box is off, the GUI shall display “no data available.”

### Controls

6. The GUI will include a button that turns on/off the temperature box’s LED screen temperature display. The response time of this button shall be less than 1 second.

### Graph Display

7. When the computer running the GUI software is wirelessly connected to the box and the temperature box is on, the GUI shall display a graph containing the past 300 seconds of temperature data from the temperature box.
8. For the GUI’s graph of the past 300 seconds of temperature data:
    - The y-axis shall be the temperature value.
    - The x-axis shall be the number of seconds past going back 300 seconds.
    - The graph shall take no more than 10 seconds to load when the GUI SW starts.
    - The graph shall include the option to convert between degrees Celsius and Fahrenheit.
    - The top of the graph’s y-axis shall be 50 degrees Celsius/122 degrees Fahrenheit. The bottom of the graph’s y-axis shall be 10 degrees Celsius/50 degrees Fahrenheit.
    - The graph shall scroll horizontally, with the current temperature on the right side and the past temperatures on the left.
    - The GUI shall label the x-axis: “seconds ago from the current time.”

### Handling Missing Data

9. If there is data missing (temperature box turned off, sensor disconnected, etc.), the GUI SW shall make this absence of data clear in the graph.
10. If there is data missing, the GUI SW shall continue to scroll the graph and repopulate the graph if it gets data.

### Notifications

11. When the GUI SW is on and the temperature box is connected, a text message shall be sent to a phone number if the temperature goes above an upper threshold or below a lower threshold. The content of the message, phone number, and threshold values can be edited by the user.

## Networking Requirements

- The Third Box shall accept brightness commands, which can be set by the GUI SW.
- The Third Box shall accept contrast commands, which can be set by the GUI SW.

## Hardware Requirements for the Third Box

### Box

1. The Third Box shall be battery operated.
2. The Third Box shall be enclosed in a container which will not break when dropped from a height of 2 meters or less.
3. All external ports, connectors, and switches of the Third Box shall not break when dropped from a height of 2 meters or less; though they may become disconnected.
4. The Third Box shall be able to operate in full capacity in any physical orientation.
5. All external ports, connectors, and switches of the Third Box shall be able to withstand an insertion force of 8 Newtons or less.
6. All external ports, connectors, and switches of the Third Box shall be able to be disconnected with no more than 8 Newtons of force.
7. The Third Box shall store the last 300 seconds of valid temperature data.

### Temperature Sensor

1. The Third Box shall use a detachable temperature sensor to record temperature.
2. The detachable temperature sensor shall have a cable that is 2.0 meters long.
3. The detachable temperature sensor shall operate when placed in water.
4. The temperature sensor shall operate in a range from -10 to 63 degrees Celsius.
5. The Third Box SW shall record temperature at a rate of 1hz.
6. The Third Box shall record temperature automatically if the Third Box is powered on and the temperature sensor is connected.

### Display

1. The Third Box shall have an LCD Display.
2. The LCD Display of the Third Box shall show the current temperature (as recorded by the temperature sensor) in degrees Celsius if the button on the third box has been toggled on.
3. The LCD Display of the Third Box shall display the current temperature in degrees Celsius no more than 20 milliseconds after the button on the Third Box is depressed.
4. The LCD Display of the Third Box shall have a brightness which can be set by the GUI SW.
5. The LCD Display of the Third Box shall have a contrast which can be set by the GUI SW.
6. The LCD Display of the Third Box shall use a blue background.
7. The LCD Display of the Third Box shall use white text to display all messages.

### Button

1. The button of the Third Box shall be depressed when pressed by the user.
2. The button of the Third Box shall not be depressed when not pressed by the user.
3. The LCD Display of the Third Box shall be on while the button of the Third Box is depressed.
4. The LCD Display of the Third Box shall be off while the button of the Third Box is not depressed and the display has not been toggled on via the GUI SW.
5. The LCD Display of the Third Box shall display an error message while the temperature sensor is not connected to the Third Box.
6. The LCD Display of the Third Box shall display an error message while the Third Box is experiencing any error with the temperature sensor.

