# Temperature, Humidity, and Air Quality Sensor

## Custom built DHT11 and MQ135 device powered by an ESP8266 board

This project is a school requirement that tasked us to create a device that gathers data using 
a DHT11 Temperature sensor and a MQ135 Gas sensor in a specific location around campus. 
This device collects data every minute which then avarage the collected data every 10-mins 
and gets thrown in a Google Sheet file that is connected to the board. 

5 groups are tasked in different locations around campus to gether data for 8-hrs a day for 5 days
using our custom built devices that will then be combined into one master dataset. The data collected will
eventually be used in a Regression analysis which is also included in this repository.

We followed a YouTube tutorial on how to connect the ESP8266 board to Google Sheets in order to put the collected data which includes values of: 
* Time and Date 
* Temperature °C
* Humidity
* Air Quality

## Device
<img src="photo_2024-12-21_14-29-08-1.jpg" alt="device" class = "center" width="300"/>
![Alt text](https://drive.google.com/file/d/1l155bqiBxRXbo7lcVwDLn2uJSIkU7trI/view?usp=sharing "a title")
## Reference 
* https://www.youtube.com/watch?v=uuPyRGJSsHs 