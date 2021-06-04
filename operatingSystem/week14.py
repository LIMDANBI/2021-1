import sys

address = int(sys.argv[1])
page = address >> 12
offset = address & 0xfff

print ("The address is", address)
print ("The page# is", page)
print ("The offset is", offset)