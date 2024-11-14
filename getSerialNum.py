import json
with open("/proc/cpuinfo",'r') as serialNum:
    returnVal = serialNum.read().split("\n")
    serialNumberValue = returnVal[-3].split(":")[1]
    print(serialNumberValue.strip()) 
    with open ("boardInfo.json","w") as boardInfoFile:
        data = {
            "serial" : serialNumberValue.strip()
        }
        boardInfoFile.write(json.dumps(data))
    