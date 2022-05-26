import serial, csv
ser = serial.Serial('COM5', 115200)
ser.write("go".encode("utf-8"))


data = []
while True:
    curr = str(ser.readline())
    
    curr = curr[2:-6]
    ls = curr.split(",")

    data.append(ls)
    print(ls)

    with open('temperature_logs.csv', 'a', newline="") as f:
      
        # using csv.writer method from CSV package
        f.write(curr + "\n")