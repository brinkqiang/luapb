-- lua script

local pb = require("luapb")
local dumpmodule = require("dumpmodule")
pb.import("net.proto")

print(dumpmodule.full_encode(pb))