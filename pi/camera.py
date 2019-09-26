import io
import time

from picamera import PiCamera
from PIL import Image


class CameraController(object):
    def __init__(self, rotation=0, alpha=200):
        self.camera = PiCamera()
        self.camera.rotation = rotation
        self.camera.start_preview(alpha=alpha)

    def close(self):
        self.camera.stop_preview()
        self.camera.close()
        self.camera = None

    def take_picture(self):
        output = io.BytesIO()
        self.camera.capture(output, format='bmp')
        output.seek(0)
        return Image.open(output)


if __name__ == '__main__':
    camera = CameraController()

    start_time = time.time()
    for i in range(5):
        pic = camera.take_picture()
    end_time = time.time()
    pic.save('image.bmp')

    print('Average time', (end_time - start_time)/5)
    camera.close()
