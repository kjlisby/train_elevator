import pyserial

# Interface from JMRI scripting (Jython) code to the train elevator

ElevatorMessages = []

class ElevatorIF(jmri.jmrit.automat.AbstractAutomaton) :
    
    def init(self):
        # init() is called exactly once at the beginning to do
        # any necessary configuration.
        self.ElevatorState = "IDLE"
        self.ser = Serial()
        self.ser.baudrate = 19200
        self.ser.timeout=0
        self.ser.writeTimeout=0
        self.serBuffer = ""

    #PRIVATE
    def LockElevator(self):
        self.sendCmd("set lock\n")

    #PRIVATE
    def UnlockElevator():
        self.sendCmd("set unlock\n")
        self.ElevatorState = "UNLOCKING"

    #PRIVATE
    def isElevatorLocked():
        self.sendCmd("get locked\n")
        #wait for answer

    #PUBLIC
    def MoveElevator(level):
        if self.ElevatorState != "IDLE":
            return False
        self.sendCmd("set level "+str(level)+"\n")
        self.LockElevator()
        self.ElevatorState = "MOVING"
        self.NextLevel = level
        return True
        #wait for answer

    #PRIVATE
    def GetElevatorStatus(self):
        self.sendCmd("get status\n")
        #wait for answer
        
    #PRIVATE
    def handleMessage(self, event):
        eList = event.split()
        if eList[0] == "LOCK":
            if eList[1] == "NO":
                if self.ElevatorState == "UNLOCKING":
                    self.ElevatorState = "IDLE"
        if eList[0] == "STATUS":
            status = eList[1]
            if status == "IDLE":
                newLevel = int(eList[2])
                if newLevel == self.NextLevel and self.ElevatorState == "MOVING":
                    ElevatorMessages.append("LEVEL "+str(level))
                    self.UnlockElevator()


    #PRIVATE
    #read whatever is availbale on the serial port
    def readSerial():
        global ElevatorMessages
        while True:
            if not self.ser.is_open:
                try:
                    self.ser.port = 'COM3'
                    self.ser.open()
                except:
                    logDebug("Failed to connect to COM3")
            if not ser.is_open:
                try:
                    self.ser.port = 'COM4'
                    self.ser.open()
                except:
                    logDebug("Failed to connect to COM4")
            if not self.ser.is_open:
                break
            c = self.ser.read() # attempt to read a character from Serial
        
            #was anything read?
            if len(c) == 0:
                break

            # check if character is a delimeter
            if ord(c) == 13:
                continue  # don't want returns. chuck it
            
            if ord(c) == 10:
                self.handleMessage(self.serBuffer)
                self.serBuffer = "" # empty the buffer
            else:
                try:
                    self.serBuffer += c.decode("utf-8")  # add to the buffer
                except:
                    continue

    #PRIVATE
    #send a command to the elevator
    def sendCmd(cmd):
        if self.ser.is_open:
            cmd+="\n"
            self.ser.write(cmd.encode("utf-8"))
            return True
        else:
            return False

    unlockWait = 1000
    def handle(self):
        # handle() is called repeatedly until it returns false.
        self.ReadSerial()
        if self.ElevatorState == "UNLOCKING":
            if self.unlockWait-- <= 0:
                self.unlockWait = 1000
                self.UnlockElevator()
        if self.ElevatorState == "MOVING":
            if self.unlockWait-- <= 0:
                self.unlockWait = 1000
                self.GetElevatorStatus()
        return True
