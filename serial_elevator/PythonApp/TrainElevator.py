from serial import *
from tkinter import *
from functools import partial
from time import sleep, strftime

root = Tk()
root.wm_title("Train Elevator")
    
CurrentLevel = 0
ReadingCalibration = False

#handle button press
def moveElevator(level):
    sendCmd("set level "+str(level))
def restoreCalibration(level):
    global ReadingCalibration
    sendCmd("get calibration "+str(level))
    ReadingCalibration = True
def moveToCalibration(level):
    left = leftSpinboxes[level-1].get()
    right = rightSpinboxes[level-1].get()
    sendCmd("set levelfromcalibration "+str(level)+" "+left+" "+right)
def saveCalibration(level):
    left = leftSpinboxes[level-1].get()
    right = rightSpinboxes[level-1].get()
    sendCmd("set calibration "+str(level)+" "+left+" "+right)

def logDebug(line):
    log.config(state=NORMAL)
    log.insert(END, strftime("%H:%M:%S", time.localtime()) + " " + line + "\n")
    log.see(END)
    log.config(state=DISABLED)

#enable/disable calibration widgets
def disableButtons(level):
    leftSpinboxes[level-1].config(state=DISABLED)
    rightSpinboxes[level-1].config(state=DISABLED)
    restoreButtons[level-1].config(state=DISABLED, bg='darkgrey')
    moveButtons[level-1].config(state=DISABLED, bg='darkgrey')
    saveButtons[level-1].config(state=DISABLED, bg='darkgrey')
def enableButtons(level):
    leftSpinboxes[level-1].config(state=NORMAL)
    rightSpinboxes[level-1].config(state=NORMAL)
    restoreButtons[level-1].config(state=NORMAL, bg='green')
    moveButtons[level-1].config(state=NORMAL, bg='green')
    saveButtons[level-1].config(state=NORMAL, bg='green')

#add buttons etc. in a frame
buttonFrame = Frame(root)
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
    levelButtons[level].grid(column=0, row=11-level, sticky=W, padx=5, pady=5,  ipadx=25, ipady=5 )

    if level % 2 == 0:
        calibration=5000*level
    else:
        calibration=5000*(level-1)
    min_ = calibration-500
    max_ = calibration+500
    
    lButton = Spinbox(buttonFrame, width=10, from_=min_, to=max_)
    leftSpinboxes.append(lButton)
    leftSpinboxes[level].grid(column=1, row=11-level, sticky=W, padx=5, pady=5,  ipadx=25, ipady=5 )
    leftSpinboxes[level].delete(0,END)
    leftSpinboxes[level].insert(0,str(calibration))
    leftSpinboxes[level].config(state=DISABLED)

    lButton = Spinbox(buttonFrame, width=10, from_=min_, to=max_)
    rightSpinboxes.append(lButton)
    rightSpinboxes[level].grid(column=2, row=11-level, sticky=W, padx=5, pady=5,  ipadx=25, ipady=5 )
    rightSpinboxes[level].delete(0,END)
    rightSpinboxes[level].insert(0,str(calibration))
    rightSpinboxes[level].config(state=DISABLED)

    lButton = Button(buttonFrame, text ="Restore", command = partial (restoreCalibration, level+1), bg='darkgrey', fg='white', state=DISABLED)
    restoreButtons.append(lButton)
    restoreButtons[level].grid(column=3, row=11-level, sticky=W, padx=5, pady=5,  ipadx=25, ipady=5 )

    lButton = Button(buttonFrame, text ="Move", command = partial (moveToCalibration, level+1), bg='darkgrey', fg='white', state=DISABLED)
    moveButtons.append(lButton)
    moveButtons[level].grid(column=4, row=11-level, sticky=W, padx=5, pady=5,  ipadx=25, ipady=5 )

    lButton = Button(buttonFrame, text ="Save", command = partial (saveCalibration, level+1), bg='darkgrey', fg='white', state=DISABLED)
    saveButtons.append(lButton)
    saveButtons[level].grid(column=5, row=11-level, sticky=W, padx=5, pady=5,  ipadx=25, ipady=5 )

# make a text box to image the physical display
display = Text ( root, bg="darkblue", fg="white", font =("Courier", 48), width=14, height=5, takefocus=0)
display.tag_configure("tag_name", justify='center')
display.grid(column=1, row=0, sticky=E)

# function to update the Display
def displayInfo(line1, line2, line3, fontSize, height, width):
    display.config(font =("Courier", fontSize), height=height, width=width)
    display.delete('1.0', END)
    display.insert(END,line1)
    display.insert(END,line2)
    display.insert(END,line3)
    display.tag_add("tag_name", "1.0", "end")

# widget for logging what is going on
logFrame = Frame(root)
logFrame.grid(column=0, row=1, columnspan=2, sticky=S )
# make the scrollbar
scrollbar = Scrollbar(logFrame)
scrollbar.pack(side=RIGHT, fill=Y)
# make the text box
log = Text ( logFrame, width=150, height=10, takefocus=0)
log.pack( fill = BOTH, expand = True )
# attach text box to scrollbar
log.config(yscrollcommand=scrollbar.set)
scrollbar.config(command=log.yview)

#handle messages received from the elevator
def handleInput(event):
    global CurrentLevel
    global ReadingCalibration
    logDebug("Rx: "+event)
    eList = event.split()
    if len(eList) == 0:
        return
    if eList[0] == "CALIBRATION":
        level = int(eList[1])
        left  = int(eList[2])
        right = int(eList[3])
        if level == CurrentLevel and ReadingCalibration:
            #logDebug("level == CurrentLevel")
            ReadingCalibration = False
            enableButtons(level)
            leftSpinboxes[level-1].delete(0,END)
            leftSpinboxes[level-1].insert(0,str(left))
            rightSpinboxes[level-1].delete(0,END)
            rightSpinboxes[level-1].insert(0,str(right))
    if eList[0] == "STATUS":
        status = eList[1]
        if status == "BLOCKED":
            displayInfo("\n\n","Blocked\n",str(CurrentLevel), 48,5,14)
        if status == "HOMING":
            disableButtons(CurrentLevel)
            homingPhase = eList[2]
            if homingPhase == "1":
                displayInfo("\n\n","Homing\n","1", 48,5,14)
            if homingPhase == "2":
                displayInfo("\n\n","Homing\n","2", 48,5,14)
            if homingPhase == "3":
                displayInfo("\n\n","Homing\n","3", 48,5,14)
            if homingPhase == "4":
                displayInfo("\n\n","Homing\n","4", 48,5,14)
        if status == "MOVING":
            disableButtons(CurrentLevel)
            from_ = eList[2]
            to_   = eList[3]
            if from_ < to_:
                displayInfo("\n"+to_+"\n","\u21C8\n",from_, 48,5,14)
            else:
                displayInfo("\n"+from_+"\n","\u21CA\n",to_, 48,5,14)
        if status == "IDLE":
            newLevel = int(eList[2])
            logDebug("newLevel: "+str(newLevel)+" CurrentLevel: "+str(CurrentLevel))
            if newLevel != CurrentLevel:
                logDebug("newLevel != CurrentLevel")
                CurrentLevel = newLevel
                sendCmd("get calibration "+str(CurrentLevel))
                ReadingCalibration = True
            displayInfo("",    str(newLevel),     "",225,1, 3)
            disableButtons(CurrentLevel)
            enableButtons(newLevel)

# Set the serial communication up
ser = Serial()
ser.baudrate = 19200
ser.timeout=0
ser.writeTimeout=0
serBuffer = ""

#read whatever is availbale on the serial port
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

#FOR DEBUGGING: An input field and a button to fake messages that should come from the elevator
# def rx():
    # handleInput(eventInput.get())
# eventInput = Entry(root)
# eventInput.grid(column=1, row=1, sticky=E)
# rxButton = Button(root, text ="Rx above", command = rx)
# rxButton.grid(column=1, row=2, sticky=E)

#send a command to the elevator
def sendCmd(cmd):
    if ser.is_open:
        logDebug("Tx: "+cmd)
        cmd+="\n"
        ser.write(cmd.encode("utf-8"))
    else:
        logDebug("Tx: "+cmd+" FAILED")

#SHOULD NOT BE NECESSARY: Continously read status from the elevator
# def readStatus():
    # sendCmd("get status")
    # root.after(60000, readStatus)
#root.after(10000, readStatus)
    
#enter TKs main loop to handle UI events
root.mainloop()
