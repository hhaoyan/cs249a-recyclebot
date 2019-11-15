import io
import time

from picamera import PiCamera
from PIL import Image


class CameraController(object):
    def __init__(self, rotation=0, alpha=0):
        self.camera = PiCamera()
        self.camera.rotation = rotation
        #self.camera.start_preview(alpha=alpha)

    def close(self):
        #self.camera.stop_preview()
        self.camera.close()
        self.camera = None

    def take_picture(self):
        output = io.BytesIO()
        self.camera.capture(output, format='bmp')
        output.seek(0)
        return output.getvalue()

