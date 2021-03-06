import turtle
import math
import paho.mqtt.client as mqtt
from password import username, password
import threading
import time
import math

wn = turtle.Screen()  # setup screen and its attributes
turt = turtle.Turtle()  # sets turt
wn.title("Coordinate axis")
turt.pd()
turt.speed(10)

speed_mult = 0.2 # konstanter för programmet
ang_mult = 1
updatefreq = 0.35
hjul_bas = 9.5

pos = { # position objeckt som används för att styra bilen
    "ang": 0,
    "armed": False,
    "speed": 0,
    "dir": 0,
}


def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))

    # Subscribing in on_connect() means that if we lose the connection and
    # reconnect then subscriptions will be renewed.
    client.subscribe(username + "/motor")
    client.subscribe(username + "/servo")


def on_message(client, userdata, msg):
    global pos

    if msg.topic == username + "/motor": # läser in meddelandet
        res = eval(msg.payload)
        # print(res)
        pos["armed"] = res[0]
        pos["dir"] = res[1]
        pos["speed"] = round(res[2] * speed_mult)

    elif msg.topic == username + "/servo":
        ang = map_range(int(msg.payload) - 90, 90, -90, -19, 19)
        # print("ang:", ang)
        pos["ang"] = round(ang * ang_mult)


def map_range(value, low1, high1, low2, high2): # map range som i arduino
    return low2 + ((high2 - low2) * (value - low1)) / (high1 - low1)


def loop(): # rita ut allt
    print("loop", pos)
    # print(pos["speed"])
    if pos["armed"]: # om den är på

        if pos["ang"] != 0: # radie om den svänger
            Radie = (hjul_bas/math.sin(pos["ang"])) *- 1
            if pos["dir"]:
                turt.circle(Radie, pos["speed"])
            else:
                turt.circle(Radie, pos["speed"]*-1)
        else:
            if pos["dir"]:
                turt.fd(pos["speed"]) # fram / bak
            else:
                turt.fd(pos["speed"]*-1)

    # turtle.done()


def main():
    client = mqtt.Client()
    client.on_connect = on_connect
    client.on_message = on_message # mqtt saker
    client.username_pw_set(username, password)
    client.connect("maqiatto.com", 1883, 60)
    client.loop_start()
    while 1:
        time.sleep(updatefreq)
        loop()

main()
