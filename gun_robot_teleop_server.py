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
data_axes = None
data_buttons = None


def map_value(x, from_lo, from_hi, to_lo, to_hi):
    from_range = from_hi - from_lo
    to_range = to_hi - to_lo
    return (((x - from_lo) / from_range) * to_range) + to_lo


def ser_writer():

    # JOYSTICK MAPPING:
    #   Left / Right: Axis 0
    #   Forward / Back: Axis 1
    #   Twist: Axis 2

    # Convert the axis values into an actual motor mapping
    # @[Left back] [Left front] [Right back] [Right front]
    MOVE_X = np.array([-1, 1, -1, 1])
    MOVE_Y = np.array([1, 1, -1, -1])
    MOVE_Z = np.array([1, 1, 1, 1])
    
    while True:
        start_time = time.time()
        with data_lock:
            axes = copy.deepcopy(data_axes)
            buttons = copy.deepcopy(data_buttons)
        
        if axes is not None:
            logging.debug(axes)

            x = axes[0]
            y = axes[1]
            z = axes[2]

            # Determine motor commands and convert to [-1, 1]
            motion = x * MOVE_X + y * MOVE_Y + z * MOVE_Z
            motion = np.minimum(1, motion)
            motion = np.maximum(-1, motion)

            logging.info("Sending motion: %s", motion)
            
            # Convert to the requested format
            fmt = "@%0.2f %0.2f %0.2f %0.2f\n"
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
            global data_axes
            data_axes = state['axes']
            
            global data_buttons
            data_buttons = state['buttons']


    def open(self):
        logging.info("Opened websocket connection!")


    @tornado.gen.coroutine
    def on_close(self):
        logging.info("Closed websocket connection :(")
        
        with out_message_lock:
            global message
            out_message = "1500 1500\n"


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
