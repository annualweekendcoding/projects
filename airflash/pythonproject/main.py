#!/usr/bin/python3
import logging
import inotify.adapters
import requests
import re
import time
from PIL import Image
from io import BytesIO
from pathlib import Path
from os.path import expanduser

_DEFAULT_LOG_FORMAT = '%(asctime)s - %(name)s - %(levelname)s - %(message)s'
_LOGGER = logging.getLogger(__name__)


def _configure_logging():
    _LOGGER.setLevel(logging.DEBUG)

    ch = logging.StreamHandler()

    formatter = logging.Formatter(_DEFAULT_LOG_FORMAT)
    ch.setFormatter(formatter)

    _LOGGER.addHandler(ch)


def _watch():
    i = inotify.adapters.Inotify()

    i.add_watch(b'/home/horstf/tmp')

    try:
        for event in i.event_gen():
            if event is not None:
                (header, type_names, watch_path, filename) = event
                _LOGGER.info("WD=(%d) MASK=(%d) COOKIE=(%d) LEN=(%d) MASK->NAMES=%s "
                             "WATCH-PATH=[%s] FILENAME=[%s]",
                             header.wd, header.mask, header.cookie, header.len, type_names,
                             watch_path.decode('utf-8'), filename.decode('utf-8'))
    finally:
        i.remove_watch(b'/tmp')


def _mainloop():
    lastwrite = -1
    while 1:
        r = requests.get('http://192.168.0.1/command.cgi', params={'op': '121'})
        if r.status_code == requests.codes.ok:
            currentLastwrite = int(r.text)
            if currentLastwrite != lastwrite:
                lastwrite = currentLastwrite
                r = requests.get('http://192.168.0.1/command.cgi', params={'op': '100', 'DIR': '/DCIM'})
                if r.status_code == requests.codes.ok:
                    lines = r.text.splitlines()
                    for line in lines:
                        if line.startswith('/'):
                            descriptor = line.split(',')
                            if re.match(r'[0-9]{3,3}__', descriptor[1]):
                                directory = descriptor[0] + '/' + descriptor[1]
                                r = requests.get('http://192.168.0.1/command.cgi', params={'op': '100', 'DIR': directory})
                                if r.status_code == requests.codes.ok:
                                    piclines = r.text.splitlines()
                                    for picline in piclines:
                                        if picline.startswith('/'):
                                            picdesc = picline.split(',')
                                            filename = picdesc[1]
                                            if filename.startswith('IMG_') and filename.endswith('.JPG'):
                                                images_dir = Path(expanduser('~')) / 'images'
                                                original_images_dir = images_dir / 'original'
                                                if not original_images_dir.exists():
                                                    original_images_dir.mkdir(parents=True)
                                                print_images_dir = images_dir / 'print'
                                                if not print_images_dir.exists():
                                                    print_images_dir.mkdir(parents=True)
                                                localpath = original_images_dir / filename
                                                if not localpath.exists():
                                                    filepath = picdesc[0] + '/' + filename
                                                    print('Hole Bilddatei ' + filepath)
                                                    r = requests.get('http://192.168.0.1' + filepath)
                                                    if r.status_code == requests.codes.ok:
                                                        print('Speichere Bilddatei ' + filepath)
                                                        with localpath.open("wb") as outfile:
                                                            outfile.write(r.content)
                                                        print('Bilddatei ' + filepath + ' gespeichert')
                                                        image = Image.open(BytesIO(r.content))
                                                        image = image.resize((900,600), resample=Image.BICUBIC)
                                                        print_file_path = print_images_dir / filename
                                                        with print_file_path.open("wb") as imagefile:
                                                            image.save(imagefile, "JPEG", quality=85)
                                                        print('Bilddatei ' + print_file_path.name + ' gespeichert')

                else:
                    print("Request fehlgeschlagen: " + r.status_code)
            else:
                break
        else:
            print('Request fehlgeschlagen: ' + r.status_code)

        time.sleep(1)


def _main():
    _LOGGER.info('Start')

    _mainloop()

    _LOGGER.info('Ende')


if __name__ == '__main__':
    _configure_logging()
    _main()
