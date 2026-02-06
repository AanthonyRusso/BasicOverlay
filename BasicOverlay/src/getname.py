import asyncio, os
from winrt.windows.media.control import GlobalSystemMediaTransportControlsSessionManager
from winrt.windows.storage.streams import DataReader

async def main():
    base = os.path.dirname(os.path.abspath(__file__))
    assets = os.path.join(base, "..", "ChangingAssets")
    os.makedirs(assets, exist_ok=True)

    m = await GlobalSystemMediaTransportControlsSessionManager.request_async()
    s = m.get_current_session()
    p = await s.try_get_media_properties_async()

    open(os.path.join(assets, "now_playing.txt"), "w", encoding="utf-8").write(f"{p.title}\n{p.artist}")

    stream = await p.thumbnail.open_read_async()
    r = DataReader(stream)
    await r.load_async(stream.size)
    open(os.path.join(assets, "cover.jpg"), "wb").write(bytes(r.read_buffer(stream.size)))

asyncio.run(main())
