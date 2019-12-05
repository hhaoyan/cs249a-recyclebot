from google.cloud import vision
import io
from camera import CameraController
from ble import BleController
import RPi.GPIO as GPIO

def get_labels(content):
    client = vision.ImageAnnotatorClient()

    image = vision.types.Image(content=content)

    response = client.label_detection(image=image)
    labels = response.label_annotations
    return [label.description for label in labels]

recycling_labels = [
    'plastic',
    'plastics',
    'bottle',
    'bottles',
    'paper',
    'papers',
    'cardboard',
    'cereal',
    'cereals',
    'box',
    'boxes',
    'magazine',
    'magazines',
    'mail',
    'office paper',
    'newspaper',
    'metal',
    'tin',
    'aluminum',
    'steel',
    'can',
    'cans',
    'glass',
    'glasses',
    'soft drink',
    'beer',
]

trash_labels = [
    'plastic bag',
    'plastic bags',
    'plastic stretch wrap',
    'cup',
    'foam cup',
    'foam cups',
    'foam',
    'plastic utensil',
    'plastic utensils',
    'fork',
    'knife',
    'spoon',
    'grocery bags',
    'grocery bag',
    'bag',
    'bags',
    'trash bag',
    'trash bags',  
    'bottle cap',  
    'bottle caps',
    'cap'
    'styrofoam',
    'wrapper',
    'chip',
    'chip bags',
    'chip bag',
    'chips',
    'potato chip',
    'food storage containers'
]

compost_labels = [
    'meat',
    'fish',
    'dairy',
    'fruit',
    'vegetable',
    'vegetables',
    'shell',
    'shells',
    'bones',
    'pasta',
    'rice',
    'eggshells',
    'nutshells',
    'bread',
    'grains',
    'scraps',
    'leftovers',
    'coffee',
    'coffee grounds',
    'coffee filters',
    'tea bag',
    'tea bags',
    'soiled',
    'soiled paper bags',
    'paper towel',
    'paper towels',
    'paper napkin',
    'paper napkins',
    'egg carton',
    'egg cartons',
    'paper egg cartons',
    'paper plates',
    'plants',
    'flowers',
    'vegetation',
    'wood',
    'scraps'
]

def get_fine_grain_labels(google_cloud_labels):
    fine_grain_labels = []
    for elem in google_cloud_labels:
        fine_grain_labels.extend(elem.split(' '))
    fine_grain_labels.extend(google_cloud_labels)
    fine_grain_labels = [label.lower() for label in fine_grain_labels]
    return fine_grain_labels

def get_classification(fine_grain_labels):
    count_recycling = 0
    count_compost = 0
    count_trash = 0
    for elem in fine_grain_labels:
        if elem in recycling_labels:
            count_recycling += 1
        if elem in trash_labels:
            count_trash += 1
        if elem in compost_labels:
            count_compost += 1
        
    classification = None
    print('count_recycling: {}, count_compost: {}, count_trash: {}'.format(count_recycling, count_compost, count_trash))
    if count_recycling > count_trash and count_recycling > count_compost:
        classification = 'recycling'
    elif count_compost > count_trash and count_compost > count_recycling:
        classification = 'composting'
    elif count_trash > count_recycling and count_trash > count_compost:
        classification = 'trash'
    else:
        classification = 'unknown'
    return classification

# location = '/Users/btl787/Google Drive/00-fa19/03-eecs-249a-embedded-systems/project/apple.jpg'
# location = '/home/pi/Desktop/crumpled_napkin.jpg'

def send_angle(classification):
    if classification == 'trash' or classification == 'unknown':
        ble.setValue(0)
    elif classification == 'recycling':
        ble.setValue(1)
    else:
        ble.setValue(2)


def take_picture_classify_on_cloud_send_rotate_signal():
    content = cam.take_picture()
    google_cloud_labels = get_labels(content)
    fine_grain_labels = get_fine_grain_labels(google_cloud_labels)
    classification = get_classification(fine_grain_labels)
    print('labels:', google_cloud_labels)
    print('classification:', classification)
    send_angle(classification)
    # send an angle command to the buckler

if __name__ == '__main__':
    ble = BleController(3)
    cam = CameraController()
    GPIO.setwarnings(False)
    GPIO.setmode(GPIO.BOARD)
    GPIO.setup(10, GPIO.IN, pull_up_down=GPIO.PUD_DOWN)

    while True:
        # wait until the button is pressed
        if GPIO.input(10) == GPIO.HIGH:
            print("Button is pushed")
            take_picture_classify_on_cloud_send_rotate_signal()





