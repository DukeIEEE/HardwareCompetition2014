import os
import os.path
import sys
import subprocess

from util import read_results

EXE = 'process.exe'
RESULTS_FILE = 'results.txt'
PATH = '.' #path to search in

#check command line arguments
if len(sys.argv) > 1:
  PATH = sys.argv[1]
if len(sys.argv) > 2:
  EXE = sys.argv[2]
if len(sys.argv) > 3:
  RESULTS_FILE = sys.argv[3]
  
data = read_results(os.path.join(PATH, RESULTS_FILE))
#calculate error
total_error = 0
for key,value in data.items():
  print("Running on " + key + "...")
  s = subprocess.Popen([EXE, os.path.join(PATH,key)], shell=True, stdout=subprocess.PIPE)
  if sys.version_info[0] == 3:
    blah = str(s.stdout.read(),encoding='utf-8')
  else:
    blah = str(s.stdout.read())
  res = blah.split(',')
  x = float(res[0])
  y = float(res[1])
  tx = value[0]
  ty = value[1]
  
  error = (x - tx)*(x - tx) + (y - ty)*(y - ty)
  print("True: ({0},{1})".format(tx, ty))
  print("Detected: ({0},{1})".format(x,y))
  print("Error: {0}\n".format(error))
  
  total_error = error + total_error

total_error = float(total_error)/len(data)
print("Total error (normalized): {0}".format(total_error))
