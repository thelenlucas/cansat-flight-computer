import serial
ser = serial.Serial('COM4', 9600)
ser.write("go".encode("utf-8"))


data = []
while True:
    curr = str(ser.readline()) + "\n"
    data.append(curr)
    print(curr)
    a = open("logs.csv", "a")

    a.write(curr)
    a.close()
    