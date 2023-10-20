import pyserial
import Queue

# Interface from JMRI scripting (Jython) code to the train elevator

# RunTrain threads and the Elevator communicates via these threadsafe queues.
# see https://www.jython.org/jython-old-sites/docs/library/queue.html
# 1. RunTrain initiates by a message in ElevatorMoveCommands of the format "MOVE <level>"
# 2. Only when the elevator is idle, it will read and accept this message, and then start the 
#    elevator moving and locking it.
# 3. When the elevator is at the specified level, a message of the format "LEVEL <level> is
#    left in ElevatorMessages
# 4. RunTrain must check the level in the message. If it is not the expected level, 
#    it must put the message back in the queue for the correct RunTrain thread to read.
#    If it is the correct level, the RunTrain thread should move the train out of / into
#    the elevator and then issue an "UNLOCK" command in ElevatorUnlockCommands.
# 5. The Elevator thread does not reply to the UNLOCK command
ElevatorMessages       = Queue.Queue()
ElevatorMoveCommands   = Queue.Queue()
ElevatorUnlockCommands = Queue.Queue()

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
        self.ElevatorState = "UNLOCKSENT"

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
        global ElevatorMessages
        eList = event.split()
        if eList[0] == "LOCKED":
            if eList[1] == "NO":
                if self.ElevatorState == "UNLOCKSENT":
                    self.ElevatorState = "IDLE"
        if eList[0] == "STATUS":
            status = eList[1]
            if status == "IDLE":
                newLevel = int(eList[2])
                if newLevel == self.NextLevel and self.ElevatorState == "MOVING":
                    ElevatorMessages.put("LEVEL "+str(level))
                    self.ElevatorState = "WAITFORUNLOCK"

    #PRIVATE
    #read whatever is availbale on the serial port
    def readSerial():
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
        global ElevatorMoveCommands
        global ElevatorUnlockCommands
        # handle() is called repeatedly until it returns false.
        self.ReadSerial()
        if self.ElevatorState == "UNLOCKSENT":
            if self.unlockWait-- <= 0:
                self.unlockWait = 1000
                self.isElevatorLocked()
        if self.ElevatorState == "MOVING":
            if self.unlockWait-- <= 0:
                self.unlockWait = 1000
                self.GetElevatorStatus()
        if self.ElevatorState == "IDLE":
            if not ElevatorMoveCommands.empty():
                cmd = ElevatorMoveCommands.get().split()
                if cmd[0] == "MOVE":
                    self.MoveElevator(int(cmd[1]))
                else:
                    print "Elevator bad MOVE received"
        if self.ElevatorState == "WAITFORUNLOCK":
            if not ElevatorUnlockCommands.empty():
                cmd = ElevatorUnlockCommands.get()
                if cmd == "UNLOCK":
                    self.UnlockElevator()
                else:
                    print "Elevator bad UNLOCK received"
        return True
