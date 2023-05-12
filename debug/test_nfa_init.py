import asyncio
import threading
from websockets.server import serve
from contextlib import redirect_stdout, redirect_stderr
import subprocess

clients = set()

# 保存事件循环的引用
loop = None

async def notify_clients(message):
    if clients:
        await asyncio.wait([client.send(message) for client in clients])


async def register(websocket):
    clients.add(websocket)


async def unregister(websocket):
    clients.remove(websocket)


async def websocket_handler(websocket, path):
    await register(websocket)
    try:
        async for message in websocket:
            await notify_clients(message)
    finally:
        await unregister(websocket)


async def start_server():
    server = await serve(websocket_handler, "localhost", 8001)
    await server.wait_closed()


def run_server():
    with open("/tmp/gdb.log", "w") as f, redirect_stdout(f), redirect_stderr(f):
        global loop
        loop = asyncio.new_event_loop()
        asyncio.set_event_loop(loop)
        loop.run_until_complete(start_server())
        loop.close()


thread = threading.Thread(target=run_server)
thread.daemon = True
thread.start()

subprocess.run(
    [
        "xdg-open",
        "/home/zoran/project/Cpp/project/Principle-of-compilation-from-scratch/cpp_lexer_generator/debug/visualizer/index.html",
    ],
    stdout=subprocess.DEVNULL,
    stderr=subprocess.DEVNULL,
)
# NOTE :
#  ╭──────────────────────────────────────────────────────────╮
#  │                       Gdb handler                        │
#  ╰──────────────────────────────────────────────────────────╯

import gdb
import json


class Visual(gdb.Command):
    """Send a message to all WebSocket clients."""

    def __init__(self):
        super(Visual, self).__init__("visual", gdb.COMMAND_USER)

    def invoke(self, arg, from_tty):
        assert loop, "Event loop is not initialized"
        fsa = gdb.parse_and_eval(arg)

        result = ""
        if fsa.type.code == gdb.TYPE_CODE_PTR:
            # fsa = fsa.dereference()
            result = str(gdb.parse_and_eval(f"{arg}->_toDotString()"))

        else:
            result = str(gdb.parse_and_eval(f"{arg}._toDotString()"))

        # result = fsa["_toDotString"]()
        result = result[1:-1].replace(r"\n", "\n").replace(r"\"", '"')

        # json.dumps(arg)
        # 使用asyncio.run_coroutine_threadsafe将任务提交给事件循环
        asyncio.run_coroutine_threadsafe(notify_clients(result), loop)

Visual()


def exit_handler(event):
    if loop:
        loop.stop()


gdb.events.exited.connect(exit_handler)
