from serial import *
from tkinter import *
from functools import partial
from time import sleep

#make a TkInter Window
root = Tk()
root.wm_title("Train Elevator")

# function to update the Display
def displayInfo(line1, line2, line3, fontSize, height, width):
    display.config(font =("Courier", fontSize), height=height, width=width)
    display.delete('1.0', END)
    display.insert(END,line1)
    display.insert(END,line2)
    display.insert(END,line3)
    display.tag_add("tag_name", "1.0", "end")

def restoreCalibration(level):
    log.insert(END, "Restore calibration "+str(level)+"\n")
def moveToCalibration(level):
    log.insert(END, "Move to calibration "+str(level)+"\n")
def saveCalibration(level):
    log.insert(END, "Save calibration "+str(level)+"\n")

#add buttons in a frame
buttonFrame = Frame(root)
#buttonFrame.pack( fill = NONE, expand = False, anchor = NW)
buttonFrame.grid(column=0, row=0, sticky=NW)
levelButtons = []
restoreButtons = []
moveButtons = []
saveButtons = []
for level in range (12):
    lButton = Button(buttonFrame, text ="Level "+str(level+1), command = partial (displayInfo, "",    str(level+1),     "",220,1, 3), bg='green', fg='white')
    levelButtons.append(lButton)
    levelButtons[level].grid(column=0, row=level, sticky=W, padx=25, pady=5,  ipadx=25, ipady=5 )
    lButton = Button(buttonFrame, text ="Restore", command = partial (restoreCalibration, level+1), bg='green', fg='white', state=DISABLED)
    restoreButtons.append(lButton)
    restoreButtons[level].grid(column=1, row=level, sticky=W, padx=25, pady=5,  ipadx=25, ipady=5 )
    lButton = Button(buttonFrame, text ="Move", command = partial (moveToCalibration, level+1), bg='green', fg='white', state=DISABLED)
    moveButtons.append(lButton)
    moveButtons[level].grid(column=2, row=level, sticky=W, padx=25, pady=5,  ipadx=25, ipady=5 )
    lButton = Button(buttonFrame, text ="Save", command = partial (saveCalibration, level+1), bg='green', fg='white', state=DISABLED)
    saveButtons.append(lButton)
    saveButtons[level].grid(column=3, row=level, sticky=W, padx=25, pady=5,  ipadx=25, ipady=5 )

levelButtons[5].config(command = partial (displayInfo, "\n\n","Homing","", 48,5,10))
levelButtons[6].config(command = partial (displayInfo, "\n5\n","\u21C8\n","3", 48,5,10))
levelButtons[7].config(command = partial (displayInfo, "\n7\n","\u21CA\n","12", 48,5,10))
levelButtons[10].config(state=DISABLED)
restoreButtons[5].config(state=NORMAL)
moveButtons[5].config(state=NORMAL)
saveButtons[5].config(state=NORMAL)

# make a text box to image the physical display
display = Text ( root, bg="darkblue", fg="white", font =("Courier", 48), width=10, height=5, takefocus=0)
display.tag_configure("tag_name", justify='center')
#display.pack( fill = NONE, expand = False, anchor = S )
display.grid(column=1, row=0, sticky=E)

#make a frame for a scrollbar and a text box to put the serial output
logFrame = Frame(root)
#logFrame.pack( fill = BOTH, expand = True, side=BOTTOM )
logFrame.grid(column=0, row=1, columnspan=2, sticky=S )
# make the scrollbar
scrollbar = Scrollbar(logFrame)
scrollbar.pack(side=RIGHT, fill=Y)
# make the text box
log = Text ( logFrame, width=140, height=10, takefocus=0)
log.pack( fill = BOTH, expand = True )
# attach text box to scrollbar
log.config(yscrollcommand=scrollbar.set)
scrollbar.config(command=log.yview)



log.insert(END,"davs du lad os skrive noget ganske forfærdeligt langt så vi kan se, hvordan det wrapper eller ekspanderer\n")
log.insert(END,"linie 2\n")
log.insert(END,"davs du lad os skrive noget ganske forfærdeligt langt så vi kan se, hvordan det wrapper eller ekspanderer\n")
log.insert(END,"linie 4\n")

displayInfo("",    "2",     "",144,1, 3)
displayInfo("\n\n","Homing","", 48,5,10)

# Set the serial communication up
# serialPort = "COM4"
# baudRate = 19200
# ser = Serial(serialPort , baudRate, timeout=0, writeTimeout=0) #ensure non-blocking
ser = Serial()
ser.baudrate = 19200

#make our own buffer
#useful for parsing commands
#Serial.readline seems unreliable at times too
serBuffer = ""

def readSerial():
    while True:
        while not ser.is_open:
            try:
                ser.port = 'COM3'
                ser.open()
            except:
                log.insert(END, "Failed to connect to COM3")
            if ser.is_open:
                break
            try:
                ser.port = 'COM4'
                ser.open()
            except:
                log.insert(END, "Failed to connect to COM4")
                sleep(15)
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
    if ser.is_open
        ser.write(cmd.encode("utf-8"))
    root.after(10000, readStatus)
    
root.after(10000, readStatus)
    

root.mainloop()
