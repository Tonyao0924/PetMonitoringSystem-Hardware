import asyncio
import websockets


async def connect():
    websocket = await websockets.connect("ws://127.0.0.1:8000/mqtt/pure/")
    await websocket.send("Hello, world!")
    message = await websocket.recv()
    print(message)


asyncio.run(connect())