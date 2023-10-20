import jarray
import jmri
import pickle
import java.util
import threading
import os
import fnmatch
oblocks = jmri.InstanceManager.getDefault(jmri.jmrit.logix.OBlockManager)

###### ELEVATOR BEGIN
# "import" elevator handling
with open("JMRI_Interface.py") as f: exec(f.read())
###### ELEVATOR END


###################################################################################################
###################################################################################################
#
# Each locomotive is defined by:
#    - the parameters that are needed to start an SCWarrant
#    - Entertainment in terms of a few lines of code to execute in these cases:
#         - EPE = Enter Platform Entertainment
#         - LPE = Leave Platform Entertainment
#         - SLE = Start Locomotive Entertainment
#         - ELE = End Locomotive Entertainment
#    - A set of alternative warrants for the locomotive out of which one is chosen at random. 
#      However, only if the destination block of the warrant is free.
#
# The following code imports all available locomotive definitions by executing any file matching
# *_lokdef.py in any subdirectory. Each of those file should define a locomotive and add that
# definition to the global variable (Dictionary) TrainsAndTrips.
#
###################################################################################################
###################################################################################################

#First a few methods to traverse the file system
def yield_files_with_extensions(folder_path, file_match):
    for root, dirs, files in os.walk(folder_path):
        for file in files:
            if fnmatch.fnmatch(file.lower(),file_match.lower()):
                yield file
        break  # without this line it traverses the subfolders too
def yield_files_in_subfolders(folder_path, file_match):
    yield [folder_path, [f for f in yield_files_with_extensions(folder_path, file_match)]]
    for root, dirs, files in os.walk(folder_path):
        for d in dirs:
            subfolder = os.path.join(root, d)
            my_files = [subfolder, [f for f in yield_files_with_extensions(subfolder, file_match)]]
            yield my_files
#Start in current working directory and look through all subdirectories
the_path = r'.'
def_files = [f for f in yield_files_in_subfolders(the_path, "*_lokdef.py")]
def_files1 = []
#For each subdirectory
for d in def_files:
    dir = str(d[0])
    for f in d[1]:
        #Add each filename matching the pattern including its relative directory to a list
        def_files1.append(dir+'/'+str(f))
#Traverse the list of files and execute each of them to define all trains / locomotives
TrainsAndTrips = {}
for t in def_files1:
  with open(t) as f: exec(f.read())


# Initialize the random number generator. We will use it to find random warrants for trains
Randomizer = java.util.Random()


###################################################################################################
###################################################################################################
#
# CLASS IMPLEMENTING A THREAD PER LOCOMOTIVE TO RUN THAT LOCOMOTIVE
#
###################################################################################################
###################################################################################################
class RunTrain(jmri.jmrit.automat.AbstractAutomaton) :

    global TrainsAndTrips
    global Randomizer
    
    ###### ELEVATOR BEGIN
    def MoveElevator():
        #Move the elevator to our level
        global ElevatorMessages
        global ElevatorMoveCommands
        ElevatorMoveCommands.put("MOVE "+self.Elevator)
        while True:
            #Wait for the elevator to reach our level
            myReply = ElevatorMessages.get()
            rList = myReply.split()
            if rList[0] == 'LEVEL' and rList[1] = self.Elevator:
                # Elevator at our level
                self.ElevatorLocked = True
                break
            else:
                # This message was not for this train. Put it back
                ElevatorMessages.put(myReply)
        
    def UnlockElevator():
        #We have moved our train. Give the elevator to the next train waiting for it
        global ElevatorUnlockCommands
        if self.ElevatorLocked:
            ElevatorUnlockCommands.put('UNLOCK')
            self.ElevatorLocked = False
    ###### ELEVATOR END

    def powerOffOn(self):
        print "powerOffOn ", self.name
        # Flip the power off and on again - but not if it is already off by purpose
        if (powermanager.getPower() == jmri.PowerManager.ON):
            powermanager.setPower(jmri.PowerManager.OFF)
            self.waitMsec(3000)
            powermanager.setPower(jmri.PowerManager.ON)
    
    def init(self):
        # init() is called exactly once at the beginning to do
        # any necessary configuration.
        print "Inside init()", self.name
        self.setLocomotive(TrainsAndTrips[self.name]['Locomotive'])
        self.runSensor = sensors.provideSensor(TrainsAndTrips[self.name]['Sensor'])
        print 'sensor', TrainsAndTrips[self.name]['Sensor']
        self.runSensor.setKnownState(INACTIVE)
        self.StoredStatusVariable = memories.provideMemory(TrainsAndTrips[self.name]['StoredStatus'])
        self.DisplayStatusVariable = memories.provideMemory(TrainsAndTrips[self.name]['DisplayStatus'])
        self.PlaceVariable = memories.provideMemory(TrainsAndTrips[self.name]['Place'])
        self.restoreStatus()
        print "Init ",self.name
        self.MODE_RUN = 2
        self.oldRunSensorState = False
        self.setPlace(self.status['Block'])
        self.setDisplayStatus(self.status['Direction'])

        ###### ELEVATOR BEGIN
        self.ElevatorBlocks = ['OB01', 'OB02'] #Or whatever blocks your elevator might have
        self.ElevatorLocked = False
        self.Elevator = TrainsAndTrips[self.name]['Elevator']
        ###### ELEVATOR END

        return
        
    def statusFileName(self):
        return self.name+".status"

    def chosenWarrantFileName(self):
        return self.name+".usedwarrants"

    def restoreStatus(self):
        try:
            f = open(self.statusFileName(),"r")
            self.status = pickle.load(f)
            f.close()
        except:
            self.status = { 'Block': TrainsAndTrips[self.name]['DefaultStartBlock'], 'Direction': TrainsAndTrips[self.name]['DefaultDirection'] }
        try:
            f = open(self.chosenWarrantFileName(),"r")
            self.previousWarrantFromBlock = pickle.load(f)
            f.close()
        except:
            self.previousWarrantFromBlock = { 'noBlock': 'NoWarrant' }
        return
        
    def storeStatus(self):
        print "storeStatus 1 ",self.status
        f = open(self.statusFileName(),"w")
        print "storeStatus 2"
        pickle.dump(self.status,f)
        print "storeStatus 3"
        f.close()
        print "storeStatus 4"
        f = open(self.chosenWarrantFileName(),"w")
        print "storeStatus 5"
        pickle.dump(self.previousWarrantFromBlock,f)
        print "storeStatus 6"
        f.close()
        self.StoredStatusVariable.setValue(self.status)
        print "storeStatus 7"
        
        return

    def updateStatus(self,block,direction):
        self.status = { 'Block': block, 'Direction': direction }
        self.storeStatus()
        return

    def setDisplayStatus(self,m):
        self.DisplayStatusVariable.setValue(m)
        return
        
    def setPlace(self,p):
        self.PlaceVariable.setValue(p)
        
    def setLocomotive(self, Loc):
        print "setLocomotive", Loc
        self.DCCAddress = Loc['DCCAddress']
        self.Train = Loc['Train']
        self.SoundDecoder = Loc['SoundDecoder']
        E = Loc['E']
        self.EPE = E['EPE']
        self.LPE = E['LPE']
        self.SLE = E['SLE']
        self.ELE = E['ELE']
        return

    def runTrainOnce(self, Route):
        print "runTrainOnce ",self.name
        Warrant = Route['Warrant']
        print "runTrainOnce 3 ",self.name
        w = warrants.getWarrant(Warrant)

        ###### ELEVATOR BEGIN
        if self.Elevator != 'NONE:
            if w.StartBlock in self.ElevatorBlocks or w.EndBlock in self.ElevatorBlocks:
                self.MoveElevator()
        ###### ELEVATOR END

        print "runTrainOnce 4 ",self.name
        self.RosterEntry = w.getSpeedUtil().getRosterEntry()
        print "runTrainOnce 5 ",self.name
        self.Throttle = self.getThrottle(self.RosterEntry)
        print "runTrainOnce 6 ",self.name
        self.setPlace(self.status['Block'])
        print "runTrainOnce 10 ",self.name
        self.setDisplayStatus('Ready')
        exec self.LPE
        while (not w.isRouteFree()):
            print "ROUTE STOLEN ", self.name
            self.waitMsec(100)
        self.setDisplayStatus('Running')
        w.runWarrant(self.MODE_RUN)
        self.waitWarrantRunState(w, self.MODE_RUN)
        print "running ",self.name
        block = "Start block"
        # Some Märklin locomotives locks after a certain period and needs to be reset by power off/on
        timer = threading.Timer(40.0, self.powerOffOn)
        timer.start()
        while (block != None):
            self.setPlace(block)
            block = self.waitWarrantBlockChange(w)
            print "ude af waitWarrantBlockChange ",block
            timer.cancel()
            if (self.Entertainment.keys().count(block) > 0):
                exec self.Entertainment[block]
        print "ude af while loop"
        self.updateStatus(Route['EndBlock'],Route['NextDirection'])
        self.setPlace(self.status['Block'])
        self.setDisplayStatus("Entering")
        # Re-acquire the throttle. The Warrant has probably taken it away from us.
        self.Throttle = self.getThrottle(self.RosterEntry)
        exec self.EPE

        ###### ELEVATOR BEGIN
        self.UnlockElevator()
        ###### ELEVATOR END

        return

    def handle(self):
        # handle() is called repeatedly until it returns false.
        print "handle ", self.name, "waiting for sensor to run train"
        self.setDisplayStatus('Push Run')
        self.setPlace(self.status['Block'])
        if (self.DCCAddress > 127):
            self.Throttle = self.getThrottle(self.DCCAddress, True)
        else:
            self.Throttle = self.getThrottle(self.DCCAddress, False) 
        if (self.runSensor.getKnownState() == INACTIVE):
            if (self.oldRunSensorState == True):
                # The train has been running, but shall now stop - shut off sounds etc.
                exec self.ELE
            self.oldRunSensorState = False
            self.waitSensorActive(self.runSensor)
        if (self.oldRunSensorState == False):
            # The train is about to start running - turn on sounds etc.
            exec self.SLE
        self.oldRunSensorState = True
        print self.name, 'sensor active'
        #Wait and then pick a random warrant
        WarrantCandidates = TrainsAndTrips[self.name]['Warrants']
        print 'WarrantCandidates',WarrantCandidates
        print 'len',len(WarrantCandidates)
        print 'WarrantCandidates 0',WarrantCandidates[0]
        print 'random',Randomizer.nextInt(7)
        self.setDisplayStatus('Waiting')
        print "wait at platform ",self.name
        TimeToWait = 1
        for Warrant in WarrantCandidates:
            if (Warrant['StartBlock'] == self.status['Block']):
                TimeToWait = Warrant['TimeToWait']
                break
        self.waitMsec(TimeToWait)
        FilteredWarrantCandidates = []
        for Warrant in WarrantCandidates:
            WarrantName = Warrant['Warrant']
            WarrantMayRepeat = Warrant['MayRepeat']
            StartBlockName = Warrant['StartBlock']
            WarrantDirection = Warrant['Direction']
            if (self.status['Block'] in self.previousWarrantFromBlock.keys()):
                prevWFromBlock = self.previousWarrantFromBlock[self.status['Block']]['Warrant']
            else:
                prevWFromBlock = 'None'
            print "StartBlock: ",StartBlockName
            if (StartBlockName == self.status['Block'] and
                (WarrantDirection == self.status['Direction'] or self.status['Direction'] == 'DontCare') and
                (WarrantName != prevWFromBlock or WarrantMayRepeat == 'Yes')):
                EndBlockName   = Warrant['EndBlock']
                print "Warrant: ", Warrant['Warrant'], " StartBlock: ",StartBlockName, " EndBlock: ", EndBlockName
                StartBlock = oblocks.getBySystemName(StartBlockName)
                EndBlock   = oblocks.getBySystemName(EndBlockName)
                w = warrants.getWarrant(WarrantName)
                if (w.isRouteFree()):
                    FilteredWarrantCandidates.append(Warrant)
        if (len(FilteredWarrantCandidates) == 0 and self.status['Block'] in self.previousWarrantFromBlock.keys()):
            prevWarrant = self.previousWarrantFromBlock[self.status['Block']]
            prevWarrantName = prevWarrant['Warrant']
            w = warrants.getWarrant(prevWarrantName)
            prevWarrantDirection = prevWarrant['Direction']
            if (w.isRouteFree() and (prevWarrantDirection == self.status['Direction'] or self.status['Direction'] == 'DontCare')):
                FilteredWarrantCandidates.append(prevWarrant)
        if (len(FilteredWarrantCandidates) == 0):
            self.waitMsec(3000)
            return 1
        ChosenIndex = Randomizer.nextInt(len(FilteredWarrantCandidates))
        print 'ChosenIndex',ChosenIndex
        Warrant = FilteredWarrantCandidates[ChosenIndex]
        self.previousWarrantFromBlock[self.status['Block']] = Warrant
        print "CHOSEN Warrant: ",Warrant
        self.runTrainOnce(Warrant)
        return 1


###################################################################################################
###################################################################################################
#
# START A THREAD PER LOCOMOTIVE TO RUN THAT LOCOMOTIVE
#
###################################################################################################
###################################################################################################
for Train in TrainsAndTrips.iterkeys():
    print 'start tog',Train
    NextTrain = RunTrain(Train)
    NextTrain.start()
