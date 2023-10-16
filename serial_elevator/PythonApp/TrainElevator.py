from serial import *
from tkinter import *

serialPort = "COM4"
baudRate = 19200
ser = Serial(serialPort , baudRate, timeout=0, writeTimeout=0) #ensure non-blocking

#make a TkInter Window
root = Tk()
root.wm_title("Reading Serial")

# make a scrollbar
scrollbar = Scrollbar(root)
scrollbar.pack(side=RIGHT, fill=Y)

# make a text box to put the serial output
log = Text ( root, width=30, height=30, takefocus=0)
log.pack()

# attach text box to scrollbar
log.config(yscrollcommand=scrollbar.set)
scrollbar.config(command=log.yview)

#make our own buffer
#useful for parsing commands
#Serial.readline seems unreliable at times too
serBuffer = ""

def readSerial():
    while True:
        c = ser.read() # attempt to read a character from Serial
        
        #was anything read?
        if len(c) == 0:
            break

        # log.insert('0.0', " ")
        # log.insert('0.0', ord(c))
        # log.insert('0.0', " ")
        # log.insert('0.0', c)
        
        # get the buffer from outside of this function
        global serBuffer
        
        # check if character is a delimeter
        if ord(c) == 13:
            continue  # don't want returns. chuck it
            
        if ord(c) == 10:
            serBuffer += "\n" # add the newline to the buffer
            
            #add the line to the TOP of the log
            log.insert('0.0', serBuffer)
            serBuffer = "" # empty the buffer
        else:
            try:
                serBuffer += c.decode("utf-8")  # add to the buffer
            except:
                continue
    
    root.after(10, readSerial) # check serial again soon


# after initializing serial, an arduino may need a bit of time to reset
root.after(100, readSerial)

def readStatus():
    cmd = "get status\n"
    ser.write(cmd.encode("utf-8"))
    root.after(10000, readStatus)
    
root.after(10000, readStatus)
    

root.mainloop()