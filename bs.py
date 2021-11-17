import os;
import json;
import sys;

with open("bs.json", "r") as read_file:
    data = json.load(read_file)

bs_mod = False

if(sys.argv[1] == "+i"):
	data["incl"] += [sys.argv[2]]
	bs_mod = True
elif(sys.argv[1] == "+l"):
	data["link"] += [sys.argv[2]]
	bs_mod = True
elif(sys.argv[1] == "+s"):
	data["src"] += [sys.argv[2]]
	bs_mod = True
elif(sys.argv[1] == "b"):
	bgn = "vsdevcmd -arch=x64 & cl"
	end = " -o ." + data["out"] + " /std:" + data["std"] + " /EHsc /DebugExe"

	inc = ""
	for include in data["incl"]:
		if(include[0] == "." and include[1] == "/"):
			inc += " -I ." + include
		else:
			inc += " -I " + include
	lnk = ""
	for link in data["link"]:
		if(link[0] == "." and link[1] == "/"):
			lnk += " ." + link
		else:
			lnk += " " + link
	src = ""
	for source in data["src"]:
		if(source[0] == "." and source[1] == "/"):
			src += " ." + source
		else:
			src += " " + source
	
	command = bgn + inc + lnk + src + end
	print(command)
	os.chdir('./obj/')
	os.system(command)
elif(sys.argv[1] == "r"):
	os.chdir(data["out"][:data["out"].rfind("/")+1])
	os.system(data["out"][data["out"].rfind("/")+1:])
elif(sys.argv[1] == "c"):
	if os.path.exists(data["out"]):
		os.remove(data["out"])
		print("Removed: \"" + data["out"] + "\".")
	else:
		print("Executable deleted already. :D")
	
	os.chdir("./obj/")
	for filename in os.listdir(os.getcwd()):
		if(filename[filename.rfind("."):] == ".obj"):
			os.remove(filename)
			print("Removed: \"./obj/" + filename + "\".")

if(bs_mod):
	print(data)
	with open('bs.json', 'w') as write_file:
		json.dump(data, write_file)

# print(data)
# os.system("pause")
