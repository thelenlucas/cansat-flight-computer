import serial
ser = serial.Serial('COM7', 9600)
ser.write("go".encode("utf-8"))


data = []
while True:
    curr = str(ser.readline()) + "\n"
    curr = curr[2:-6]
    data.append(curr)
    print(curr)
    a = open("logs.csv", "a")

    a.write(curr)
    a.close()
    