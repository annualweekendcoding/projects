import requests
import asyncio
import aiohttp
import async_timeout

class Airflash:

    def __init__(self, url, images_dir_path):
        super().__init__()
        self.images_dir_path = images_dir_path
        self.url = url
        self.last_write = -1

    @asyncio.coroutine
    def fetch(session, url):
        with async_timeout.timeout(10):
            response = yield from session.get(url)
            try:
                return (yield from response.text())
            except Exception as e:
                response.close()
                raise e
            finally:
                yield from response.release()

    @asyncio.coroutine
    def handle_changed(self, loop, on_changed):
        session = aiohttp.ClientSession(loop=loop)
        try:
            html = yield from self.fetch(session, 'http://192.168.0.1/command.cgi?op=121')
            print(html)
        finally:
            session.close()
        pass

    def changed(self, on_changed):
        loop = asyncio.get_event_loop()
        loop.run_until_complete(self.handle_changed(loop, on_changed))
        response = requests.get(self.url + '/command.cgi', params={'op': '121'})
        if response.status_code == requests.codes.ok:
            return self.last_write != int(response.text)
        else:
            return False

    def synchronize(self):
        pass