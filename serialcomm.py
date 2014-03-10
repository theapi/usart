import serial

s = serial.Serial(port='/dev/ttyUSB0', baudrate=19200)
print s
while True:
    line = s.readline()
    line = line.strip()
    print line
