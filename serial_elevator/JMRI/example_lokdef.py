# Assume this layout. Block names are B01, B02 etc. BE is the elevator.
#
#   ====B01====|====B02====|====B03====|====BE====|====B05====|====B06====|====B07====
#             /             \                                /             \
#            /               \                              /               \
# ====B08====                 =======B09====|====B10========                 ====B11====
#
# Two trains have their homebase in the elevator (TE07 on level 7 and TE10 on level 10) and the third and last train (TB11) is having its homebase in block B11. Still, no other train is allowed in a trains homebase.
# Note that:
#     - The only elevator-addition is the Elevator tag in the train definition.
#     - If Elevator equals NONE, the train is not permitted in the elevator at all.
#     - If Elevator equals a valid elevator level, the train must have its homebase in the elevator at that level.
#     - No train may use the elevator, except for start and end block.


TB11_Warrants = []
TB11_Warrants.append({ 'Warrant': 'TB11_11-1', 'TimeToWait': 10000, 'Direction': 'FWD', 'StartBlock': 'B11', 'EndBlock': 'B01', 'NextDirection': 'REV', 'MayRepeat': 'Yes' })
TB11_Warrants.append({ 'Warrant': 'TB11_1-11', 'TimeToWait': 10000, 'Direction': 'REV', 'StartBlock': 'B01', 'EndBlock': 'B11', 'NextDirection': 'FWD', 'MayRepeat': 'Yes' })

TB11EPE = """
print "TB11 entering platform"
"""
TB11LPE = """
print "TB11 leaving platform"
"""
TB11SLE = """
print "TB11 starting"
self.Throttle.setF0(True)
"""
TB11ELE = """
print "TB11 ending"
self.Throttle.setF0(False)
"""
TB11_B10 = """
print "TB11 entering main station"
self.Throttle.setF3(True)
self.waitMsec(1000)
self.Throttle.setF3(False)
"""
TB11Entertainment = { 'EPE': TB11EPE, 'LPE': TB11LPE, 'SLE': TB11SLE, 'ELE': TB11ELE, 'B10': TB11_B10 }
TB11Train = { 'Train': 'TB11', 'DCCAddress': 5003, 'SoundDecoder': 0, 'E': TB11Entertainment }
TB11Definition = { 'Sensor': 'RunTB11', 'Locomotive': TB11Train, 'Warrants': TB11_Warrants, 'Elevator': 'NONE', 'DefaultStartBlock': 'B11', 'DefaultDirection': 'FWD', 'StoredStatus': 'TB11_StoredStatus', 'DisplayStatus': 'TB11_Status', 'Place': 'TB11_Place' }
TrainsAndTrips['TB11'] = TB11Definition

# For the next trains, we will reuse the entertainment stuff - one example should do it.

TE07_Warrants = []
TE07_Warrants.append({ 'Warrant': 'TBE07_E-7', 'TimeToWait': 10000, 'Direction': 'FWD', 'StartBlock': 'BE', 'EndBlock': 'B07', 'NextDirection': 'REV', 'MayRepeat': 'Yes' })
TE07_Warrants.append({ 'Warrant': 'TBE07_7-8', 'TimeToWait': 10000, 'Direction': 'REV', 'StartBlock': 'B07', 'EndBlock': 'B08', 'NextDirection': 'FWD', 'MayRepeat': 'Yes' })
TE07_Warrants.append({ 'Warrant': 'TBE07_8-E', 'TimeToWait': 10000, 'Direction': 'FWD', 'StartBlock': 'B08', 'EndBlock': 'BE', 'NextDirection': 'FWD', 'MayRepeat': 'Yes' })
TE07Train = { 'Train': 'TE07', 'DCCAddress': 5004, 'SoundDecoder': 1, 'E': TB11Entertainment }
TE07Definition = { 'Sensor': 'RunTE07', 'Locomotive': TE07Train, 'Warrants': TE07_Warrants, 'Elevator': '7', 'DefaultStartBlock': 'BE', 'DefaultDirection': 'FWD', 'StoredStatus': 'TE07_StoredStatus', 'DisplayStatus': 'TE07_Status', 'Place': 'TE07_Place' }
TrainsAndTrips['TE07'] = TE07Definition


TE12_Warrants = []
TE12_Warrants.append({ 'Warrant': 'TBE12_E-7', 'TimeToWait': 10000, 'Direction': 'FWD', 'StartBlock': 'BE', 'EndBlock': 'B07', 'NextDirection': 'REV', 'MayRepeat': 'Yes' })
TE12_Warrants.append({ 'Warrant': 'TBE12_7-8', 'TimeToWait': 10000, 'Direction': 'REV', 'StartBlock': 'B07', 'EndBlock': 'B08', 'NextDirection': 'FWD', 'MayRepeat': 'Yes' })
TE12_Warrants.append({ 'Warrant': 'TBE12_8-E', 'TimeToWait': 10000, 'Direction': 'FWD', 'StartBlock': 'B08', 'EndBlock': 'BE', 'NextDirection': 'FWD', 'MayRepeat': 'Yes' })
TE12Train = { 'Train': 'TE12', 'DCCAddress': 5005, 'SoundDecoder': 1, 'E': TB11Entertainment }
TE12Definition = { 'Sensor': 'RunTE12', 'Locomotive': TE12Train, 'Warrants': TE12_Warrants, 'Elevator': '12', 'DefaultStartBlock': 'BE', 'DefaultDirection': 'FWD', 'StoredStatus': 'TE12_StoredStatus', 'DisplayStatus': 'TE12_Status', 'Place': 'TE12_Place' }
TrainsAndTrips['TE12'] = TE12Definition
