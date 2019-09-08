import os
import os.path
rootdir = "./48/part/"
file_object = open('./48/part_48.txt','w')
for parent,dirnames,filenames in os.walk(rootdir):
 for filename in filenames:
    file_object.write("48/part/"+filename.split(".")[0]+ ' 0\n')
file_object.close()