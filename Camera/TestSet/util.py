#parses a line into respective tokens
def parse(line):
  tokens = line.split(',')
  if len(tokens) < 3:
    print("Bad line: " + line)
    return None
  return (tokens[0],int(tokens[1]),int(tokens[2]))
  
def read_results(file):
  data = {}
  with open(file, 'r') as outfile:
    #read in existing data
    data = [parse(line) for line in outfile if line.strip()]
    data = [x for x in data if x is not None] #filter out invalid data
    data = {x[0]: x[1:] for x in data} #convert to dictionary
  return data
  
def write_results(file, data):
  with open(file,'w') as file:
    for key,value in data.items():
      file.write("{0},{1},{2}\n".format(key,value[0],value[1]))