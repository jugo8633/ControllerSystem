[Mongo DB]
# Run mongodb
#mongod -auth
mongod

# Set Authentication
#use admin
#db.addUser("admin","admin")

# Run mongodb command shell
mongo

# Run mongodb access shell
use center
#db.auth("admin","admin")
db.access.find()				// query record
db.access.remove({})			// remove all record


[sqlite]
sqlite xxxx.db


[Socket]
# Check socket status
netstat -npt
ss -t -a

[Process]
# Check ELF File
readelf -a xxxx

# Check Linking Library
ldd xxx

# 用 strace 執行程式, 觀察開啟的檔案: strace -f -e open PROGRAM 2>&1 | grep "\.so"

# 在程式執行中觀察 /proc/PID/maps, 這個檔案記錄 process 用到的各區段記憶體為何, 可從對應到的檔案看出有載入的 shared library

#  monitor memory usage
while true; do date; ps aux | grep controlCenter ; sleep 1; done;

# List Socket Connect
lsof | grep controlCenter

# List File Description
/proc/PID/fd

# Use apt-get to install mongodb
apt-get install mongodb-dev libmongo-client-dev
