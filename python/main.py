from KVDB import PyKVDB, PyTLV

db = PyKVDB()
db.create("testdb.db", 64)

key = PyTLV()
key.new(b"key1", len(b"key1"), 1)

val = PyTLV()
val.new(b"value1", len(b"value1"), 2)

db.put(key, val)
print(db.get(0))

db.close()
