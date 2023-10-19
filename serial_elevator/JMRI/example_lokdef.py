IC3_Warrants = []
IC3_Warrants.append({ 'Warrant': 'IC3_3-1', 'TimeToWait': 10000, 'Direction': 'FWD', 'StartBlock': 'OB3', 'EndBlock': 'OB1', 'NextDirection': 'FWD', 'MayRepeat': 'Yes' })
IC3_Warrants.append({ 'Warrant': 'IC3_1-3', 'TimeToWait': 10000, 'Direction': 'FWD', 'StartBlock': 'OB1', 'EndBlock': 'OB3', 'NextDirection': 'FWD', 'MayRepeat': 'Yes' })

IC3EPE = """
print "IC3 entering platform"
"""
IC3LPE = """
print "IC3 leaving platform"
"""
IC3SLE = """
print "IC3 starting"
self.Throttle.setF0(True)
"""
IC3ELE = """
print "IC3 ending"
self.Throttle.setF0(False)
"""
IC3OB26 = """
print "IC3 entering main station"
self.Throttle.setF3(True)
self.waitMsec(1000)
self.Throttle.setF3(False)
"""
IC3Entertainment = { 'EPE': IC3EPE, 'LPE': IC3LPE, 'SLE': IC3SLE, 'ELE': IC3ELE, 'OB26': IC3OB26 }
IC3Train = { 'Train': 'IC3', 'DCCAddress': 5003, 'SoundDecoder': 0, 'E': IC3Entertainment }
IC3Definition = { 'Sensor': 'RunIC3', 'Locomotive': IC3Train, 'Warrants': IC3_Warrants, 'DefaultStartBlock': 'OB3', 'DefaultDirection': 'FWD', 'StoredStatus': 'IC3_StoredStatus', 'DisplayStatus': 'IC3_Status', 'Place': 'IC3_Place' }

TrainsAndTrips['IC3'] = IC3Definition
