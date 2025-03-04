#!/usr/bin/env python3

import time
import os
import logging
import pprint
import json
import serial
import copy

import numpy as np
import tornado
import tornado.web
import tornado.websocket
import tornado.httpserver

import threading


# TODO: Search through serial ports, open up the first one.
try:
    ser = serial.Serial('/dev/ttyACM0', 115200)
    print("Opened port!")
except Exception:
    ser = None

ALIVE = False
alive_time = time.time()

data_lock = threading.Lock()
data = {}
data['axes'] = None
data['buttons'] = None
data['trigger'] = 0

HOLD_TIME = 0.5 # seconds
MIN_SHOT_DELAY = 2 # seconds
shooting = 0
last_shot_time = time.time() - MIN_SHOT_DELAY


def map_value(x, from_lo, from_hi, to_lo, to_hi):
    from_range = from_hi - from_lo
    to_range = to_hi - to_lo
    return (((x - from_lo) / from_range) * to_range) + to_lo


def ser_writer():

    # JOYSTICK MAPPING:
    #   Left / Right: Axis 0
    #   BUTTON 5: Shoot

    # Convert the axis values into an actual motor mapping
    # @[Barrel] [Trigger]
    MOVE_X = np.array([-1, 1, 0], dtype=float)
    
    while True:
        start_time = time.time()
        with data_lock:
            local_data = copy.deepcopy(data)

            # If we're shooting but we've passed the hold time, reset.
            if (time.time() - last_shot_time) > HOLD_TIME:
                data['trigger'] = 0

        axes = data['axes']
        buttons = data['buttons']
        trigger = data['trigger']
        
        if axes is not None and buttons is not None:
            logging.debug(axes)
            logging.debug(buttons)

            x = axes[0]

            # Determine motor commands and convert to [-1, 1]
            motion = x * MOVE_X
            motion = np.minimum(1, motion)
            motion = np.maximum(-1, motion)

            motion *= 0.25

            motion[2] = trigger

            logging.info("Sending motion: %s", motion)
            
            # Convert to the requested format
            fmt = "@ %0.2f %0.2f %0.2f\n"
            out_string = fmt % tuple(motion)
            logging.info("Sending motion: %s", out_string)

            if ser is not None:
                ser.write(out_string.encode('ascii'))

        end_time = time.time()
        time.sleep(max(0, 0.1 - (end_time - start_time)))


class RootHandler(tornado.web.RequestHandler):
    def get(self):
        self.render("static/html/index.html")


class HeartbeatHandler(tornado.websocket.WebSocketHandler):

    def on_message(self, message):
        logging.debug("Received heartbeat message")
        # Doesn't matter what the message is, we just need a message of some
        # sort to update the alive handler.
        global alive_time
        alive_time = time.time()


class GamepadHandler(tornado.websocket.WebSocketHandler):

    def on_message(self, message):
        state = json.loads(message)
        logging.debug("Received message: %s", message)

        with data_lock:
            global data
            data.update(state)

            # Shoot with button 5.
            buttons = data['buttons']
            trigger_pressed = buttons[5]

            global last_shot_time
            if trigger_pressed:
                # If we're not shooting right now and enough time has elapsed,
                # allow the shot to go through.
                if ((time.time() - last_shot_time) > MIN_SHOT_DELAY):
                    data['trigger'] = 1
                    last_shot_time = time.time()
                else:
                    print("I'm sorry Vasu, I can't do that.")



    def open(self):
        logging.info("Opened websocket connection!")


    @tornado.gen.coroutine
    def on_close(self):
        logging.info("Closed websocket connection :(")


class JoystickServer(tornado.httpserver.HTTPServer):
    
    def __init__(self, *args, **kwargs):
        return


    def __new__(cls, *args, **kwargs):
        return super().__new__(cls, cls.make_app())


    @staticmethod
    def make_app():
        settings = {
            "static_path": os.path.join(os.path.dirname(__file__), "static"),
        }

        return tornado.web.Application([
            (r"/", RootHandler),
            (r"/gamepad", GamepadHandler),
            (r"/heartbeat", HeartbeatHandler),
        ], xheaders=True, debug=True, **settings)


if __name__ == "__main__":
    threading.Thread(target=ser_writer).start()
    logging.getLogger().setLevel(logging.INFO)
    JoystickServer().listen(8000)
    tornado.ioloop.IOLoop.current().start()
