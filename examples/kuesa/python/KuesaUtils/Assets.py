import os

def assetsUrl():
    wd = os.path.dirname(os.path.abspath(__file__))
    path = os.path.abspath(wd + "/../../assets")
    return "file://" + path