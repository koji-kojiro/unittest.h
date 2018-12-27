import os

if not os.path.exists("./bin"):
    os.mkdir("./bin")

with open("./src/utest.bash.tpl", "r") as f:
    src = f.read()

with open("./unittest.h", "r") as f:
    header_src = f.read().replace("#pragma once", "")

with open("./bin/utest", "w") as f:
    f.write(src.replace("##src##", header_src))
