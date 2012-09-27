import time, sys
from struct import *
from serial import *
from xbee import XBee
from datetime import *
from lib.payload import Payload
from lib.command_interface import CommandInterface

def txCallback(dest, packet):
	global xb
	xb.tx(dest_addr = dest, data = packet)
	
def processPacket(packet):
	global rec
	print "Packet received!"
	rec = True
	
if __name__ == '__main__':

	#DEFAULT_COM_PORT = 'COM21'
	DEFAULT_COM_PORT = '/dev/ttyUSB0'
	DEFAULT_BAUD_RATE = 57600
	DEFAULT_ADDRESS = 0x1020
	addr = '\x10\x21'
	DEFAULT_PAN = 0x1005
	DEFAULT_ITERS = 10
	DEFAULT_CHAN = 0x12
	
	if len(sys.argv) == 1:
		com = DEFAULT_COM_PORT
		baud = DEFAULT_BAUD_RATE
		#addr = DEFAULT_ADDRESS
		iters = DEFAULT_ITERS
		
	elif len(sys.argv) == 5:
		com = sys.argv[1]
		baud = int(sys.argv[2])
		addr = pack('>H', int(sys.argv[3], 16))
		iters = int(sys.argv[4])

	coord = CommandInterface(addr, txCallback)
	coord.enableDebug()
	
	# Enable XBee
	ser = Serial(port = com, baudrate = baud)
	xb = XBee(ser, callback = processPacket)
	print "Setting PAN ID to " + hex(DEFAULT_PAN)
	xb.at(command = 'ID', parameter = pack('>H', DEFAULT_PAN))
	xb.at(command = 'CH', parameter = pack('>H', DEFAULT_CHAN))
	xb.at(command = 'MY', parameter = pack('>H', DEFAULT_ADDRESS))
	
	# Create file
	today = datetime.today()
	d = str(today.year) + "_" + str(today.month) + "_" + str(today.day)
	t = str(today.hour) + "_" + str(today.minute) + "_" + str(today.second)
	fname = 'Latency_Test-' + d + '-' + t + '.txt'
	record_log = open(fname, 'w')
	
	raw_input("Press any key to begin test...")
	
	capture_times = []
	roundtrip_times = []
	
	# Get iters number of samples
	for i in range(0, iters):
		start_time = datetime.now()
		end_time = datetime.now()
		round_time = end_time - start_time
		dt = round_time.microseconds/1000.0
		capture_times.insert(i, dt)
	
	for i in range(0, iters):
		try:
			print "Beginning iteration " + str(i) + "..."
			rec = False
			start_time = datetime.now()
			coord.sendEcho()
			#xb.wait_read_frame()
			while rec == False:
				pass
			end_time = datetime.now()
			round_time = end_time - start_time
			dt = round_time.microseconds/1000.0
			print "Response received. Total time: " + str(dt) + " milliseconds."
			roundtrip_times.insert(i, dt)
		except KeyboardInterrupt:
			roundtrip_times.insert(i, 0)
			continue
			
	record_log.write("Capture Time\tRoundtrip Time\n")
	total_time = 0.0
	for i in range(0, iters):
		cap_time = capture_times[i]
		round_time = roundtrip_times[i]
		total_time = total_time + round_time
		record_log.write(str(cap_time) + "\t" + str(round_time) + "\n")
	
	print "Average Time: " + str(total_time/iters) + " milliseconds."
	
	xb.halt()
	record_log.close()
	coord.close()
	ser.close()
