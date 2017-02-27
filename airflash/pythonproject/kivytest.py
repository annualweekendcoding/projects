#!/usr/bin/python3
import kivy
from kivy.app import App
from kivy.core.window import Window
# noinspection PyUnresolvedReferences
from kivy.properties import ObjectProperty
from kivy.uix.boxlayout import BoxLayout
from pathlib import Path
from os.path import expanduser
from kivy.clock import Clock
# from kivy.uix.button import Button
# from kivy.uix.image import Image

kivy.require('1.8.0')  # replace with your current kivy version !



# from kivy.config import Config
# Config.set('graphics', 'fullscreen', 'auto')


class KivytestScreen(BoxLayout):
    def show_image(self):
        if self.index >= 0:
            image_name = str(self.images[self.index])
            print('Zeige Bild ' + image_name)
            self.ids.image.source = image_name

    def set_button_state(self):
        if self.index < 0:
            self.disable_buttons()
        else:
            self.print_button.disabled = False
            if self.index == 0:
                self.prev_button.disabled = True
            else:
                self.prev_button.disabled = False
            if self.index >= (len(self.images) - 1):
                self.next_button.disabled = True
            else:
                self.next_button.disabled = False

    def disable_buttons(self):
        self.prev_button.disabled = True
        self.print_button.disabled = True
        self.next_button.disabled = True

    def on_print_complete(self, delta):
        self.print_label.opacity = 0.0
        self.set_button_state()

    def update(self):
        self.set_button_state()
        self.show_image()

    # noinspection PyUnusedLocal
    def print_image(self, instance):
        if self.index >= 0:
            image_name = str(self.images[self.index])
            print('Drucke das Bild ' + image_name)
            self.disable_buttons()
            self.print_label.opacity = 1.0
            Clock.schedule_once(self.on_print_complete, 2.0)

    # noinspection PyUnusedLocal
    def prev_image(self, instance):
        print('Zum vorherigen Bild (aktuell: ' + str(self.index) + ')')
        if self.index > 0:
            self.index -= 1
            self.update()
        else:
            self.index = 0

    # noinspection PyUnusedLocal
    def next_image(self, instance):
        print('Zum nächsten Bild (aktuell: ' + str(self.index) + ')')
        if self.index >= 0:
            if self.index < (len(self.images) - 1):
                self.index += 1
                self.update()
            else:
                self.index = len(self.images) - 1

    def __init__(self, **kwargs):
        super(KivytestScreen, self).__init__(**kwargs)
        self.cols = 2

        self.prev_button = self.ids.prev_button
        self.print_button = self.ids.print_button
        self.next_button = self.ids.next_button
        self.image = self.ids.image
        self.print_label = self.ids.print_label

        self.images_dir = Path(expanduser('~')) / 'images' / 'print'
        # print(self.images_dir)
        self.images = list(self.images_dir.glob('*.JPG'))
        # print(self.images)
        if len(self.images) > 0:
            self.index = 0
        else:
            self.index = -1
        # print(self.index)

        self.update()

        self.prev_button.bind(on_press=self.prev_image)
        self.print_button.bind(on_press=self.print_image)
        self.next_button.bind(on_press=self.next_image)


class KivytestApp(App):
    def build(self):
        return KivytestScreen()


if __name__ == '__main__':
    Window.size = (1024, 600)
    KivytestApp().run()
