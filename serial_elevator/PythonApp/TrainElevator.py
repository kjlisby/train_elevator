from serial import *
from tkinter import *
from functools import partial
from time import sleep, strftime

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
    
currentLevel = 0;

def disableButton(b):
    b.config(state=DISABLED, bg='darkgrey')
def enableButton(b):
    b.config(state=NORMAL, bg='green')

def moveElevator(level):
    logDebug("Move Elevator "+str(level))
    sendCmd("set level "+str(level))
def restoreCalibration(level):
    logDebug("Restore calibration "+str(level))
    sendCmd("get calibration "+str(level))
def moveToCalibration(level):
    left = leftSpinboxes[level-1].get()
    right = rightSpinboxes[level-1].get()
    logDebug("Move to calibration "+str(level))
    sendCmd("set levelfromcalibration "+str(level)+" "+left+" "+right)
def saveCalibration(level):
    left = leftSpinboxes[level-1].get()
    right = rightSpinboxes[level-1].get()
    logDebug("Save calibration "+str(level))
    sendCmd("set calibration"+str(level)+" "+left+" "+right)

def displayIdle(level):
    displayInfo("",    str(level+1),     "",220,1, 3)
def displayMoving(fromLevel, toLevel):
    displayInfo("\n5\n","\u21C8\n","3", 48,5,10)
    displayInfo("\n7\n","\u21CA\n","12", 48,5,10)
def displayHoming():
    displayInfo("\n\n","Homing","", 48,5,10)

def logDebug(line):
    log.insert(END, strftime("%H:%M:%S", time.localtime()) + " " + line + "\n")
    log.see(END)

#add buttons etc. in a frame
buttonFrame = Frame(root)
#buttonFrame.pack( fill = NONE, expand = False, anchor = NW)
buttonFrame.grid(column=0, row=0, sticky=NW)
levelButtons = []
leftSpinboxes = []
rightSpinboxes = []
restoreButtons = []
moveButtons = []
saveButtons = []
for level in range (12):
    lButton = Button(buttonFrame, text ="Level "+str(level+1), command = partial (moveElevator, level+1), bg='green', fg='white')
    levelButtons.append(lButton)
    levelButtons[level].grid(column=0, row=level, sticky=W, padx=5, pady=5,  ipadx=25, ipady=5 )

    if level % 2 == 0:
        calibration=5000*level
    else:
        calibration=5000*(level-1)
    min_ = calibration-500
    max_ = calibration+500
    lButton = Spinbox(buttonFrame, width=10, from_=min_, to=max_)
    leftSpinboxes.append(lButton)
    leftSpinboxes[level].grid(column=1, row=level, sticky=W, padx=5, pady=5,  ipadx=25, ipady=5 )
    leftSpinboxes[level].delete(0,END)
    leftSpinboxes[level].insert(0,str(calibration))

    lButton = Spinbox(buttonFrame, width=10, from_=min_, to=max_)
    rightSpinboxes.append(lButton)
    rightSpinboxes[level].grid(column=2, row=level, sticky=W, padx=5, pady=5,  ipadx=25, ipady=5 )
    rightSpinboxes[level].delete(0,END)
    rightSpinboxes[level].insert(0,str(calibration))

    lButton = Button(buttonFrame, text ="Restore", command = partial (restoreCalibration, level+1), bg='darkgrey', fg='white', state=DISABLED)
    restoreButtons.append(lButton)
    restoreButtons[level].grid(column=3, row=level, sticky=W, padx=5, pady=5,  ipadx=25, ipady=5 )

    lButton = Button(buttonFrame, text ="Move", command = partial (moveToCalibration, level+1), bg='darkgrey', fg='white', state=DISABLED)
    moveButtons.append(lButton)
    moveButtons[level].grid(column=4, row=level, sticky=W, padx=5, pady=5,  ipadx=25, ipady=5 )

    lButton = Button(buttonFrame, text ="Save", command = partial (saveCalibration, level+1), bg='darkgrey', fg='white', state=DISABLED)
    saveButtons.append(lButton)
    saveButtons[level].grid(column=5, row=level, sticky=W, padx=5, pady=5,  ipadx=25, ipady=5 )

# levelButtons[4].config(command = partial (displayInfo, "",    str(level+1),     "",220,1, 3))
# levelButtons[5].config(command = partial (displayInfo, "\n\n","Homing","", 48,5,10))
# levelButtons[6].config(command = partial (displayInfo, "\n5\n","\u21C8\n","3", 48,5,10))
# levelButtons[7].config(command = partial (displayInfo, "\n7\n","\u21CA\n","12", 48,5,10))
enableButton(restoreButtons[5])
enableButton(moveButtons[5])
enableButton(saveButtons[5])

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


# log.insert(END,"davs du lad os skrive noget ganske forfærdeligt langt så vi kan se, hvordan det wrapper eller ekspanderer\n")
# log.insert(END,"linie 2\n")
# log.insert(END,"davs du lad os skrive noget ganske forfærdeligt langt så vi kan se, hvordan det wrapper eller ekspanderer\n")
# log.insert(END,"linie 4\n")

# displayInfo("",    "2",     "",144,1, 3)
# displayInfo("\n\n","Homing","", 48,5,10)

def handleInput(event):
    #add the line to the TOP of the log
    logDebug("Rx: "+serBuffer)

# Set the serial communication up
# serialPort = "COM4"
# baudRate = 19200
# ser = Serial(serialPort , baudRate, timeout=0, writeTimeout=0) #ensure non-blocking
ser = Serial()
ser.baudrate = 19200
ser.timeout=0
ser.writeTimeout=0

#make our own buffer
#useful for parsing commands
#Serial.readline seems unreliable at times too
serBuffer = ""

def readSerial():
    while True:
        if not ser.is_open:
            try:
                ser.port = 'COM3'
                ser.open()
            except:
                logDebug("Failed to connect to COM3")
        if not ser.is_open:
            try:
                ser.port = 'COM4'
                ser.open()
            except:
                logDebug("Failed to connect to COM4")
        if not ser.is_open:
            break
        c = ser.read() # attempt to read a character from Serial
        
        #was anything read?
        if len(c) == 0:
            break
        
        # get the buffer from outside of this function
        global serBuffer
        
        # check if character is a delimeter
        if ord(c) == 13:
            continue  # don't want returns. chuck it
            
        if ord(c) == 10:
            handleInput(serBuffer)
            serBuffer = "" # empty the buffer
        else:
            try:
                serBuffer += c.decode("utf-8")  # add to the buffer
            except:
                continue
    if ser.is_open:
        root.after(10, readSerial) # check serial again soon
    else:
        root.after(30000, readSerial) # take a break before checking serial again


# after initializing serial, an arduino may need a bit of time to reset
root.after(100, readSerial)

def sendCmd(cmd):
    if ser.is_open:
        logDebug("Tx: "+cmd)
        cmd+="\n"
        ser.write(cmd.encode("utf-8"))
    else:
        logDebug("Tx: "+cmd+" FAILED")

def readStatus():
    sendCmd("get status")
    root.after(60000, readStatus)
    
root.after(10000, readStatus)
    

root.mainloop()
